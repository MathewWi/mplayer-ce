/*
 * Network layer for MPlayer
 *
 * Copyright (C) 2001 Bertrand Baudet <bertrand_baudet@yahoo.com>
 *
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MPLAYER_NETWORK_H
#define MPLAYER_NETWORK_H

#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>

#include "config.h"

#if !defined(GEKKO)
#if !HAVE_WINSOCK2_H
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#endif

#if defined(GEKKO)
#include <network.h>
#define send(a, b, c, d) net_send(a, b, c, d)
#define recv(a, b, c, d) net_recv(a, b, c, d)
#define select(a, b, c, d, e) net_select(a, b, c, d, e)
#define socket(a, b, c) net_socket(a, b, c)
#define gethostbyname(a) net_gethostbyname(a)
#define closesocket(a) net_close(a)
#define setsockopt(a, b, c, d, e) net_setsockopt(a, b, c, d, e)
#define bind(a, b, c) net_bind(a, b, c)
#define connect(a, b, c) net_connect(a, b, c)
#endif

#include "stream.h"
#include "url.h"
#include "http.h"

#ifdef MSG_NOSIGNAL
#define DEFAULT_SEND_FLAGS MSG_NOSIGNAL
#else
#define DEFAULT_SEND_FLAGS 0
#endif

#if !HAVE_CLOSESOCKET
//#define closesocket close
#endif
#if !HAVE_SOCKLEN_T
typedef int socklen_t;
#endif

#define BUFFER_SIZE		2048

typedef struct {
	const char *mime_type;
	int demuxer_type;
} mime_struct_t;

extern char *cookies_file;
extern char *network_password;
extern char *network_referrer;
extern char *network_useragent;
extern char *network_username;

extern int   network_bandwidth;
extern int   network_cookies_enabled;
extern int   network_ipv4_only_proxy;

streaming_ctrl_t *streaming_ctrl_new(void);
int streaming_bufferize( streaming_ctrl_t *streaming_ctrl, char *buffer, int size);

int nop_streaming_read( int fd, char *buffer, int size, streaming_ctrl_t *stream_ctrl );
int nop_streaming_seek( int fd, off_t pos, streaming_ctrl_t *stream_ctrl );
void streaming_ctrl_free( streaming_ctrl_t *streaming_ctrl );

int http_send_request(URL_t *url, off_t pos);
HTTP_header_t *http_read_response(int fd);

int http_authenticate(HTTP_header_t *http_hdr, URL_t *url, int *auth_retry);
URL_t* check4proxies(URL_t *url);

void fixup_network_stream_cache(stream_t *stream);
int http_seek(stream_t *stream, off_t pos);

#endif /* MPLAYER_NETWORK_H */
