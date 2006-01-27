/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_cp_p_c) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l;
   for (; s < e; s++, d++) {
      *d = *s;
   }
}
#endif
#ifdef OP_REG
OPREG(_op_cp_p_c, PIXEL_OP_COPY, /* register the function */
      S_P1, S_MN, S_CN, S_AN, D_P, D_AN, CPU_C); /* what parameters it does */
#endif
/*-----*/
#ifdef OP_FN /* do it using OS provided calls */
OPFN(_op_cp_p_os) {
   memcpy(p->dst.p, p->src.p, p->l * sizeof(DATA32));
}
#endif
#ifdef OP_REG
OPREG(_op_cp_p_os, PIXEL_OP_COPY, /* register the function */
      S_P1, S_MN, S_CN, S_AN, D_P, D_AN, CPU_OS); /* what parameters it does */
#endif
/*-----*/
