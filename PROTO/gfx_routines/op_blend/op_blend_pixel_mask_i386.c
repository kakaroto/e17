
/* blend pixel x mask --> dst */

/*-----*/

#ifdef OP_FN /* do it in mmx */
OPFN(_op_blend_p_mas_dp_mmx) {
   DATA32 *s, *d, *e;
   DATA8  *m;
   d = p->dst.p; e = d + p->l;
   s = p->src.p;
   m = p->src.m;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_HALF, mm7, mm0)
   psllw_i2r(1, mm7);
   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		MOV_P2R(*s, mm2, mm0)
		MOV_RA2R(mm2, mm1)

		movq_r2r(mm7, mm3);
		psubw_r2r(mm1, mm3);

		MOV_P2R(*d, mm1, mm0)
		MUL4_256_R2R(mm3, mm1)

		paddw_r2r(mm2, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	    default:
		a++;
		MOV_A2R(a, mm3)
		MOV_P2R(*s, mm2, mm0)
		MUL4_256_R2R(mm3, mm2)
		MOV_RA2R(mm2, mm1)

		movq_r2r(mm7, mm3);
		psubw_r2r(mm1, mm3);

		MOV_P2R(*d, mm1, mm0)
		MUL4_256_R2R(mm3, mm1)

		paddw_r2r(mm2, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	  }
	m++;  s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in mmx */
OPFN(_op_blend_pas_mas_dp_mmx) {
   DATA32 *s, *d, *e;
   DATA8  *m;
   d = p->dst.p; e = d + p->l;
   s = p->src.p;
   m = p->src.m;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_HALF, mm7, mm0)
   psllw_i2r(1, mm7);
   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
	      {
		DATA32 sa = *s & 0xff000000;

		switch(sa)
		  {
		    case 0:
			break;
		    case 0xff000000:
			*d = *s;
			break;
		    default:
			MOV_P2R(*s, mm2, mm0)
			MOV_RA2R(mm2, mm1)

			movq_r2r(mm7, mm3);
			psubw_r2r(mm1, mm3);

			MOV_P2R(*d, mm1, mm0)
			MUL4_256_R2R(mm3, mm1)

			paddw_r2r(mm2, mm1);
			MOV_R2P(mm1, *d, mm0)
			break;
		  }
	      }
		break;
	    default:
	      {
		DATA32 sa = *s & 0xff000000;

		switch(sa)
		  {
		    case 0:
			break;
		    case 0xff000000:
			a++;
			MOV_A2R(a, mm3)
			MOV_P2R(*s, mm2, mm0)
			MOV_P2R(*d, mm1, mm0)
			INTERP_256_R2R(mm3, mm2, mm1);
			MOV_R2P(mm1, *d, mm0)
			break;
		    default:
			a++;
			MOV_A2R(a, mm3)
			MOV_P2R(*s, mm2, mm0)
			MUL4_256_R2R(mm3, mm2)
			MOV_RA2R(mm2, mm1)

			movq_r2r(mm7, mm3);
			psubw_r2r(mm1, mm3);

			MOV_P2R(*d, mm1, mm0)
			MUL4_256_R2R(mm3, mm1)

			paddw_r2r(mm2, mm1);
			MOV_R2P(mm1, *d, mm0)
			break;
		  }
	      }
		break;
	  }
	m++;  s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in mmx */
OPFN(_op_blend_pan_mas_dp_mmx) {
   DATA32 *s, *d, *e;
   DATA8  *m;
   d = p->dst.p; e = d + p->l;
   s = p->src.p;
   m = p->src.m;

   pxor_r2r(mm0, mm0);
   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		*d = *s;
		break;
	    default:
		a++;
		MOV_A2R(a, mm3)
		MOV_P2R(*s, mm2, mm0)
		MOV_P2R(*d, mm1, mm0)
		INTERP_256_R2R(mm3, mm2, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	  }
	m++;  s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_blend_p_mas_dpan_mmx _op_blend_p_mas_dp_mmx
#define _op_blend_pan_mas_dpan_mmx _op_blend_pan_mas_dp_mmx
#define _op_blend_pas_mas_dpan_mmx _op_blend_pas_mas_dp_mmx

#define _op_blend_p_mas_dpas_mmx _op_blend_p_mas_dp_mmx
#define _op_blend_pan_mas_dpas_mmx _op_blend_pan_mas_dp_mmx
#define _op_blend_pas_mas_dpas_mmx _op_blend_pas_mas_dp_mmx
#endif


#ifdef OP_REG
OPREG(_op_blend_p_mas_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_N, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_mas_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_N, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_mas_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_N, DP, CPU_MMX); /* what parameters it does */

OPREG(_op_blend_p_mas_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_N, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_mas_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_N, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_mas_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_N, DP_AN, CPU_MMX); /* what parameters it does */

OPREG(_op_blend_p_mas_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_mas_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_mas_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_N, DP_AS, CPU_MMX); /* what parameters it does */
#endif
/*-----*/
