

/* blend pixel x color --> dst  (color assumed pre-mul) */

/*-----*/

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_p_c_dp) {
   DATA32 *s, *d, *e, c;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = p->src.c;

   while (d < e)
     {
	DATA32 sc = MUL4_SYM(c, *s);
	DATA32 a = 256 - (sc >> 24);

	*d = sc + MUL_256(a, *d);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_pas_c_dp) {
   DATA32 *s, *d, *e, c;
   DATA32 ca0, ca1;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = p->src.c;
   ca0 = c & 0xff000000;
   ca1 = 256 - (ca0 >> 24);

   while (d < e)
     {
	DATA32 a = *s & 0xff000000;

	switch(a)
	  {
	    case 0:
		break;
	    case 0xff000000:
	      {
		a = ca0 + MUL3_SYM(c, *s);
		*d = a + MUL_256(ca1, *d);
	      }
		break;
	    default:
	      {
		DATA32 sc = MUL4_SYM(c, *s);

		a = 256 - (sc >> 24);
		*d = sc + MUL_256(a, *d);
	      }
		break;
	  }
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_pan_c_dp) {
   DATA32 *s, *d, *e, c;
   DATA32 ca0, ca1;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = p->src.c;
   ca0 = c & 0xff000000;
   ca1 = 256 - (ca0 >> 24);

   while (d < e)
     {
	*d = MUL_256(ca1, *d) + 
	     ca0 + MUL3_SYM(c, *s);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_p_can_dp) {
   DATA32 *s, *d, *e, c;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = p->src.c;

   while (d < e)
     {
	DATA32 sc = (*s & 0xff000000) + MUL3_SYM(c, *s);
	DATA32 a = 256 - (sc >> 24);

	*d = sc + MUL_256(a, *d);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_pas_can_dp) {
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
		break;
	    case 0xff000000:
		*d = 0xff000000 + MUL3_SYM(c, *s);
		break;
	    default:
	      {
		DATA32 sc = a + MUL3_SYM(c, *s);

		a = 256 - (a >> 24);
		*d = sc + MUL_256(a, *d);
	      }
		break;
	  }
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_pan_can_dp) {
   DATA32 *s, *d, *e, c;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   c = p->src.c;

   while (d < e)
     {
	*d = 0xff000000 + MUL3_SYM(c, *s);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_p_caa_dp) {
   DATA32 *s, *d, *e, ca;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   ca = 1 + (p->src.c & 0xff);

  while (d < e)
     {
	DATA32 sc = MUL_256(ca, *s);
	DATA32 a = 256 - (sc >> 24);

	*d = sc + MUL_256(a, *d);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_pas_caa_dp) {
   DATA32 *s, *d, *e, ca;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   ca = 1 + (p->src.c & 0xff);

   while (d < e)
     {
	DATA32 a = *s & 0xff000000;

	switch(a)
	  {
	    case 0:
		break;
	    case 0xff000000:
		*d = INTERP_256(ca, *s, *d);
		break;
	    default:
	      {
		DATA32 sc = MUL_256(ca, *s);

		a = 256 - (sc >> 24);
		*d = sc + MUL_256(a, *d);
	      }
		break;
	  }
	s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_pan_caa_dp) {
   DATA32 *s, *d, *e, ca;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   ca = 1 + ((p->src.c) & 0xff);

   while (d < e)
     {
	*d = INTERP_256(ca, *s, *d);
	s++;  d++;
     }
}
#endif

#ifdef OP_FN
#define _op_blend_p_c_dpan _op_blend_p_c_dp
#define _op_blend_pas_c_dpan _op_blend_pas_c_dp
#define _op_blend_pan_c_dpan _op_blend_pan_c_dp
#define _op_blend_p_can_dpan _op_blend_p_can_dp
#define _op_blend_pas_can_dpan _op_blend_pas_can_dp
#define _op_blend_pan_can_dpan _op_blend_pan_can_dp
#define _op_blend_p_caa_dpan _op_blend_p_caa_dp
#define _op_blend_pas_caa_dpan _op_blend_pas_caa_dp
#define _op_blend_pan_caa_dpan _op_blend_pan_caa_dp

#define _op_blend_p_c_dpas _op_blend_p_c_dp
#define _op_blend_pas_c_dpas _op_blend_pas_c_dp
#define _op_blend_pan_c_dpas _op_blend_pan_c_dp
#define _op_blend_p_can_dpas _op_blend_p_can_dp
#define _op_blend_pas_can_dpas _op_blend_pas_can_dp
#define _op_blend_pan_can_dpas _op_blend_pan_can_dp
#define _op_blend_p_caa_dpas _op_blend_p_caa_dp
#define _op_blend_pas_caa_dpas _op_blend_pas_caa_dp
#define _op_blend_pan_caa_dpas _op_blend_pan_caa_dp
#endif

#ifdef OP_REG
OPREG(_op_blend_p_c_dp, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_c_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_c_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_p_can_dp, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_can_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_can_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_p_caa_dp, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_AA, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_caa_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_AA, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_caa_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_AA, DP, CPU_C); /* what parameters it does */

OPREG(_op_blend_p_c_dpan, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_c_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_c_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_p_can_dpan, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_can_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_can_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_p_caa_dpan, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_AA, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_caa_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_AA, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_caa_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_AA, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_blend_p_c_dpas, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_c_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_c_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_p_can_dpas, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_can_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_can_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_p_caa_dpas, PIXEL_OP_BLEND, /* register the function */
      SP, SM_N, SC_AA, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_caa_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_N, SC_AA, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_caa_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_N, SC_AA, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/
