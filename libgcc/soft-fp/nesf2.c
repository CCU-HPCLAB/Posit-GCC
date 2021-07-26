#include "internals.h"
#include "platform.h"

#define CMPtype int
typedef float SFtype;
// These functions return a nonzero value if either
// argument is NaN, or if a and b are unequal.
int p32_ne(posit32_t a, posit32_t b) {
  union ui32_p32 uA, uB;
  int32_t uiA, uiB;

  uA.p = a;
  uiA = (int32_t)uA.ui;
  uB.p = b;
  uiB = (int32_t)uB.ui;

  if (uiA == uiB)
    return 0;
  else
    return 1;
}

CMPtype __nesf2(SFtype a, SFtype b) {
  CMPtype r;

  r = p32_ne(*(posit32_t*)&a, *(posit32_t*)&b);

  return r;
}