
/* mul pixel --> dst */

/*-----*/
#ifdef OP_FN /* do it in mmx */
OPFN(_op_mul_p_dp_mmx) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_FULL, mm6, mm0)
   for (; s < e; s++, d++) {
	MOV_P2R(*d, mm1, mm0)
	MOV_P2R(*s, mm2, mm0)
	MUL4_SYM_R2R(mm2, mm1, mm6)
	MOV_R2P(mm1, *d, mm0)
   }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in mmx */
OPFN(_op_mul_pas_dp_mmx) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_FULL, mm6, mm0)
   while (s < e)
     {
	switch(*s)
	  {
	    case 0:
		*d = 0;
		break;
	    default:
		MOV_P2R(*d, mm1, mm0)
		MOV_P2R(*s, mm2, mm0)
		MUL4_SYM_R2R(mm2, mm1, mm6)
		MOV_R2P(mm1, *d, mm0)
		break;
	  }
	s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_mul_pan_dp_mmx _op_mul_p_dp_mmx

#define _op_mul_p_dpan_mmx _op_mul_p_dp_mmx
#define _op_mul_pan_dpan_mmx _op_mul_pan_dp_mmx
#define _op_mul_pas_dpan_mmx _op_mul_pas_dp_mmx

#define _op_mul_p_dpas_mmx _op_mul_p_dp_mmx
#define _op_mul_pan_dpas_mmx _op_mul_pan_dp_mmx
#define _op_mul_pas_dpas_mmx _op_mul_pas_dp_mmx
#endif

#ifdef OP_REG
OPREG(_op_mul_p_dp_mmx, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC_N, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_pan_dp_mmx, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC_N, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_pas_dp_mmx, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC_N, DP, CPU_MMX); /* what parameters it does */

OPREG(_op_mul_p_dpan_mmx, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC_N, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_pan_dpan_mmx, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC_N, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_pas_dpan_mmx, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC_N, DP_AN, CPU_MMX); /* what parameters it does */

OPREG(_op_mul_p_dpas_mmx, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_pan_dpas_mmx, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_pas_dpas_mmx, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
#endif
/*-----*/
