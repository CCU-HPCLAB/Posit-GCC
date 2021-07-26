/* Software floating-point emulation.
   Convert a 128bit signed integer to IEEE single
   Copyright (C) 2007-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Uros Bizjak (ubizjak@gmail.com).

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
typedef long long TItype;

#include "internals.h"
#include "platform.h"

posit32_t i64_to_p32(int64_t iA) {
  int_fast8_t k, log2 = 63;  // length of bit (e.g. 9222809086901354496) in int
                             // (64 but because we have only 64 bits, so one bit
                             // off to accomdate that fact)
  union ui32_p32 uZ;
  uint_fast64_t uiA;
  uint_fast64_t mask = 0x8000000000000000, fracA;
  uint_fast32_t expA;
  bool sign;

  if (iA <
      -9222809086901354495) {  //-9222809086901354496 to -9223372036854775808
                               // will be P32 value -9223372036854775808
    uZ.ui = 0x80005000;
    return uZ.p;
  }
  sign = iA >> 63;
  if (sign) iA = -iA;

  if (iA > 9222809086901354495)  // 9222809086901354495 bcos 9222809086901354496
                                 // to 9223372036854775807 will be P32 value
                                 // 9223372036854775808
    uiA = 0x7FFFB000;            // P32: 9223372036854775808
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

    uiA = (0x7FFFFFFF ^ (0x3FFFFFFF >> k)) | expA | fracA >> (k + 36);

    mask = 0x800000000 << k;  // bitNPlusOne

    if (mask & fracA) {
      if (((mask - 1) & fracA) | ((mask << 1) & fracA)) uiA++;
    }
  }
  (sign) ? (uZ.ui = -uiA) : (uZ.ui = uiA);
  return uZ.p;
}

SFtype __floattisf(TItype i) {
  posit32_t a;
  a = i64_to_p32(i);
  return *(SFtype *)&a;
}
