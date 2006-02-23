
/* mul pixel x mask --> dst */

/*-----*/
#ifdef OP_FN /* do it in mmx */
OPFN(_op_mul_p_mas_dp_mmx) {
   DATA32 *s, *d, *e;
   DATA8  *m;
   d = p->dst.p; e = d + p->l;
   s = p->src.p;
   m = p->src.m;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_FULL, mm6, mm0)
   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		*d = 0;
		break;
	    case 255:
		MOV_P2R(*s, mm2, mm0)
		MOV_P2R(*d, mm1, mm0)
		MUL4_SYM_R2R(mm2, mm1, mm6)
		MOV_R2P(mm1, *d, mm0)
		break;
	    default:
		a++;
		MOV_P2R(*s, mm2, mm0)
		MOV_A2R(a, mm1)
		MUL4_256_R2R(mm1, mm2)
		MOV_P2R(*d, mm1, mm0)
		MUL4_SYM_R2R(mm2, mm1, mm6)
		MOV_R2P(mm1, *d, mm0)
		break;
	  }
	m++;  s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in mmx */
OPFN(_op_mul_pas_mas_dp_mmx) {
   DATA32 *s, *d, *e;
   DATA8  *m;
   d = p->dst.p; e = d + p->l;
   s = p->src.p;
   m = p->src.m;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_FULL, mm6, mm0)
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
			MOV_P2R(*s, mm2, mm0)
			MOV_P2R(*d, mm1, mm0)
			MUL4_SYM_R2R(mm2, mm1, mm6)
			MOV_R2P(mm1, *d, mm0)
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
			MOV_P2R(*s, mm2, mm0)
			MOV_A2R(a, mm1)
			MUL4_256_R2R(mm1, mm2)
			MOV_P2R(*d, mm1, mm0)
			MUL4_SYM_R2R(mm2, mm1, mm6)
			MOV_R2P(mm1, *d, mm0)
			break;
		  }
		break;
	  }
	m++;  s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in mmx */
OPFN(_op_mul_pan_mas_dp_mmx) {
   DATA32 *s, *d, *e;
   DATA8  *m;
   d = p->dst.p; e = d + p->l;
   s = p->src.p;
   m = p->src.m;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_FULL, mm6, mm0)
   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		*d = 0;
		break;
	    case 255:
		MOV_P2R(*s, mm2, mm0)
		MOV_P2R(*d, mm1, mm0)
		MUL4_SYM_R2R(mm2, mm1, mm6)
		MOV_R2P(mm1, *d, mm0)
		break;
	    default:
		a++;
		MOV_P2R(*s, mm2, mm0)
		MOV_A2R(a, mm1)
		MUL4_256_R2R(mm1, mm2)
		MOV_P2R(*d, mm1, mm0)
		MUL4_SYM_R2R(mm2, mm1, mm6)
		MOV_R2P(mm1, *d, mm0)
		break;
	  }
	m++;  s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_mul_p_mas_dpan_mmx _op_mul_p_mas_dp_mmx
#define _op_mul_pan_mas_dpan_mmx _op_mul_pan_mas_dp_mmx
#define _op_mul_pas_mas_dpan_mmx _op_mul_pas_mas_dp_mmx

#define _op_mul_p_mas_dpas_mmx _op_mul_p_mas_dp_mmx
#define _op_mul_pan_mas_dpas_mmx _op_mul_pan_mas_dp_mmx
#define _op_mul_pas_mas_dpas_mmx _op_mul_pas_mas_dp_mmx
#endif

#ifdef OP_REG
OPREG(_op_mul_p_mas_dp_mmx, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC_N, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_pas_mas_dp_mmx, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC_N, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_pan_mas_dp_mmx, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC_N, DP, CPU_MMX); /* what parameters it does */

OPREG(_op_mul_p_mas_dpan_mmx, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC_N, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_pas_mas_dpan_mmx, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC_N, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_pan_mas_dpan_mmx, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC_N, DP_AN, CPU_MMX); /* what parameters it does */

OPREG(_op_mul_p_mas_dpas_mmx, PIXEL_OP_MUL, /* register the function */
      SP, SM_AS, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_pas_mas_dpas_mmx, PIXEL_OP_MUL, /* register the function */
      SP_AS, SM_AS, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_mul_pan_mas_dpas_mmx, PIXEL_OP_MUL, /* register the function */
      SP_AN, SM_AS, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
#endif
/*-----*/
