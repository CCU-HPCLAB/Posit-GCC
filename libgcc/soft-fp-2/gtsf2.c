#include "internals.h"
#include "platform.h"

#define CMPtype int
typedef float SFtype;

// These functions return a value greater than zero
// if neither argument is NaN,
// and a is strictly greater than b.
int p32_gts(posit32_t a, posit32_t b) {
  union ui32_p32 uA, uB;
  int32_t uiA, uiB;

  uA.p = a;
  uiA = (int32_t)uA.ui;
  uB.p = b;
  uiB = (int32_t)uB.ui;

  if (uiA > uiB)
    return 1;
  else
    return 0;
}

CMPtype __gtsf2(SFtype a, SFtype b) {
  int r;

  r = p32_gts(*(posit32_t*)&a, *(posit32_t*)&b);

  return r;
}