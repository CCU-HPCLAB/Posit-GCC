#include "internals.h"
#include "platform.h"

#define CMPtype int
typedef float SFtype;
// These functions calculate a <=> b. That is,
// if a is less than b, they return -1;
// if a is greater than b, they return 1;
// and if a and b are equal they return 0.
// If either argument is NaN they return 1,
// but you should not rely on this;
// if NaN is a possibility, use one of the higher-level
// comparison functions.
int p32_cmp(posit32_t a, posit32_t b) {
  union ui32_p32 uA, uB;
  int32_t uiA, uiB;

  uA.p = a;
  uiA = (int32_t)uA.ui;
  uB.p = b;
  uiB = (int32_t)uB.ui;

  if (uiA == uiB)
    return 0;
  else if (uiA > uiB)
    return 1;
  else
    return -1;
}

CMPtype __cmpsf2(SFtype a, SFtype b) {
  CMPtype r;

  r = p32_cmp(*(posit32_t*)&a, *(posit32_t*)&b);

  return r;
}