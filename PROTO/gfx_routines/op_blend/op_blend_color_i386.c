
/* blend pixel --> dst */

/*-----*/
#ifdef OP_FN /* do it in mmx */
OPFN(_op_blend_c_dp_mmx) {
   DATA32 c, *d, *e;
   c = p->src.c; d = p->dst.p; e = d + p->l;

   pxor_r2r(mm4, mm4); // mm4 = 0
   pxor_r2r(mm5, mm5); // mm5 = 0
   pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
   psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]

   movd_m2r(c, mm2); // mm2 = [  ][  ][AR][GB] (C)
   punpcklbw_r2r(mm4, mm2);   // mm2 = [0A][0R][0G][0B]

   movq_r2r(mm2, mm3);        // mm3 = [0A][0R][0G][0B]
   punpckhwd_r2r(mm3, mm3);   // mm3 = [0A][0A][0R][0R]
   punpckhdq_r2r(mm3, mm3);   // mm3 = [0A][0A][0A][0A]
   paddw_r2r(mm5, mm3);       // mm3 = mm3 + mm5;
   for (; d < e; d++) {
	movd_m2r(d[0], mm1); // mm1 = [  ][  ][ar][gb] (DST)
	punpcklbw_r2r(mm4, mm1);   // mm1 = [0a][0r][0g][0b]

	movq_r2r(mm1, mm5);        // mm5 = mm1
	pmullw_r2r(mm3, mm5);      // mm5 = mm3 * mm5;
	psrlw_i2r(8, mm5);         // mm5 = mm5 >> 8;

	paddw_r2r(mm2, mm1);       // mm1 = mm1 + mm2;
	psubw_r2r(mm5, mm1);       // mm1 = mm1 - mm5;

	packuswb_r2r(mm4, mm1);    // mm1 = [  ][  ][AR][GB]
	movd_r2m(mm1, d[0]); // DST = mm1;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_blend_cn_dp_mmx _op_copy_cn_dp_mmx
#define _op_blend_can_dp_mmx _op_copy_can_dp_mmx
#define _op_blend_caa_dp_mmx _op_blend_c_dp_mmx

#define _op_blend_c_dpan_mmx _op_blend_c_dp_mmx
#define _op_blend_cn_dpan_mmx _op_blend_cn_dp_mmx
#define _op_blend_can_dpan_mmx _op_blend_can_dp_mmx
#define _op_blend_caa_dpan_mmx _op_blend_c_dp_mmx

#define _op_blend_c_dpas_mmx _op_blend_c_dp_mmx
#define _op_blend_cn_dpas_mmx _op_blend_cn_dp_mmx
#define _op_blend_can_dpas_mmx _op_blend_can_dp_mmx
#define _op_blend_caa_dpas_mmx _op_blend_c_dp_mmx
#endif

/*-----*/
#ifdef OP_REG
OPREG(_op_blend_cn_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_N, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_c_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_can_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AN, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_caa_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AA, DP, CPU_MMX); /* what parameters it does */

OPREG(_op_blend_cn_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_N, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_c_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_can_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AN, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_caa_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AA, DP_AN, CPU_MMX); /* what parameters it does */

OPREG(_op_blend_cn_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_c_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_can_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AN, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_caa_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AA, DP_AS, CPU_MMX); /* what parameters it does */
#endif


/*-----*/
#ifdef OP_FN /* do it in sse */
OPFN(_op_blend_c_dp_sse) {
   DATA32 c, *d, *e;
   c = p->src.c; d = p->dst.p; e = d + p->l;

   pxor_r2r(mm4, mm4); // mm4 = 0
   pxor_r2r(mm5, mm5); // mm5 = 0
   pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
   psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]

   movd_m2r(c, mm2); // mm1 = [  ][  ][AR][GB] (SRC)
   punpcklbw_r2r(mm4, mm2);   // mm2 = [0A][0R][0G][0B]

   pshufw(mm2, mm3, 0xe4);    // mm3 = mm1; [0A][0R][0G][0B]
   pshufw(mm2, mm3, 0xff);    // mm3 = [0A][0A][0A][0A]
   paddw_r2r(mm5, mm3);       // mm3 = mm3 + mm5;
   for (; d < e; d++) {
	movd_m2r(d[0], mm1); // mm1 = [  ][  ][ar][gb] (DST)
	punpcklbw_r2r(mm4, mm1);   // mm1 = [0a][0r][0g][0b]

	movq_r2r(mm1, mm5);        // mm5 = mm1
	pmullw_r2r(mm3, mm5);      // mm5 = mm3 * mm5;
	psrlw_i2r(8, mm5);         // mm5 = mm5 >> 8;

	paddw_r2r(mm2, mm1);       // mm1 = mm1 + mm2;
	psubw_r2r(mm5, mm1);       // mm1 = mm1 - mm5;

	packuswb_r2r(mm4, mm1);    // mm1 = [  ][  ][AR][GB]
	movd_r2m(mm1, d[0]); // DST = mm1;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_blend_cn_dp_sse _op_copy_cn_dp_sse
#define _op_blend_can_dp_sse _op_copy_can_dp_sse
#define _op_blend_caa_dp_sse _op_blend_c_dp_sse

#define _op_blend_c_dpan_sse _op_blend_c_dp_sse
#define _op_blend_cn_dpan_sse _op_blend_cn_dp_sse
#define _op_blend_can_dpan_sse _op_blend_can_dp_sse
#define _op_blend_caa_dpan_sse _op_blend_c_dp_sse

#define _op_blend_c_dpas_sse _op_blend_c_dp_sse
#define _op_blend_cn_dpas_sse _op_blend_cn_dp_sse
#define _op_blend_can_dpas_sse _op_blend_can_dp_sse
#define _op_blend_caa_dpas_sse _op_blend_c_dp_sse
#endif

/*-----*/
#ifdef OP_REG
OPREG(_op_blend_cn_dp_sse, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_N, DP, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_c_dp_sse, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC, DP, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_can_dp_sse, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AN, DP, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_caa_dp_sse, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AA, DP, CPU_SSE); /* what parameters it does */

OPREG(_op_blend_cn_dpan_sse, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_N, DP_AN, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_c_dpan_sse, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC, DP_AN, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_can_dpan_sse, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AN, DP_AN, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_caa_dpan_sse, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AA, DP_AN, CPU_SSE); /* what parameters it does */

OPREG(_op_blend_cn_dpas_sse, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_N, DP_AS, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_c_dpas_sse, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC, DP_AS, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_can_dpas_sse, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AN, DP_AS, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_caa_dpas_sse, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AA, DP_AS, CPU_SSE); /* what parameters it does */
#endif


/*-----*/
#ifdef OP_FN /* do it in sse2 */
OPFN(_op_blend_c_dp_sse2) {
   DATA32 c, *d, *e;
   c = p->src.c; d = p->dst.p; e = d + p->l - 1;

   pxor_r2r(xmm4, xmm4); // xmm4 = 0
   pxor_r2r(xmm5, xmm5); // xmm5 = 0
   pcmpeqd_r2r(xmm5, xmm5); // xmm5 = [FFFF][FFFF][FFFF][FFFF][FFFF][FFFF][FFFF][FFFF]
   psrlw_i2r(15, xmm5); // xmm5 = xmm5 >> 15; [0001][0001][0001][0001][0001][0001][0001][0001]

   movq_m2r(c, xmm2);   // xmm2 = [  ][  ][  ][  ][AR][GB][AR][GB] (SRC)
   punpcklbw_r2r(xmm4, xmm2);    // xmm2 = [0A][0R][0G][0B][0A][0R][0G][0B]

   movdqa_r2r(xmm2, xmm3);       // xmm3 = xmm2;
   pshufhw(xmm3, xmm3, 0xff);    // xmm3 = [0A][0A][0A][0A][0A][0R][0G][0B]
   pshuflw(xmm3, xmm3, 0xff);    // xmm3 = [0A][0A][0A][0A][0A][0A][0A][0A]
   paddw_r2r(xmm5, xmm3);        // xmm3 = xmm3 + xmm5;
   for (; d < e; d++) {
	movq_m2r(d[0], xmm1);   // xmm1 = [  ][  ][  ][  ][ar][gb][ar][gb] (DST)
	punpcklbw_r2r(xmm4, xmm1);    // xmm1 = [0a][0r][0g][0b][0a][0r][0g][0b]

	movdqa_r2r(xmm1, xmm5);        // xmm5 = xmm1
	pmullw_r2r(xmm3, xmm5);      // xmm5 = xmm3 * xmm5;
	psrlw_i2r(8, xmm5);         // xmm5 = xmm5 >> 8;

	paddw_r2r(xmm2, xmm1);       // xmm1 = xmm1 + xmm2;
	psubw_r2r(xmm5, xmm1);       // xmm1 = xmm1 - xmm5;

	packuswb_r2r(xmm4, xmm1);     // xmm1 = [  ][  ][  ][  ][AR][GB][AR][GB]
	movq_r2m(xmm1, d[0]);   // DST = xmm1;
   }
   e++;
   pxor_r2r(mm4, mm4); // mm4 = 0
   pxor_r2r(mm5, mm5); // mm5 = 0
   pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
   psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]

   movd_m2r(c, mm2); // mm2 = [  ][  ][AR][GB] (SRC)
   punpcklbw_r2r(mm4, mm2);   // mm2 = [0A][0R][0G][0B]

   pshufw(mm2, mm3, 0xe4);    // mm3 = mm1; [0A][0R][0G][0B]
   pshufw(mm2, mm3, 0xff);    // mm3 = [0A][0A][0A][0A]
   paddw_r2r(mm5, mm3);       // mm3 = mm3 + mm5;
   for (; d < e; d++) {
	movd_m2r(d[0], mm1); // mm1 = [  ][  ][ar][gb] (DST)
	punpcklbw_r2r(mm4, mm1);   // mm1 = [0a][0r][0g][0b]

	movq_r2r(mm1, mm5);        // mm5 = mm1
	pmullw_r2r(mm3, mm5);      // mm5 = mm3 * mm5;
	psrlw_i2r(8, mm5);         // mm5 = mm5 >> 8;

	paddw_r2r(mm2, mm1);       // mm1 = mm1 + mm2;
	psubw_r2r(mm5, mm1);       // mm1 = mm1 - mm5;

	packuswb_r2r(mm4, mm1);    // mm1 = [  ][  ][AR][GB]
	movd_r2m(mm1, d[0]); // DST = mm1;
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_blend_cn_dp_sse2 _op_copy_cn_dp_sse2
#define _op_blend_can_dp_sse2 _op_copy_can_dp_sse2
#define _op_blend_caa_dp_sse2 _op_blend_c_dp_sse2

#define _op_blend_c_dpan_sse2 _op_blend_c_dp_sse2
#define _op_blend_cn_dpan_sse2 _op_blend_cn_dp_sse2
#define _op_blend_can_dpan_sse2 _op_blend_can_dp_sse2
#define _op_blend_caa_dpan_sse2 _op_blend_c_dp_sse2

#define _op_blend_c_dpas_sse2 _op_blend_c_dp_sse2
#define _op_blend_cn_dpas_sse2 _op_blend_cn_dp_sse2
#define _op_blend_can_dpas_sse2 _op_blend_can_dp_sse2
#define _op_blend_caa_dpas_sse2 _op_blend_c_dp_sse2
#endif

/*-----*/
#ifdef OP_REG
OPREG(_op_blend_cn_dp_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_N, DP, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_c_dp_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC, DP, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_can_dp_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AN, DP, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_caa_dp_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AA, DP, CPU_SSE2); /* what parameters it does */

OPREG(_op_blend_cn_dpan_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_N, DP_AN, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_c_dpan_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC, DP_AN, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_can_dpan_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AN, DP_AN, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_caa_dpan_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AA, DP_AN, CPU_SSE2); /* what parameters it does */

OPREG(_op_blend_cn_dpas_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_N, DP_AS, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_c_dpas_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC, DP_AS, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_can_dpas_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AN, DP_AS, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_caa_dpas_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AA, DP_AS, CPU_SSE2); /* what parameters it does */
#endif
/*-----*/
