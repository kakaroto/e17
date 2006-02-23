
/* mul color --> dst (color assumed pre-mul) */

/*-----*/

#ifdef OP_FN /* do it in mmx */
OPFN(_op_mul_c_dp_mmx) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l; c = p->src.c;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_FULL, mm6, mm0)
   MOV_P2R(c, mm2, mm0)
   for (; d < e; d++) {
	MOV_P2R(*d, mm1, mm0)
	MUL4_SYM_R2R(mm2, mm1, mm6)
	MOV_R2P(mm1, *d, mm0)
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in mmx */
OPFN(_op_mul_caa_dp_mmx) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l;
   c = 1 + (p->src.c & 0xff);

   pxor_r2r(mm0, mm0);
   MOV_A2R(c, mm2)
   for (; d < e; d++) {
	MOV_P2R(*d, mm1, mm0)
	MUL4_256_R2R(mm2, mm1)
	MOV_R2P(mm1, *d, mm0)
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_mul_can_dp_mmx _op_mul_c_dp_mmx

#define _op_mul_c_dpan_mmx _op_mul_c_dp_mmx
#define _op_mul_can_dpan_mmx _op_mul_can_dp_mmx
#define _op_mul_caa_dpan_mmx _op_mul_caa_dp_mmx

#define _op_mul_c_dpas_mmx _op_mul_c_dp_mmx
#define _op_mul_can_dpas_mmx _op_mul_can_dp_mmx
#define _op_mul_caa_dpas_mmx _op_mul_caa_dp_mmx
#endif

#ifdef OP_REG
OPREG(_op_mul_c_dp_mmx, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_can_dp_mmx, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC_AN, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_caa_dp_mmx, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC_AA, DP, CPU_MMX); /* what parameters it does */

OPREG(_op_mul_c_dpan_mmx, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_can_dpan_mmx, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC_AN, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_caa_dpan_mmx, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC_AA, DP_AN, CPU_MMX); /* what parameters it does */

OPREG(_op_mul_c_dpas_mmx, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_can_dpas_mmx, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC_AN, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_caa_dpas_mmx, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_N, SC_AA, DP_AS, CPU_MMX); /* what parameters it does */
#endif
/*-----*/

