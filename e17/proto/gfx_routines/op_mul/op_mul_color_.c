
/* mul color --> dst (color assumed pre-mul) */

/*-----*/

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_c_dp) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l; c = p->src.c;
   for (; d < e; d++) {
      *d = MUL4_SYM(c, *d);
   }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_can_dp) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l; c = p->src.c;
   for (; d < e; d++) {
      *d = (*d & 0xff000000) + MUL3_SYM(c, *d);
   }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_caa_dp) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l;
   c = 1 + (p->src.c & 0xff);
   for (; d < e; d++) {
      *d = MUL_256(c, *d);
   }
}
#endif

#ifdef OP_FN
#define _op_mul_c_dpan _op_mul_c_dp
#define _op_mul_can_dpan _op_mul_can_dp
#define _op_mul_caa_dpan _op_mul_caa_dp

#define _op_mul_c_dpas _op_mul_c_dp
#define _op_mul_can_dpas _op_mul_can_dp
#define _op_mul_caa_dpas _op_mul_caa_dp
#endif

#ifdef OP_REG
OPREG(_op_mul_c_dp, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_can_dp, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_caa_dp, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC_AA, DP, CPU_C); /* what parameters it does */

OPREG(_op_mul_c_dpan, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_can_dpan, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_caa_dpan, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC_AA, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_mul_c_dpas, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_can_dpas, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_caa_dpas, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC_AA, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/

