
/* blend color -> dst  (color assumed pre-mul) */

/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_blend_c_dp) {
   DATA32 *d, *e, c, a;

   d = p->dst.p; e = d + p->l;
   c = p->src.c; a = 256 - (c >> 24);

   while (d < e)
	*d++ = c + MUL_256(a, *d);
}
#endif


#ifdef OP_FN
#define _op_blend_cn_dp _op_copy_cn_dp
#define _op_blend_can_dp _op_copy_can_dp
#define _op_blend_caa_dp _op_blend_c_dp

#define _op_blend_c_dpan _op_blend_c_dp
#define _op_blend_cn_dpan _op_blend_cn_dp
#define _op_blend_can_dpan _op_blend_can_dp
#define _op_blend_caa_dpan _op_blend_c_dp

#define _op_blend_c_dpas _op_blend_c_dp
#define _op_blend_cn_dpas _op_blend_cn_dp
#define _op_blend_can_dpas _op_blend_can_dp
#define _op_blend_caa_dpas _op_blend_c_dp
#endif

#ifdef OP_REG
OPREG(_op_blend_c_dp, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_cn_dp, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_can_dp, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_caa_dp, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AA, DP, CPU_C); /* what parameters it does */

OPREG(_op_blend_c_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_cn_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_can_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_caa_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AA, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_blend_c_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_cn_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_can_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_caa_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_N, SC_AA, DP_AS, CPU_C); /* what parameters it does */
#endif
