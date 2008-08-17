
/* blend pixel x mask x color --> dst  (color assumed pre-mul) */

/*-----*/

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_p_mas_c_dp) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
	      {
		DATA32 cs = MUL4_SYM(c, *s);

		a = 256 - (cs >> 24);
		*d = cs + MUL_256(a, *d);
	      }
		break;
	    default:
	      {
		DATA32 cs = MUL4_SYM(c, *s);

		a++;
		cs = MUL_256(a, cs);
		a = 256 - (cs >> 24);
		*d = cs + MUL_256(a, *d);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_pas_mas_c_dp) {
   DATA32 *s, *d, *e, c;
   DATA32 ca0, ca1;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;
   ca0 = c & 0xff000000;
   ca1 = 256 - (ca0 >> 24);

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
			sa = ca0 + MUL3_SYM(c, *s);
			*d = sa + MUL_256(ca1, *d);
			break;
		    default:
			sa = MUL4_SYM(c, *s);
			a = 256 - (sa >> 24);
			*d = sa + MUL_256(a, *d);
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
			sa = ca0 + MUL3_SYM(c, *s);
			a++;
			sa = MUL_256(a, sa);
			a = 256 - (sa >> 24);
			*d = sa + MUL_256(a, *d);
			break;
		    default:
			sa = MUL4_SYM(c, *s);
			a++;
			sa = MUL_256(a, sa);
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
OPFN(_op_blend_pan_mas_c_dp) {
   DATA32 *s, *d, *e, c;
   DATA32 ca0, ca1;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;
   ca0 = c & 0xff000000;
   ca1 = 256 - (ca0 >> 24);

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		a = ca0 + MUL3_SYM(c, *s);
		*d = a + MUL_256(ca1, *d);
		break;
	    default:
	      {
		DATA32 cs = ca0 + MUL3_SYM(c, *s);

		a++;
		cs = MUL_256(a, cs);
		a = 256 - (cs >> 24);
		*d = cs + MUL_256(a, *d);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_p_mas_can_dp) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
	      {
		DATA32 cs = (*s & 0xff000000) + MUL3_SYM(c, *s);

		a = 256 - (cs >> 24);
		*d = cs + MUL_256(a, *d);
	      }
		break;
	    default:
	      {
		DATA32 cs = (*s & 0xff000000) + MUL3_SYM(c, *s);

		a++;
		cs = MUL_256(a, cs);
		a = 256 - (cs >> 24);
		*d = cs + MUL_256(a, *d);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_pas_mas_can_dp) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;

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
			*d = 0xff000000 + MUL3_SYM(c, *s);
			break;
		    default:
			sa += MUL3_SYM(c, *s);
			a = 256 - (sa >> 24);
			*d = sa + MUL_256(a, *d);
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
			sa = 0xff000000 + MUL3_SYM(c, *s);
			a++;
			*d = INTERP_256(a, sa, *d);
			break;
		    default:
			sa += MUL3_SYM(c, *s);
			a++;
			sa = MUL_256(a, sa);
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
OPFN(_op_blend_pan_mas_can_dp) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		*d = 0xff000000 + MUL3_SYM(c, *s);
		break;
	    default:
	      {
		DATA32 cs = 0xff000000 + MUL3_SYM(c, *s);

		a++;
		*d = INTERP_256(a, cs, *d);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_p_mas_caa_dp) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = 1 + (p->src.c & 0xff);

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
	      {
		DATA32 cs = MUL_256(c, *s);

		a = 256 - (cs >> 24);
		*d = cs + MUL_256(c, *d);
	      }
		break;
	    default:
	      {
		DATA32 cs = 1 + ((c * a) >> 8);

		cs = MUL_256(cs, *s);
		a = 256 - (cs >> 24);
		*d = cs + MUL_256(a, *d);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_pas_mas_caa_dp) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = 1 + (p->src.c & 0xff);

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
			*d = INTERP_256(c, *s, *d);
			break;
		    default:
			sa = MUL_256(c, *s);
			a = 256 - (sa >> 24);
			*d = sa + MUL_256(a, *d);
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
			a = 1 + ((c * a) >> 8);
			*d = INTERP_256(a, *s, *d);
			break;
		    default:
			sa = 1 + ((c * a) >> 8);
			sa = MUL_256(sa, *s);
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
OPFN(_op_blend_pan_mas_caa_dp) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = 1 + (p->src.c & 0xff);

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		*d = INTERP_256(c, *s, *d);
		break;
	    default:
		a = 1 + ((c * a) >> 8);
		*d = INTERP_256(a, *s, *d);
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN
#define _op_blend_p_mas_c_dpan _op_blend_p_mas_c_dp
#define _op_blend_pas_mas_c_dpan _op_blend_pas_mas_c_dp
#define _op_blend_pan_mas_c_dpan _op_blend_pan_mas_c_dp
#define _op_blend_p_mas_can_dpan _op_blend_p_mas_can_dp
#define _op_blend_pas_mas_can_dpan _op_blend_pas_mas_can_dp
#define _op_blend_pan_mas_can_dpan _op_blend_pan_mas_can_dp
#define _op_blend_p_mas_caa_dpan _op_blend_p_mas_caa_dp
#define _op_blend_pas_mas_caa_dpan _op_blend_pas_mas_caa_dp
#define _op_blend_pan_mas_caa_dpan _op_blend_pan_mas_caa_dp

#define _op_blend_p_mas_c_dpas _op_blend_p_mas_c_dp
#define _op_blend_pas_mas_c_dpas _op_blend_pas_mas_c_dp
#define _op_blend_pan_mas_c_dpas _op_blend_pan_mas_c_dp
#define _op_blend_p_mas_can_dpas _op_blend_p_mas_can_dp
#define _op_blend_pas_mas_can_dpas _op_blend_pas_mas_can_dp
#define _op_blend_pan_mas_can_dpas _op_blend_pan_mas_can_dp
#define _op_blend_p_mas_caa_dpas _op_blend_p_mas_caa_dp
#define _op_blend_pas_mas_caa_dpas _op_blend_pas_mas_caa_dp
#define _op_blend_pan_mas_caa_dpas _op_blend_pan_mas_caa_dp
#endif

#ifdef OP_REG
OPREG(_op_blend_p_mas_c_dp, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_mas_c_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_mas_c_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_p_mas_can_dp, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_mas_can_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_mas_can_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_p_mas_caa_dp, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_AA, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_mas_caa_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_AA, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_mas_caa_dp, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_AA, DP, CPU_C); /* what parameters it does */

OPREG(_op_blend_p_mas_c_dpan, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_mas_c_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_mas_c_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_p_mas_can_dpan, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_mas_can_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_mas_can_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_p_mas_caa_dpan, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_AA, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_mas_caa_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_AA, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_mas_caa_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_AA, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_blend_p_mas_c_dpas, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_mas_c_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_mas_c_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_p_mas_can_dpas, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_mas_can_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_mas_can_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_p_mas_caa_dpas, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_AA, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pas_mas_caa_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_AA, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_pan_mas_caa_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_AA, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/


