
/* mul pixel --> dst */

/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_mul_p_dp) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l;
   for (; s < e; s++, d++) {
      *d = MUL4_SYM(*s, *d);
   }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_pas_dp) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l;
   while (s < e)
     {
	DATA32 a = *s & 0xff000000;

	switch(a)
	  {
	    case 0:
		*d = 0;
		break;
	    case 0xff000000:
		*d = (*d & 0xff000000) + MUL3_SYM(*s, *d);
		break;
	    default:
		*d = MUL4_SYM(*s, *d);
		break;
	  }
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_pan_dp) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l;
   while (s < e)
     {
	*d = (*d & 0xff000000) + MUL3_SYM(*s, *d);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN
#define _op_mul_p_dpan _op_mul_p_dp
#define _op_mul_pan_dpan _op_mul_pan_dp
#define _op_mul_pas_dpan _op_mul_pas_dp

#define _op_mul_p_dpas _op_mul_p_dp
#define _op_mul_pan_dpas _op_mul_pan_dp
#define _op_mul_pas_dpas _op_mul_pas_dp
#endif

#ifdef OP_REG
OPREG(_op_mul_p_dp, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_dp, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_dp, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC_N, DP, CPU_C); /* what parameters it does */

OPREG(_op_mul_p_dpan, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC_N, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_mul_p_dpas, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC_N, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/
