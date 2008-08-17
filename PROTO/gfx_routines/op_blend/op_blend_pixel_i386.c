
/* blend pixel --> dst */

/*-----*/
#ifdef OP_FN /* do it in mmx */
OPFN(_op_blend_p_dp_mmx) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l;

   pxor_r2r(mm4, mm4); // mm4 = 0
   pxor_r2r(mm5, mm5); // mm5 = 0
   pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
   psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]
   for (; s < e; s++, d++) {
	movd_m2r(s[0], mm1); // mm1 = [  ][  ][AR][GB] (SRC)
	movd_m2r(d[0], mm2); // mm2 = [  ][  ][ar][gb] (DST)
	punpcklbw_r2r(mm4, mm1);   // mm1 = [0A][0R][0G][0B]

	movq_r2r(mm1, mm3);        // mm3 = [0A][0R][0G][0B]
	punpckhwd_r2r(mm3, mm3);   // mm3 = [0A][0A][0R][0R]
	punpckhdq_r2r(mm3, mm3);   // mm3 = [0A][0A][0A][0A]
	paddw_r2r(mm5, mm3);       // mm3 = mm3 + mm5;

	punpcklbw_r2r(mm4, mm2);   // mm2 = [0a][0r][0g][0b]
	pmullw_r2r(mm2, mm3);      // mm3 = mm3 * mm2;
	psrlw_i2r(8, mm3);         // mm3 = mm3 >> 8;

	paddw_r2r(mm2, mm1);       // mm1 = mm1 + mm2;
	psubw_r2r(mm3, mm1);       // mm1 = mm1 - mm3;

	packuswb_r2r(mm4, mm1);    // mm1 = [  ][  ][AR][GB]
	movd_r2m(mm1, d[0]); // DST = mm1;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in mmx */
OPFN(_op_blend_pas_dp_mmx) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l;

   pxor_r2r(mm4, mm4); // mm4 = 0
   pxor_r2r(mm5, mm5); // mm5 = 0
   pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
   psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]
   for (; s < e; s++, d++) {
	switch (*s & 0xff000000)
	   {
	     case 0:
		break;
	     case 0xff000000:
		*d = *s;
		break;
	     default:
		movd_m2r(s[0], mm1); // mm1 = [  ][  ][AR][GB] (SRC)
		movd_m2r(d[0], mm2); // mm2 = [  ][  ][ar][gb] (DST)
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
		movd_r2m(mm1, d[0]); // DST = mm1;
		break;
	  }
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_blend_pan_dp_mmx _op_copy_p_dp_mmx

#define _op_blend_p_dpan_mmx _op_blend_p_dp_mmx
#define _op_blend_pas_dpan_mmx _op_blend_pas_dp_mmx
#define _op_blend_pan_dpan_mmx _op_blend_pan_dp_mmx

#define _op_blend_p_dpas_mmx _op_blend_p_dp_mmx
#define _op_blend_pas_dpas_mmx _op_blend_pas_dp_mmx
#define _op_blend_pan_dpas_mmx _op_blend_pan_dp_mmx
#endif

/*-----*/
#ifdef OP_REG
OPREG(_op_blend_p_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_N, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_N, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_N, DP, CPU_MMX); /* what parameters it does */

OPREG(_op_blend_p_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_N, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_N, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_N, DP_AN, CPU_MMX); /* what parameters it does */

OPREG(_op_blend_p_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
#endif


/*-----*/
#ifdef OP_FN /* do it in sse */
OPFN(_op_blend_p_dp_sse) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l;

   pxor_r2r(mm4, mm4); // mm4 = 0
   pxor_r2r(mm5, mm5); // mm5 = 0
   pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
   psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]
   for (; s < e; s++, d++) {
	movd_m2r(s[0], mm1); // mm1 = [  ][  ][AR][GB] (SRC)
	movd_m2r(d[0], mm2); // mm2 = [  ][  ][ar][gb] (DST)
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
	movd_r2m(mm1, d[0]); // DST = mm1;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in sse */
OPFN(_op_blend_pas_dp_sse) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l;

   pxor_r2r(mm4, mm4); // mm4 = 0
   pxor_r2r(mm5, mm5); // mm5 = 0
   pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
   psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]
   for (; s < e; s++, d++) {
	switch (*s & 0xff000000)
	   {
	     case 0:
		break;
	     case 0xff000000:
		*d = *s;
		break;
	     default:
		movd_m2r(s[0], mm1); // mm1 = [  ][  ][AR][GB] (SRC)
		movd_m2r(d[0], mm2); // mm2 = [  ][  ][ar][gb] (DST)
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
		movd_r2m(mm1, d[0]); // DST = mm1;
		break;
	  }
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_blend_pan_dp_sse _op_copy_p_dp_sse

#define _op_blend_p_dpan_sse _op_blend_p_dp_sse
#define _op_blend_pas_dpan_sse _op_blend_pas_dp_sse
#define _op_blend_pan_dpan_sse _op_blend_pan_dp_sse

#define _op_blend_p_dpas_sse _op_blend_p_dp_sse
#define _op_blend_pas_dpas_sse _op_blend_pas_dp_sse
#define _op_blend_pan_dpas_sse _op_blend_pan_dp_sse
#endif

/*-----*/
#ifdef OP_REG
OPREG(_op_blend_p_dp_sse, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_N, DP, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_pas_dp_sse, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_N, DP, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_pan_dp_sse, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_N, DP, CPU_SSE); /* what parameters it does */

OPREG(_op_blend_p_dpan_sse, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_N, DP_AN, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_pas_dpan_sse, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_N, DP_AN, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_pan_dpan_sse, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_N, DP_AN, CPU_SSE); /* what parameters it does */

OPREG(_op_blend_p_dpas_sse, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_N, DP_AS, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_pas_dpas_sse, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_N, DP_AS, CPU_SSE); /* what parameters it does */
OPREG(_op_blend_pan_dpas_sse, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_N, DP_AS, CPU_SSE); /* what parameters it does */
#endif


/*-----*/
#ifdef OP_FN /* do it in sse2 */
OPFN(_op_blend_p_dp_sse2) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l - 1;

   pxor_r2r(xmm4, xmm4); // xmm4 = 0
   pxor_r2r(xmm5, xmm5); // xmm5 = 0
   pcmpeqd_r2r(xmm5, xmm5); // xmm5 = [FFFF][FFFF][FFFF][FFFF][FFFF][FFFF][FFFF][FFFF]
   psrlw_i2r(15, xmm5); // xmm5 = xmm5 >> 15; [0001][0001][0001][0001][0001][0001][0001][0001]
   for (; s < e; s++, d++) {
	movq_m2r(s[0], xmm1);   // xmm1 = [  ][  ][  ][  ][AR][GB][AR][GB] (SRC)
	movq_m2r(d[0], xmm2);   // xmm2 = [  ][  ][  ][  ][ar][gb][ar][gb] (DST)
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
	movq_r2m(xmm1, d[0]);   // DST = xmm1;
   }
   e++;
   pxor_r2r(mm4, mm4); // mm4 = 0
   pxor_r2r(mm5, mm5); // mm5 = 0
   pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
   psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]
   for (; s < e; s++, d++) {
	movd_m2r(s[0], mm1); // mm1 = [  ][  ][AR][GB] (SRC)
	movd_m2r(d[0], mm2); // mm2 = [  ][  ][ar][gb] (DST)
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
	movd_r2m(mm1, d[0]); // DST = mm1;
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in sse2 */
OPFN(_op_blend_pas_dp_sse2) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l - 1;

   pxor_r2r(xmm4, xmm4); // xmm4 = 0
   pxor_r2r(xmm5, xmm5); // xmm5 = 0
   pcmpeqd_r2r(xmm5, xmm5); // xmm5 = [FFFF][FFFF][FFFF][FFFF][FFFF][FFFF][FFFF][FFFF]
   psrlw_i2r(15, xmm5); // xmm5 = xmm5 >> 15; [0001][0001][0001][0001][0001][0001][0001][0001]
   for (; s < e; s++, d++) {
	switch ((A_VAL(s) << 8) | A_VAL(s + 1))
	   {
	     case 0:
		break;
	     case 0xffff:
		movq_m2r(s[0], xmm1);
		movq_r2m(xmm1, d[0]);
		break;
	     default:
		movq_m2r(s[0], xmm1);   // xmm1 = [  ][  ][  ][  ][AR][GB][AR][GB] (SRC)
		movq_m2r(d[0], xmm2);   // xmm2 = [  ][  ][  ][  ][ar][gb][ar][gb] (DST)
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
		movq_r2m(xmm1, d[0]);   // DST = xmm1;
		break;
	   }
   }
   e++;
   pxor_r2r(mm4, mm4); // mm4 = 0
   pxor_r2r(mm5, mm5); // mm5 = 0
   pcmpeqd_r2r(mm5, mm5); // mm5 = [FFFF][FFFF][FFFF][FFFF]
   psrlw_i2r(15, mm5); // mm5 = mm5 >> 15; [0001][0001][0001][0001]
   for (; s < e; s++, d++) {
	movd_m2r(s[0], mm1); // mm1 = [  ][  ][AR][GB] (SRC)
	movd_m2r(d[0], mm2); // mm2 = [  ][  ][ar][gb] (DST)
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
	movd_r2m(mm1, d[0]); // DST = mm1;
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_blend_pan_dp_sse2 _op_copy_p_dp_sse2

#define _op_blend_p_dpan_sse2 _op_blend_p_dp_sse2
#define _op_blend_pas_dpan_sse2 _op_blend_pas_dp_sse2
#define _op_blend_pan_dpan_sse2 _op_blend_pan_dp_sse2

#define _op_blend_p_dpas_sse2 _op_blend_p_dp_sse2
#define _op_blend_pas_dpas_sse2 _op_blend_pas_dp_sse2
#define _op_blend_pan_dpas_sse2 _op_blend_pan_dp_sse2
#endif

/*-----*/
#ifdef OP_REG
OPREG(_op_blend_p_dp_sse2, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_N, DP, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_pas_dp_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_N, DP, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_pan_dp_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_N, DP, CPU_SSE2); /* what parameters it does */

OPREG(_op_blend_p_dpan_sse2, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_N, DP_AN, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_pas_dpan_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_N, DP_AN, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_pan_dpan_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_N, DP_AN, CPU_SSE2); /* what parameters it does */

OPREG(_op_blend_p_dpas_sse2, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_N, DP_AS, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_pas_dpas_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_N, DP_AS, CPU_SSE2); /* what parameters it does */
OPREG(_op_blend_pan_dpas_sse2, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_N, DP_AS, CPU_SSE2); /* what parameters it does */
#endif
/*-----*/
