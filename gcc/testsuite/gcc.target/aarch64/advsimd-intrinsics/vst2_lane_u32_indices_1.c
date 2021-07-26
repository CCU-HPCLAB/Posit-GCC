#include <arm_neon.h>

/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-fno-fat-lto-objects" } } */

void
f_vst2_lane_u32 (uint32_t * p, uint32x2x2_t v)
{
  /* { dg-error "lane 2 out of range 0 - 1" "" { target *-*-* } 0 } */
  vst2_lane_u32 (p, v, 2);
  /* { dg-error "lane -1 out of range 0 - 1" "" { target *-*-* } 0 } */
  vst2_lane_u32 (p, v, -1);
  return;
}
