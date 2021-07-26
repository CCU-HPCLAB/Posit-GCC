#include <stdio.h>
#include <stdlib.h>

#include "internals.h"
#include "platform.h"

#define INFINITY (__builtin_inff())
#define NAN (__builtin_nanf(""))
#define castUI(a) ((a).v)

// transform P32 -> Dec && Dec -> P32
#define castQ32(l0, l1, l2, l3, l4, l5, l6, l7) \
  ({                                            \
    union ui512_q32 uA;                         \
    uA.ui[0] = l0;                              \
    uA.ui[1] = l1;                              \
    uA.ui[2] = l2;                              \
    uA.ui[3] = l3;                              \
    uA.ui[4] = l4;                              \
    uA.ui[5] = l5;                              \
    uA.ui[6] = l6;                              \
    uA.ui[7] = l7;                              \
    uA.q;                                       \
  })

void checkExtraP32TwoBits(double f32, double temp, bool* bitsNPlusOne,
                          bool* bitsMore) {
  temp /= 2;
  if (temp <= f32) {
    *bitsNPlusOne = 1;
    f32 -= temp;
  }
  if (f32 > 0) *bitsMore = 1;
}
uint_fast32_t convertFractionP32(double f32, uint_fast16_t fracLength,
                                 bool* bitsNPlusOne, bool* bitsMore) {
  uint_fast32_t frac = 0;

  if (f32 == 0)
    return 0;
  else if (f32 == INFINITY)
    return 0x80000000;

  f32 -= 1;  // remove hidden bit
  if (fracLength == 0)
    checkExtraP32TwoBits(f32, 1.0, bitsNPlusOne, bitsMore);
  else {
    double temp = 1;
    while (true) {
      temp /= 2;
      if (temp <= f32) {
        f32 -= temp;
        fracLength--;
        frac = (frac << 1) + 1;  // shift in one
        if (f32 == 0) {
          frac <<= (uint_fast16_t)fracLength;
          break;
        }

        if (fracLength == 0) {
          checkExtraP32TwoBits(f32, temp, bitsNPlusOne, bitsMore);
          break;
        }
      } else {
        frac <<= 1;  // shift in a zero
        fracLength--;
        if (fracLength == 0) {
          checkExtraP32TwoBits(f32, temp, bitsNPlusOne, bitsMore);
          break;
        }
      }
    }
  }

  return frac;
}

posit32_t convertDoubleToP32(double f32) {
  union ui32_p32 uZ;
  bool sign, regS;
  uint_fast32_t reg, frac = 0;
  int_fast32_t exp = 0;
  bool bitNPlusOne = 0, bitsMore = 0;

  (f32 >= 0) ? (sign = 0) : (sign = 1);

  if (f32 == 0) {
    uZ.ui = 0;
    return uZ.p;
  } else if (f32 == INFINITY || f32 == -INFINITY || f32 == NAN) {
    uZ.ui = 0x80000000;
    return uZ.p;
  } else if (f32 == 1) {
    uZ.ui = 0x40000000;
    return uZ.p;
  } else if (f32 == -1) {
    uZ.ui = 0xC0000000;
    return uZ.p;
  } else if (f32 >= 1.329227995784916e+36) {
    // maxpos
    uZ.ui = 0x7FFFFFFF;
    return uZ.p;
  } else if (f32 <= -1.329227995784916e+36) {
    // -maxpos
    uZ.ui = 0x80000001;
    return uZ.p;
  } else if (f32 <= 7.52316384526264e-37 && !sign) {
    // minpos
    uZ.ui = 0x1;
    return uZ.p;
  } else if (f32 >= -7.52316384526264e-37 && sign) {
    //-minpos
    uZ.ui = 0xFFFFFFFF;
    return uZ.p;
  } else if (f32 > 1 || f32 < -1) {
    if (sign) {
      // Make negative numbers positive for easier computation
      f32 = -f32;
    }

    regS = 1;
    reg = 1;  // because k = m-1; so need to add back 1
    // minpos
    if (f32 <= 7.52316384526264e-37) {
      uZ.ui = 1;
    } else {
      // regime
      while (f32 >= 16) {
        f32 *= 0.0625;  // f32/=16;
        reg++;
      }
      while (f32 >= 2) {
        f32 *= 0.5;
        exp++;
      }

      int8_t fracLength = 28 - reg;

      if (fracLength < 0) {
        // in both cases, reg=29 and 30, e is n+1 bit and frac are sticky bits
        if (reg == 29) {
          bitNPlusOne = exp & 0x1;
          exp >>= 1;  // taken care of by the pack algo
        } else {      // reg=30
          bitNPlusOne = exp >> 1;
          bitsMore = exp & 0x1;
          exp = 0;
        }
        if (f32 != 1) {  // because of hidden bit
          bitsMore = 1;
          frac = 0;
        }
      } else
        frac = convertFractionP32(f32, fracLength, &bitNPlusOne, &bitsMore);

      if (reg > 30) {
        (regS) ? (uZ.ui = 0x7FFFFFFF) : (uZ.ui = 0x1);
      }
      // rounding off fraction bits
      else {
        uint_fast32_t regime = 1;
        if (regS) regime = ((1 << reg) - 1) << 1;
        if (reg <= 28) exp <<= (28 - reg);
        uZ.ui = ((uint32_t)(regime) << (30 - reg)) + ((uint32_t)exp) +
                ((uint32_t)(frac));
        uZ.ui += (bitNPlusOne & (uZ.ui & 1)) | (bitNPlusOne & bitsMore);
      }
      if (sign) uZ.ui = -uZ.ui & 0xFFFFFFFF;
    }
  } else if (f32 < 1 || f32 > -1) {
    if (sign) {
      // Make negative numbers positive for easier computation
      f32 = -f32;
    }
    regS = 0;
    reg = 0;

    // regime
    while (f32 < 1) {
      f32 *= 16;
      reg++;
    }

    while (f32 >= 2) {
      f32 *= 0.5;
      exp++;
    }

    // only possible combination for reg=15 to reach here is 7FFF (maxpos) and
    // FFFF (-minpos) but since it should be caught on top, so no need to handle
    int_fast8_t fracLength = 28 - reg;
    if (fracLength < 0) {
      // in both cases, reg=29 and 30, e is n+1 bit and frac are sticky bits
      if (reg == 29) {
        bitNPlusOne = exp & 0x1;
        exp >>= 1;  // taken care of by the pack algo
      } else {      // reg=30
        bitNPlusOne = exp >> 1;
        bitsMore = exp & 0x1;
        exp = 0;
      }
      if (f32 != 1) {  // because of hidden bit
        bitsMore = 1;
        frac = 0;
      }
    } else
      frac = convertFractionP32(f32, fracLength, &bitNPlusOne, &bitsMore);

    if (reg > 30) {
      (regS) ? (uZ.ui = 0x7FFFFFFF) : (uZ.ui = 0x1);
    }
    // rounding off fraction bits
    else {
      uint_fast32_t regime = 1;
      if (regS) regime = ((1 << reg) - 1) << 1;
      if (reg <= 28) exp <<= (28 - reg);
      uZ.ui = ((uint32_t)(regime) << (30 - reg)) + ((uint32_t)exp) +
              ((uint32_t)(frac));
      uZ.ui += (bitNPlusOne & (uZ.ui & 1)) | (bitNPlusOne & bitsMore);
    }
    if (sign) uZ.ui = -uZ.ui & 0xFFFFFFFF;

  } else {
    // NaR - for NaN, INF and all other combinations
    uZ.ui = 0x80000000;
  }
  return uZ.p;
}

posit32_t convertFloatToP32(float a) { return convertDoubleToP32((double)a); }

double convertP32ToDouble(posit32_t pA) {
  union ui32_p32 uA;
  union ui64_double uZ;
  uint_fast32_t uiA, tmp = 0;
  uint_fast64_t expA = 0, uiZ, fracA = 0;
  bool signA = 0, regSA;
  int_fast32_t scale, kA = 0;

  uA.p = pA;
  uiA = uA.ui;

  if (uA.ui == 0)
    return 0;
  else if (uA.ui == 0x80000000)
    return NAN;

  else {
    signA = signP32UI(uiA);
    if (signA) uiA = (-uiA & 0xFFFFFFFF);
    regSA = signregP32UI(uiA);
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

    fracA = (((uint64_t)tmp << 3) & 0xFFFFFFFF) << 20;

    expA = (((kA << 2) + expA) + 1023) << 52;
    uiZ = expA + fracA + (((uint64_t)signA & 0x1) << 63);

    uZ.ui = uiZ;
    return uZ.d;
  }
}

double convertPX2ToDouble(posit_2_t a) {
  union ui32_pX2 uZ;
  double d32;
  uZ.p = a;

  if (uZ.ui == 0) {
    return 0;
  } else if (uZ.ui == 0x7FFFFFFF) {  // maxpos
    return 1.329227995784916e+36;
  } else if (uZ.ui == 0x80000001) {  //-maxpos
    return -1.329227995784916e+36;
  } else if (uZ.ui == 0x80000000) {
    return INFINITY;
  }

  bool regS, sign;
  uint_fast32_t reg, shift = 2, frac, tmp;
  int_fast32_t k = 0;
  int_fast8_t exp;
  double fraction_max;

  sign = signP32UI(uZ.ui);
  if (sign) uZ.ui = -uZ.ui & 0xFFFFFFFF;
  regS = signregP32UI(uZ.ui);

  tmp = (uZ.ui << 2) & 0xFFFFFFFF;
  if (regS) {
    while (tmp >> 31) {
      k++;
      shift++;
      tmp = (tmp << 1) & 0xFFFFFFFF;
    }
    reg = k + 1;
  } else {
    k = -1;
    while (!(tmp >> 31)) {
      k--;
      shift++;
      tmp = (tmp << 1) & 0xFFFFFFFF;
    }
    tmp &= 0x7FFFFFFF;
    reg = -k;
  }
  exp = tmp >> 29;

  frac = (tmp & 0x1FFFFFFF) >> shift;

  (reg > 28) ? (fraction_max = 1) : (fraction_max = pow(2, 28 - reg));

  d32 =
      (double)(pow(16, k) * pow(2, exp) * (1 + ((double)frac / fraction_max)));
  if (sign) d32 = -d32;

  return d32;
}

// P32 operations
posit32_t p32_add(posit32_t a, posit32_t b) {
  union ui32_p32 uA, uB, uZ;
  uint_fast32_t uiA, uiB;

  uA.p = a;
  uiA = uA.ui;
  uB.p = b;
  uiB = uB.ui;

#ifdef SOFTPOSIT_EXACT
  uZ.ui.exact = (uiA.ui.exact & uiB.ui.exact);
#endif

  // Zero or infinity
  if (uiA == 0 || uiB == 0) {  // Not required but put here for speed
#ifdef SOFTPOSIT_EXACT
    uZ.ui.v = uiA | uiB;
    uZ.ui.exact = (uiA.ui.exact & uiB.ui.exact);
#else
    uZ.ui = uiA | uiB;
#endif
    return uZ.p;
  } else if (uiA == 0x80000000 || uiB == 0x80000000) {
    // printf("in infinity\n");
#ifdef SOFTPOSIT_EXACT
    uZ.ui.v = 0x80000000;
    uZ.ui.exact = 0;
#else
    uZ.ui = 0x80000000;
#endif
    return uZ.p;
  }

  // different signs
  if ((uiA ^ uiB) >> 31)
    return softposit_subMagsP32(uiA, uiB);
  else
    return softposit_addMagsP32(uiA, uiB);
}

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

posit32_t p32_mul(posit32_t pA, posit32_t pB) {
  union ui32_p32 uA, uB, uZ;
  uint_fast32_t uiA, uiB;
  uint_fast32_t regA, fracA, regime, tmp;
  bool signA, signB, signZ, regSA, regSB, bitNPlusOne = 0, bitsMore = 0, rcarry;
  int_fast32_t expA;
  int_fast8_t kA = 0;
  uint_fast64_t frac64Z;

  uA.p = pA;
  uiA = uA.ui;
  uB.p = pB;
  uiB = uB.ui;

#ifdef SOFTPOSIT_EXACT
  uZ.ui.exact = (uiA.ui.exact & uiB.ui.exact);
#endif
  // NaR or Zero
  if (uiA == 0x80000000 || uiB == 0x80000000) {
#ifdef SOFTPOSIT_EXACT
    uZ.ui.v = 0x80000000;
    uZ.ui.exact = 0;
#else
    uZ.ui = 0x80000000;
#endif
    return uZ.p;
  } else if (uiA == 0 || uiB == 0) {
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

  tmp = (uiB << 2) & 0xFFFFFFFF;
  if (regSB) {
    while (tmp >> 31) {
      kA++;
      tmp = (tmp << 1) & 0xFFFFFFFF;
    }
  } else {
    kA--;
    while (!(tmp >> 31)) {
      kA--;
      tmp = (tmp << 1) & 0xFFFFFFFF;
    }
    tmp &= 0x7FFFFFFF;
  }
  expA += tmp >> 29;
  frac64Z = (uint_fast64_t)fracA * (((tmp << 1) | 0x40000000) & 0x7FFFFFFF);

  if (expA > 3) {
    kA++;
    expA &= 0x3;  // -=4
  }

  rcarry = frac64Z >> 61;  // 3rd bit of frac64Z
  if (rcarry) {
    expA++;
    if (expA > 3) {
      kA++;
      expA &= 0x3;
    }
    frac64Z >>= 1;
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
    // remove carry and rcarry bits and shift to correct position (2 bits exp,
    // so + 1 than 16 bits)
    frac64Z = (frac64Z & 0xFFFFFFFFFFFFFFF) >> regA;
    fracA = (uint_fast32_t)(frac64Z >> 32);

    if (regA <= 28) {
      bitNPlusOne |= (0x80000000 & frac64Z);
      expA <<= (28 - regA);
    } else {
      if (regA == 30) {
        bitNPlusOne = expA & 0x2;
        bitsMore = (expA & 0x1);
        expA = 0;
      } else if (regA == 29) {
        bitNPlusOne = expA & 0x1;
        expA >>= 1;  // taken care of by the pack algo
      }
      if (fracA > 0) {
        fracA = 0;
        bitsMore = 1;
      }
    }
    // sign is always zero
    uZ.ui = packToP32UI(regime, expA, fracA);
    // n+1 frac bit is 1. Need to check if another bit is 1 too if not round to
    // even
    if (bitNPlusOne) {
      (0x7FFFFFFF & frac64Z) ? (bitsMore = 1) : (bitsMore = 0);
      uZ.ui += (uZ.ui & 1) | bitsMore;
    }
  }

  if (signZ) uZ.ui = -uZ.ui & 0xFFFFFFFF;
  return uZ.p;
}

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

posit32_t p32_sqrt(posit32_t pA) {
  union ui32_p32 uA;
  uint_fast32_t index, r0, shift, fracA, expZ, expA;
  uint_fast32_t mask, uiA, uiZ;
  uint_fast64_t eSqrR0, fracZ, negRem, recipSqrt, shiftedFracZ, sigma0,
      sqrSigma0;
  int_fast32_t eps, shiftZ;

  uA.p = pA;
  uiA = uA.ui;

  // If NaR or a negative number, return NaR.
  if (uiA & 0x80000000) {
    uA.ui = 0x80000000;
    return uA.p;
  }
  // If the argument is zero, return zero.
  else if (!uiA) {
    return uA.p;
  }
  // Compute the square root; shiftZ is the power-of-2 scaling of the result.
  // Decode regime and exponent; scale the input to be in the range 1 to 4:
  if (uiA & 0x40000000) {
    shiftZ = -2;
    while (uiA & 0x40000000) {
      shiftZ += 2;
      uiA = (uiA << 1) & 0xFFFFFFFF;
    }
  } else {
    shiftZ = 0;
    while (!(uiA & 0x40000000)) {
      shiftZ -= 2;
      uiA = (uiA << 1) & 0xFFFFFFFF;
    }
  }

  uiA &= 0x3FFFFFFF;
  expA = (uiA >> 28);
  shiftZ += (expA >> 1);
  expA = (0x1 ^ (expA & 0x1));
  uiA &= 0x0FFFFFFF;
  fracA = (uiA | 0x10000000);

  // Use table look-up of first 4 bits for piecewise linear approx. of 1/sqrt:
  index = ((fracA >> 24) & 0xE) + expA;
  eps = ((fracA >> 9) & 0xFFFF);
  r0 = softposit_approxRecipSqrt0[index] -
       (((uint_fast32_t)softposit_approxRecipSqrt1[index] * eps) >> 20);

  // Use Newton-Raphson refinement to get 33 bits of accuracy for 1/sqrt:
  eSqrR0 = (uint_fast64_t)r0 * r0;
  if (!expA) eSqrR0 <<= 1;
  sigma0 = 0xFFFFFFFF & (0xFFFFFFFF ^ ((eSqrR0 * (uint64_t)fracA) >> 20));
  recipSqrt = ((uint_fast64_t)r0 << 20) + (((uint_fast64_t)r0 * sigma0) >> 21);

  sqrSigma0 = ((sigma0 * sigma0) >> 35);
  recipSqrt += (((recipSqrt + (recipSqrt >> 2) - ((uint_fast64_t)r0 << 19)) *
                 sqrSigma0) >>
                46);

  fracZ = (((uint_fast64_t)fracA) * recipSqrt) >> 31;
  if (expA) fracZ = (fracZ >> 1);

  // Find the exponent of Z and encode the regime bits.
  expZ = shiftZ & 0x3;
  if (shiftZ < 0) {
    shift = (-1 - shiftZ) >> 2;
    uiZ = 0x20000000 >> shift;
  } else {
    shift = shiftZ >> 2;
    uiZ = 0x7FFFFFFF - (0x3FFFFFFF >> shift);
  }

  // Trick for eliminating off-by-one cases that only uses one multiply:
  fracZ++;
  if (!(fracZ & 0xF)) {
    shiftedFracZ = fracZ >> 1;
    negRem = (shiftedFracZ * shiftedFracZ) & 0x1FFFFFFFF;
    if (negRem & 0x100000000) {
      fracZ |= 1;
    } else {
      if (negRem) fracZ--;
    }
  }
  // Strip off the hidden bit and round-to-nearest using last shift+5 bits.
  fracZ &= 0xFFFFFFFF;
  mask = (1 << (4 + shift));
  if (mask & fracZ) {
    if (((mask - 1) & fracZ) | ((mask << 1) & fracZ)) fracZ += (mask << 1);
  }
  // Assemble the result and return it.
  uA.ui = uiZ | (expZ << (27 - shift)) | (fracZ >> (5 + shift));
  return uA.p;
}

posit32_t softposit_subMagsP32(uint_fast32_t uiA, uint_fast32_t uiB) {
  uint_fast16_t regA /*, regB*/;
  uint_fast64_t frac64A = 0, frac64B = 0;
  uint_fast32_t fracA = 0, regime, tmp;
  bool sign, regSA, regSB, ecarry = 0, bitNPlusOne = 0, bitsMore = 0;
  int_fast8_t kA = 0;
  int_fast32_t expA = 0;
  int_fast16_t shiftRight;
  union ui32_p32 uZ;

  sign = signP32UI(uiA);
  if (sign)
    uiA = -uiA & 0xFFFFFFFF;
  else
    uiB = -uiB & 0xFFFFFFFF;

  if (uiA == uiB) {  // essential, if not need special handling
    uZ.ui = 0;
    return uZ.p;
  }
  if ((int_fast32_t)uiA < (int_fast32_t)uiB) {
    uiA ^= uiB;
    uiB ^= uiA;
    uiA ^= uiB;
    (sign) ? (sign = 0) : (sign = 1);  // A becomes B
  }
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
  frac64A = ((0x40000000ULL | tmp << 1) & 0x7FFFFFFFULL) << 32;
  shiftRight = kA;

  tmp = (uiB << 2) & 0xFFFFFFFF;
  if (regSB) {
    while (tmp >> 31) {
      shiftRight--;
      tmp = (tmp << 1) & 0xFFFFFFFF;
    }

  } else {
    shiftRight++;
    while (!(tmp >> 31)) {
      shiftRight++;
      tmp = (tmp << 1) & 0xFFFFFFFF;
    }
    tmp &= 0x7FFFFFFF;
  }
  frac64B = ((0x40000000ULL | tmp << 1) & 0x7FFFFFFFULL) << 32;

  // This is 4kZ + expZ; (where kZ=kA-kB and expZ=expA-expB)
  shiftRight = (shiftRight << 2) + expA - (tmp >> 29);
  if (shiftRight > 63) {
    uZ.ui = uiA;
    if (sign) uZ.ui = -uZ.ui & 0xFFFFFFFF;
    return uZ.p;
  } else
    (frac64B >>= shiftRight);

  frac64A -= frac64B;

  while ((frac64A >> 59) == 0) {
    kA--;
    frac64A <<= 4;
  }
  ecarry =
      (0x4000000000000000 & frac64A);  //(0x4000000000000000 & frac64A)>>62;
  while (!ecarry) {
    if (expA == 0) {
      kA--;
      expA = 3;
    } else
      expA--;
    frac64A <<= 1;
    ecarry = (0x4000000000000000 & frac64A);
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
    // remove hidden bits
    frac64A = (frac64A & 0x3FFFFFFFFFFFFFFF) >> (regA + 2);  // 2 bits exp

    fracA = frac64A >> 32;

    if (regA <= 28) {
      bitNPlusOne |= (0x80000000 & frac64A);
      expA <<= (28 - regA);
    } else {
      if (regA == 30) {
        bitNPlusOne = expA & 0x2;
        bitsMore = (expA & 0x1);
        expA = 0;
      } else if (regA == 29) {
        bitNPlusOne = expA & 0x1;
        expA >>= 1;
      }
      if (fracA > 0) {
        fracA = 0;
        bitsMore = 1;
      }
    }

    uZ.ui = packToP32UI(regime, expA, fracA);
    // n+1 frac bit is 1. Need to check if another bit is 1 too if not round to
    // even
    if (bitNPlusOne) {
      (0x7FFFFFFF & frac64A) ? (bitsMore = 1) : (bitsMore = 0);
      uZ.ui += (uZ.ui & 1) | bitsMore;
    }
  }
  if (sign) uZ.ui = -uZ.ui & 0xFFFFFFFF;
  return uZ.p;
}

posit32_t softposit_addMagsP32(uint_fast32_t uiA, uint_fast32_t uiB) {
  uint_fast16_t regA /*, regB*/;
  uint_fast64_t frac64A = 0, frac64B = 0;
  uint_fast32_t fracA = 0, regime, tmp;
  bool sign, regSA, regSB, rcarry = 0, bitNPlusOne = 0, bitsMore = 0;
  int_fast8_t kA = 0;
  int_fast32_t expA;
  int_fast16_t shiftRight;
  union ui32_p32 uZ;

  sign = signP32UI(uiA);
  if (sign) {
    uiA = -uiA & 0xFFFFFFFF;
    uiB = -uiB & 0xFFFFFFFF;
  }

  if ((int_fast32_t)uiA < (int_fast32_t)uiB) {
    uiA ^= uiB;
    uiB ^= uiA;
    uiA ^= uiB;
  }
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
  frac64A = ((0x40000000ULL | tmp << 1) & 0x7FFFFFFFULL) << 32;
  shiftRight = kA;

  tmp = (uiB << 2) & 0xFFFFFFFF;
  if (regSB) {
    while (tmp >> 31) {
      shiftRight--;
      tmp = (tmp << 1) & 0xFFFFFFFF;
    }
  } else {
    shiftRight++;
    while (!(tmp >> 31)) {
      shiftRight++;
      tmp = (tmp << 1) & 0xFFFFFFFF;
    }
    tmp &= 0x7FFFFFFF;
  }
  frac64B = ((0x40000000ULL | tmp << 1) & 0x7FFFFFFFULL) << 32;
  // This is 4kZ + expZ; (where kZ=kA-kB and expZ=expA-expB)
  shiftRight = (shiftRight << 2) + expA - (tmp >> 29);

  // Manage CLANG (LLVM) compiler when shifting right more than number of bits
  (shiftRight > 63) ? (frac64B = 0)
                    : (frac64B >>= shiftRight);  // frac64B >>= shiftRight

  frac64A += frac64B;

  rcarry = 0x8000000000000000 & frac64A;  // first left bit
  if (rcarry) {
    expA++;
    if (expA > 3) {
      kA++;
      expA &= 0x3;
    }
    frac64A >>= 1;
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
    // remove hidden bits
    frac64A = (frac64A & 0x3FFFFFFFFFFFFFFF) >> (regA + 2);  // 2 bits exp

    fracA = frac64A >> 32;

    if (regA <= 28) {
      bitNPlusOne |= (0x80000000 & frac64A);
      expA <<= (28 - regA);
    } else {
      if (regA == 30) {
        bitNPlusOne = expA & 0x2;
        bitsMore = (expA & 0x1);
        expA = 0;
      } else if (regA == 29) {
        bitNPlusOne = expA & 0x1;
        expA >>= 1;
      }
      if (fracA > 0) {
        fracA = 0;
        bitsMore = 1;
      }
    }

    uZ.ui = packToP32UI(regime, expA, fracA);
    // n+1 frac bit is 1. Need to check if another bit is 1 too if not round to
    // even
    if (bitNPlusOne) {
      (0x7FFFFFFF & frac64A) ? (bitsMore = 1) : (bitsMore = 0);
      uZ.ui += (uZ.ui & 1) | bitsMore;
    }
  }
  if (sign) uZ.ui = -uZ.ui & 0xFFFFFFFF;
  return uZ.p;
}

double func1(double t, int n) {
  double answer = 1;
  for (int i = 0; i < n; i++) answer = answer * t;
  return answer;
}

double func2(double b, int n) {
  double answer = 1;
  for (int i = 1; i <= n; i++) answer = answer * (b - i + 1) / i;
  return answer;
}

double pow(double a, double b) {
  if (a == 0 && b > 0) {
    return 0;
  } else if (a == 0 && b <= 0) {
    return 1 / 0;
  } else if (a < 0 && !(b - (int)b < 0.0001 || (b - (int)b > 0.999))) {
    return 1 / 0;
  }

  if (a <= 2 && a >= 0) {
    double t = a - 1;
    double answer = 1;
    for (int i = 1; i < 100; i++) {
      answer = answer + func1(t, i) * func2(b, i);
    }
    return answer;
  }

  else if (a > 2) {
    int time = 0;

    while (a > 2) {
      a = a / 2;
      time++;
    }

    return pow(a, b) * pow(2, b * time);
  }

  else {
    if ((int)b % 2 == 0) {
      return pow(-a, b);
    } else {
      return -pow(-a, b);
    }
  }
}

void printBinary(uint64_t* s, int size) {
  int i;
  uint64_t number = *s;
  int bitSize = size - 1;
  for (i = 0; i < size; ++i) {
    if (i % 8 == 0) putchar(' ');
    printf("%llu", (number >> (bitSize - i)) & 1);
  }
  printf("\n");
}

// other operations

const uint_fast16_t softposit_approxRecipSqrt0[16] = {
    0xb4c9, 0xffab, 0xaa7d, 0xf11c, 0xa1c5, 0xe4c7, 0x9a43, 0xda29,
    0x93b5, 0xd0e5, 0x8ded, 0xc8b7, 0x88c6, 0xc16d, 0x8424, 0xbae1};

const uint_fast16_t softposit_approxRecipSqrt1[16] = {
    0xa5a5, 0xea42, 0x8c21, 0xc62d, 0x788f, 0xaa7f, 0x6928, 0x94b6,
    0x5cc7, 0x8335, 0x52a6, 0x74e2, 0x4a3e, 0x68fe, 0x432b, 0x5efd};
