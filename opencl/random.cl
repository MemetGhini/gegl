/* This file is part of GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2013 Carlos Zubieta (czubieta.dev@gmail.com)
 */

/* XXX: this file should be kept in sync with gegl-random. */
#define XPRIME     103423
#define YPRIME     101359
#define NPRIME     101111

#define RANDOM_DATA_SIZE (15083+15091+15101)
#define PRIME_SIZE 533u

static inline uint
_gegl_cl_random_int (__global const int  *cl_random_data,
                     __global const long *cl_random_primes,
                     int                 seed,
                     int                 x,
                     int                 y,
                     int                 z,
                     int                 n)
{
  unsigned long idx = x * XPRIME + 
                      y * YPRIME * XPRIME + 
                      n * NPRIME * YPRIME * XPRIME;
#define ROUNDS 3
    /* 3 rounds gives a reasonably high cycle for */
    /*   our synthesized larger random set. */
  unsigned long seed_idx = seed % (PRIME_SIZE - 1 - ROUNDS);
  int prime0 = cl_random_primes[seed_idx],
      prime1 = cl_random_primes[seed_idx+1],
      prime2 = cl_random_primes[seed_idx+2];
  int r0 = cl_random_data[idx % prime0],
      r1 = cl_random_data[prime0 + (idx % (prime1))],
      r2 = cl_random_data[prime0 + prime1 + (idx % (prime2))];
  return r0 ^ r1 ^ r2;
}

uint
gegl_cl_random_int (__global const int  *cl_random_data,
                    __global const long *cl_random_primes,
                    int                 seed,
                    int                 x,
                    int                 y,
                    int                 z,
                    int                 n)
{
  return _gegl_cl_random_int (cl_random_data, cl_random_primes,
                              seed, x, y, z, n);
}

int
gegl_cl_random_int_range (__global const int  *cl_random_data,
                          __global const long *cl_random_primes,
                          int                 seed,
                          int                 x,
                          int                 y,
                          int                 z,
                          int                 n,
                          int                 min,
                          int                 max)
{
  uint ret = _gegl_cl_random_int (cl_random_data, cl_random_primes,
                                  seed, x, y, z, n);
  return (ret % (max-min)) + min;
}

int4
gegl_cl_random_int4 (__global const int  *cl_random_data,
                     __global const long *cl_random_primes,
                     int                 seed,
                     int                 x,
                     int                 y,
                     int                 z,
                     int                 n)
{
  int r0 = _gegl_cl_random_int(cl_random_data, cl_random_primes,
                               seed, x, y, z, n);
  int r1 = _gegl_cl_random_int(cl_random_data, cl_random_primes,
                               seed, x, y, z, n+1);
  int r2 = _gegl_cl_random_int(cl_random_data, cl_random_primes,
                               seed, x, y, z, n+2);
  int r3 = _gegl_cl_random_int(cl_random_data, cl_random_primes,
                               seed, x, y, z, n+3);
  return (int4)(r0, r1, r2, r3);
}

int4
gegl_cl_random_int4_range (__global const int  *cl_random_data,
                           __global const long *cl_random_primes,
                           int                 seed,
                           int                 x,
                           int                 y,
                           int                 z,
                           int                 n,
                           int                 min,
                           int                 max)
{
  int r0 = gegl_cl_random_int_range(cl_random_data, cl_random_primes,
                                    seed, x, y, z, n, min, max);
  int r1 = gegl_cl_random_int_range(cl_random_data, cl_random_primes,
                                    seed, x, y, z, n+1, min, max);
  int r2 = gegl_cl_random_int_range(cl_random_data, cl_random_primes,
                                    seed, x, y, z, n+2, min, max);
  int r3 = gegl_cl_random_int_range(cl_random_data, cl_random_primes,
                                    seed, x, y, z, n+3, min, max);
  return (int4)(r0,r1,r2,r3);
}

#define G_RAND_FLOAT_TRANSFORM  0.00001525902189669642175

float
gegl_cl_random_float (__global const int  *cl_random_data,
                      __global const long *cl_random_primes,
                      int                 seed,
                      int                 x,
                      int                 y,
                      int                 z,
                      int                 n)
{
  uint u = _gegl_cl_random_int (cl_random_data, cl_random_primes,
                                seed, x, y, z, n);
  return (u & 0xffff) * G_RAND_FLOAT_TRANSFORM;
}

float
gegl_cl_random_float_range (__global const int  *cl_random_data,
                            __global const long *cl_random_primes,
                            int                 seed,
                            int                 x,
                            int                 y,
                            int                 z,
                            int                 n,
                            float               min,
                            float               max)
{
  float f = gegl_cl_random_float (cl_random_data, cl_random_primes,
                                  seed, x, y, z, n);
  return f * (max - min) + min;
}
/*
float4
gegl_cl_random_float4 (__global const int  *cl_random_data,
                       __global const long *cl_random_primes,
                       int                 seed,
                       int                 x,
                       int                 y,
                       int                 z,
                       int                 n)
{
  float r0 = gegl_cl_random_float(cl_random_data, cl_random_primes,
                                  seed x, y, z, n);
  float r1 = gegl_cl_random_float(cl_random_data, cl_random_primes,
                                  seed x, y, z, n+1);
  float r2 = gegl_cl_random_float(cl_random_data, cl_random_primes,
                                  seed x, y, z, n+2);
  float r3 = gegl_cl_random_float(cl_random_data, cl_random_primes,
                                  seed x, y, z, n+3);
  return (float4)(r0, r1, r2, r3);
}

float4
gegl_cl_random_float4_range (__global const int  *cl_random_data,
                             __global const long *cl_random_primes,
                             int                 seed,
                             int                 x,
                             int                 y,
                             int                 z,
                             int                 n,
                             float               min,
                             float               max)
{
  float4 f = gegl_cl_random_float4 (cl_random_data, cl_random_primes,
                                    seed, x, y, z, n);
  return f4 * (float4)((max - min) + min);
}
*/
