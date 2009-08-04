/**
 * ntfsinternal.h - Internal support routines for NTFS-based devices.
 *
 * Copyright (c) 2009 Rhys "Shareese" Koedijk
 * Copyright (c) 2006 Michael "Chishm" Chisholm
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "ntfsinternal.h"

#define DEV_FD(x) ((gekko_fd*)(x)->dev->d_private)

#if defined(__wii__)
#include <sdcard/wiisd_io.h>
#include <sdcard/gcsd.h>
#include <ogc/usbstorage.h>

const INTERFACE_ID ntfs_disc_interfaces[] = {
    { "sd", &__io_wiisd },
    { "usb", &__io_usbstorage },
    { "carda", &__io_gcsda },
    { "cardb", &__io_gcsdb },
    { NULL, NULL }
};

#elif defined(__gamecube__)
#include <sdcard/gcsd.h>

const INTERFACE_ID ntfs_disc_interfaces[] = {
    { "carda", &__io_gcsda },
    { "cardb", &__io_gcsdb },
    { NULL, NULL }
};

#endif

ntfs_volume *ntfsGetVolumeFromPath (const char *path)
{
    // Get the volume from the paths associated devoptab (if possible)
    const devoptab_t *devops_ntfs = ntfsDeviceOpTab();
    const devoptab_t *devops = ntfsGetDeviceOpTab(path);
    if (devops && devops_ntfs && (devops->open_r == devops_ntfs->open_r))
        return (ntfs_volume*)devops->deviceData;
    
    return NULL;
}

int ntfsUnicodeToLocal (const ntfschar *ins, const int ins_len, char **outs, int outs_len)
{
    int len = 0;
    int i;

    // Convert the unicode string to our current local
    len = ntfs_ucstombs(ins, ins_len, outs, outs_len); 
    if (len == -1 && errno == EILSEQ) {
        
        // The string could not be converted to the current local,
        // do it manually by replacing non-ASCII characters with underscores
        if (!*outs || outs_len >= ins_len) {
            if (!*outs) {
                *outs = ntfs_alloc(ins_len + 1);
                if (!*outs) {
                    errno = ENOMEM;
                    return -1;
                }
            }
            for (i = 0; i < ins_len; i++) {
                ntfschar uc = le16_to_cpu(ins[i]);
                if (uc > 0xff)
                    uc = (ntfschar)'_';
                *outs[i] = (char)uc;
            }
            *outs[ins_len] = (ntfschar)'\0';
            len = ins_len;
        }
        
    }

    return len;
}

int ntfsLocalToUnicode(const char *ins, ntfschar **outs)
{
    // Convert the local string to unicode
    return ntfs_mbstoucs(ins, outs);
}

int ntfsCreate (const char *path, dev_t type, dev_t dev, const char *target)
{
    ntfs_volume *vol = NULL;
    ntfs_inode *dir_ni = NULL, *ni = NULL;
    char *dir = NULL;
    char *name = NULL;
    ntfschar *uname = NULL, *utarget = NULL;
    int uname_len, utarget_len;
    int res = 0;
    
    // Get the volume for this path
    vol = ntfsGetVolumeFromPath(path);
    if (!vol) {
        errno = ENODEV;
        return -1;
    }
    
    // You cannot link between devices
    if(target) {
        if(vol != ntfsGetVolumeFromPath(target)) {
            errno = EXDEV;
            return -1;
        }
    }

    // Move the path pointer to the start of the actual path
    if (strchr(path, ':') != NULL) {
        path = strchr(path, ':') + 1;
    }
    if (strchr(path, ':') != NULL) {
        errno = EINVAL;
        return -1;
    }
    
    // Lock
    ntfsLock(DEV_FD(vol));
    
    // Copy the path
    dir = strdup(path);
    if (!dir) {
        errno = EINVAL;
        res = -1;
        goto cleanup;
    }
    
    // Get the unicode name for the entry and find its parent directory
    name = strrchr(dir, '/');
    if (name) {
        name++;
        uname_len = ntfsLocalToUnicode(name, &uname);
        if (uname_len < 0) {
            errno = EINVAL;
            res = -1;
            goto cleanup;
        }
        *name = 0;
    } else {
        errno = EINVAL;
        res = -1;
        goto cleanup;
    }
    
    // Open the entries parent directory
    dir_ni = ntfs_pathname_to_inode(vol, NULL, dir);
    if (!dir_ni) {
        goto cleanup;
        res = -1;
        return -1;
    }

    // Create the entry
    switch (type) {
        
        // Character/block device
        case S_IFCHR:
        case S_IFBLK:
            ni = ntfs_create_device(dir_ni, uname, uname_len, type, dev);
            break;
        
        // Symbolic link
        case S_IFLNK:
            utarget_len = ntfsLocalToUnicode(target, &utarget);
            if (utarget_len < 0) {
                errno = EINVAL;
                res = -1;
                goto cleanup;
            }
            ni = ntfs_create_symlink(dir_ni, uname, uname_len,  utarget, utarget_len);
            break;
        
        // Standard file/directory
        default:
            ni = ntfs_create(dir_ni, uname, uname_len, type);
            break;
            
    }

    // If the entry was created, close it and update its parent directories times
    if (ni) {
        ntfs_inode_close(ni);
        ntfsUpdateTimes(dir_ni, NTFS_UPDATE_MCTIME);
    } else {
        res = -1;
    }
    
cleanup:

    if(dir_ni)
        ntfs_inode_close(dir_ni);
        
    if(utarget)
        ntfs_free(utarget);
    
    if(uname)
        ntfs_free(uname);
    
    if(dir)
        ntfs_free(dir);
    
    // Unlock
    ntfsUnlock(DEV_FD(vol));
    
    return res;
}

int ntfsLink (const char *old_path, const char *new_path)
{
    ntfs_volume *vol = NULL;
    ntfs_inode *dir_ni = NULL, *ni = NULL;
    char *dir = NULL;
    char *name = NULL;
    ntfschar *uname = NULL;
    int uname_len;
    int res = 0;
    
    // Get the volume for this path
    vol = ntfsGetVolumeFromPath(old_path);
    if (!vol) {
        errno = ENODEV;
        return -1;
    }
    
    // You cannot link between devices
    if(vol != ntfsGetVolumeFromPath(new_path)) {
        errno = EXDEV;
        return -1;
    }
    
    // Move the path pointers to the start of the actual path
    if (strchr(old_path, ':') != NULL) {
        old_path = strchr(old_path, ':') + 1;
    }
    if (strchr(new_path, ':') != NULL) {
        new_path = strchr(new_path, ':') + 1;
    }
    if (strchr(old_path, ':') != NULL) {
        errno = EINVAL;
        return -1;
    }
    if (strchr(new_path, ':') != NULL) {
        errno = EINVAL;
        return -1;
    }
    
    // Lock
    ntfsLock(DEV_FD(vol));
    
    // Copy the destination path
    dir = strdup(new_path);
    if (!dir) {
        errno = EINVAL;
        res = -1;
        goto cleanup;
    }
    
    // Get the unicode name for the destination entry and find its parent directory
    name = strrchr(dir, '/');
    if (name) {
        name++;
        uname_len = ntfsLocalToUnicode(name, &uname);
        if (uname_len < 0) {
            errno = EINVAL;
            res = -1;
            goto cleanup;
        }
        *name = 0;
    } else {
        errno = EINVAL;
        res = -1;
        goto cleanup;
    }
    
    // Find the entry
    ni = ntfs_pathname_to_inode(vol, NULL, old_path);
    if (!ni) {
        errno = ENOENT;
        res = -1;
        goto cleanup;
    }
    
    // Open the entries new parent directory
    dir_ni = ntfs_pathname_to_inode(vol, NULL, dir);
    if (!dir_ni) {
        errno = ENOENT;
        res = -1;
        goto cleanup;
    }
    
    // Link the entry to its new parent
    if (ntfs_link(ni, dir_ni, uname, uname_len)) {
        res = -1;
        goto cleanup;
    }
    
    // Update entry times
    ntfsUpdateTimes(ni, NTFS_UPDATE_CTIME);
    ntfsUpdateTimes(dir_ni, NTFS_UPDATE_MCTIME);
    
cleanup:
    
    if(dir_ni)
        ntfs_inode_close(dir_ni);

    if(ni)
        ntfs_inode_close(ni);
    
    if(uname)
        ntfs_free(uname);
    
    if(dir)
        ntfs_free(dir);
    
    // Unlock
    ntfsUnlock(DEV_FD(vol));
    
    return res;
}

int ntfsUnlink (const char *path)
{
    ntfs_volume *vol = NULL;
    ntfs_inode *dir_ni = NULL, *ni = NULL;
    char *dir = NULL;
    char *name = NULL;
    ntfschar *uname = NULL;
    int uname_len;
    int res = 0;
    
    // Get the volume for this path
    vol = ntfsGetVolumeFromPath(path);
    if (!vol) {
        errno = ENODEV;
        return -1;
    }
    
    // Move the path pointer to the start of the actual path
    if (strchr(path, ':') != NULL) {
        path = strchr(path, ':') + 1;
    }
    if (strchr(path, ':') != NULL) {
        errno = EINVAL;
        return -1;
    }
    
    // Lock
    ntfsLock(DEV_FD(vol));
    
    // Copy the path
    dir = strdup(path);
    if (!dir) {
        errno = EINVAL;
        res = -1;
        goto cleanup;
    }
    
    // Get the unicode name for the entry and find its parent directory
    name = strrchr(dir, '/');
    if (name) {
        name++;
        uname_len = ntfsLocalToUnicode(name, &uname);
        if (uname_len < 0) {
            errno = EINVAL;
            res = -1;
            goto cleanup;
        }
        *name = 0;
    } else {
        errno = EINVAL;
        res = -1;
        goto cleanup;
    }
    
    // Find the entry
    ni = ntfs_pathname_to_inode(vol, NULL, path);
    if (!ni) {
        errno = ENOENT;
        res = -1;
        goto cleanup;
    }
    
    // Open the entries parent directory
    dir_ni = ntfs_pathname_to_inode(vol, NULL, dir);
    if (!dir_ni) {
        errno = ENOENT;
        res = -1;
        goto cleanup;
    }
    
    // Unlink the entry from its parent
    if (ntfs_delete(ni, dir_ni, uname, uname_len)) {
        res = -1;
    }
    
    // ntfs_delete() ALWAYS closes ni and dir_ni; so no need for us to anymore
    dir_ni = ni = NULL;
    
cleanup:
    
    if(dir_ni)
        ntfs_inode_close(dir_ni);
    
    if(ni)
        ntfs_inode_close(ni);
    
    if(uname)
        ntfs_free(uname);
    
    if(dir)
        ntfs_free(dir);
    
    // Unlock
    ntfsUnlock(DEV_FD(vol));
    
    return 0;
}

int ntfsStat (const char *path, struct stat *st)
{
    ntfs_volume *vol = NULL;
    gekko_fd *fd = NULL;
    ntfs_inode *ni = NULL;
    ntfs_attr *na = NULL;
    INTX_FILE *intx_file = NULL;
    int res = 0;
    
    // Get the volume for this path
    vol = ntfsGetVolumeFromPath(path);
    if (!vol) {
        errno = ENODEV;
        return -1;
    }
    
    // Get the device driver descriptor
    fd = DEV_FD(vol);
    if (!vol) {
        errno = EBADF;
        return -1;
    }
    
    // Move the path pointer to the start of the actual path
    if (strchr(path, ':') != NULL) {
        path = strchr(path, ':') + 1;
    }
    if (strchr(path, ':') != NULL) {
        errno = EINVAL;
        return -1;
    }
    
    // Lock
    ntfsLock(DEV_FD(vol));
    
    // Find the entry
    ni = ntfs_pathname_to_inode(vol, NULL, path);
    if (!ni) {
        errno = ENOENT;
        res = -1;
        goto cleanup;
    }
    
    // Zero out the stat buffer
    //memset(st, 0, sizeof(struct stat));

    // Is this entry a directory
    if (ni->mrec->flags & MFT_RECORD_IS_DIRECTORY) {
        st->st_mode = S_IFDIR | (0777 & ~fd->dmask);
        st->st_nlink = 1;
        na = ntfs_attr_open(ni, AT_INDEX_ALLOCATION, NTFS_INDEX_I30, 4);
        if (na) {
            st->st_size = na->data_size;
            st->st_blocks = na->allocated_size >> 9;
        }
        
    // Else it must be a file (of some sort)
    } else {
        st->st_mode = S_IFREG;
        st->st_size = ni->data_size;
        st->st_blocks = (ni->allocated_size + 511) >> 9;
        st->st_nlink = le16_to_cpu(ni->mrec->link_count);
        if (ni->flags & FILE_ATTR_SYSTEM) {
            
            // Open the files data attribute
            na = ntfs_attr_open(ni, AT_DATA, AT_UNNAMED, 0);
            if (!na) {
                res = -1;
                goto cleanup;
            }

            // Check if the file is a Interix FIFO or socket
            if (!(ni->flags & FILE_ATTR_HIDDEN)) {
                if (na->data_size == 0)
                    st->st_mode = S_IFIFO;
                if (na->data_size == 1)
                    st->st_mode = S_IFSOCK;
            }
            
            // Check if the file is a Interix symbolic link, block or character device
            if (na->data_size <= sizeof(INTX_FILE_TYPES) + sizeof(ntfschar) * PATH_MAX && 
                na->data_size > sizeof(INTX_FILE_TYPES)) {
                
                intx_file = ntfs_alloc(na->data_size);
                if (!intx_file) {
                    res = -1;
                    goto cleanup;
                }
                if (ntfs_attr_pread(na, 0, na->data_size, intx_file) != na->data_size) {
                    res = -1;
                    goto cleanup;
                }
                if (intx_file->magic == INTX_BLOCK_DEVICE &&
                    na->data_size == offsetof(INTX_FILE, device_end)) {
                    st->st_mode = S_IFBLK;
                    st->st_rdev = mkdev(le64_to_cpu(intx_file->major), le64_to_cpu(intx_file->minor));
                }
                if (intx_file->magic == INTX_CHARACTER_DEVICE &&
                    na->data_size == offsetof(INTX_FILE, device_end)) {
                    st->st_mode = S_IFCHR;
                    st->st_rdev = mkdev(le64_to_cpu(intx_file->major), le64_to_cpu(intx_file->minor));
                }
                if (intx_file->magic == INTX_SYMBOLIC_LINK) {
                    st->st_mode = S_IFLNK;
                }
                
            }
            
        }
        st->st_mode |= (0777 & ~fd->fmask);
    }
    
    // Fill in the generic entry stats
    st->st_dev = fd->interface->ioType;
    st->st_uid = fd->uid;
    st->st_gid = fd->gid;
    st->st_ino = ni->mft_no;
    st->st_atime = ni->last_access_time;
    st->st_ctime = ni->last_mft_change_time;
    st->st_mtime = ni->last_data_change_time;
    
    // Update entry times
    ntfsUpdateTimes(ni, NTFS_UPDATE_ATIME);
    
cleanup:

    if(intx_file)
        ntfs_free(intx_file);
    
    if(na)
        ntfs_attr_close(na);
    
    if(ni)
        ntfs_inode_close(ni);
    
    // Unlock
    ntfsUnlock(DEV_FD(vol));
    
    return res;
}

void ntfsUpdateTimes (ntfs_inode *ni, ntfs_time_update_flags mask)
{
    gekko_fd *fd = DEV_FD(ni->vol);
    
    // Run access time updates against the device driver settings first
    if (fd->atime == ATIME_DISABLED)
        mask &= ~NTFS_UPDATE_ATIME;
    if (fd->atime == ATIME_RELATIVE && mask == NTFS_UPDATE_ATIME &&
        ni->last_access_time >= ni->last_data_change_time &&
        ni->last_access_time >= ni->last_mft_change_time)
        return;
    
    // Update entry times
    ntfs_inode_update_times(ni, mask);
    
    return;
}