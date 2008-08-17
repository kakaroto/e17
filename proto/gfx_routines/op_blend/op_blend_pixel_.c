
/* blend pixel --> dst */

/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_blend_p_dp) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = d + p->l;

   while (d < e)
     {
	DATA32 a = 256 - (*s >> 24);

	*d = *s + MUL_256(a, *d);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_pas_dp) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = d + p->l;

   while (d < e)
     {
	DATA32 a = *s & 0xff000000;

	switch(a)
	  {
	     case 0:
		break;
	     case 0xff000000:
		*d = *s;
		break;
	     default:
		a = 256 - (a >> 24);
		*d = *s + MUL_256(a, *d);
		break;
	  }
	s++;  d++;
     }
}
#endif


#ifdef OP_FN
#define _op_blend_pan_dp _op_copy_p_dp

#define _op_blend_p_dpan _op_blend_p_dp
#define _op_blend_pas_dpan _op_blend_pas_dp
#define _op_blend_pan_dpan _op_blend_pan_dp

#define _op_blend_p_dpas _op_blend_p_dp
#define _op_blend_pas_dpas _op_blend_pas_dp
#define _op_blend_pan_dpas _op_blend_pan_dp
#endif


#ifdef OP_REG
OPREG(_op_blend_p_dp, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_N, DP, CPU_C); /* what parameters it does */

OPREG(_op_blend_p_dpan, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_N, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_blend_p_dpas, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_N, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/

