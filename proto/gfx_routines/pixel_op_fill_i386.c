/*-----*/
#ifdef OP_FN /* do it in MMX */
OPFN(_op_set_mmx) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l - 1; c = p->src.c;
   movd_m2r(c, mm1);
   movq_r2r(mm1, mm2);
   psllq_i2r(32, mm1);
   por_r2r(mm2, mm1);
   for (; d < e; d+=2) {
      movq_r2m(mm1, d[0]);
   }
   e+=1;
   for (; d < e; d++) {
      *d = c;
   }
   did_mmx = 1;
}
#endif
#ifdef OP_REG
OPREG(_op_set_mmx, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_CN, S_AN, D_P, D_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_set_mmx, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_C1, S_AN, D_P, D_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_set_mmx, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_CA, S_AN, D_P, D_AN, CPU_MMX); /* what parameters it does */
#endif
/*-----*/
#ifdef OP_FN /* do it in SSE */
OPFN(_op_set_sse) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l - 1; c = p->src.c;
   movd_m2r(c, mm1);
   pshufw(mm1, mm1, 0x44);
   for (; d < e; d+=2) {
      movntq_r2m(mm1, d[0]);
   }
   e+=1;
   for (; d < e; d++) {
      *d = c;
   }
   did_mmx = 1;
}
#endif
#ifdef OP_REG
OPREG(_op_set_sse, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_CN, S_AN, D_P, D_AN, CPU_SSE); /* what parameters it does */
OPREG(_op_set_sse, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_C1, S_AN, D_P, D_AN, CPU_SSE); /* what parameters it does */
OPREG(_op_set_sse, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_CA, S_AN, D_P, D_AN, CPU_SSE); /* what parameters it does */
#endif
/*-----*/
#ifdef OP_FN /* do it in SSE2 */
OPFN(_op_set_sse2) {
   DATA32 *d, *e, c;
   d = p->dst.p; e = d + p->l - 3; c = p->src.c;
   movd_m2r(c, xmm1);
   pshufd(xmm1, xmm1, 0x00);
   while ((((unsigned long)((unsigned char *)d)) & 0x1f))
     { *d = c; d++;}
   for (; d < e; d+=4) {
      movntdq_r2m(xmm1, d[0]);
   }
   e+=3;
   for (; d < e; d++) {
      *d = c;
   }
   did_mmx = 1;
}
#endif
#ifdef OP_REG
OPREG(_op_set_sse2, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_CN, S_AN, D_P, D_AN, CPU_SSE2); /* what parameters it does */
OPREG(_op_set_sse2, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_C1, S_AN, D_P, D_AN, CPU_SSE2); /* what parameters it does */
OPREG(_op_set_sse2, PIXEL_OP_COPY, /* register the function */
      S_PN, S_MN, S_CA, S_AN, D_P, D_AN, CPU_SSE2); /* what parameters it does */
#endif
/*-----*/
