/* Software floating-point emulation.
   Convert a 64bit unsigned integer to IEEE single
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

typedef float SFtype;
typedef long UDItype;
#include <stdint.h>

#include "internals.h"
#include "platform.h"

posit32_t ui64_to_p32(uint64_t a) {
  int_fast8_t k, log2 = 63;  // length of bit (e.g. 18445618173802708992) in int
                             // (64 but because we have only 64 bits, so one bit
                             // off to accommodate that fact)
  union ui32_p32 uZ;
  uint_fast64_t uiA;
  uint_fast64_t mask = 0x8000000000000000, fracA;
  uint_fast32_t expA;

  if (a > 18445618173802708991ULL)  // 0xFFFBFFFFFFFFFFFF  is the midpoint
    uiA = 0x7FFFC000;               // P32: 18446744073709552000
  else if (a < 0x2)
    uiA = (a << 30);
  else {
    fracA = a;
    while (!(fracA & mask)) {
      log2--;
      fracA <<= 1;
    }

    k = (log2 >> 2);

    expA = (log2 & 0x3) << (27 - k);
    fracA = (fracA ^ mask);

    uiA = (0x7FFFFFFF ^ (0x3FFFFFFF >> k)) | expA | fracA >> (k + 36);

    mask = 0x800000000 << k;  // bitNPlusOne

    if (mask & fracA) {
      if (((mask - 1) & fracA) | ((mask << 1) & fracA)) uiA++;
    }
  }
  uZ.ui = uiA;
  return uZ.p;
}

SFtype __floatundisf(UDItype i) {
  posit32_t a;

  a = ui64_to_p32(i);

  return *(SFtype*)&a;
}
