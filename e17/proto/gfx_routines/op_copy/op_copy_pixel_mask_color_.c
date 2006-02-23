
/* copy pixel x mask x color --> dst  (color assumed pre-mul) */

/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_copy_p_mas_c_dp) {
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
		*d = MUL4_SYM(c, *s);
		break;
	    default:
	      {
		DATA32 cs = MUL4_SYM(c, *s);

		a++;
		*d = MUL_256(a, cs);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_copy_pan_mas_c_dp) {
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
		*d = (c & 0xff000000) + MUL3_SYM(c, *s);
		break;
	    default:
	      {
		DATA32 cs = (c & 0xff000000) + MUL3_SYM(c, *s);

		a++;
		*d = MUL_256(a, cs);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_copy_pas_mas_c_dp) {
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
			*d = (c & 0xff000000) + MUL3_SYM(c, *s);
			break;
		    default:
			*d = MUL4_SYM(c, *s);
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
			*d = MUL_256(a, sa);
			break;
		    default:
			sa = MUL4_SYM(c, *s);
			a++;
			*d = MUL_256(a, sa);
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
OPFN(_op_copy_p_mas_can_dp) {
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
		*d = (*s & 0xff000000) + MUL3_SYM(c, *s);
		break;
	    default:
	      {
		DATA32 cs = (*s & 0xff000000) + MUL3_SYM(c, *s);

		a++;
		*d = MUL_256(a, cs);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_copy_pan_mas_can_dp) {
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
		*d = 0xff000000 + MUL3_SYM(c, *s);
		break;
	    default:
	      {
		DATA32 cs = 0xff000000 + MUL3_SYM(c, *s);

		a++;
		*d = MUL_256(a, cs);
	      }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_copy_pas_mas_can_dp) {
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
			*d = 0xff000000 + MUL3_SYM(c, *s);
			break;
		    default:
			*d = sa + MUL3_SYM(c, *s);
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
			sa = 0xff000000 + MUL3_SYM(c, *s);
			a++;
			*d = MUL_256(a, sa);
			break;
		    default:
			sa += MUL3_SYM(c, *s);
			a++;
			*d = MUL_256(a, sa);
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
OPFN(_op_copy_p_mas_caa_dp) {
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
		*d = MUL_256(ca, *s);
		break;
	    default:
		a = 1 + ((ca * a) >> 8);
		*d = MUL_256(a, *s);
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_copy_pan_mas_caa_dp) {
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
		*d = MUL_256(ca, *s);
		break;
	    default:
		a = 1 + ((ca * a) >> 8);
		*d = MUL_256(a, *s);
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_copy_pas_mas_caa_dp) {
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
			*d = MUL_256(ca, *s);
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
			*d = MUL_256(a, *s);
			break;
		  }
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN
#define _op_copy_p_mas_c_dpan _op_copy_p_mas_c_dp
#define _op_copy_pan_mas_c_dpan _op_copy_pan_mas_c_dp
#define _op_copy_pas_mas_c_dpan _op_copy_pas_mas_c_dp

#define _op_copy_p_mas_can_dpan _op_copy_p_mas_can_dp
#define _op_copy_pan_mas_can_dpan _op_copy_pan_mas_can_dp
#define _op_copy_pas_mas_can_dpan _op_copy_pas_mas_can_dp

#define _op_copy_p_mas_caa_dpan _op_copy_p_mas_caa_dp
#define _op_copy_pan_mas_caa_dpan _op_copy_pan_mas_caa_dp
#define _op_copy_pas_mas_caa_dpan _op_copy_pas_mas_caa_dp

#define _op_copy_p_mas_c_dpas _op_copy_p_mas_c_dp
#define _op_copy_pan_mas_c_dpas _op_copy_pan_mas_c_dp
#define _op_copy_pas_mas_c_dpas _op_copy_pas_mas_c_dp

#define _op_copy_p_mas_can_dpas _op_copy_p_mas_can_dp
#define _op_copy_pan_mas_can_dpas _op_copy_pan_mas_can_dp
#define _op_copy_pas_mas_can_dpas _op_copy_pas_mas_can_dp

#define _op_copy_p_mas_caa_dpas _op_copy_p_mas_caa_dp
#define _op_copy_pan_mas_caa_dpas _op_copy_pan_mas_caa_dp
#define _op_copy_pas_mas_caa_dpas _op_copy_pas_mas_caa_dp
#endif

#ifdef OP_REG
OPREG(_op_copy_p_mas_c_dp, PIXEL_OP_COPY, /* register the function */
      SP, SM_AS, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_pan_mas_c_dp, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_AS, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_pas_mas_c_dp, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_AS, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_p_mas_can_dp, PIXEL_OP_COPY, /* register the function */
      SP, SM_AS, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_pan_mas_can_dp, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_AS, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_pas_mas_can_dp, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_AS, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_p_mas_caa_dp, PIXEL_OP_COPY, /* register the function */
      SP, SM_AS, SC_AA, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_pan_mas_caa_dp, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_AS, SC_AA, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_pas_mas_caa_dp, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_AS, SC_AA, DP, CPU_C); /* what parameters it does */

OPREG(_op_copy_p_mas_c_dpan, PIXEL_OP_COPY, /* register the function */
      SP, SM_AS, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_pan_mas_c_dpan, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_AS, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_pas_mas_c_dpan, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_AS, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_p_mas_can_dpan, PIXEL_OP_COPY, /* register the function */
      SP, SM_AS, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_pan_mas_can_dpan, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_AS, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_pas_mas_can_dpan, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_AS, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_p_mas_caa_dpan, PIXEL_OP_COPY, /* register the function */
      SP, SM_AS, SC_AA, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_pan_mas_caa_dpan, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_AS, SC_AA, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_pas_mas_caa_dpan, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_AS, SC_AA, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_copy_p_mas_c_dpas, PIXEL_OP_COPY, /* register the function */
      SP, SM_AS, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_pan_mas_c_dpas, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_AS, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_pas_mas_c_dpas, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_AS, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_p_mas_can_dpas, PIXEL_OP_COPY, /* register the function */
      SP, SM_AS, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_pan_mas_can_dpas, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_AS, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_pas_mas_can_dpas, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_AS, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_p_mas_caa_dpas, PIXEL_OP_COPY, /* register the function */
      SP, SM_AS, SC_AA, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_pan_mas_caa_dpas, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_AS, SC_AA, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_pas_mas_caa_dpas, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_AS, SC_AA, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/
