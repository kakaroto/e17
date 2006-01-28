/*-----*/
#ifdef OP_FN /* do it in c */
OPFN(_op_set_c) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l; c = p->src.c;
   for (; d < e; d++) {
      *d = c;
   }
}
#endif
#ifdef OP_REG
OPREG(_op_set_c, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_CN, S_AN, D_P, D_AN, CPU_C); /* what parameters it does */
OPREG(_op_set_c, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_C1, S_AN, D_P, D_AN, CPU_C); /* what parameters it does */
OPREG(_op_set_c, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_CA, S_AN, D_P, D_AN, CPU_C); /* what parameters it does */
#endif
/*-----*/
#ifdef OP_FN /* do it using OS provided calls */
OPFN(_op_set_os) {
   DATA32 *d, *e, c;
   c = p->src.c;
   if (c == 0x00000000) {
      memset(p->dst.p, 0x00, p->l * 4);
   }
   else if (c == 0xffffffff) {
      memset(p->dst.p, 0xff, p->l * 4);
   }
   else {
      d = p->dst.p; e = d + p->l; 
      for (; d < e; d++) {
	 *d = c;
      }
   }
}
#endif
#ifdef OP_REG
OPREG(_op_set_os, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_CN, S_AN, D_P, D_AN, CPU_OS); /* what parameters it does */
OPREG(_op_set_os, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_C1, S_AN, D_P, D_AN, CPU_OS); /* what parameters it does */
OPREG(_op_set_os, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_CA, S_AN, D_P, D_AN, CPU_OS); /* what parameters it does */
#endif
/*-----*/
