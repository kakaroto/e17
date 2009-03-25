#ifdef EFL_HAVE_MMX


/*
 * [a a a a]
 */
static inline mmx_t a2v_mmx(uint16_t a)
{
	mmx_t r;

	r = _mm_cvtsi32_si64(a);
	r = _mm_unpacklo_pi16(r, r);
	r = _mm_unpacklo_pi32(r, r);

	return r;
}
/*
 * [0a 0r 0g 0b]
 */
static inline mmx_t c2v_mmx(uint32_t c)
{
	mmx_t z, r;

	r = _mm_cvtsi32_si64(c);
	z = _mm_cvtsi32_si64(0);
	r = _mm_unpacklo_pi8(r, z);

	return r;
}
/*============================================================================*
 *                                 Blend                                      *
 *============================================================================*/
static inline void blend_mmx(uint32_t *d, mmx_t alpha, mmx_t color)
{
	mmx_t r;

	r = c2v_mmx(*d);
	r = _mm_mullo_pi16(alpha, r);
	r = _mm_srli_pi16(r, 8);
	r = _mm_add_pi16(r, color);
	r  = _mm_packs_pu16(r, r);
	*d = _mm_cvtsi64_si32(r);
}

static void argb8888_pt_color_blend_mmx(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	uint8_t a;
	uint32_t *dtmp = d->data.argb8888.plane0;
	mmx_t r0, r1;

	a = color->pixel.argb8888.plane0 >> 24;
	r0 = a2v_mmx(256 - a);
	r1 = c2v_mmx(color->pixel.argb8888.plane0);

	blend_mmx(dtmp, r0, r1);
	_mm_empty();
}

static void argb8888_sp_color_blend_mmx(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	uint8_t a;
	mmx_t r0, r1;
	uint32_t *dtmp = d->data.argb8888.plane0;
	uint32_t *end = d->data.argb8888.plane0 + len;

	a = color->pixel.argb8888.plane0 >> 24;
	r0 = a2v_mmx(256 - a);
	r1 = c2v_mmx(color->pixel.argb8888.plane0);
	while (dtmp < end)
	{
		blend_mmx(dtmp, r0, r1);
		dtmp++;
	}
	_mm_empty();
}
static void argb8888_sp_pixel_blend_argb8888_mmx(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	mmx_t r0, r1;
	uint32_t *stmp = s->data.argb8888.plane0;
	uint32_t *dtmp = d->data.argb8888.plane0;
	uint32_t *end = d->data.argb8888.plane0 + len;

	while (dtmp < end)
	{
		uint8_t a;

		a = (*stmp) >> 24;
		r0 = a2v_mmx(256 - a);
		r1 = c2v_mmx(*stmp);

		blend_mmx(dtmp, r0, r1);
		dtmp++;
		stmp++;
	}
	_mm_empty();
}
/*============================================================================*
 *                                  Fill                                      *
 *============================================================================*/
static void argb8888_sp_pixel_fill_argb8888_mmx(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	uint32_t *stmp = s->data.argb8888.plane0, *dtmp = d->data.argb8888.plane0;
	uint32_t *end;
	int l = 0;

	l = (len / 16);
	end = d->data.argb8888.plane0 + (len - (len % 16));
	while (dtmp < end)
	{
		mmx_t m0, m1, m2, m3, m4, m5, m6, m7;

		m0 = *((__m64 *)stmp);
		m1 = *((__m64 *)(stmp + 2));
		m2 = *((__m64 *)(stmp + 4));
		m3 = *((__m64 *)(stmp + 6));
		m4 = *((__m64 *)(stmp + 8));
		m5 = *((__m64 *)(stmp + 10));
		m6 = *((__m64 *)(stmp + 12));
		m7 = *((__m64 *)(stmp + 14));
		*(__m64 *)dtmp = m0;
		*(__m64 *)(dtmp + 2) = m1;
		*(__m64 *)(dtmp + 4) = m2;
		*(__m64 *)(dtmp + 6) = m3;
		*(__m64 *)(dtmp + 8) = m4;
		*(__m64 *)(dtmp + 10) = m5;
		*(__m64 *)(dtmp + 12) = m6;
		*(__m64 *)(dtmp + 14) = m7;

		stmp += 16;
		dtmp += 16;
	}
	_mm_empty();
}

#undef argb8888_sp_color_blend
#undef argb8888_pt_color_blend
#undef argb8888_sp_pixel_fill_argb8888
#undef argb8888_sp_pixel_blend_argb8888

#define argb8888_pt_color_blend argb8888_pt_color_blend_mmx
#define argb8888_sp_color_blend argb8888_sp_color_blend_mmx
#define argb8888_sp_pixel_fill_argb8888 argb8888_sp_pixel_fill_argb8888_mmx
#define argb8888_sp_pixel_blend_argb8888 argb8888_sp_pixel_blend_argb8888_mmx
#endif
