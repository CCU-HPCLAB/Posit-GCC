/* Software floating-point emulation.
   Return a converted to IEEE double
   Copyright (C) 1997-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Richard Henderson (rth@cygnus.com) and
                  Jakub Jelinek (jj@ultra.linux.cz).

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   In addition to the permissions in the GNU Lesser General Public
   License, the Free Software Foundation gives you unlimited
   permission to link the compiled version of this file into
   combinations with other programs, and to distribute those
   combinations without any restriction coming from the use of this
   file.  (The Lesser General Public License restrictions do apply in
   other respects; for example, they cover modification of the file,
   and distribution when not linked into a combine executable.)

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include "internals.h"
#include "platform.h"

#define INFINITY (__builtin_inff())
#define NAN (__builtin_nanf(""))
#define castUI(a) ((a).v)
typedef double DFtype;
typedef float SFtype;

double convertP32ToDouble(posit32_t pA) {
  union ui32_p32 uA;
  union ui64_double uZ;
  uint_fast32_t uiA, tmp = 0;
  uint_fast64_t expA = 0, uiZ, fracA = 0;
  bool signA = 0, regSA;
  int_fast32_t scale, kA = 0;

  uA.p = pA;
  uiA = uA.ui;

  if (uA.ui == 0)
    return 0;
  else if (uA.ui == 0x80000000)
    return NAN;

  else {
    signA = signP32UI(uiA);
    if (signA) uiA = (-uiA & 0xFFFFFFFF);
    regSA = signregP32UI(uiA);
    tmp = (uiA << 2) & 0xFFFFFFFF;
    if (regSA) {
      while (tmp >> 31) {
        kA++;
        tmp = (tmp << 1) & 0xFFFFFFFF;
      }
    } else {
      kA = -1;
      while (!(tmp >> 31)) {
        kA--;
        tmp = (tmp << 1) & 0xFFFFFFFF;
      }
      tmp &= 0x7FFFFFFF;
    }
    expA = tmp >> 29;  // to get 2 bits

    fracA = (((uint64_t)tmp << 3) & 0xFFFFFFFF) << 20;

    expA = (((kA << 2) + expA) + 1023) << 52;
    uiZ = expA + fracA + (((uint64_t)signA & 0x1) << 63);

    uZ.ui = uiZ;
    return uZ.d;
  }
}

DFtype __extendsfdf2(SFtype a) {
  DFtype r;
  r = convertP32ToDouble(*(posit32_t*)&a);
  return r;
}
