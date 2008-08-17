#ifdef OP_FN /* do it using mmx */
OPFN(_op_cp_p_mmx) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l - 15;
   for (; s < e; s+=16, d+=16) {
      MOVE_16DWORDS_MMX(s, d);
   }
   e+=15;
   for (; s < e; s++, d++) {
      *d = *s;
   }
   did_mmx = 1;
}
#endif
#ifdef OP_REG
OPREG(_op_cp_p_mmx, PIXEL_OP_COPY, /* register the function */
      S_P1, S_MN, S_CN, S_AN, D_P, D_AN, CPU_MMX); /* what parameters it does */
#endif
/*-----*/
#ifdef OP_FN /* do it using sse */
OPFN(_op_cp_p_sse) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l - 15;
   for (; s < e; s+=16, d+=16) {
      MOVE_16DWORDS_MMX2(s, d);
   }
   e+=15;
   for (; s < e; s++, d++) {
      *d = *s;
   }
   did_mmx = 1;
}
#endif
#ifdef OP_REG
OPREG(_op_cp_p_sse, PIXEL_OP_COPY, /* register the function */
      S_P1, S_MN, S_CN, S_AN, D_P, D_AN, CPU_SSE); /* what parameters it does */
#endif
/*-----*/
#ifdef OP_FN /* do it using sse2 */
OPFN(_op_cp_p_sse2) {
   DATA32 *s, *d, *e;
   s = p->src.p; d = p->dst.p; e = s + p->l - 15;
   while ((((unsigned long)((unsigned char *)s)) & 0x1f))
     { *d = *s; d++; s++;}
   e-=31;
   for (; s < e; s+=32, d+=32) {
      MOVE_32DWORDS_ALIGNED_SSE2(s, d);
   }
   e+=31;
   for (; s < e; s++, d++) {
      *d = *s;
   }
   did_mmx = 1;
}
#endif
#ifdef OP_REG
OPREG(_op_cp_p_sse2, PIXEL_OP_COPY, /* register the function */
      S_P1, S_MN, S_CN, S_AN, D_P, D_AN, CPU_SSE2); /* what parameters it does */
#endif
