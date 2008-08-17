
/* blend pixel x mask --> dst */

/*-----*/

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_p_mas_dp) {
   DATA32 *s, *d, *e;
   DATA8  *m;
   d = p->dst.p; e = d + p->l;
   s = p->src.p;
   m = p->src.m;

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		a = 256 - (*s >> 24);
		*d = *s + MUL_256(a, *d);
		break;
	    default:
	      {
		DATA32 sm = a + 1;

		sm = MUL_256(sm, *s);
		a = 256 - (sm >> 24);
		*d = sm + MUL_256(a, *d);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_pas_mas_dp) {
   DATA32 *s, *d, *e;
   DATA8  *m;
   d = p->dst.p; e = d + p->l;
   s = p->src.p;
   m = p->src.m;

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
	      {
		DATA32 sa = *s & 0xff000000;

		switch(sa)
		  {
		    case 0:
			break;
		    case 0xff000000:
			*d = *s;
			break;
		    default:
			sa = 256 - (sa >> 24);
			*d = *s + MUL_256(sa, *d);
			break;
		  }
	      }
		break;
	    default:
	      {
		DATA32 sa = *s & 0xff000000;

		switch(sa)
		  {
		    case 0:
			break;
		    case 0xff000000:
			a++;
			*d = INTERP_256(a, *s, *d);
			break;
		    default:
			a++;
			sa = MUL_256(a, *s);
			a = 256 - (sa >> 24);
			*d = sa + MUL_256(a, *d);
			break;
		  }
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_pan_mas_dp) {
   DATA32 *s, *d, *e;
   DATA8  *m;
   d = p->dst.p; e = d + p->l;
   s = p->src.p;
   m = p->src.m;

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		*d = *s;
		break;
	    default:
		a++;
		*d = INTERP_256(a, *s, *d);
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN
#define _op_blend_p_mas_dpan _op_blend_p_mas_dp
#define _op_blend_pas_mas_dpan _op_blend_p_mas_dp
#define _op_blend_pan_mas_dpan _op_blend_p_mas_dp

#define _op_blend_p_mas_dpas _op_blend_p_mas_dp
#define _op_blend_pas_mas_dpas _op_blend_p_mas_dp
#define _op_blend_pan_mas_dpas _op_blend_p_mas_dp
#endif

#ifdef OP_REG
OPREG(_op_blend_p_mas_dp, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_mas_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_mas_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_N, DP, CPU_C); /* what parameters it does */

OPREG(_op_blend_p_mas_dpan, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_mas_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_mas_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_N, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_blend_p_mas_dpas, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_mas_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_mas_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_N, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/
