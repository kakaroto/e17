
/* blend pixel x mask x color --> dst  (color assumed pre-mul) */

/*-----*/

#ifdef OP_FN /* do it in mmx */
OPFN(_op_blend_p_mas_c_dp_mmx) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_HALF, mm7, mm0)
   psllw_i2r(1, mm7);
   MOV_P2R(PIX_FULL, mm6, mm0)
   MOV_P2R(c, mm2, mm0)
   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		MOV_P2R(*s, mm3, mm0)
		MUL4_SYM_R2R(mm2, mm3, mm6)

		MOV_RA2R(mm3, mm1)
		movq_r2r(mm7, mm5);
		psubw_r2r(mm1, mm5);

		MOV_P2R(*d, mm1, mm0)
		MUL4_256_R2R(mm5, mm1)

		paddw_r2r(mm3, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	    default:
		a++;
		MOV_P2R(*s, mm3, mm0)
		MUL4_SYM_R2R(mm2, mm3, mm6)

		MOV_A2R(a, mm1)
		MUL4_256_R2R(mm1, mm3)

		MOV_RA2R(mm3, mm1)
		movq_r2r(mm7, mm5);
		psubw_r2r(mm1, mm5);

		MOV_P2R(*d, mm1, mm0)
		MUL4_256_R2R(mm5, mm1)

		paddw_r2r(mm3, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	  }
	m++;  s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in mmx */
OPFN(_op_blend_pas_mas_c_dp_mmx) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_HALF, mm7, mm0)
   psllw_i2r(1, mm7);
   MOV_P2R(PIX_FULL, mm6, mm0)
   MOV_P2R(c, mm2, mm0)
   c = 256 - (c >> 24);
   MOV_A2R(c, mm4)
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
			MOV_P2R(*s, mm3, mm0)
			MUL4_SYM_R2R(mm2, mm3, mm6)

			MOV_P2R(*d, mm1, mm0)
			MUL4_256_R2R(mm4, mm1)

			paddw_r2r(mm3, mm1);
			MOV_R2P(mm1, *d, mm0)
			break;
		    default:
			MOV_P2R(*s, mm3, mm0)
			MUL4_SYM_R2R(mm2, mm3, mm6)

			MOV_RA2R(mm3, mm1)
			movq_r2r(mm7, mm5);
			psubw_r2r(mm1, mm5);

			MOV_P2R(*d, mm1, mm0)
			MUL4_256_R2R(mm5, mm1)

			paddw_r2r(mm3, mm1);
			MOV_R2P(mm1, *d, mm0)
			break;
		  }
	      }
		break;
	    default:
	      {
		switch(*s)
		  {
		    case 0:
			break;
		    default:
			a++;
			MOV_P2R(*s, mm3, mm0)
			MUL4_SYM_R2R(mm2, mm3, mm6)

			MOV_A2R(a, mm1)
			MUL4_256_R2R(mm1, mm3)

			MOV_RA2R(mm3, mm1)
			movq_r2r(mm7, mm5);
			psubw_r2r(mm1, mm5);

			MOV_P2R(*d, mm1, mm0)
			MUL4_256_R2R(mm5, mm1)

			paddw_r2r(mm3, mm1);
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
OPFN(_op_blend_pan_mas_c_dp_mmx) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_HALF, mm7, mm0)
   psllw_i2r(1, mm7);
   MOV_P2R(PIX_FULL, mm6, mm0)
   MOV_P2R(c, mm2, mm0)
   c = 256 - (c >> 24);
   MOV_A2R(c, mm4)
   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		MOV_P2R(*s, mm3, mm0)
		MUL4_SYM_R2R(mm2, mm3, mm6)

		MOV_P2R(*d, mm1, mm0)
		MUL4_256_R2R(mm4, mm1)

		paddw_r2r(mm3, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	    default:
		a++;
		MOV_P2R(*s, mm3, mm0)
		MUL4_SYM_R2R(mm2, mm3, mm6)

		MOV_A2R(a, mm1)
		MUL4_256_R2R(mm1, mm3)

		MOV_RA2R(mm3, mm1)
		movq_r2r(mm7, mm5);
		psubw_r2r(mm1, mm5);

		MOV_P2R(*d, mm1, mm0)
		MUL4_256_R2R(mm5, mm1)

		paddw_r2r(mm3, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	  }
	m++;  s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in mmx */
OPFN(_op_blend_p_mas_can_dp_mmx) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_HALF, mm7, mm0)
   psllw_i2r(1, mm7);
   MOV_P2R(PIX_FULL, mm6, mm0)
   MOV_P2R(c, mm2, mm0)
   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		MOV_P2R(*s, mm3, mm0)
		MUL4_SYM_R2R(mm2, mm3, mm6)

		MOV_RA2R(mm3, mm1)
		movq_r2r(mm7, mm5);
		psubw_r2r(mm1, mm5);

		MOV_P2R(*d, mm1, mm0)
		MUL4_256_R2R(mm5, mm1)

		paddw_r2r(mm3, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	    default:
		a++;
		MOV_P2R(*s, mm3, mm0)
		MUL4_SYM_R2R(mm2, mm3, mm6)

		MOV_A2R(a, mm1)
		MUL4_256_R2R(mm1, mm3)

		MOV_RA2R(mm3, mm1)
		movq_r2r(mm7, mm5);
		psubw_r2r(mm1, mm5);

		MOV_P2R(*d, mm1, mm0)
		MUL4_256_R2R(mm5, mm1)

		paddw_r2r(mm3, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	  }
	m++;  s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in mmx */
OPFN(_op_blend_pas_mas_can_dp_mmx) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_HALF, mm7, mm0)
   psllw_i2r(1, mm7);
   MOV_P2R(PIX_FULL, mm6, mm0)
   MOV_P2R(c, mm2, mm0)
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
			MOV_P2R(*s, mm1, mm0)
			MUL4_SYM_R2R(mm2, mm1, mm6)
			MOV_R2P(mm1, *d, mm0)
			break;
		    default:
			MOV_P2R(*s, mm3, mm0)
			MUL4_SYM_R2R(mm2, mm3, mm6)

			MOV_RA2R(mm3, mm1)
			movq_r2r(mm7, mm5);
			psubw_r2r(mm1, mm5);

			MOV_P2R(*d, mm1, mm0)
			MUL4_256_R2R(mm5, mm1)

			paddw_r2r(mm3, mm1);
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
			MOV_P2R(*s, mm3, mm0)
			MUL4_SYM_R2R(mm2, mm3, mm6)

			MOV_A2R(a, mm5)
			MOV_P2R(*d, mm1, mm0)
			INTERP_256_R2R(mm5, mm3, mm1);
			MOV_R2P(mm1, *d, mm0)
			break;
		    default:
			a++;
			MOV_P2R(*s, mm3, mm0)
			MUL4_SYM_R2R(mm2, mm3, mm6)

			MOV_A2R(a, mm1)
			MUL4_256_R2R(mm1, mm3)

			MOV_RA2R(mm3, mm1)
			movq_r2r(mm7, mm5);
			psubw_r2r(mm1, mm5);

			MOV_P2R(*d, mm1, mm0)
			MUL4_256_R2R(mm5, mm1)

			paddw_r2r(mm3, mm1);
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
OPFN(_op_blend_pan_mas_can_dp_mmx) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = p->src.c;

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_FULL, mm6, mm0)
   MOV_P2R(c, mm2, mm0)
   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		MOV_P2R(*s, mm1, mm0)
		MUL4_SYM_R2R(mm2, mm1, mm6)
		MOV_R2P(mm1, *d, mm0)
		break;
	    default:
		a++;
		MOV_P2R(*s, mm3, mm0)
		MUL4_SYM_R2R(mm2, mm3, mm6)

		MOV_A2R(a, mm5)
		MOV_P2R(*d, mm1, mm0)
		INTERP_256_R2R(mm5, mm3, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	  }
	m++;  s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in mmx */
OPFN(_op_blend_p_mas_caa_dp_mmx) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = 1 + (p->src.c & 0xff);

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_HALF, mm7, mm0)
   psllw_i2r(1, mm7);
   MOV_P2R(PIX_FULL, mm4, mm0)
   psrlw_i2r(7, mm4);
   MOV_A2R(c, mm2)
   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		MOV_P2R(*s, mm3, mm0)
		MUL4_256_R2R(mm2, mm3)

		MOV_RA2R(mm3, mm1)
		movq_r2r(mm7, mm5);
		psubw_r2r(mm1, mm5);

		MOV_P2R(*d, mm1, mm0)
		MUL4_256_R2R(mm5, mm1)

		paddw_r2r(mm3, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	    default:
		MOV_A2R(a, mm5)
		MUL4_256_R2R(mm2, mm5)
		paddw_r2r(mm4, mm5);

		MOV_P2R(*s, mm3, mm0)
		MUL4_256_R2R(mm5, mm3)

		MOV_RA2R(mm3, mm1)
		movq_r2r(mm7, mm5);
		psubw_r2r(mm1, mm5);

		MOV_P2R(*d, mm1, mm0)
		MUL4_256_R2R(mm5, mm1)

		paddw_r2r(mm3, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	  }
	m++;  s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN /* do it in mmx */
OPFN(_op_blend_pas_mas_caa_dp_mmx) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = 1 + (p->src.c & 0xff);

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_HALF, mm7, mm0)
   psllw_i2r(1, mm7);
   MOV_P2R(PIX_FULL, mm4, mm0)
   psrlw_i2r(7, mm4);
   MOV_A2R(c, mm2)
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
			MOV_P2R(*s, mm3, mm0)
			MOV_P2R(*d, mm1, mm0)
			INTERP_256_R2R(mm2, mm3, mm1);
			MOV_R2P(mm1, *d, mm0)
			break;
		    default:
			MOV_P2R(*s, mm3, mm0)
			MUL4_256_R2R(mm2, mm3)

			MOV_RA2R(mm3, mm1)
			movq_r2r(mm7, mm5);
			psubw_r2r(mm1, mm5);

			MOV_P2R(*d, mm1, mm0)
			MUL4_256_R2R(mm5, mm1)

			paddw_r2r(mm3, mm1);
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
			MOV_A2R(a, mm5)
			MUL4_256_R2R(mm2, mm5)
			paddw_r2r(mm4, mm5);

			MOV_P2R(*s, mm3, mm0)
			MOV_P2R(*d, mm1, mm0)
			INTERP_256_R2R(mm5, mm3, mm1);
			MOV_R2P(mm1, *d, mm0)
			break;
		    default:
			MOV_A2R(a, mm5)
			MUL4_256_R2R(mm2, mm5)
			paddw_r2r(mm4, mm5);

			MOV_P2R(*s, mm3, mm0)
			MUL4_256_R2R(mm5, mm3)

			MOV_RA2R(mm3, mm1)
			movq_r2r(mm7, mm5);
			psubw_r2r(mm1, mm5);

			MOV_P2R(*d, mm1, mm0)
			MUL4_256_R2R(mm5, mm1)

			paddw_r2r(mm3, mm1);
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
OPFN(_op_blend_pan_mas_caa_dp_mmx) {
   DATA32 *s, *d, *e, c;
   DATA8  *m;
   s = p->src.p;
   d = p->dst.p; e = d + p->l;
   m = p->src.m;
   c = 1 + (p->src.c & 0xff);

   pxor_r2r(mm0, mm0);
   MOV_P2R(PIX_FULL, mm4, mm0)
   psrlw_i2r(7, mm4);
   MOV_A2R(c, mm2)
   while (d < e)
     {
	DATA32 a = *m;

	switch(a)
	  {
	    case 0:
		break;
	    case 255:
		MOV_P2R(*s, mm3, mm0)
		MOV_P2R(*d, mm1, mm0)
		INTERP_256_R2R(mm2, mm3, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	    default:
		MOV_A2R(a, mm5)
		MUL4_256_R2R(mm2, mm5)
		paddw_r2r(mm4, mm5);

		MOV_P2R(*s, mm3, mm0)
		MOV_P2R(*d, mm1, mm0)
		INTERP_256_R2R(mm5, mm3, mm1);
		MOV_R2P(mm1, *d, mm0)
		break;
	  }
	m++;  s++;  d++;
     }
   did_mmx = 1;
}
#endif

#ifdef OP_FN
#define _op_blend_p_mas_c_dpan_mmx _op_blend_p_mas_c_dp_mmx
#define _op_blend_pas_mas_c_dpan_mmx _op_blend_pas_mas_c_dp_mmx
#define _op_blend_pan_mas_c_dpan_mmx _op_blend_pan_mas_c_dp_mmx
#define _op_blend_p_mas_can_dpan_mmx _op_blend_p_mas_can_dp_mmx
#define _op_blend_pas_mas_can_dpan_mmx _op_blend_pas_mas_can_dp_mmx
#define _op_blend_pan_mas_can_dpan_mmx _op_blend_pan_mas_can_dp_mmx
#define _op_blend_p_mas_caa_dpan_mmx _op_blend_p_mas_caa_dp_mmx
#define _op_blend_pas_mas_caa_dpan_mmx _op_blend_pas_mas_caa_dp_mmx
#define _op_blend_pan_mas_caa_dpan_mmx _op_blend_pan_mas_caa_dp_mmx

#define _op_blend_p_mas_c_dpas_mmx _op_blend_p_mas_c_dp_mmx
#define _op_blend_pas_mas_c_dpas_mmx _op_blend_pas_mas_c_dp_mmx
#define _op_blend_pan_mas_c_dpas_mmx _op_blend_pan_mas_c_dp_mmx
#define _op_blend_p_mas_can_dpas_mmx _op_blend_p_mas_can_dp_mmx
#define _op_blend_pas_mas_can_dpas_mmx _op_blend_pas_mas_can_dp_mmx
#define _op_blend_pan_mas_can_dpas_mmx _op_blend_pan_mas_can_dp_mmx
#define _op_blend_p_mas_caa_dpas_mmx _op_blend_p_mas_caa_dp_mmx
#define _op_blend_pas_mas_caa_dpas_mmx _op_blend_pas_mas_caa_dp_mmx
#define _op_blend_pan_mas_caa_dpas_mmx _op_blend_pan_mas_caa_dp_mmx
#endif

#ifdef OP_REG
OPREG(_op_blend_p_mas_c_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_mas_c_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_mas_c_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_p_mas_can_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_AN, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_mas_can_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_AN, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_mas_can_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_AN, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_p_mas_caa_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_AA, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_mas_caa_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_AA, DP, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_mas_caa_dp_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_AA, DP, CPU_MMX); /* what parameters it does */

OPREG(_op_blend_p_mas_c_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_mas_c_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_mas_c_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_p_mas_can_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_AN, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_mas_can_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_AN, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_mas_can_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_AN, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_p_mas_caa_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_AA, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_mas_caa_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_AA, DP_AN, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_mas_caa_dpan_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_AA, DP_AN, CPU_MMX); /* what parameters it does */

OPREG(_op_blend_p_mas_c_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_mas_c_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_mas_c_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_p_mas_can_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_AN, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_mas_can_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_AN, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_mas_can_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_AN, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_p_mas_caa_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP, SM_AS, SC_AA, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pas_mas_caa_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AS, SM_AS, SC_AA, DP_AS, CPU_MMX); /* what parameters it does */
OPREG(_op_blend_pan_mas_caa_dpas_mmx, PIXEL_OP_BLEND, /* register the function */
      SP_AN, SM_AS, SC_AA, DP_AS, CPU_MMX); /* what parameters it does */
#endif
/*-----*/


