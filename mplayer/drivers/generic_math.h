/*
 * generic implementation of sin(x) and cos(x) functions specially for Linux
 * Copyright (C) 2002 Nick Kurshev
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

#ifndef MPLAYER_GENERIC_MATH_H
#define MPLAYER_GENERIC_MATH_H

typedef struct gen_sincos
{
  double x;
  double sinx;
  double cosx;
}gen_sincos_t;

static gen_sincos_t g_sincos[201] = {
{ -3.141600e+00, 7.346410e-06, -1.000000e-00 },
{ -3.110184e+00, -3.140349e-02, -9.995068e-01 },
{ -3.078768e+00, -6.278333e-02, -9.980272e-01 },
{ -3.047352e+00, -9.410122e-02, -9.955626e-01 },
{ -3.015936e+00, -1.253262e-01, -9.921156e-01 },
{ -2.984520e+00, -1.564276e-01, -9.876894e-01 },
{ -2.953104e+00, -1.873745e-01, -9.822885e-01 },
{ -2.921688e+00, -2.181366e-01, -9.759183e-01 },
{ -2.890272e+00, -2.486833e-01, -9.685848e-01 },
{ -2.858856e+00, -2.789847e-01, -9.602956e-01 },
{ -2.827440e+00, -3.090107e-01, -9.510586e-01 },
{ -2.796024e+00, -3.387318e-01, -9.408830e-01 },
{ -2.764608e+00, -3.681185e-01, -9.297789e-01 },
{ -2.733192e+00, -3.971420e-01, -9.177572e-01 },
{ -2.701776e+00, -4.257736e-01, -9.048297e-01 },
{ -2.670360e+00, -4.539849e-01, -8.910094e-01 },
{ -2.638944e+00, -4.817483e-01, -8.763097e-01 },
{ -2.607528e+00, -5.090362e-01, -8.607451e-01 },
{ -2.576112e+00, -5.358217e-01, -8.443312e-01 },
{ -2.544696e+00, -5.620785e-01, -8.270839e-01 },
{ -2.513280e+00, -5.877805e-01, -8.090204e-01 },
{ -2.481864e+00, -6.129025e-01, -7.901586e-01 },
{ -2.450448e+00, -6.374196e-01, -7.705169e-01 },
{ -2.419032e+00, -6.613076e-01, -7.501148e-01 },
{ -2.387616e+00, -6.845430e-01, -7.289724e-01 },
{ -2.356200e+00, -7.071029e-01, -7.071107e-01 },
{ -2.324784e+00, -7.289649e-01, -6.845511e-01 },
{ -2.293368e+00, -7.501075e-01, -6.613159e-01 },
{ -2.261952e+00, -7.705099e-01, -6.374281e-01 },
{ -2.230536e+00, -7.901518e-01, -6.129112e-01 },
{ -2.199120e+00, -8.090140e-01, -5.877894e-01 },
{ -2.167704e+00, -8.270777e-01, -5.620876e-01 },
{ -2.136288e+00, -8.443252e-01, -5.358310e-01 },
{ -2.104872e+00, -8.607395e-01, -5.090457e-01 },
{ -2.073456e+00, -8.763043e-01, -4.817579e-01 },
{ -2.042040e+00, -8.910044e-01, -4.539948e-01 },
{ -2.010624e+00, -9.048251e-01, -4.257835e-01 },
{ -1.979208e+00, -9.177528e-01, -3.971521e-01 },
{ -1.947792e+00, -9.297748e-01, -3.681288e-01 },
{ -1.916376e+00, -9.408793e-01, -3.387421e-01 },
{ -1.884960e+00, -9.510552e-01, -3.090212e-01 },
{ -1.853544e+00, -9.602925e-01, -2.789953e-01 },
{ -1.822128e+00, -9.685821e-01, -2.486940e-01 },
{ -1.790712e+00, -9.759158e-01, -2.181473e-01 },
{ -1.759296e+00, -9.822865e-01, -1.873854e-01 },
{ -1.727880e+00, -9.876877e-01, -1.564385e-01 },
{ -1.696464e+00, -9.921142e-01, -1.253372e-01 },
{ -1.665048e+00, -9.955616e-01, -9.411219e-02 },
{ -1.633632e+00, -9.980265e-01, -6.279433e-02 },
{ -1.602216e+00, -9.995064e-01, -3.141450e-02 },
{ -1.570800e+00, -1.000000e-00, -3.673205e-06 },
{ -1.539384e+00, -9.995067e-01, 3.140716e-02 },
{ -1.507968e+00, -9.980269e-01, 6.278700e-02 },
{ -1.476552e+00, -9.955623e-01, 9.410488e-02 },
{ -1.445136e+00, -9.921151e-01, 1.253299e-01 },
{ -1.413720e+00, -9.876889e-01, 1.564312e-01 },
{ -1.382304e+00, -9.822879e-01, 1.873781e-01 },
{ -1.350888e+00, -9.759175e-01, 2.181402e-01 },
{ -1.319472e+00, -9.685839e-01, 2.486869e-01 },
{ -1.288056e+00, -9.602945e-01, 2.789882e-01 },
{ -1.256640e+00, -9.510574e-01, 3.090142e-01 },
{ -1.225224e+00, -9.408817e-01, 3.387352e-01 },
{ -1.193808e+00, -9.297775e-01, 3.681220e-01 },
{ -1.162392e+00, -9.177557e-01, 3.971454e-01 },
{ -1.130976e+00, -9.048282e-01, 4.257769e-01 },
{ -1.099560e+00, -8.910077e-01, 4.539882e-01 },
{ -1.068144e+00, -8.763079e-01, 4.817515e-01 },
{ -1.036728e+00, -8.607433e-01, 5.090393e-01 },
{ -1.005312e+00, -8.443292e-01, 5.358248e-01 },
{ -9.738960e-01, -8.270819e-01, 5.620815e-01 },
{ -9.424800e-01, -8.090183e-01, 5.877835e-01 },
{ -9.110640e-01, -7.901563e-01, 6.129054e-01 },
{ -8.796480e-01, -7.705146e-01, 6.374224e-01 },
{ -8.482320e-01, -7.501124e-01, 6.613104e-01 },
{ -8.168160e-01, -7.289699e-01, 6.845457e-01 },
{ -7.854000e-01, -7.071081e-01, 7.071055e-01 },
{ -7.539840e-01, -6.845484e-01, 7.289674e-01 },
{ -7.225680e-01, -6.613131e-01, 7.501100e-01 },
{ -6.911520e-01, -6.374252e-01, 7.705122e-01 },
{ -6.597360e-01, -6.129083e-01, 7.901541e-01 },
{ -6.283200e-01, -5.877864e-01, 8.090161e-01 },
{ -5.969040e-01, -5.620845e-01, 8.270798e-01 },
{ -5.654880e-01, -5.358279e-01, 8.443272e-01 },
{ -5.340720e-01, -5.090425e-01, 8.607414e-01 },
{ -5.026560e-01, -4.817547e-01, 8.763061e-01 },
{ -4.712400e-01, -4.539915e-01, 8.910060e-01 },
{ -4.398240e-01, -4.257802e-01, 9.048266e-01 },
{ -4.084080e-01, -3.971488e-01, 9.177542e-01 },
{ -3.769920e-01, -3.681254e-01, 9.297762e-01 },
{ -3.455760e-01, -3.387387e-01, 9.408805e-01 },
{ -3.141600e-01, -3.090177e-01, 9.510563e-01 },
{ -2.827440e-01, -2.789917e-01, 9.602935e-01 },
{ -2.513280e-01, -2.486905e-01, 9.685830e-01 },
{ -2.199120e-01, -2.181437e-01, 9.759166e-01 },
{ -1.884960e-01, -1.873817e-01, 9.822872e-01 },
{ -1.570800e-01, -1.564348e-01, 9.876883e-01 },
{ -1.256640e-01, -1.253335e-01, 9.921147e-01 },
{ -9.424800e-02, -9.410853e-02, 9.955619e-01 },
{ -6.283200e-02, -6.279067e-02, 9.980267e-01 },
{ -3.141600e-02, -3.141083e-02, 9.995066e-01 },
{ 0.000000e+00, 0.000000e+00, 1.000000e+00 },
{ 3.141600e-02, 3.141083e-02, 9.995066e-01 },
{ 6.283200e-02, 6.279067e-02, 9.980267e-01 },
{ 9.424800e-02, 9.410853e-02, 9.955619e-01 },
{ 1.256640e-01, 1.253335e-01, 9.921147e-01 },
{ 1.570800e-01, 1.564348e-01, 9.876883e-01 },
{ 1.884960e-01, 1.873817e-01, 9.822872e-01 },
{ 2.199120e-01, 2.181437e-01, 9.759166e-01 },
{ 2.513280e-01, 2.486905e-01, 9.685830e-01 },
{ 2.827440e-01, 2.789917e-01, 9.602935e-01 },
{ 3.141600e-01, 3.090177e-01, 9.510563e-01 },
{ 3.455760e-01, 3.387387e-01, 9.408805e-01 },
{ 3.769920e-01, 3.681254e-01, 9.297762e-01 },
{ 4.084080e-01, 3.971488e-01, 9.177542e-01 },
{ 4.398240e-01, 4.257802e-01, 9.048266e-01 },
{ 4.712400e-01, 4.539915e-01, 8.910060e-01 },
{ 5.026560e-01, 4.817547e-01, 8.763061e-01 },
{ 5.340720e-01, 5.090425e-01, 8.607414e-01 },
{ 5.654880e-01, 5.358279e-01, 8.443272e-01 },
{ 5.969040e-01, 5.620845e-01, 8.270798e-01 },
{ 6.283200e-01, 5.877864e-01, 8.090161e-01 },
{ 6.597360e-01, 6.129083e-01, 7.901541e-01 },
{ 6.911520e-01, 6.374252e-01, 7.705122e-01 },
{ 7.225680e-01, 6.613131e-01, 7.501100e-01 },
{ 7.539840e-01, 6.845484e-01, 7.289674e-01 },
{ 7.854000e-01, 7.071081e-01, 7.071055e-01 },
{ 8.168160e-01, 7.289699e-01, 6.845457e-01 },
{ 8.482320e-01, 7.501124e-01, 6.613104e-01 },
{ 8.796480e-01, 7.705146e-01, 6.374224e-01 },
{ 9.110640e-01, 7.901563e-01, 6.129054e-01 },
{ 9.424800e-01, 8.090183e-01, 5.877835e-01 },
{ 9.738960e-01, 8.270819e-01, 5.620815e-01 },
{ 1.005312e+00, 8.443292e-01, 5.358248e-01 },
{ 1.036728e+00, 8.607433e-01, 5.090393e-01 },
{ 1.068144e+00, 8.763079e-01, 4.817515e-01 },
{ 1.099560e+00, 8.910077e-01, 4.539882e-01 },
{ 1.130976e+00, 9.048282e-01, 4.257769e-01 },
{ 1.162392e+00, 9.177557e-01, 3.971454e-01 },
{ 1.193808e+00, 9.297775e-01, 3.681220e-01 },
{ 1.225224e+00, 9.408817e-01, 3.387352e-01 },
{ 1.256640e+00, 9.510574e-01, 3.090142e-01 },
{ 1.288056e+00, 9.602945e-01, 2.789882e-01 },
{ 1.319472e+00, 9.685839e-01, 2.486869e-01 },
{ 1.350888e+00, 9.759175e-01, 2.181402e-01 },
{ 1.382304e+00, 9.822879e-01, 1.873781e-01 },
{ 1.413720e+00, 9.876889e-01, 1.564312e-01 },
{ 1.445136e+00, 9.921151e-01, 1.253299e-01 },
{ 1.476552e+00, 9.955623e-01, 9.410488e-02 },
{ 1.507968e+00, 9.980269e-01, 6.278700e-02 },
{ 1.539384e+00, 9.995067e-01, 3.140716e-02 },
{ 1.570800e+00, 1.000000e-00, -3.673205e-06 },
{ 1.602216e+00, 9.995064e-01, -3.141450e-02 },
{ 1.633632e+00, 9.980265e-01, -6.279433e-02 },
{ 1.665048e+00, 9.955616e-01, -9.411219e-02 },
{ 1.696464e+00, 9.921142e-01, -1.253372e-01 },
{ 1.727880e+00, 9.876877e-01, -1.564385e-01 },
{ 1.759296e+00, 9.822865e-01, -1.873854e-01 },
{ 1.790712e+00, 9.759158e-01, -2.181473e-01 },
{ 1.822128e+00, 9.685821e-01, -2.486940e-01 },
{ 1.853544e+00, 9.602925e-01, -2.789953e-01 },
{ 1.884960e+00, 9.510552e-01, -3.090212e-01 },
{ 1.916376e+00, 9.408793e-01, -3.387421e-01 },
{ 1.947792e+00, 9.297748e-01, -3.681288e-01 },
{ 1.979208e+00, 9.177528e-01, -3.971521e-01 },
{ 2.010624e+00, 9.048251e-01, -4.257835e-01 },
{ 2.042040e+00, 8.910044e-01, -4.539948e-01 },
{ 2.073456e+00, 8.763043e-01, -4.817579e-01 },
{ 2.104872e+00, 8.607395e-01, -5.090457e-01 },
{ 2.136288e+00, 8.443252e-01, -5.358310e-01 },
{ 2.167704e+00, 8.270777e-01, -5.620876e-01 },
{ 2.199120e+00, 8.090140e-01, -5.877894e-01 },
{ 2.230536e+00, 7.901518e-01, -6.129112e-01 },
{ 2.261952e+00, 7.705099e-01, -6.374281e-01 },
{ 2.293368e+00, 7.501075e-01, -6.613159e-01 },
{ 2.324784e+00, 7.289649e-01, -6.845511e-01 },
{ 2.356200e+00, 7.071029e-01, -7.071107e-01 },
{ 2.387616e+00, 6.845430e-01, -7.289724e-01 },
{ 2.419032e+00, 6.613076e-01, -7.501148e-01 },
{ 2.450448e+00, 6.374196e-01, -7.705169e-01 },
{ 2.481864e+00, 6.129025e-01, -7.901586e-01 },
{ 2.513280e+00, 5.877805e-01, -8.090204e-01 },
{ 2.544696e+00, 5.620785e-01, -8.270839e-01 },
{ 2.576112e+00, 5.358217e-01, -8.443312e-01 },
{ 2.607528e+00, 5.090362e-01, -8.607451e-01 },
{ 2.638944e+00, 4.817483e-01, -8.763097e-01 },
{ 2.670360e+00, 4.539849e-01, -8.910094e-01 },
{ 2.701776e+00, 4.257736e-01, -9.048297e-01 },
{ 2.733192e+00, 3.971420e-01, -9.177572e-01 },
{ 2.764608e+00, 3.681185e-01, -9.297789e-01 },
{ 2.796024e+00, 3.387318e-01, -9.408830e-01 },
{ 2.827440e+00, 3.090107e-01, -9.510586e-01 },
{ 2.858856e+00, 2.789847e-01, -9.602956e-01 },
{ 2.890272e+00, 2.486833e-01, -9.685848e-01 },
{ 2.921688e+00, 2.181366e-01, -9.759183e-01 },
{ 2.953104e+00, 1.873745e-01, -9.822885e-01 },
{ 2.984520e+00, 1.564276e-01, -9.876894e-01 },
{ 3.015936e+00, 1.253262e-01, -9.921156e-01 },
{ 3.047352e+00, 9.410122e-02, -9.955626e-01 },
{ 3.078768e+00, 6.278333e-02, -9.980272e-01 },
{ 3.110184e+00, 3.140349e-02, -9.995068e-01 },
{ 3.141600e+00, -7.346410e-06, -1.000000e-00 }
};

# define M_PI		3.14159265358979323846	/* pi */

static double inline gen_sin(double x)
{
  int i;
  if(x < 0) while(x < -M_PI) x+= M_PI;
  else      while(x > M_PI) x-= M_PI;
  for(i=0;i<sizeof(g_sincos)/sizeof(gen_sincos_t)-1;i++)
  {
    if(x>=g_sincos[i].x && x <= g_sincos[i+1].x)
    {
      return (g_sincos[i+1].sinx-g_sincos[i].sinx)*(x-g_sincos[i].x)/(g_sincos[i+1].x-g_sincos[i].x)+g_sincos[i].sinx;
    }
  }
  return x<0?1:-1;
}
#undef sin
#define sin(x) gen_sin(x)

static double inline gen_cos(double x)
{
  int i;
  if(x < 0) while(x < -M_PI) x+= M_PI;
  else      while(x > M_PI) x-= M_PI;
  for(i=0;i<sizeof(g_sincos)/sizeof(gen_sincos_t)-1;i++)
  {
    if(x>=g_sincos[i].x && x <= g_sincos[i+1].x)
    {
      return (g_sincos[i+1].cosx-g_sincos[i].cosx)*(x-g_sincos[i].x)/(g_sincos[i+1].x-g_sincos[i].x)+g_sincos[i].cosx;
    }
  }
  return x<0?1:-1;
}
#undef cos
#define cos(x) gen_cos(x)

#endif /* MPLAYER_GENERIC_MATH_H */
