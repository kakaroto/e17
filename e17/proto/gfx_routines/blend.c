#define X_DISPLAY_MISSING 1
#include <Imlib2.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include "mmx.h"
#include "pixel_op.h"

/* convert this to whatever your favorite "universal safe typing" scheme is */
#ifndef DATA32
typedef unsigned int    DATA32;
typedef unsigned short  DATA16;
typedef unsigned char   DATA8;
#endif

/* this is little endian - for big endian, reverse the order of 0, 1, 2, 3 4 */
#define A_VAL(p) ((DATA8 *)(p))[3]
#define R_VAL(p) ((DATA8 *)(p))[2]
#define G_VAL(p) ((DATA8 *)(p))[1]
#define B_VAL(p) ((DATA8 *)(p))[0]

/* saves typing a lot in setting up a new function */
#define FN_ARGB32_START(name) \
void \
name(DATA32 *src, DATA32 *dst, int len) \
{ \
   DATA32 *src_ptr, *dst_ptr, *dst_end_ptr; \
   \
   src_ptr = src; \
   dst_ptr = dst; \
   dst_end_ptr = dst + len;

/* saves re-writing the same loop */
#define FN_ARGB32_DO() \
   while (dst_ptr < dst_end_ptr) \
     {

/* saves ending the loop and the function */
#define FN_ARGB32_DONE() \
        src_ptr++; \
	dst_ptr++; \
     }
#define FN_ARGB32_END() \
}

#define MUL(a, b, t) (((a) * (b)) >> 8)
#define BLEND(p, q, a, t) ((p) + (q) - (MUL(a, p, t)))

/* function hell */
/* all functions start with pix_blend_ then comes the cpu it's for (c is 
 * generic, mmx is for p1mmx, and m2, sse is for p3 and p4), and then
 * and optional _sparse indicating the source image has sparse "alpha" values
 * and most are either fully blank or fully opaque and so a switch statement
 * will speed things up by skipping pixels entirely or skipping a blend stage
 * converting it into a copy. then finally there may be a _destalpha indicating
 * that the destination pixels have a "working alpha channel"
 * 
 * NOTE: the non destalpha calls will "damage" the destination alpha channel
 * which means that if you suddenly activate it it may be all screwed. there
 * are 2 solutions to this. 1. when you activate it going from non destalpha
 * to alpha, just fill all alpha channel pixels with 0xff to fill it. IMHO this
 * is the cleaner option as you will rarely go back and forth removing/adding
 * the alpha.
 * 
 * FIXME: cannot handle constant color or alpha (or both) multiplier
 * FIXME: cannot handle alpha multiplier mask (DATA8) 
 * FIXME: cannot handle source alpha only (DATA8)
 * FIXME: cannot handle source alpha only + constant color and/or alpha multiplier
 * 
 * HELP: altivec versions of the mmx/sse code?
 * 
 * WONTFIX: cannot handle separate r, g and b alpha channel masks
 * WONTFIX: cannot handle source alpha only + alpha multiplier mask (DATA8) + constant color and/or alpha multiplier
 */

/****************************************************************************/
FN_ARGB32_START(pix_blend_c);
FN_ARGB32_DO();
DATA32 tmp, a;
a = A_VAL(src_ptr);
a++;
R_VAL(dst_ptr) = BLEND(R_VAL(dst_ptr), R_VAL(src_ptr), a, tmp);
G_VAL(dst_ptr) = BLEND(G_VAL(dst_ptr), G_VAL(src_ptr), a, tmp);
B_VAL(dst_ptr) = BLEND(B_VAL(dst_ptr), B_VAL(src_ptr), a, tmp);
FN_ARGB32_DONE();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_blend_c2);
FN_ARGB32_DO();
DATA32 a;
a = 256 - (*src_ptr >> 24);
*dst_ptr = *src_ptr + 
  ((((*dst_ptr >> 8) & 0x00ff00ff) * a) & 0xff00ff00) +
  ((((*dst_ptr & 0x00ff00ff) * a) >> 8) & 0x00ff00ff);
FN_ARGB32_DONE();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_blend_mmx);
pxor_r2r(mm4, mm4); // mm4 = 0
pxor_r2r(mm5, mm5); // mm5 = 0
pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]
FN_ARGB32_DO();
movd_m2r(src_ptr[0], mm1); // mm1 = [  ][  ][AR][GB] (SRC)
movd_m2r(dst_ptr[0], mm2); // mm2 = [  ][  ][ar][gb] (DST)
punpcklbw_r2r(mm4, mm1);   // mm1 = [0A][0R][0G][0B]

movq_r2r(mm1, mm3);        // mm3 = [  ][  ][AR][GB]
punpckhwd_r2r(mm3, mm3);   // mm3 = [0A][0A][0R][0R]
punpckhdq_r2r(mm3, mm3);   // mm3 = [0A][0A][0A][0A]
paddw_r2r(mm5, mm3);       // mm3 = mm3 + mm5;

punpcklbw_r2r(mm4, mm2);   // mm2 = [0a][0r][0g][0b]
pmullw_r2r(mm2, mm3);      // mm3 = mm3 * mm2;
psrlw_i2r(8, mm3);         // mm3 = mm3 >> 8;

paddw_r2r(mm2, mm1);       // mm1 = mm1 + mm2;
psubw_r2r(mm3, mm1);       // mm1 = mm1 - mm3;

packuswb_r2r(mm4, mm1);    // mm1 = [  ][  ][AR][GB]
movd_r2m(mm1, dst_ptr[0]); // DST = mm1;
FN_ARGB32_DONE();
emms();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_blend_sse);
pxor_r2r(mm4, mm4); // mm4 = 0
pxor_r2r(mm5, mm5); // mm5 = 0
pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]
FN_ARGB32_DO();
movd_m2r(src_ptr[0], mm1); // mm1 = [  ][  ][AR][GB] (SRC)
movd_m2r(dst_ptr[0], mm2); // mm2 = [  ][  ][ar][gb] (DST)
punpcklbw_r2r(mm4, mm1);   // mm1 = [0A][0R][0G][0B]

pshufw(mm1, mm3, 0xe4);    // mm3 = mm1; [0A][0R][0G][0B]
pshufw(mm1, mm3, 0xff);    // mm3 = [0A][0A][0A][0A]
paddw_r2r(mm5, mm3);       // mm3 = mm3 + mm5;

punpcklbw_r2r(mm4, mm2);   // mm2 = [0a][0r][0g][0b]
pmullw_r2r(mm2, mm3);      // mm3 = mm3 * mm2;
psrlw_i2r(8, mm3);         // mm3 = mm3 >> 8;

paddw_r2r(mm2, mm1);       // mm1 = mm1 + mm2;
psubw_r2r(mm3, mm1);       // mm1 = mm1 - mm3;

packuswb_r2r(mm4, mm1);    // mm1 = [  ][  ][AR][GB]
movd_r2m(mm1, dst_ptr[0]); // DST = mm1;
FN_ARGB32_DONE();
emms();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_blend_sse2);
pxor_r2r(xmm4, xmm4); // xmm4 = 0
pxor_r2r(xmm5, xmm5); // xmm5 = 0
pcmpeqd_r2r(xmm5, xmm5); // xmm5 = [FFFF][FFFF][FFFF][FFFF][FFFF][FFFF][FFFF][FFFF]
psrlw_i2r(15, xmm5); // xmm5 = xmm5 >> 15; [0001][0001][0001][0001][0001][0001][0001][0001]
dst_end_ptr--;
FN_ARGB32_DO();
movq_m2r(src_ptr[0], xmm1);   // xmm1 = [  ][  ][  ][  ][AR][GB][AR][GB] (SRC)
movq_m2r(dst_ptr[0], xmm2);   // xmm2 = [  ][  ][  ][  ][ar][gb][ar][gb] (DST)
punpcklbw_r2r(xmm4, xmm1);    // xmm1 = [0A][0R][0G][0B][0A][0R][0G][0B]

movdqa_r2r(xmm1, xmm3);       // xmm3 = xmm1;
pshufhw(xmm3, xmm3, 0xff);    // xmm3 = [0A][0A][0A][0A][0A][0R][0G][0B]
pshuflw(xmm3, xmm3, 0xff);    // xmm3 = [0A][0A][0A][0A][0A][0A][0A][0A]

paddw_r2r(xmm5, xmm3);        // xmm3 = xmm3 + xmm5;

punpcklbw_r2r(xmm4, xmm2);    // xmm2 = [0a][0r][0g][0b][0a][0r][0g][0b]
pmullw_r2r(xmm2, xmm3);       // xmm3 = xmm3 * xmm2;
psrlw_i2r(8, xmm3);           // xmm3 = xmm3 >> 8;

paddw_r2r(xmm2, xmm1);        // xmm1 = xmm1 + xmm2;
psubw_r2r(xmm3, xmm1);        // xmm1 = xmm1 - xmm3;

packuswb_r2r(xmm4, xmm1);     // xmm1 = [  ][  ][  ][  ][AR][GB][AR][GB]
movq_r2m(xmm1, dst_ptr[0]);   // DST = xmm1;
dst_ptr++;
src_ptr++;
FN_ARGB32_DONE();
dst_end_ptr++;
pxor_r2r(mm4, mm4); // mm4 = 0
pxor_r2r(mm5, mm5); // mm5 = 0
pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]
FN_ARGB32_DO();
movd_m2r(src_ptr[0], mm1); // mm1 = [  ][  ][AR][GB] (SRC)
movd_m2r(dst_ptr[0], mm2); // mm2 = [  ][  ][ar][gb] (DST)
punpcklbw_r2r(mm4, mm1);   // mm1 = [0A][0R][0G][0B]

pshufw(mm1, mm3, 0xe4);    // mm3 = mm1; [0A][0R][0G][0B]
pshufw(mm1, mm3, 0xff);    // mm3 = [0A][0A][0A][0A]
paddw_r2r(mm5, mm3);       // mm3 = mm3 + mm5;

punpcklbw_r2r(mm4, mm2);   // mm2 = [0a][0r][0g][0b]
pmullw_r2r(mm2, mm3);      // mm3 = mm3 * mm2;
psrlw_i2r(8, mm3);         // mm3 = mm3 >> 8;

paddw_r2r(mm2, mm1);       // mm1 = mm1 + mm2;
psubw_r2r(mm3, mm1);       // mm1 = mm1 - mm3;

packuswb_r2r(mm4, mm1);    // mm1 = [  ][  ][AR][GB]
movd_r2m(mm1, dst_ptr[0]); // DST = mm1;
FN_ARGB32_DONE();
emms();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_blend_c_destalpha);
FN_ARGB32_DO();
DATA32 tmp, a;
a = A_VAL(src_ptr);
a++;
R_VAL(dst_ptr) = BLEND(R_VAL(dst_ptr), R_VAL(src_ptr), a, tmp);
G_VAL(dst_ptr) = BLEND(G_VAL(dst_ptr), G_VAL(src_ptr), a, tmp);
B_VAL(dst_ptr) = BLEND(B_VAL(dst_ptr), B_VAL(src_ptr), a, tmp);
A_VAL(dst_ptr) = A_VAL(dst_ptr) + a - 1 - MUL(a, A_VAL(dst_ptr), tmp);
FN_ARGB32_DONE();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_blend_c_sparse);
FN_ARGB32_DO();
DATA32 tmp, a;
a = A_VAL(src_ptr);
switch (a)
{
 case 0:
   break;
 case 255:
   *dst_ptr = *src_ptr;
   break;
 default:
   a++;
   R_VAL(dst_ptr) = BLEND(R_VAL(dst_ptr), R_VAL(src_ptr), a, tmp);
   G_VAL(dst_ptr) = BLEND(G_VAL(dst_ptr), G_VAL(src_ptr), a, tmp);
   B_VAL(dst_ptr) = BLEND(B_VAL(dst_ptr), B_VAL(src_ptr), a, tmp);
   break;
}
FN_ARGB32_DONE();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_blend_c2_sparse);
FN_ARGB32_DO();
DATA32 a;
a = A_VAL(src_ptr);
switch (a)
{
 case 0:
   break;
 case 255:
   *dst_ptr = *src_ptr;
   break;
 default:
   a = 256 - a;
   *dst_ptr = *src_ptr + 
     ((((*dst_ptr >> 8) & 0x00ff00ff) * a) & 0xff00ff00) +
     ((((*dst_ptr & 0x00ff00ff) * a) >> 8) & 0x00ff00ff);
   break;
}
FN_ARGB32_DONE();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_blend_mmx_sparse);
pxor_r2r(mm4, mm4); // mm4 = 0
pxor_r2r(mm5, mm5); // mm5 = 0
pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]
FN_ARGB32_DO();
switch (A_VAL(src_ptr))
{
 case 0:
   break;
 case 255:
   *dst_ptr = *src_ptr;
   break;
 default:
   movd_m2r(src_ptr[0], mm1); // mm1 = [  ][  ][AR][GB] (SRC)
   movd_m2r(dst_ptr[0], mm2); // mm2 = [  ][  ][ar][gb] (DST)
   punpcklbw_r2r(mm4, mm1);   // mm1 = [0A][0R][0G][0B]
   
   movq_r2r(mm1, mm3);        // mm3 = [  ][  ][AR][GB]
   punpckhwd_r2r(mm3, mm3);   // mm3 = [0A][0A][0R][0R]
   punpckhdq_r2r(mm3, mm3);   // mm3 = [0A][0A][0A][0A]
   paddw_r2r(mm5, mm3);       // mm3 = mm3 + mm5;
   
   punpcklbw_r2r(mm4, mm2);   // mm2 = [0a][0r][0g][0b]
   pmullw_r2r(mm2, mm3);      // mm3 = mm3 * mm2;
   psrlw_i2r(8, mm3);         // mm3 = mm3 >> 8;
   
   paddw_r2r(mm2, mm1);       // mm1 = mm1 + mm2;
   psubw_r2r(mm3, mm1);       // mm1 = mm1 - mm3;
   
   packuswb_r2r(mm4, mm1);    // mm1 = [  ][  ][AR][GB]
   movd_r2m(mm1, dst_ptr[0]); // DST = mm1;
   break;
}
FN_ARGB32_DONE();
emms();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_blend_sse_sparse);
pxor_r2r(mm4, mm4); // mm4 = 0
pxor_r2r(mm5, mm5); // mm5 = 0
pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]
FN_ARGB32_DO();
switch (A_VAL(src_ptr))
{
 case 0:
   break;
 case 255:
   *dst_ptr = *src_ptr;
   break;
 default:
   movd_m2r(src_ptr[0], mm1); // mm1 = [  ][  ][AR][GB] (SRC)
   movd_m2r(dst_ptr[0], mm2); // mm2 = [  ][  ][ar][gb] (DST)
   punpcklbw_r2r(mm4, mm1);   // mm1 = [0A][0R][0G][0B]
   
   pshufw(mm1, mm3, 0xe4);    // mm3 = mm1; [0A][0R][0G][0B]
   pshufw(mm3, mm3, 0xff);    // mm3 = [0A][0A][0A][0A]
   paddw_r2r(mm5, mm3);       // mm3 = mm3 + mm5;
   
   punpcklbw_r2r(mm4, mm2);   // mm2 = [0a][0r][0g][0b]
   pmullw_r2r(mm2, mm3);      // mm3 = mm3 * mm2;
   psrlw_i2r(8, mm3);         // mm3 = mm3 >> 8;
   
   paddw_r2r(mm2, mm1);       // mm1 = mm1 + mm2;
   psubw_r2r(mm3, mm1);       // mm1 = mm1 - mm3;
   
   packuswb_r2r(mm4, mm1);    // mm1 = [  ][  ][AR][GB]
   movd_r2m(mm1, dst_ptr[0]); // DST = mm1;
   break;
}
FN_ARGB32_DONE();
emms();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_blend_sse2_sparse);
pxor_r2r(xmm4, xmm4); // xmm4 = 0
pxor_r2r(xmm5, xmm5); // xmm5 = 0
pcmpeqd_r2r(xmm5, xmm5); // xmm5 = [FFFF][FFFF][FFFF][FFFF][FFFF][FFFF][FFFF][FFFF]
psrlw_i2r(15, xmm5); // xmm5 = xmm5 >> 15; [0001][0001][0001][0001][0001][0001][0001][0001]
FN_ARGB32_DO();
switch ((A_VAL(src_ptr) << 8) | A_VAL(src_ptr + 1))
{
 case 0:
   break;
 case 0xffff:
   movq_m2r(src_ptr[0], xmm1);
   movq_r2m(xmm1, dst_ptr[0]);
   break;
 default:
   movq_m2r(src_ptr[0], xmm1);   // xmm1 = [  ][  ][  ][  ][AR][GB][AR][GB] (SRC)
   movq_m2r(dst_ptr[0], xmm2);   // xmm2 = [  ][  ][  ][  ][ar][gb][ar][gb] (DST)
   punpcklbw_r2r(xmm4, xmm1);    // xmm1 = [0A][0R][0G][0B][0A][0R][0G][0B]
   
   movdqa_r2r(xmm1, xmm3);       // xmm3 = xmm1;
   pshufhw(xmm3, xmm3, 0xff);    // xmm3 = [0A][0A][0A][0A][0A][0R][0G][0B]
   pshuflw(xmm3, xmm3, 0xff);    // xmm3 = [0A][0A][0A][0A][0A][0A][0A][0A]
   
   paddw_r2r(xmm5, xmm3);        // xmm3 = xmm3 + xmm5;
   
   punpcklbw_r2r(xmm4, xmm2);    // xmm2 = [0a][0r][0g][0b][0a][0r][0g][0b]
   pmullw_r2r(xmm2, xmm3);       // xmm3 = xmm3 * xmm2;
   psrlw_i2r(8, xmm3);           // xmm3 = xmm3 >> 8;
   
   paddw_r2r(xmm2, xmm1);        // xmm1 = xmm1 + xmm2;
   psubw_r2r(xmm3, xmm1);        // xmm1 = xmm1 - xmm3;
   
   packuswb_r2r(xmm4, xmm1);     // xmm1 = [  ][  ][  ][  ][AR][GB][AR][GB]
   movq_r2m(xmm1, dst_ptr[0]);   // DST = xmm1;
   break;
}
dst_ptr++;
src_ptr++;
FN_ARGB32_DONE();
dst_end_ptr++;
pxor_r2r(mm4, mm4); // mm4 = 0
pxor_r2r(mm5, mm5); // mm5 = 0
pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]
FN_ARGB32_DO();
movd_m2r(src_ptr[0], mm1); // mm1 = [  ][  ][AR][GB] (SRC)
movd_m2r(dst_ptr[0], mm2); // mm2 = [  ][  ][ar][gb] (DST)
punpcklbw_r2r(mm4, mm1);   // mm1 = [0A][0R][0G][0B]

pshufw(mm1, mm3, 0xe4);    // mm3 = mm1; [0A][0R][0G][0B]
pshufw(mm1, mm3, 0xff);    // mm3 = [0A][0A][0A][0A]
paddw_r2r(mm5, mm3);       // mm3 = mm3 + mm5;

punpcklbw_r2r(mm4, mm2);   // mm2 = [0a][0r][0g][0b]
pmullw_r2r(mm2, mm3);      // mm3 = mm3 * mm2;
psrlw_i2r(8, mm3);         // mm3 = mm3 >> 8;

paddw_r2r(mm2, mm1);       // mm1 = mm1 + mm2;
psubw_r2r(mm3, mm1);       // mm1 = mm1 - mm3;

packuswb_r2r(mm4, mm1);    // mm1 = [  ][  ][AR][GB]
movd_r2m(mm1, dst_ptr[0]); // DST = mm1;
FN_ARGB32_DONE();
emms();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_blend_c_sparse_destalpha);
FN_ARGB32_DO();
DATA32 tmp, a;
a = A_VAL(src_ptr);
switch (a)
{
 case 0:
   break;
 case 255:
   *dst_ptr = *src_ptr;
   break;
 default:
   a++;
   R_VAL(dst_ptr) = BLEND(R_VAL(dst_ptr), R_VAL(src_ptr), a, tmp);
   G_VAL(dst_ptr) = BLEND(G_VAL(dst_ptr), G_VAL(src_ptr), a, tmp);
   B_VAL(dst_ptr) = BLEND(B_VAL(dst_ptr), B_VAL(src_ptr), a, tmp);
   A_VAL(dst_ptr) = A_VAL(dst_ptr) + a - 1 - MUL(a, A_VAL(dst_ptr), tmp);
   break;
}
FN_ARGB32_DONE();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_blend_c2_sparse_destalpha);
FN_ARGB32_DO();
DATA32 tmp, a;
a = A_VAL(src_ptr);
switch (a)
{
 case 0:
   break;
 case 255:
   *dst_ptr = *src_ptr;
   break;
 default:
   a++;
   R_VAL(dst_ptr) = BLEND(R_VAL(dst_ptr), R_VAL(src_ptr), a, tmp);
   G_VAL(dst_ptr) = BLEND(G_VAL(dst_ptr), G_VAL(src_ptr), a, tmp);
   B_VAL(dst_ptr) = BLEND(B_VAL(dst_ptr), B_VAL(src_ptr), a, tmp);
   A_VAL(dst_ptr) = A_VAL(dst_ptr) + a - 1 - MUL(a, A_VAL(dst_ptr), tmp);
   break;
}
FN_ARGB32_DONE();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_copy_memcpy);
memcpy(dst_ptr, src_ptr, len * 4);
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_copy_c);
FN_ARGB32_DO();
*dst_ptr = *src_ptr;
FN_ARGB32_DONE();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_copy_mmx);
dst_end_ptr-=15;
FN_ARGB32_DO();
MOVE_16DWORDS_MMX(src_ptr, dst_ptr);
src_ptr+=15;
dst_ptr+=15;
FN_ARGB32_DONE();
dst_end_ptr+=15;
FN_ARGB32_DO();
*dst_ptr = *src_ptr;
FN_ARGB32_DONE();
emms();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_copy_sse);
dst_end_ptr-=15;
FN_ARGB32_DO();
MOVE_16DWORDS_MMX2(src_ptr, dst_ptr);
src_ptr+=15;
dst_ptr+=15;
FN_ARGB32_DONE();
dst_end_ptr+=15;
FN_ARGB32_DO();
*dst_ptr = *src_ptr;
FN_ARGB32_DONE();
emms();
FN_ARGB32_END();

/****************************************************************************/
FN_ARGB32_START(pix_copy_sse2);
while ((((unsigned long)((unsigned char *)src_ptr)) & 0xf))
{ *dst_ptr = *src_ptr; dst_ptr++; src_ptr++;}
dst_end_ptr-=31;
FN_ARGB32_DO();
MOVE_32DWORDS_ALIGNED_SSE2(src_ptr, dst_ptr);
src_ptr+=31;
dst_ptr+=31;
FN_ARGB32_DONE();
dst_end_ptr+=31;
FN_ARGB32_DO();
*dst_ptr = *src_ptr;
FN_ARGB32_DONE();
emms();
FN_ARGB32_END();




DATA32 *
load_image_premul(char *file, int w, int h)
{
   Imlib_Image img;
   int iw, ih, i;
   DATA32 *pix, *dt;
   
   img = imlib_load_image(file);
   imlib_context_set_image(img);
   iw = imlib_image_get_width();
   ih = imlib_image_get_height();
   img = imlib_create_cropped_scaled_image(0, 0, iw, ih, w, h);
   imlib_context_set_image(img);
   dt = imlib_image_get_data_for_reading_only();
   pix = malloc(w * h * sizeof(DATA32));
   memcpy(pix, dt, w * h * sizeof(DATA32));
   imlib_free_image();
   for (i = 0; i < (w * h); i++)
     {
	R_VAL(&(pix[i])) = (R_VAL(&(pix[i])) * A_VAL(&(pix[i]))) / 255;
	G_VAL(&(pix[i])) = (G_VAL(&(pix[i])) * A_VAL(&(pix[i]))) / 255;
	B_VAL(&(pix[i])) = (B_VAL(&(pix[i])) * A_VAL(&(pix[i]))) / 255;
     }
   return pix;
}

void
save_image_premul(DATA32 *pix, int w, int h, char *file, int alpha)
{
   Imlib_Image img;
   int i;

   if (alpha)
     {
	for (i = 0; i < (w * h); i++)
	  {
	     if (A_VAL(&(pix[i])) != 0)
	       {
		  double a, v1;
		  DATA32 tmp;
		  
		  a = (double)A_VAL(&(pix[i])) / 255.0;
		  v1 = (double)R_VAL(&(pix[i])) / 255.0;
		  tmp = (255.0 * (v1 / a));
		  if (tmp > 255) tmp = 255;
		  R_VAL(&(pix[i])) = tmp;
		  v1 = (double)G_VAL(&(pix[i])) / 255.0;
		  tmp = (255.0 * (v1 / a));
		  if (tmp > 255) tmp = 255;
		  G_VAL(&(pix[i])) = tmp;
		  v1 = (double)B_VAL(&(pix[i])) / 255.0;
		  tmp = (255.0 * (v1 / a));
		  if (tmp > 255) tmp = 255;
		  B_VAL(&(pix[i])) = tmp;
	       }
	  }
     }
   img = imlib_create_image_using_copied_data(w, h, pix);
   imlib_context_set_image(img);
   imlib_image_set_has_alpha(alpha);
   imlib_save_image(file);
   imlib_free_image();
}

double
get_time(void)
{
   struct timeval      timev;
   
   gettimeofday(&timev, NULL);
   return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

typedef struct _Thparam Thparam;

struct _Thparam
{
   void (*func) (DATA32 *src, DATA32 *dst, int len);
   DATA32 *src, *dst;
   int len;
};

static void *
th_launch(void *data)
{
   Thparam *tpa;
   
   tpa = data;
   tpa->func(tpa->src, tpa->dst, tpa->len);
   return NULL;
}

int
test_p32_p32(DATA32 *src, DATA32 *dst, DATA32 *dstsrc, int w, int h,
	     Pixel_Op pop, int srcalpha, int dstalpha, int sparse,
	     int loops, char *name, int tnum)
{
   char buf[4096];
   int i, j, c;
   double t1, t2;
   Pixel_Op_Func fn;
   Pixel_Op_Params pp;
   char *cpu;
   
   PO_INIT(&pp);
   pp.l = w;
   pp.op = pop;
   pp.src.p = src;
   pp.dst.p = dst;
   pp.src.alpha = srcalpha;
   pp.dst.alpha = dstalpha;
   pp.src.sparse = sparse;
   for (c = 0; c < 5; c++)
     {
	fn = pixel_op_get(&pp, c);
	if (!fn)
	  {
	     printf("[%02i] %38s            : NO FUNCTION\n", tnum, name);
	  }
	else
	  {
	     pp.l = w;
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     t1 = get_time();
	     for (i = 0; i < loops; i++)
	       {
		  for (j = 0; j < h; j++)
		    {
		       pp.src.p = src + (w * j);
		       pp.dst.p = dst + (w * j);
		       fn(&pp);
		    }
	       }
	     pixel_op_end();
	     t2 = get_time();
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%02i] %38s %11s: %3.3f mpix/sec\n", tnum, name, cpu,
		    (double)((w * h * loops) / 1000000.0) / (t2 - t1));
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     PO_INIT(&pp);
	     pp.l = w * h;
	     pp.op = pop;
	     pp.src.p = src;
	     pp.dst.p = dst;
	     pp.src.alpha = srcalpha;
	     pp.dst.alpha = dstalpha;
	     pp.src.sparse = sparse;
	     fn(&pp);
	     pixel_op_end();
	     snprintf(buf, sizeof(buf), "out_%02i.png", tnum);
	     save_image_premul(dst, w, h, buf, 0);
	  }
	tnum++;
     }
   return tnum;
}

int
test_c_p32(DATA32 col, DATA32 *dst, DATA32 *dstsrc, int w, int h,
	   Pixel_Op pop, int dstalpha,
	   int loops, char *name, int tnum)
{
   char buf[4096];
   int i, j, c;
   double t1, t2;
   Pixel_Op_Func fn;
   Pixel_Op_Params pp;
   char *cpu;
   
   PO_INIT(&pp);
   pp.l = w;
   pp.op = pop;
   pp.src.c = col;
   pp.dst.p = dst;
   pp.dst.alpha = dstalpha;
   for (c = 0; c < 5; c++)
     {
	fn = pixel_op_get(&pp, c);
	if (!fn)
	  {
	     printf("[%02i] %38s            : NO FUNCTION\n", tnum, name);
	  }
	else
	  {
	     pp.l = w;
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     t1 = get_time();
	     for (i = 0; i < loops; i++)
	       {
		  for (j = 0; j < h; j++)
		    {
		       pp.dst.p = dst + (w * j);
		       fn(&pp);
		    }
	       }
	     pixel_op_end();
	     t2 = get_time();
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%02i] %38s %11s: %3.3f mpix/sec\n", tnum, name, cpu,
		    (double)((w * h * loops) / 1000000.0) / (t2 - t1));
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     PO_INIT(&pp);
	     pp.l = w * h;
	     pp.op = pop;
	     pp.src.c = col;
	     pp.dst.p = dst;
	     pp.dst.alpha = dstalpha;
	     fn(&pp);
	     pixel_op_end();
	     snprintf(buf, sizeof(buf), "out_%02i.png", tnum);
	     save_image_premul(dst, w, h, buf, 0);
	  }
	tnum++;
     }
   return tnum;
}

int
main(int argc, char **argv)
{
   int w, h;
   int loops;
   DATA32 *src;
   DATA32 *dst;
   DATA32 *solid_dst;
   DATA32 *alpha_dst;
   char *file;
   int i, j, k, test;
   double t1, t2;
   char buf[4096];
   pthread_t tid[16];
   Thparam tpa[16];

   if (argc < 2)
     {
	printf("blend   test_image.png width height loops\n");
	exit(-1);
     }
   file = argv[1];
   w = atoi(argv[2]);
   h = atoi(argv[3]);
   loops = atoi(argv[4]);
   
   solid_dst = load_image_premul("back.png", w, h);
   alpha_dst = load_image_premul("backa.png", w, h);
   src = load_image_premul(file, w, h);
   dst = malloc(w * h * sizeof(DATA32));

   pixel_op_init();
   
   test = 0;
   test = test_p32_p32(src, dst, solid_dst, w, h, PIXEL_OP_COPY, 0, 0, 0, loops,
		       "copy", test);
   test = test_c_p32(0xffffffff, dst, solid_dst, w, h, PIXEL_OP_COPY, 0, loops,
		     "set_white", test);
   test = test_c_p32(0x00000000, dst, solid_dst, w, h, PIXEL_OP_COPY, 0, loops,
		     "set_black", test);
   test = test_c_p32(0x12345678, dst, solid_dst, w, h, PIXEL_OP_COPY, 0, loops,
		     "set_color", test);
   /* FIXME: remember all ops could have an alpha mask as a destination */
   /* FIXME: test solid alpha mask fills */
   
   test = test_p32_p32(src, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, 0, 0, loops,
		       "blend_srcalpha", test);
   test = test_p32_p32(src, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, 0, 1, loops,
		       "blend_srcalpha_sparse", test);
   test = test_p32_p32(src, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, 1, 0, loops,
		       "blend_srcalpha_dstalpha", test);
   test = test_p32_p32(src, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, 1, 1, loops,
		       "blend_srcalpha_sparse_dstalpha", test);
   test = test_p32_p32(src, dst, solid_dst, w, h, PIXEL_OP_BLEND, 0, 1, 0, loops,
		       "blend_dstalpha", test);
   test = test_p32_p32(src, dst, solid_dst, w, h, PIXEL_OP_BLEND, 0, 0, 0, loops,
		       "blend", test);
   test = test_c_p32(0xffffffff, dst, solid_dst, w, h, PIXEL_OP_BLEND, 0, loops,
		     "blend_white", test);
   test = test_c_p32(0x00000000, dst, solid_dst, w, h, PIXEL_OP_BLEND, 0, loops,
		     "blend_black", test);
   test = test_c_p32(0x12345678, dst, solid_dst, w, h, PIXEL_OP_BLEND, 0, loops,
		     "blend_color", test);
   test = test_c_p32(0xffffffff, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, loops,
		     "blend_white_dstalpha", test);
   test = test_c_p32(0x00000000, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, loops,
		     "blend_black_dstalpha", test);
   test = test_c_p32(0x12345678, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, loops,
		     "blend_color_dstalpha", test);
   /* FIXME: test blend with color multiplier */
   /* FIXME: test blend with alpha mask */
   /* FIXME: test blend with alpha mask and color multiplier */
   /* FIXME: test blend with color and alpha mask only */
   
   /* FIXME: test add & sub modes too */
   
   return 0;
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   /* now do the speed tests */
#define TEST(fn, name, dest, alpha) \
   memcpy(dst, dest, w * h * sizeof(DATA32)); \
   t1 = get_time(); \
   for (i = 0; i < loops; i++) { \
     for (j = 0; j < h; j++) fn(src + (w * j), dst + (w * j), w); \
   } \
   t2 = get_time(); \
   printf("[%02i] %50s: %3.3f mpix/sec\n", test, name, (double)((w * h * loops) / 1000000) / (t2 - t1)); \
   memcpy(dst, dest, w * h * sizeof(DATA32)); \
   fn(src, dst, w * h); \
   snprintf(buf, sizeof(buf), "out_%02i.png", test); \
   save_image_premul(dst, w, h, buf, alpha); \
   test++;
   
#define TH_TEST(nth, fn, name, dest, alpha) \
   memcpy(dst, dest, w * h * sizeof(DATA32)); \
   t1 = get_time(); \
   for (i = 0; i < loops; i++) { \
     for (j = 0; j < h;) { \
	int pj; \
	pj = j; \
	for (k = 0; (k < (nth)) && (j < h); k++, j += (h / nth)) { \
	   tpa[k].func = fn; \
	   tpa[k].src = src + (w * j); \
	   tpa[k].dst = dst + (w * j); \
	   tpa[k].len = w * (h / nth); \
	   pthread_create(&(tid[k]), NULL, th_launch, &(tpa[k])); \
	} \
	j = pj; \
	for (k = 0; (k < (nth)) && (j < h); k++, j += (h / nth)) { \
           pthread_join(tid[k], NULL); \
	} \
     } \
   } \
   t2 = get_time(); \
   printf("[%02i] %50s: %3.3f mpix/sec\n", test, name, (double)((w * h * loops) / 1000000) / (t2 - t1)); \
   memcpy(dst, dest, w * h * sizeof(DATA32)); \
   fn(src, dst, w * h); \
   snprintf(buf, sizeof(buf), "out_%02i.png", test); \
   save_image_premul(dst, w, h, buf, alpha); \
   test++;
   
   TEST(pix_copy_memcpy, "pix_copy_memcpy", solid_dst, 0);
   TEST(pix_copy_c, "pix_copy_c", solid_dst, 0);
   TEST(pix_copy_mmx, "pix_copy_mmx", solid_dst, 0);
   TEST(pix_copy_sse, "pix_copy_sse", solid_dst, 0);
   TEST(pix_copy_sse2, "pix_copy_sse2", solid_dst, 0);
   printf(" -- \n");
   TEST(pix_blend_c, "pix_blend_c", solid_dst, 0);
   TEST(pix_blend_c2 ,"pix_blend_c2", solid_dst, 0);
   TEST(pix_blend_mmx, "pix_blend_mmx", solid_dst, 0);
   TEST(pix_blend_sse, "pix_blend_sse", solid_dst, 0);
   TEST(pix_blend_sse2, "pix_blend_sse2", solid_dst, 0);
   TEST(pix_blend_c_sparse, "pix_blend_c_sparse", solid_dst, 0);
   TEST(pix_blend_c2_sparse, "pix_blend_c2_sparse", solid_dst, 0);
   TEST(pix_blend_mmx_sparse, "pix_blend_mmx_sparse", solid_dst, 0);
   TEST(pix_blend_sse_sparse, "pix_blend_sse_sparse", solid_dst, 0);
   TEST(pix_blend_sse2_sparse, "pix_blend_sse2_sparse", solid_dst, 0);
   printf(" -- \n");
   TEST(pix_blend_c_destalpha ,"pix_blend_c_destalpha", alpha_dst, 1);
   TEST(pix_blend_c2 ,"pix_blend_c2_destalpha", alpha_dst, 1);
   TEST(pix_blend_mmx ,"pix_blend_mmx_destalpha", alpha_dst, 1);
   TEST(pix_blend_sse ,"pix_blend_sse_destalpha", alpha_dst, 1);
   TEST(pix_blend_sse2 ,"pix_blend_sse2_destalpha", alpha_dst, 1);
   TEST(pix_blend_c_sparse_destalpha ,"pix_blend_c_sparse_destalpha", alpha_dst, 1);
   TEST(pix_blend_c2_sparse, "pix_blend_c2_sparse_destalpha", alpha_dst, 1);
   TEST(pix_blend_mmx_sparse ,"pix_blend_mmx_sparse_destalpha", alpha_dst, 1);
   TEST(pix_blend_sse_sparse ,"pix_blend_sse_sparse_destalpha", alpha_dst, 1);
   TEST(pix_blend_sse2_sparse ,"pix_blend_sse2_sparse_destalpha", alpha_dst, 1);
/*   
   printf(" ---- \n");
   TH_TEST(2, pix_copy_memcpy, "pix_copy_memcpy th", solid_dst, 0);
   TH_TEST(2, pix_copy_c, "pix_copy_c th", solid_dst, 0);
   TH_TEST(2, pix_copy_mmx, "pix_copy_mmx th", solid_dst, 0);
   TH_TEST(2, pix_copy_sse, "pix_copy_sse th", solid_dst, 0);
   TH_TEST(2, pix_copy_sse2, "pix_copy_sse2 th", solid_dst, 0);
   printf(" -- \n");
   TH_TEST(2, pix_blend_c, "pix_blend_c th", solid_dst, 0);
   TH_TEST(2, pix_blend_c2, "pix_blend_c2 th", solid_dst, 0);
   TH_TEST(2, pix_blend_mmx, "pix_blend_mmx th", solid_dst, 0);
   TH_TEST(2, pix_blend_sse, "pix_blend_sse th", solid_dst, 0);
   TH_TEST(2, pix_blend_sse2, "pix_blend_sse2 th", solid_dst, 0);
   TH_TEST(2, pix_blend_c_sparse, "pix_blend_c_sparse th", solid_dst, 0);
   TH_TEST(2, pix_blend_c2_sparse, "pix_blend_c2_sparse th", solid_dst, 0);
   TH_TEST(2, pix_blend_mmx_sparse, "pix_blend_mmx_sparse th", solid_dst, 0);
   TH_TEST(2, pix_blend_sse_sparse, "pix_blend_sse_sparse th", solid_dst, 0);
   TH_TEST(2, pix_blend_sse2_sparse, "pix_blend_sse2_sparse th", solid_dst, 0);
   printf(" -- \n");
   TH_TEST(2, pix_blend_c_destalpha ,"pix_blend_c_destalpha th", alpha_dst, 1);
   TH_TEST(2, pix_blend_mmx ,"pix_blend_mmx_destalpha th", alpha_dst, 1);
   TH_TEST(2, pix_blend_sse ,"pix_blend_sse_destalpha th", alpha_dst, 1);
   TH_TEST(2, pix_blend_sse2 ,"pix_blend_sse2_destalpha th", alpha_dst, 1);
   TH_TEST(2, pix_blend_c_sparse_destalpha ,"pix_blend_c_sparse_destalpha th", alpha_dst, 1);
   TH_TEST(2, pix_blend_mmx_sparse ,"pix_blend_mmx_sparse_destalpha th", alpha_dst, 1);
   TH_TEST(2, pix_blend_sse_sparse ,"pix_blend_sse_sparse_destalpha th", alpha_dst, 1);
   TH_TEST(2, pix_blend_sse2_sparse ,"pix_blend_sse2_sparse_destalpha th", alpha_dst, 1);
 */
}
