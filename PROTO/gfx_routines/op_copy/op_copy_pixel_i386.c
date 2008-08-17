
/*-----*/
#ifdef OP_FN /* do it using mmx */
OPFN(_op_copy_p_dp_mmx) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l - 15;
   for (; s < e; s+=16, d+=16) {
      MOVE_16DWORDS_MMX(s, d);
   }
   e+=15;
   for (; s < e; s++, d++) {
      *d = *s;
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_copy_pan_dp_mmx _op_copy_p_dp_mmx
#define _op_copy_pas_dp_mmx _op_copy_p_dp_mmx

#define _op_copy_p_dpan_mmx _op_copy_p_dp_mmx
#define _op_copy_pan_dpan_mmx _op_copy_pan_dp_mmx
#define _op_copy_pas_dpan_mmx _op_copy_pas_dp_mmx

#define _op_copy_p_dpas_mmx _op_copy_p_dp_mmx
#define _op_copy_pan_dpas_mmx _op_copy_pan_dp_mmx
#define _op_copy_pas_dpas_mmx _op_copy_pas_dp_mmx
#endif

#ifdef OP_REG
OPREG(_op_copy_p_dp_mmx, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_pan_dp_mmx, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_pas_dp_mmx, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP, CPU_MMX); /* what parameters it does */

OPREG(_op_copy_p_dpan_mmx, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_pan_dpan_mmx, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_pas_dpan_mmx, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP_AN, CPU_MMX); /* what parameters it does */

OPREG(_op_copy_p_dpas_mmx, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_pan_dpas_mmx, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_copy_pas_dpas_mmx, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
#endif
/*-----*/

/*-----*/
#ifdef OP_FN /* do it using sse */
OPFN(_op_copy_p_dp_sse) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l - 15;
   for (; s < e; s+=16, d+=16) {
      MOVE_16DWORDS_MMX2(s, d);
   }
   e+=15;
   for (; s < e; s++, d++) {
      *d = *s;
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_copy_pan_dp_sse _op_copy_p_dp_sse
#define _op_copy_pas_dp_sse _op_copy_p_dp_sse

#define _op_copy_p_dpan_sse _op_copy_p_dp_sse
#define _op_copy_pan_dpan_sse _op_copy_pan_dp_sse
#define _op_copy_pas_dpan_sse _op_copy_pas_dp_sse

#define _op_copy_p_dpas_sse _op_copy_p_dp_sse
#define _op_copy_pan_dpas_sse _op_copy_pan_dp_sse
#define _op_copy_pas_dpas_sse _op_copy_pas_dp_sse
#endif

#ifdef OP_REG
OPREG(_op_copy_p_dp_sse, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_pan_dp_sse, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_pas_dp_sse, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP, CPU_SSE); /* what parameters it does */

OPREG(_op_copy_p_dpan_sse, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP_AN, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_pan_dpan_sse, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP_AN, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_pas_dpan_sse, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP_AN, CPU_SSE); /* what parameters it does */

OPREG(_op_copy_p_dpas_sse, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP_AS, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_pan_dpas_sse, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP_AS, CPU_SSE); /* what parameters it does */
OPREG(_op_copy_pas_dpas_sse, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP_AS, CPU_SSE); /* what parameters it does */
#endif
/*-----*/


/*-----*/
#ifdef OP_FN /* do it using sse2 */
OPFN(_op_copy_p_dp_sse2) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l - 31;
   while ((((unsigned long)((unsigned char *)s)) & 0x1f))
     { *d = *s; d++; s++;}
   e-=31;
   for (; s < e; s+=32, d+=32) {
      MOVE_32DWORDS_ALIGNED_SSE2(s, d);
   }
   e+=31;
   for (; s < e; s++, d++) {
      *d = *s;
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_copy_pan_dp_sse2 _op_copy_p_dp_sse2
#define _op_copy_pas_dp_sse2 _op_copy_p_dp_sse2

#define _op_copy_p_dpan_sse2 _op_copy_p_dp_sse2
#define _op_copy_pan_dpan_sse2 _op_copy_pan_dp_sse2
#define _op_copy_pas_dpan_sse2 _op_copy_pas_dp_sse2

#define _op_copy_p_dpas_sse2 _op_copy_p_dp_sse2
#define _op_copy_pan_dpas_sse2 _op_copy_pan_dp_sse2
#define _op_copy_pas_dpas_sse2 _op_copy_pas_dp_sse2
#endif

#ifdef OP_REG
OPREG(_op_copy_p_dp_sse2, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_pan_dp_sse2, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_pas_dp_sse2, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP, CPU_SSE2); /* what parameters it does */

OPREG(_op_copy_p_dpan_sse2, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP_AN, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_pan_dpan_sse2, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP_AN, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_pas_dpan_sse2, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP_AN, CPU_SSE2); /* what parameters it does */

OPREG(_op_copy_p_dpas_sse2, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP_AS, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_pan_dpas_sse2, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP_AS, CPU_SSE2); /* what parameters it does */
OPREG(_op_copy_pas_dpas_sse2, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP_AS, CPU_SSE2); /* what parameters it does */
#endif
/*-----*/
