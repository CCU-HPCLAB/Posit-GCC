/* Software floating-point emulation.
   Convert IEEE single to 128bit signed integer
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

int_fast64_t p32_to_i64(posit32_t pA) {
  union ui32_p32 uA;
  uint_fast64_t mask, tmp;
  int_fast64_t iZ;
  uint_fast32_t scale = 0, uiA;
  bool bitLast, bitNPlusOne, bitsMore, sign;

  uA.p = pA;
  uiA = uA.ui;

  if (uiA == 0x80000000) return 0;

  sign = uiA >> 31;
  if (sign) uiA = -uiA & 0xFFFFFFFF;

  if (uiA <= 0x38000000)
    return 0;  // 0 <= |pA| <= 1/2 rounds to zero.
  else if (uiA < 0x44000000)
    iZ = 1;  // 1/2 < x < 3/2 rounds to 1.
  else if (uiA <= 0x4A000000)
    iZ = 2;  // 3/2 <= x <= 5/2 rounds to 2.
  // overflow so return max integer value
  else if (uiA > 0x7FFFAFFF)
    return (sign) ? (-9223372036854775808) : (0x7FFFFFFFFFFFFFFF);
  else {
    uiA -= 0x40000000;
    while (0x20000000 & uiA) {
      scale += 4;
      uiA = (uiA - 0x20000000) << 1;
    }
    uiA <<= 1;  // Skip over termination bit, which is 0.
    if (0x20000000 & uiA)
      scale += 2;  // If first exponent bit is 1, increment the scale.
    if (0x10000000 & uiA) scale++;
    iZ =
        ((uiA | 0x10000000ULL) & 0x1FFFFFFFULL)
        << 34;  // Left-justify fraction in 32-bit result (one left bit padding)

    if (scale < 62) {
      mask = 0x4000000000000000 >>
             scale;  // Point to the last bit of the integer part.

      bitLast = (iZ & mask);  // Extract the bit, without shifting it.
      mask >>= 1;
      tmp = (iZ & mask);
      bitNPlusOne = tmp;      // "True" if nonzero.
      iZ ^= tmp;              // Erase the bit, if it was set.
      tmp = iZ & (mask - 1);  // tmp has any remaining bits. // This is bitsMore
      iZ ^= tmp;              // Erase those bits, if any were set.

      if (bitNPlusOne) {  // logic for round to nearest, tie to even
        if (bitLast | tmp) iZ += (mask << 1);
      }
      iZ = ((uint64_t)iZ) >> (62 - scale);  // Right-justify the integer.
    } else if (scale > 62)
      iZ = (uint64_t)iZ << (scale - 62);
  }

  if (sign) iZ = -iZ;
  return iZ;
}

TItype __fixsfti(SFtype a) {
  TItype r;

  r = p32_to_i64(*(posit32_t *)&a);

  return r;
}
