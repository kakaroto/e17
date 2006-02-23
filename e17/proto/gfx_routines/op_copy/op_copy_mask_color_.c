
/* copy mask x color -> dst  (color assumed pre-mul) */

/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_copy_mas_c_dp) {
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
		*d = c;
		break;
	    default:
		a++;
		*d = MUL_256(a, c);
		break;
	  }
	m++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_copy_mas_cn_dp) {
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
		*d = 0xffffffff;
		break;
	    default:
		*d = ARGB_JOIN(a, a, a, a);
		break;
	  }
	m++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_copy_mas_caa_dp) {
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
		*d = c;
		break;
	    default:
		a = ((a * (c & 0xff)) + 0xff) >> 8;
		*d = ARGB_JOIN(a, a, a, a);
		break;
	  }
	m++;  d++;
     }
}
#endif

#ifdef OP_FN
#define _op_copy_mas_can_dp _op_copy_mas_c_dp

#define _op_copy_mas_c_dpan _op_copy_mas_c_dp
#define _op_copy_mas_cn_dpan _op_copy_mas_cn_dp
#define _op_copy_mas_can_dpan _op_copy_mas_can_dp
#define _op_copy_mas_caa_dpan _op_copy_mas_caa_dp

#define _op_copy_mas_c_dpas _op_copy_mas_c_dp
#define _op_copy_mas_cn_dpas _op_copy_mas_cn_dp
#define _op_copy_mas_can_dpas _op_copy_mas_can_dp
#define _op_copy_mas_caa_dpas _op_copy_mas_caa_dp
#endif

#ifdef OP_REG
OPREG(_op_copy_mas_cn_dp, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_AS, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_mas_c_dp, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_AS, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_mas_can_dp, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_AS, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_mas_caa_dp, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_AS, SC_AA, DP, CPU_C); /* what parameters it does */

OPREG(_op_copy_mas_cn_dpan, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_AS, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_mas_c_dpan, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_AS, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_mas_can_dpan, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_AS, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_mas_caa_dpan, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_AS, SC_AA, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_copy_mas_cn_dpas, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_AS, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_mas_c_dpas, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_AS, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_mas_can_dpas, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_AS, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_mas_caa_dpas, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_AS, SC_AA, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/

