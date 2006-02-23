
/* mul pixel x mask --> dst */

/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_mul_p_mas_dp) {
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
		*d = 0;
		break;
	    case 255:
		*d = MUL4_SYM(*s, *d);
		break;
	    default:
		a++;
		a = MUL_256(a, *s);
		*d = MUL4_SYM(a, *d);
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_pas_mas_dp) {
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
		*d = 0;
		break;
	    case 255:
		switch(*s)
		  {
		    case 0:
			*d = 0;
			break;
		    default:
			*d = MUL4_SYM(*s, *d);
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
			a++;
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
OPFN(_op_mul_pan_mas_dp) {
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
		*d = 0;
		break;
	    case 255:
		*d = (*d & 0xff000000) + MUL3_SYM(*s, *d);
		break;
	    default:
		a++;
		a = MUL_256(a, *s);
		*d = MUL4_SYM(a, *d);
		break;
	  }
	m++;  s++;  d++;
     }
}
#endif

#ifdef OP_FN
#define _op_mul_p_mas_dpan _op_mul_p_mas_dp
#define _op_mul_pan_mas_dpan _op_mul_pan_mas_dp
#define _op_mul_pas_mas_dpan _op_mul_pas_mas_dp

#define _op_mul_p_mas_dpas _op_mul_p_mas_dp
#define _op_mul_pan_mas_dpas _op_mul_pan_mas_dp
#define _op_mul_pas_mas_dpas _op_mul_pas_mas_dp
#endif

#ifdef OP_REG
OPREG(_op_mul_p_mas_dp, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_mas_dp, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_mas_dp, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC_N, DP, CPU_C); /* what parameters it does */

OPREG(_op_mul_p_mas_dpan, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_mas_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_mas_dpan, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC_N, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_mul_p_mas_dpas, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pas_mas_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_pan_mas_dpas, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC_N, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/
