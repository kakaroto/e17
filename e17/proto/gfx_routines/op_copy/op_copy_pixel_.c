
/* copy pixel --> dst */

/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_copy_p_dp) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l;
   for (; s < e; s++, d++) {
      *d = *s;
   }
}
#endif

#ifdef OP_FN
#define _op_copy_pan_dp _op_copy_p_dp
#define _op_copy_pas_dp _op_copy_p_dp

#define _op_copy_p_dpan _op_copy_p_dp
#define _op_copy_pan_dpan _op_copy_pan_dp
#define _op_copy_pas_dpan _op_copy_pas_dp

#define _op_copy_p_dpas _op_copy_p_dp
#define _op_copy_pan_dpas _op_copy_pan_dp
#define _op_copy_pas_dpas _op_copy_pas_dp
#endif

#ifdef OP_REG
OPREG(_op_copy_p_dp, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_pan_dp, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP, CPU_C); /* what parameters it does */
OPREG(_op_copy_pas_dp, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP, CPU_C); /* what parameters it does */

OPREG(_op_copy_p_dpan, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_pan_dpan, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP_AN, CPU_C); /* what parameters it does */
OPREG(_op_copy_pas_dpan, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP_AN, CPU_C); /* what parameters it does */

OPREG(_op_copy_p_dpas, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_pan_dpas, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP_AS, CPU_C); /* what parameters it does */
OPREG(_op_copy_pas_dpas, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP_AS, CPU_C); /* what parameters it does */
#endif
/*-----*/

/*-----*/
#ifdef OP_FN /* do it using OS provided calls */
OPFN(_op_copy_p_dp_os) {
   memcpy(p->dst.p, p->src.p, p->l * sizeof(DATA32));
}
#endif

#ifdef OP_FN
#define _op_copy_pan_dp_os _op_copy_p_dp_os
#define _op_copy_pas_dp_os _op_copy_p_dp_os

#define _op_copy_p_dpan_os _op_copy_p_dp_os
#define _op_copy_pan_dpan_os _op_copy_pan_dp_os
#define _op_copy_pas_dpan_os _op_copy_pas_dp_os

#define _op_copy_p_dpas_os _op_copy_p_dp_os
#define _op_copy_pan_dpas_os _op_copy_pan_dp_os
#define _op_copy_pas_dpas_os _op_copy_pas_dp_os
#endif

#ifdef OP_REG
OPREG(_op_copy_p_dp_os, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP, CPU_OS); /* what parameters it does */
OPREG(_op_copy_pan_dp_os, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP, CPU_OS); /* what parameters it does */
OPREG(_op_copy_pas_dp_os, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP, CPU_OS); /* what parameters it does */

OPREG(_op_copy_p_dpan_os, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP_AN, CPU_OS); /* what parameters it does */
OPREG(_op_copy_pan_dpan_os, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP_AN, CPU_OS); /* what parameters it does */
OPREG(_op_copy_pas_dpan_os, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP_AN, CPU_OS); /* what parameters it does */

OPREG(_op_copy_p_dpas_os, PIXEL_OP_COPY, /* register the function */
      SP, SM_N, SC_N, DP_AS, CPU_OS); /* what parameters it does */
OPREG(_op_copy_pan_dpas_os, PIXEL_OP_COPY, /* register the function */
      SP_AN, SM_N, SC_N, DP_AS, CPU_OS); /* what parameters it does */
OPREG(_op_copy_pas_dpas_os, PIXEL_OP_COPY, /* register the function */
      SP_AS, SM_N, SC_N, DP_AS, CPU_OS); /* what parameters it does */
#endif
/*-----*/
