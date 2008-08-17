
/* mul pixel x color --> dst  (color assumed pre-mul) */

/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_mul_p_c_dp) {
   DATA32 *s, *d, *e, c;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = p->src.c;

   while (d < e)
     {
	DATA32  cs = MUL4_SYM(c, *s);

	*d = MUL4_SYM(cs, *d);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_pas_c_dp) {
   DATA32 *s, *d, *e, c;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = p->src.c;

   while (d < e)
     {
	DATA32 a = *s & 0xff000000;

	switch(a)
	  {
	    case 0:
		*d = 0;
		break;
	    case 0xff000000:
		a = (c & 0xff000000) + MUL3_SYM(c, *s);
		*d = MUL4_SYM(a, *d);
		break;
	    default:
		a = MUL4_SYM(c, *s);
		*d = MUL4_SYM(a, *d);
		break;
	  }
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_pan_c_dp) {
   DATA32 *s, *d, *e, c;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = p->src.c;

   while (d < e)
     {
	DATA32 cs = (c & 0xff000000) + MUL3_SYM(c, *s);

	*d = MUL4_SYM(cs, *d);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_p_can_dp) {
   DATA32 *s, *d, *e, c;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = p->src.c;

   while (d < e)
     {
	DATA32 cs = (*s & 0xff000000) + MUL3_SYM(c, *s);

	*d = MUL4_SYM(cs, *d);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_pas_can_dp) {
   DATA32 *s, *d, *e, c;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = p->src.c;

   while (d < e)
     {
	DATA32 a = *s & 0xff000000;

	switch(a)
	  {
	    case 0:
		*d = 0;
		break;
	    case 0xff000000:
		a += MUL3_SYM(c, *s);
		*d = (*d & 0xff000000) + MUL3_SYM(a, *d);
		break;
	    default:
		a += MUL3_SYM(c, *s);
		*d = MUL4_SYM(a, *d);
		break;
	  }
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_pan_can_dp) {
   DATA32 *s, *d, *e, c;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = p->src.c;

   while (d < e)
     {
	DATA32 cs = 0xff000000 + MUL3_SYM(c, *s);

	*d = (*d & 0xff000000) + MUL3_SYM(cs, *d);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_p_caa_dp) {
   DATA32 *s, *d, *e, c;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = 1 + (p->src.c & 0xff);

   while (d < e)
     {
	DATA32 cs = MUL_256(c, *s);

	*d = MUL4_SYM(cs, *d);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_pan_caa_dp) {
   DATA32 *s, *d, *e, c;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = 1 + (p->src.c & 0xff);

   while (d < e)
     {
	DATA32 cs = MUL_256(c, *s);

	*d = MUL4_SYM(cs, *d);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN
#define _op_mul_pas_caa_dp _op_mul_p_caa_dp

#define _op_mul_p_c_dpan _op_mul_p_c_dp
#define _op_mul_pan_c_dpan _op_mul_pan_c_dp
#define _op_mul_pas_c_dpan _op_mul_pas_c_dp
#define _op_mul_p_can_dpan _op_mul_p_can_dp
#define _op_mul_pan_can_dpan _op_mul_pan_can_dp
#define _op_mul_pas_can_dpan _op_mul_pas_can_dp
#define _op_mul_p_caa_dpan _op_mul_p_caa_dp
#define _op_mul_pan_caa_dpan _op_mul_pan_caa_dp
#define _op_mul_pas_caa_dpan _op_mul_pas_caa_dp

#define _op_mul_p_c_dpas _op_mul_p_c_dp
#define _op_mul_pan_c_dpas _op_mul_pan_c_dp
#define _op_mul_pas_c_dpas _op_mul_pas_c_dp
#define _op_mul_p_can_dpas _op_mul_p_can_dp
#define _op_mul_pan_can_dpas _op_mul_pan_can_dp
#define _op_mul_pas_can_dpas _op_mul_pas_can_dp
#define _op_mul_p_caa_dpas _op_mul_p_caa_dp
#define _op_mul_pan_caa_dpas _op_mul_pan_caa_dp
#define _op_mul_pas_caa_dpas _op_mul_pas_caa_dp
#endif

#ifdef OP_REG
OPREG(_op_mul_p_c_dp, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_c_dp, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_c_dp, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_p_can_dp, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_can_dp, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_can_dp, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_p_caa_dp, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC_AA, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_caa_dp, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC_AA, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_caa_dp, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC_AA, DP, CPU_C); /* what parameters it does */

OPREG(_op_mul_p_c_dpan, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_c_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_c_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_p_can_dpan, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_can_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_can_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_p_caa_dpan, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC_AA, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_caa_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC_AA, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_caa_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC_AA, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_mul_p_c_dpas, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_c_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_c_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_p_can_dpas, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_can_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_can_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_p_caa_dpas, PIXEL_OP_MUL, /* register the function */
      SP, SM_N, SC_AA, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_caa_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_N, SC_AA, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_caa_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_N, SC_AA, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/

