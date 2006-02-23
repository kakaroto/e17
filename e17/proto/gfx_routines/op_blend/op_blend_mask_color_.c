
/* blend mask x color -> dst  (color assumed pre-mul) */

/*-----*/

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_mas_c_dp) {
   DATA32 *d, *e, c, ca;
   DATA8  *m;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;
   ca = 256 - (c >> 24);

   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		*d = c + MUL_256(ca, *d);
		break;
	    default:
	      {
		DATA32 mc = 1 + a;

		mc = MUL_256(mc, c);
		a = 256 - (mc >> 24);
		*d = mc + MUL_256(a, *d);
		break;
	      }
	  }
	m++;  d++;
     }
}
#endif

#ifdef OP_FN /* do it in c */
OPFN(_op_blend_mas_can_dp) {
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
		break;
	    case 255:
		*d = c;
		break;
	    default:
		a++;
		*d = INTERP_256(a, c, *d);
		break;
	  }
	m++;  d++;
     }
}
#endif

#ifdef OP_FN
#define _op_blend_mas_cn_dp _op_blend_mas_can_dp
#define _op_blend_mas_caa_dp _op_blend_mas_c_dp

#define _op_blend_mas_c_dpan _op_blend_mas_c_dp
#define _op_blend_mas_cn_dpan _op_blend_mas_cn_dp
#define _op_blend_mas_can_dpan _op_blend_mas_can_dp
#define _op_blend_mas_caa_dpan _op_blend_mas_caa_dp

#define _op_blend_mas_c_dpas _op_blend_mas_c_dp
#define _op_blend_mas_cn_dpas _op_blend_mas_cn_dp
#define _op_blend_mas_can_dpas _op_blend_mas_can_dp
#define _op_blend_mas_caa_dpas _op_blend_mas_caa_dp
#endif

#ifdef OP_REG
OPREG(_op_blend_mas_c_dp, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_AS, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_mas_can_dp, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_AS, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_mas_can_dp, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_AS, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_blend_mas_c_dp, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_AS, SC_AA, DP, CPU_C); /* what parameters it does */

OPREG(_op_blend_mas_c_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_AS, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_mas_cn_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_AS, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_mas_can_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_AS, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_blend_mas_caa_dpan, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_AS, SC_AA, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_blend_mas_c_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_AS, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_mas_cn_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_AS, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_mas_can_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_AS, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_blend_mas_caa_dpas, PIXEL_OP_BLEND, /* register the function */
      SP_N, SM_AS, SC_AA, DP_AS, CPU_C); /* what parameters it does */
#endif

