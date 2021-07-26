/* Software floating-point emulation.
   Convert a 32bit signed integer to IEEE single
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
typedef int SItype;

#include "internals.h"
#include "platform.h"

posit32_t i32_to_p32(int32_t iA) {
  int_fast8_t k,
      log2 = 31;  // length of bit (e.g. 4294966271) in int (32 but because we
                  // have only 32 bits, so one bit off to accommodate that fact)
  union ui32_p32 uZ;
  uint_fast32_t uiA;
  uint_fast32_t expA, mask = 0x80000000, fracA;
  bool sign;

  if (iA < -2147483135) {  //-2147483648 to -2147483136 rounds to P32 value
                           //-2147483648
    uZ.ui = 0x80500000;
    return uZ.p;
  }

  sign = iA >> 31;
  if (sign) iA = -iA & 0xFFFFFFFF;

  if (iA > 2147483135)  // 2147483136 to 2147483647 rounds to P32 value
                        // (2147483648)=> 0x7FB00000
    uiA = 0x7FB00000;
  else if (iA < 0x2)
    uiA = (iA << 30);
  else {
    fracA = iA;
    while (!(fracA & mask)) {
      log2--;
      fracA <<= 1;
    }
    k = (log2 >> 2);
    expA = (log2 & 0x3) << (27 - k);
    fracA = (fracA ^ mask);
    uiA = (0x7FFFFFFF ^ (0x3FFFFFFF >> k)) | expA | fracA >> (k + 4);

    mask = 0x8 << k;  // bitNPlusOne

    if (mask & fracA)
      if (((mask - 1) & fracA) | ((mask << 1) & fracA)) uiA++;
  }
  (sign) ? (uZ.ui = -uiA & 0xFFFFFFFF) : (uZ.ui = uiA);
  return uZ.p;
}

SFtype __floatsisf(SItype i) {
  posit32_t a;
  a = i32_to_p32(i);
  return *(SFtype*)&a;
}
