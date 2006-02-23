
/* mul pixel x mask x color --> dst  (color assumed pre-mul) */

/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_mul_p_mas_c_dp) {
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
		*d = 0;
		break;
	    case 255:
		a = MUL4_SYM(c, *s);
		*d = MUL4_SYM(a, *d);
		break;
	    default:
	      {
		DATA32 cs = MUL4_SYM(c, *s);

		a++;
		a = MUL_256(a, cs);
		*d = MUL4_SYM(a, *d);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_pas_mas_c_dp) {
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
		*d = 0;
		break;
	    case 255:
	      {
		DATA32 sa = *s & 0xff000000;

		switch(sa)
		  {
		    case 0:
			*d = 0;
			break;
		    case 0xff000000:
			sa = (c & 0xff000000) + MUL3_SYM(c, *s);
			*d = MUL4_SYM(sa, *d);
			break;
		    default:
			sa = MUL4_SYM(c, *s);
			*d = MUL4_SYM(sa, *d);
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
			*d = 0;
			break;
		    case 0xff000000:
			sa = (c & 0xff000000) + MUL3_SYM(c, *s);
			a++;
			sa = MUL_256(a, sa);
			*d = MUL4_SYM(sa, *d);
			break;
		    default:
			sa = MUL4_SYM(c, *s);
			a++;
			sa = MUL_256(a, sa);
			*d = MUL4_SYM(sa, *d);
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
OPFN(_op_mul_pan_mas_c_dp) {
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
		*d = 0;
		break;
	    case 255:
		a = (c & 0xff000000) + MUL3_SYM(c, *s);
		*d = MUL4_SYM(a, *d);
		break;
	    default:
	      {
		DATA32 cs = (c & 0xff000000) + MUL3_SYM(c, *s);

		a++;
		cs = MUL_256(a, cs);
		*d = MUL4_SYM(cs, *d);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_p_mas_can_dp) {
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
		*d = 0;
		break;
	    case 255:
		a = (*s & 0xff000000) + MUL3_SYM(c, *s);
		*d = MUL4_SYM(a, *d);
		break;
	    default:
	      {
		DATA32 cs = (*s & 0xff000000) + MUL3_SYM(c, *s);

		a++;
		cs = MUL_256(a, cs);
		*d = MUL4_SYM(cs, *d);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_pas_mas_can_dp) {
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
		*d = 0;
		break;
	    case 255:
	      {
		DATA32 sa = *s & 0xff000000;

		switch(sa)
		  {
		    case 0:
			*d = 0;
			break;
		    case 0xff000000:
			sa += MUL3_SYM(c, *s);
			*d = (*d & 0xff000000) + MUL3_SYM(sa, *d);
			break;
		    default:
			sa += MUL3_SYM(c, *s);
			*d = MUL4_SYM(sa, *d);
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
			*d = 0;
			break;
		    case 0xff000000:
			sa += MUL3_SYM(c, *s);
			a++;
			sa = MUL_256(a, sa);
			*d = MUL4_SYM(sa, *d);
			break;
		    default:
			sa += MUL3_SYM(c, *s);
			a++;
			sa = MUL_256(a, sa);
			*d = MUL4_SYM(sa, *d);
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
OPFN(_op_mul_pan_mas_can_dp) {
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
		*d = 0;
		break;
	    case 255:
		a = 0xff000000 + MUL3_SYM(c, *s);
		*d = (*d & 0xff000000) + MUL3_SYM(a, *d);
		break;
	    default:
	      {
		DATA32 cs = 0xff000000 + MUL3_SYM(c, *s);

		a++;
		cs = MUL_256(a, cs);
		*d = MUL4_SYM(cs, *d);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_p_mas_caa_dp) {
   DATA32 *s, *d, *e, c, ca;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;
   ca = 1 + (c & 0xff);

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		*d = 0;
		break;
	    case 255:
		a = MUL_256(ca, *s);
		*d = MUL4_SYM(a, *d);
		break;
	    default:
		a = 1 + ((ca * a) >> 8);
		a = MUL_256(a, *s);
		*d = MUL4_SYM(a, *d);
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_pas_mas_caa_dp) {
   DATA32 *s, *d, *e, c, ca;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;
   ca = 1 + (c & 0xff);

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		*d = 0;
		break;
	    case 255:
		switch(*s)
		  {
		    case 0:
			*d = 0;
			break;
		    default:
			a = MUL_256(ca, *s);
			*d = MUL4_SYM(a, *d);
			break;
		  }
		break;
	    default:
		switch(*s)
		  {
		    case 0:
			*d = 0;
			break;
		    default:
			a = 1 + ((ca * a) >> 8);
			a = MUL_256(a, *s);
			*d = MUL4_SYM(a, *d);
			break;
		  }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_pan_mas_caa_dp) {
   DATA32 *s, *d, *e, c, ca;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;
   ca = 1 + (c & 0xff);

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		*d = 0;
		break;
	    case 255:
		a = MUL_256(ca, *s);
		*d = MUL4_SYM(a, *d);
		break;
	    default:
		a = 1 + ((ca * a) >> 8);
		a = MUL_256(a, *s);
		*d = MUL4_SYM(a, *d);
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN
#define _op_mul_p_mas_c_dpan _op_mul_p_mas_c_dp
#define _op_mul_pas_mas_c_dpan _op_mul_pas_mas_c_dp
#define _op_mul_pan_mas_c_dpan _op_mul_pan_mas_c_dp
#define _op_mul_p_mas_can_dpan _op_mul_p_mas_can_dp
#define _op_mul_pas_mas_can_dpan _op_mul_pas_mas_can_dp
#define _op_mul_pan_mas_can_dpan _op_mul_pan_mas_can_dp
#define _op_mul_p_mas_caa_dpan _op_mul_p_mas_caa_dp
#define _op_mul_pas_mas_caa_dpan _op_mul_pas_mas_caa_dp
#define _op_mul_pan_mas_caa_dpan _op_mul_pan_mas_caa_dp

#define _op_mul_p_mas_c_dpas _op_mul_p_mas_c_dp
#define _op_mul_pas_mas_c_dpas _op_mul_pas_mas_c_dp
#define _op_mul_pan_mas_c_dpas _op_mul_pan_mas_c_dp
#define _op_mul_p_mas_can_dpas _op_mul_p_mas_can_dp
#define _op_mul_pas_mas_can_dpas _op_mul_pas_mas_can_dp
#define _op_mul_pan_mas_can_dpas _op_mul_pan_mas_can_dp
#define _op_mul_p_mas_caa_dpas _op_mul_p_mas_caa_dp
#define _op_mul_pas_mas_caa_dpas _op_mul_pas_mas_caa_dp
#define _op_mul_pan_mas_caa_dpas _op_mul_pan_mas_caa_dp
#endif

#ifdef OP_REG
OPREG(_op_mul_p_mas_c_dp, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_mas_c_dp, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_mas_c_dp, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_p_mas_can_dp, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_mas_can_dp, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_mas_can_dp, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_p_mas_caa_dp, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC_AA, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_mas_caa_dp, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC_AA, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_mas_caa_dp, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC_AA, DP, CPU_C); /* what parameters it does */

OPREG(_op_mul_p_mas_c_dpan, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_mas_c_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_mas_c_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_p_mas_can_dpan, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_mas_can_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_mas_can_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_p_mas_caa_dpan, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC_AA, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_mas_caa_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC_AA, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_mas_caa_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC_AA, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_mul_p_mas_c_dpas, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_mas_c_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_mas_c_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_p_mas_can_dpas, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_mas_can_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_mas_can_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_p_mas_caa_dpas, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC_AA, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_mas_caa_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC_AA, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_mas_caa_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC_AA, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/

