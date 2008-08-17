
/* copy color --> dst (color assumed pre-mul) */

/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_copy_c_dp) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l; c = p->src.c;
   for (; d < e; d++) {
      *d = c;
   }
}
#endif

#ifdef OP_FN
#define _op_copy_cn_dp _op_copy_c_dp
#define _op_copy_can_dp _op_copy_c_dp
#define _op_copy_caa_dp _op_copy_c_dp

#define _op_copy_cn_dpan _op_copy_c_dp
#define _op_copy_c_dpan _op_copy_c_dp
#define _op_copy_can_dpan _op_copy_c_dp
#define _op_copy_caa_dpan _op_copy_c_dp

#define _op_copy_cn_dpas _op_copy_c_dp
#define _op_copy_c_dpas _op_copy_c_dp
#define _op_copy_can_dpas _op_copy_c_dp
#define _op_copy_caa_dpas _op_copy_c_dp
#endif

#ifdef OP_REG
OPREG(_op_copy_cn_dp, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_c_dp, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_can_dp, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_caa_dp, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP, CPU_C); /* what parameters it does */

OPREG(_op_copy_cn_dpan, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_c_dpan, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_can_dpan, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_caa_dpan, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_copy_cn_dpas, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_c_dpas, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_can_dpas, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_caa_dpas, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/

/*-----*/
#ifdef OP_FN /* do it using OS provided calls */
OPFN(_op_copy_c_dp_os) {
   DATA32 *d, *e, c;
   c = p->src.c;
   d = p->dst.p; e = d + p->l; 
   for (; d < e; d++) {
	 *d = c;
      }
}
#endif

#ifdef OP_FN /* do it using OS provided calls */
OPFN(_op_copy_cn_dp_os) {
   memset(p->dst.p, 0xff, p->l * sizeof(DATA32));
}
#endif

#ifdef OP_FN /* do it using OS provided calls */
OPFN(_op_copy_caa_dp_os) {
   DATA8 a = (p->src.c & 0xff);
   memset(p->dst.p, a, p->l * sizeof(DATA32));
}
#endif

#ifdef OP_FN
#define _op_copy_can_dp_os _op_copy_c_dp_os

#define _op_copy_cn_dpan_os _op_copy_cn_dp_os
#define _op_copy_c_dpan_os _op_copy_c_dp_os
#define _op_copy_can_dpan_os _op_copy_can_dp_os
#define _op_copy_caa_dpan_os _op_copy_caa_dp_os

#define _op_copy_cn_dpas_os _op_copy_cn_dp_os
#define _op_copy_c_dpas_os _op_copy_c_dp_os
#define _op_copy_can_dpas_os _op_copy_can_dp_os
#define _op_copy_caa_dpas_os _op_copy_caa_dp_os
#endif

#ifdef OP_REG
OPREG(_op_copy_cn_dp_os, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP, CPU_OS); /* what parameters it does */
OPREG(_op_copy_c_dp_os, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP, CPU_OS); /* what parameters it does */
OPREG(_op_copy_can_dp_os, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP, CPU_OS); /* what parameters it does */
OPREG(_op_copy_caa_dp_os, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP, CPU_OS); /* what parameters it does */

OPREG(_op_copy_cn_dpan_os, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP_AN, CPU_OS); /* what parameters it does */
OPREG(_op_copy_c_dpan_os, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP_AN, CPU_OS); /* what parameters it does */
OPREG(_op_copy_can_dpan_os, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP_AN, CPU_OS); /* what parameters it does */
OPREG(_op_copy_caa_dpan_os, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP_AN, CPU_OS); /* what parameters it does */

OPREG(_op_copy_cn_dpas_os, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_N, DP_AS, CPU_OS); /* what parameters it does */
OPREG(_op_copy_c_dpas_os, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC, DP_AS, CPU_OS); /* what parameters it does */
OPREG(_op_copy_can_dpas_os, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AN, DP_AS, CPU_OS); /* what parameters it does */
OPREG(_op_copy_caa_dpas_os, PIXEL_OP_COPY, /* register the function */
      SP_N, SM_N, SC_AA, DP_AS, CPU_OS); /* what parameters it does */
#endif
/*-----*/
