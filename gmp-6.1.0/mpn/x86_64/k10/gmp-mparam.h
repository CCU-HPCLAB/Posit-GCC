/* AMD K10 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000-2012, 2014, 2015 Free Software Foundation,
Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the GNU MP Library.  If not,
see https://www.gnu.org/licenses/.  */

#define GMP_LIMB_BITS 64
#define GMP_LIMB_BYTES 8

#if 0
#undef mpn_sublsh_n
#define mpn_sublsh_n(rp,up,vp,n,c)					\
  (((rp) == (up)) ? mpn_submul_1 (rp, vp, n, CNST_LIMB(1) << (c))	\
   : MPN(mpn_sublsh_n)(rp,up,vp,n,c))
#endif

/* 3200 MHz K10 Thuban */
/* FFT tuning limit = 200 M */
/* Generated by tuneup.c, 2015-10-07, gcc 4.8 */

#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          4
#define MOD_1U_TO_MOD_1_1_THRESHOLD          3
#define MOD_1_1_TO_MOD_1_2_THRESHOLD        17
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        28
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD      8
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIV_QR_1_NORM_THRESHOLD              1
#define DIV_QR_1_UNNORM_THRESHOLD        MP_SIZE_T_MAX  /* never */
#define DIV_QR_2_PI2_THRESHOLD           MP_SIZE_T_MAX  /* never */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           14

#define MUL_TOOM22_THRESHOLD                27
#define MUL_TOOM33_THRESHOLD                81
#define MUL_TOOM44_THRESHOLD               232
#define MUL_TOOM6H_THRESHOLD               369
#define MUL_TOOM8H_THRESHOLD               462

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      97
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     154
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     145
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     163
#define MUL_TOOM43_TO_TOOM54_THRESHOLD     203

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 34
#define SQR_TOOM3_THRESHOLD                117
#define SQR_TOOM4_THRESHOLD                348
#define SQR_TOOM6_THRESHOLD                552
#define SQR_TOOM8_THRESHOLD                  0  /* always */

#define MULMID_TOOM42_THRESHOLD             34

#define MULMOD_BNM1_THRESHOLD               15
#define SQRMOD_BNM1_THRESHOLD               17

#define MUL_FFT_MODF_THRESHOLD             565  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    565, 5}, {     23, 6}, {     12, 5}, {     25, 6}, \
    {     27, 7}, {     14, 6}, {     29, 7}, {     15, 6}, \
    {     31, 7}, {     25, 8}, {     13, 7}, {     28, 8}, \
    {     15, 7}, {     31, 8}, {     17, 7}, {     35, 8}, \
    {     19, 7}, {     39, 8}, {     21, 7}, {     43, 8}, \
    {     23, 7}, {     47, 8}, {     25, 7}, {     51, 8}, \
    {     29, 9}, {     15, 8}, {     35, 9}, {     19, 8}, \
    {     43, 9}, {     23, 8}, {     51, 9}, {     27, 8}, \
    {     55,10}, {     15, 9}, {     31, 8}, {     63, 9}, \
    {     43,10}, {     23, 9}, {     59,11}, {     15,10}, \
    {     31, 9}, {     67,10}, {     39, 9}, {     83,10}, \
    {     47, 9}, {     95,10}, {     55,11}, {     31,10}, \
    {     79,11}, {     47,10}, {    103,12}, {     31,11}, \
    {     63,10}, {    135,11}, {     79,10}, {    167,11}, \
    {     95,10}, {    191,11}, {    111,12}, {     63,11}, \
    {    143,10}, {    287,11}, {    159,12}, {     95,11}, \
    {    191,10}, {    383,11}, {    207,13}, {     63,12}, \
    {    127,11}, {    255,10}, {    511,11}, {    271,10}, \
    {    543,11}, {    287,12}, {    159,11}, {    319,10}, \
    {    639,11}, {    335,10}, {    671,11}, {    351,10}, \
    {    703,12}, {    191,11}, {    383,10}, {    767,11}, \
    {    415,12}, {    223,11}, {    447,13}, {    127,12}, \
    {    255,11}, {    543,12}, {    287,11}, {    607,12}, \
    {    319,11}, {    671,12}, {    351,11}, {    703,13}, \
    {    191,12}, {    383,11}, {    767,12}, {    415,11}, \
    {    831,12}, {    447,14}, {    127,13}, {    255,12}, \
    {    543,11}, {   1087,12}, {    607,11}, {   1215,13}, \
    {    319,12}, {    671,11}, {   1343,12}, {    735,13}, \
    {    383,12}, {    799,11}, {   1599,12}, {    831,13}, \
    {    447,12}, {    959,13}, {    511,12}, {   1087,13}, \
    {    575,12}, {   1215,13}, {    639,12}, {   1343,13}, \
    {    703,12}, {   1407,14}, {    383,13}, {    767,12}, \
    {   1599,13}, {    831,12}, {   1663,13}, {    895,12}, \
    {   1791,13}, {    959,14}, {    511,13}, {   1087,12}, \
    {   2175,13}, {   1215,14}, {    639,13}, {   1471,14}, \
    {    767,13}, {   1663,14}, {    895,13}, {   1791,15}, \
    {    511,14}, {   1023,13}, {   2175,14}, {   1151,13}, \
    {   2431,14}, {   1279,13}, {   2559,14}, {   1407,15}, \
    {    767,14}, {   1791,16}, {    511,15}, {   1023,14}, \
    {   2175,13}, {   4351,14}, {   2431,15}, {   1279,14}, \
    {   2687,13}, {   5375,14}, {   2815,15}, {   1535,14}, \
    {   3199,15}, {   1791,14}, {   3583,16}, {   1023,15}, \
    {   2047,14}, {   4351,15}, {   2303,14}, {   4991,15}, \
    {   2559,14}, {   5247,16}, {   1535,15}, {   3071,14}, \
    {   6399,15}, {   3327,14}, {   6911,15}, {   3583,17}, \
    {   1023,16}, {   2047,15}, {   4351,14}, {   8959,15}, \
    {   4863,16}, {   2559,15}, {   5375,14}, {  10751,15}, \
    {   5887,14}, {  11775,16}, {  65536,17}, { 131072,18}, \
    { 262144,19}, { 524288,20}, {1048576,21}, {2097152,22}, \
    {4194304,23}, {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 198
#define MUL_FFT_THRESHOLD                 7552

#define SQR_FFT_MODF_THRESHOLD             440  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    440, 5}, {     23, 6}, {     12, 5}, {     25, 6}, \
    {     27, 7}, {     14, 6}, {     29, 7}, {     15, 6}, \
    {     31, 7}, {     29, 8}, {     15, 7}, {     32, 8}, \
    {     17, 7}, {     35, 8}, {     19, 7}, {     39, 8}, \
    {     21, 7}, {     43, 8}, {     29, 9}, {     15, 8}, \
    {     35, 9}, {     19, 8}, {     43, 9}, {     23, 8}, \
    {     51, 9}, {     27, 8}, {     55,10}, {     15, 9}, \
    {     31, 8}, {     63, 9}, {     43,10}, {     23, 9}, \
    {     55,11}, {     15,10}, {     31, 9}, {     67,10}, \
    {     39, 9}, {     83,10}, {     47, 9}, {     95,10}, \
    {     55,11}, {     31,10}, {     79,11}, {     47,10}, \
    {    103,12}, {     31,11}, {     63,10}, {    135,11}, \
    {     79,10}, {    159,11}, {    111,12}, {     63,11}, \
    {    127,10}, {    255,11}, {    143,10}, {    287,11}, \
    {    159,12}, {     95,11}, {    191,10}, {    383, 9}, \
    {    767,10}, {    399,11}, {    207,13}, {     63,12}, \
    {    127,11}, {    255,10}, {    511,11}, {    271,10}, \
    {    543,11}, {    287,10}, {    575,12}, {    159,11}, \
    {    319,10}, {    639,11}, {    335,10}, {    671,11}, \
    {    351,10}, {    703,11}, {    367,12}, {    191,11}, \
    {    383,10}, {    767,11}, {    399,10}, {    799,11}, \
    {    415,12}, {    223,11}, {    447,13}, {    127,12}, \
    {    255,11}, {    543,12}, {    287,11}, {    607,12}, \
    {    319,11}, {    639,10}, {   1279,11}, {    671,12}, \
    {    351,11}, {    703,13}, {    191,12}, {    383,11}, \
    {    767,10}, {   1535,11}, {    799,12}, {    415,11}, \
    {    831,12}, {    447,14}, {    127,13}, {    255,12}, \
    {    511,11}, {   1023,12}, {    543,11}, {   1087,12}, \
    {    575,11}, {   1151,12}, {    607,13}, {    319,12}, \
    {    639,11}, {   1279,12}, {    671,11}, {   1343,12}, \
    {    735,13}, {    383,12}, {    831,13}, {    447,12}, \
    {    959,14}, {    255,13}, {    511,12}, {   1087,13}, \
    {    575,12}, {   1215,13}, {    639,12}, {   1343,13}, \
    {    703,12}, {   1407,14}, {    383,13}, {    767,12}, \
    {   1599,13}, {    831,12}, {   1663,13}, {    895,12}, \
    {   1791,13}, {    959,15}, {    255,14}, {    511,13}, \
    {   1087,12}, {   2175,13}, {   1215,14}, {    639,13}, \
    {   1407,14}, {    767,13}, {   1663,14}, {    895,13}, \
    {   1791,15}, {    511,14}, {   1023,13}, {   2175,14}, \
    {   1151,13}, {   2303,14}, {   1279,13}, {   2559,14}, \
    {   1407,15}, {    767,14}, {   1791,16}, {    511,15}, \
    {   1023,14}, {   2303,15}, {   1279,14}, {   2687,15}, \
    {   1535,14}, {   3199,15}, {   1791,16}, {   1023,15}, \
    {   2047,14}, {   4351,15}, {   2303,14}, {   4863,15}, \
    {   2559,14}, {   5247,16}, {   1535,15}, {   3071,14}, \
    {   6399,15}, {   3327,14}, {   6911,17}, {   1023,16}, \
    {   2047,15}, {   4351,14}, {   8959,15}, {   4863,16}, \
    {   2559,15}, {   5375,14}, {  10751,15}, {   5887,14}, \
    {  11775,16}, {  65536,17}, { 131072,18}, { 262144,19}, \
    { 524288,20}, {1048576,21}, {2097152,22}, {4194304,23}, \
    {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 201
#define SQR_FFT_THRESHOLD                 5568

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                  60
#define MULLO_MUL_N_THRESHOLD            14281
#define SQRLO_BASECASE_THRESHOLD             8
#define SQRLO_DC_THRESHOLD                   0  /* never mpn_sqrlo_basecase */
#define SQRLO_SQR_THRESHOLD              10950

#define DC_DIV_QR_THRESHOLD                 56
#define DC_DIVAPPR_Q_THRESHOLD             240
#define DC_BDIV_QR_THRESHOLD                54
#define DC_BDIV_Q_THRESHOLD                 44

#define INV_MULMOD_BNM1_THRESHOLD           54
#define INV_NEWTON_THRESHOLD               226
#define INV_APPR_THRESHOLD                 218

#define BINV_NEWTON_THRESHOLD              324
#define REDC_1_TO_REDC_2_THRESHOLD          25
#define REDC_2_TO_REDC_N_THRESHOLD          71

#define MU_DIV_QR_THRESHOLD               1718
#define MU_DIVAPPR_Q_THRESHOLD            1652
#define MUPI_DIV_QR_THRESHOLD              102
#define MU_BDIV_QR_THRESHOLD              1528
#define MU_BDIV_Q_THRESHOLD               1589

#define POWM_SEC_TABLE  1,22,194,579,1297

#define GET_STR_DC_THRESHOLD                19
#define GET_STR_PRECOMPUTE_THRESHOLD        27
#define SET_STR_DC_THRESHOLD               248
#define SET_STR_PRECOMPUTE_THRESHOLD      1616

#define FAC_DSC_THRESHOLD                  739
#define FAC_ODD_THRESHOLD                    0  /* always */

#define MATRIX22_STRASSEN_THRESHOLD         18
#define HGCD_THRESHOLD                     135
#define HGCD_APPR_THRESHOLD                 50
#define HGCD_REDUCE_THRESHOLD             3014
#define GCD_DC_THRESHOLD                   555
#define GCDEXT_DC_THRESHOLD                469
#define JACOBI_BASE_METHOD                   4
