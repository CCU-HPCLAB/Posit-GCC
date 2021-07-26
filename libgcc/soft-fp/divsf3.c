typedef float SFtype;

#include <stdlib.h>

#include "internals.h"
#include "platform.h"

#define INFINITY (__builtin_inff())
#define NAN (__builtin_nanf(""))

posit32_t p32_div(posit32_t pA, posit32_t pB) {
  union ui32_p32 uA, uB, uZ;
  uint_fast32_t uiA, uiB, fracA, fracB, regA, regime /*, regB*/, tmp;
  bool signA, signB, signZ, regSA, regSB, bitNPlusOne = 0, bitsMore = 0, rcarry;
  int_fast8_t kA = 0;
  int_fast32_t expA;
  uint_fast64_t frac64A, frac64Z, rem;
  lldiv_t divresult;

  uA.p = pA;
  uiA = uA.ui;
  uB.p = pB;
  uiB = uB.ui;

  // Zero or infinity
  if (uiA == 0x80000000 || uiB == 0x80000000 || uiB == 0) {
#ifdef SOFTPOSIT_EXACT
    uZ.ui.v = 0x80000000;
    uZ.ui.exact = 0;
#else
    uZ.ui = 0x80000000;
#endif
    return uZ.p;
  } else if (uiA == 0) {
#ifdef SOFTPOSIT_EXACT

    uZ.ui.v = 0;
    if ((uiA == 0 && uiA.ui.exact) || (uiB == 0 && uiB.ui.exact))
      uZ.ui.exact = 1;
    else
      uZ.ui.exact = 0;
#else
    uZ.ui = 0;
#endif
    return uZ.p;
  }

  signA = signP32UI(uiA);
  signB = signP32UI(uiB);
  signZ = signA ^ signB;
  if (signA) uiA = (-uiA & 0xFFFFFFFF);
  if (signB) uiB = (-uiB & 0xFFFFFFFF);
  regSA = signregP32UI(uiA);
  regSB = signregP32UI(uiB);

  tmp = (uiA << 2) & 0xFFFFFFFF;
  if (regSA) {
    while (tmp >> 31) {
      kA++;
      tmp = (tmp << 1) & 0xFFFFFFFF;
    }
  } else {
    kA = -1;
    while (!(tmp >> 31)) {
      kA--;
      tmp = (tmp << 1) & 0xFFFFFFFF;
    }
    tmp &= 0x7FFFFFFF;
  }
  expA = tmp >> 29;  // to get 2 bits
  fracA = ((tmp << 1) | 0x40000000) & 0x7FFFFFFF;
  frac64A = (uint64_t)fracA << 30;

  tmp = (uiB << 2) & 0xFFFFFFFF;
  if (regSB) {
    while (tmp >> 31) {
      kA--;
      tmp = (tmp << 1) & 0xFFFFFFFF;
    }
  } else {
    kA++;
    while (!(tmp >> 31)) {
      kA++;
      tmp = (tmp << 1) & 0xFFFFFFFF;
    }
    tmp &= 0x7FFFFFFF;
  }
  expA -= tmp >> 29;
  fracB = ((tmp << 1) | 0x40000000) & 0x7FFFFFFF;

  divresult = lldiv(frac64A, (uint_fast64_t)fracB);
  frac64Z = divresult.quot;
  rem = divresult.rem;

  if (expA < 0) {
    expA += 4;
    kA--;
  }
  if (frac64Z != 0) {
    rcarry =
        frac64Z >>
        30;  // this is the hidden bit (14th bit) , extreme right bit is bit 0
    if (!rcarry) {
      if (expA == 0) {
        kA--;
        expA = 3;
      } else
        expA--;
      frac64Z <<= 1;
    }
  }

  if (kA < 0) {
    regA = -kA;
    regSA = 0;
    regime = 0x40000000 >> regA;
  } else {
    regA = kA + 1;
    regSA = 1;
    regime = 0x7FFFFFFF - (0x7FFFFFFF >> regA);
  }
  if (regA > 30) {
    // max or min pos. exp and frac does not matter.
    (regSA) ? (uZ.ui = 0x7FFFFFFF) : (uZ.ui = 0x1);
  } else {
    // remove carry and rcarry bits and shift to correct position
    frac64Z &= 0x3FFFFFFF;

    fracA = (uint_fast32_t)frac64Z >> (regA + 2);

    if (regA <= 28) {
      bitNPlusOne = (frac64Z >> (regA + 1)) & 0x1;
      expA <<= (28 - regA);
      if (bitNPlusOne)
        (((1 << (regA + 1)) - 1) & frac64Z) ? (bitsMore = 1) : (bitsMore = 0);
    } else {
      if (regA == 30) {
        bitNPlusOne = expA & 0x2;
        bitsMore = (expA & 0x1);
        expA = 0;
      } else if (regA == 29) {
        bitNPlusOne = expA & 0x1;
        expA >>= 1;  // taken care of by the pack algo
      }
      if (frac64Z > 0) {
        fracA = 0;
        bitsMore = 1;
      }
    }
    if (rem) bitsMore = 1;

    uZ.ui = packToP32UI(regime, expA, fracA);
    if (bitNPlusOne) uZ.ui += (uZ.ui & 1) | bitsMore;
  }

  if (signZ) uZ.ui = -uZ.ui & 0xFFFFFFFF;
  return uZ.p;
}

SFtype __divsf3(SFtype a, SFtype b) {
  posit32_t r;

  r = p32_div(*(posit32_t*)&a, *(posit32_t*)&b);

  return *(SFtype*)&r;
}
