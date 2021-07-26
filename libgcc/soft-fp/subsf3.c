#include "internals.h"
#include "platform.h"
typedef float SFtype;

#define INFINITY (__builtin_inff())
#define NAN (__builtin_nanf(""))
posit32_t p32_sub(posit32_t a, posit32_t b) {
  union ui32_p32 uA, uB, uZ;
  uint_fast32_t uiA, uiB;

  uA.p = a;
  uiA = uA.ui;
  uB.p = b;
  uiB = uB.ui;

#ifdef SOFTPOSIT_EXACT
  uZ.ui.exact = (uiA.ui.exact & uiB.ui.exact);
#endif

  // infinity
  if (uiA == 0x80000000 || uiB == 0x80000000) {
#ifdef SOFTPOSIT_EXACT
    uZ.ui.v = 0x80000000;
    uZ.ui.exact = 0;
#else
    uZ.ui = 0x80000000;
#endif
    return uZ.p;
  }
  // Zero
  else if (uiA == 0 || uiB == 0) {
#ifdef SOFTPOSIT_EXACT
    uZ.ui.v = (uiA | -uiB);
    uZ.ui.exact = 0;
#else
    uZ.ui = (uiA | -uiB);
#endif
    return uZ.p;
  }

  // different signs
  if ((uiA ^ uiB) >> 31)
    return softposit_addMagsP32(uiA, (-uiB & 0xFFFFFFFF));
  else
    return softposit_subMagsP32(uiA, (-uiB & 0xFFFFFFFF));
}

SFtype __subsf3(SFtype a, SFtype b) {
  posit32_t r;

  r = p32_sub(*(posit32_t*)&a, *(posit32_t*)&b);

  return *(SFtype*)&r;
}