/*-----*/
#ifdef OP_FN /* do it in MMX */
OPFN(_op_copy_c_dp_mmx) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l - 1; c = p->src.c;
   movd_m2r(c, mm1);
   movq_r2r(mm1, mm2);
   psllq_i2r(32, mm1);
   por_r2r(mm2, mm1);
   for (; d < e; d+=2) {
      movq_r2m(mm1, d[0]);
   }
   e+=1;
   for (; d < e; d++) {
      *d = c;
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_copy_cn_dp_mmx _op_copy_c_dp_mmx
#define _op_copy_can_dp_mmx _op_copy_c_dp_mmx
#define _op_copy_caa_dp_mmx _op_copy_c_dp_mmx

#define _op_copy_cn_dpan_mmx _op_copy_c_dp_mmx
#define _op_copy_c_dpan_mmx _op_copy_c_dp_mmx
#define _op_copy_can_dpan_mmx _op_copy_c_dp_mmx
#define _op_copy_caa_dpan_mmx _op_copy_c_dp_mmx

#define _op_copy_cn_dpas_mmx _op_copy_c_dp_mmx
#define _op_copy_c_dpas_mmx _op_copy_c_dp_mmx
#define _op_copy_can_dpas_mmx _op_copy_c_dp_mmx
#define _op_copy_caa_dpas_mmx _op_copy_c_dp_mmx
#endif

#ifdef OP_REG
OPREG(_op_copy_cn_dp_mmx, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_c_dp_mmx, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_can_dp_mmx, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_caa_dp_mmx, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP, CPU_MMX); /* what parameters it does */

OPREG(_op_copy_cn_dpan_mmx, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_c_dpan_mmx, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_can_dpan_mmx, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_caa_dpan_mmx, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP_AN, CPU_MMX); /* what parameters it does */

OPREG(_op_copy_cn_dpas_mmx, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_c_dpas_mmx, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_can_dpas_mmx, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_caa_dpas_mmx, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP_AS, CPU_MMX); /* what parameters it does */
#endif
/*-----*/

/*-----*/
#ifdef OP_FN /* do it in SSE */
OPFN(_op_copy_c_dp_sse) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l - 1; c = p->src.c;
   movd_m2r(c, mm1);
   pshufw(mm1, mm1, 0x44);
   for (; d < e; d+=2) {
      movntq_r2m(mm1, d[0]);
   }
   e+=1;
   for (; d < e; d++) {
      *d = c;
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_copy_cn_dp_sse _op_copy_c_dp_sse
#define _op_copy_can_dp_sse _op_copy_c_dp_sse
#define _op_copy_caa_dp_sse _op_copy_c_dp_sse

#define _op_copy_cn_dpan_sse _op_copy_c_dp_sse
#define _op_copy_c_dpan_sse _op_copy_c_dp_sse
#define _op_copy_can_dpan_sse _op_copy_c_dp_sse
#define _op_copy_caa_dpan_sse _op_copy_c_dp_sse

#define _op_copy_cn_dpas_sse _op_copy_c_dp_sse
#define _op_copy_c_dpas_sse _op_copy_c_dp_sse
#define _op_copy_can_dpas_sse _op_copy_c_dp_sse
#define _op_copy_caa_dpas_sse _op_copy_c_dp_sse
#endif

#ifdef OP_REG
OPREG(_op_copy_cn_dp_sse, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_c_dp_sse, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_can_dp_sse, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_caa_dp_sse, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP, CPU_SSE); /* what parameters it does */

OPREG(_op_copy_cn_dpan_sse, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP_AN, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_c_dpan_sse, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP_AN, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_can_dpan_sse, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP_AN, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_caa_dpan_sse, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP_AN, CPU_SSE); /* what parameters it does */

OPREG(_op_copy_cn_dpas_sse, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP_AS, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_c_dpas_sse, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP_AS, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_can_dpas_sse, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP_AS, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_caa_dpas_sse, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP_AS, CPU_SSE); /* what parameters it does */
#endif
/*-----*/


/*-----*/
#ifdef OP_FN /* do it in SSE2 */
OPFN(_op_copy_c_dp_sse2) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l - 3; c = p->src.c;
   movd_m2r(c, xmm1);
   pshufd(xmm1, xmm1, 0x00);
   while ((((unsigned long)((unsigned char *)d)) & 0x1f))
     { *d = c; d++;}
   for (; d < e; d+=4) {
      movntdq_r2m(xmm1, d[0]);
   }
   e+=3;
   for (; d < e; d++) {
      *d = c;
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_copy_cn_dp_sse2 _op_copy_c_dp_sse2
#define _op_copy_can_dp_sse2 _op_copy_c_dp_sse2
#define _op_copy_caa_dp_sse2 _op_copy_c_dp_sse2

#define _op_copy_cn_dpan_sse2 _op_copy_c_dp_sse2
#define _op_copy_c_dpan_sse2 _op_copy_c_dp_sse2
#define _op_copy_can_dpan_sse2 _op_copy_c_dp_sse2
#define _op_copy_caa_dpan_sse2 _op_copy_c_dp_sse2

#define _op_copy_cn_dpas_sse2 _op_copy_c_dp_sse2
#define _op_copy_c_dpas_sse2 _op_copy_c_dp_sse2
#define _op_copy_can_dpas_sse2 _op_copy_c_dp_sse2
#define _op_copy_caa_dpas_sse2 _op_copy_c_dp_sse2
#endif

#ifdef OP_REG
OPREG(_op_copy_cn_dp_sse2, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_c_dp_sse2, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_can_dp_sse2, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_caa_dp_sse2, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP, CPU_SSE2); /* what parameters it does */

OPREG(_op_copy_cn_dpan_sse2, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP_AN, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_c_dpan_sse2, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP_AN, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_can_dpan_sse2, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP_AN, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_caa_dpan_sse2, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP_AN, CPU_SSE2); /* what parameters it does */

OPREG(_op_copy_cn_dpas_sse2, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP_AS, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_c_dpas_sse2, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP_AS, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_can_dpas_sse2, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP_AS, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_caa_dpas_sse2, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP_AS, CPU_SSE2); /* what parameters it does */
#endif
/*-----*/
