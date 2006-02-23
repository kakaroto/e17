
/* mul mask x color -> dst  (color assumed pre-mul) */

/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_mul_mas_c_dp) {
   DATA32 *d, *e, c;
   DATA8  *m;
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
		*d = MUL4_SYM(c, *d);
		break;
	    default:
		a++;
		a = MUL_256(a, c);
		*d = MUL4_SYM(a, *d);
		break;
	  }
	m++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_mas_cn_dp) {
   DATA32 *d, *e;
   DATA8  *m;
   d = p->dst.p; e = d + p->l;
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
		break;
	    default:
		a++;
		*d = MUL_256(a, *d);
		break;
	  }
	m++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_mul_mas_caa_dp) {
   DATA32 *d, *e, c;
   DATA8  *m;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = 1 + (p->src.c & 0xff);

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		*d = 0;
		break;
	    case 255:
		*d = MUL_256(c, *d);
		break;
	    default:
		a = 1 + ((a * c) >> 8);
		*d = MUL_256(a, *d);
		break;
	  }
	m++;  d++;
     }
}
#endif

#ifdef OP_FN
#define _op_mul_mas_can_dp _op_mul_mas_c_dp

#define _op_mul_mas_cn_dpan _op_mul_mas_cn_dp
#define _op_mul_mas_c_dpan _op_mul_mas_c_dp
#define _op_mul_mas_can_dpan _op_mul_mas_can_dp
#define _op_mul_mas_caa_dpan _op_mul_mas_caa_dp

#define _op_mul_mas_cn_dpas _op_mul_mas_cn_dp
#define _op_mul_mas_c_dpas _op_mul_mas_c_dp
#define _op_mul_mas_can_dpas _op_mul_mas_can_dp
#define _op_mul_mas_caa_dpas _op_mul_mas_caa_dp
#endif

#ifdef OP_REG
OPREG(_op_mul_mas_cn_dp, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_AS, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_mas_c_dp, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_AS, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_mas_can_dp, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_AS, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_mul_mas_caa_dp, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_AS, SC_AA, DP, CPU_C); /* what parameters it does */

OPREG(_op_mul_mas_cn_dpan, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_AS, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_mas_c_dpan, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_AS, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_mas_can_dpan, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_AS, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_mul_mas_caa_dpan, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_AS, SC_AA, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_mul_mas_cn_dpas, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_AS, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_mas_c_dpas, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_AS, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_mas_can_dpas, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_AS, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_mul_mas_caa_dpas, PIXEL_OP_MUL, /* register the function */
      SP_N, SM_AS, SC_AA, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/
