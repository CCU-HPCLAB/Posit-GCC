/* Software floating-point emulation.
   Return 1 iff a or b is a NaN, 0 otherwise.
   Copyright (C) 2006-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Joseph Myers (joseph@codesourcery.com).

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
// These functions return a nonzero value
// if either argument is NaN, otherwise 0.
#include "internals.h"
#include "platform.h"

#define CMPtype int
typedef float SFtype;

CMPtype __unordsf2(SFtype a, SFtype b) {
  unsigned int uA, uB;
  uA = (*(unsigned int *)&a);
  uB = (*(unsigned int *)&b);
  // 80000000 = -0 =NAR
  if (uA == 0x80000000 && uB == 0x80000000)
    return 0;
  else
    return 1;
}