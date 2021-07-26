/* Software floating-point emulation.
   Return 0 iff a == b, 1 iff a > b, 2 iff a ? b, -1 iff a < b
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
// checked
#include "internals.h"
#include "platform.h"

#define CMPtype int
typedef float SFtype;
// These functions return a value less than zero
// if neither argument is NaN, and a is strictly less than b.
int p32_lts(posit32_t a, posit32_t b) {
  union ui32_p32 uA, uB;
  int32_t uiA, uiB;

  uA.p = a;
  uiA = (int32_t)uA.ui;
  uB.p = b;
  uiB = (int32_t)uB.ui;

  if (uiA < uiB)
    return -1;

  else
    return 1;
}

CMPtype __ltsf2(SFtype a, SFtype b) {
  CMPtype r;
  r = p32_lts(*(posit32_t*)&a, *(posit32_t*)&b);
  return r;
}
