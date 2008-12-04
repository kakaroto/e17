#ifdef __SSE2__

// for debug purpose
typedef union
{
	mmx_t mmx[2];
	sse2_t sse2;
} sse2_d;

/*
 * [aa aa aa aa]
 */
static inline sse2_t a2v_sse2(uint16_t a)
{
	sse2_t s;
	
	s = _mm_cvtsi32_si128(a);
	s = _mm_unpacklo_epi16(s, s);
	s = _mm_unpacklo_epi32(s, s);
	s = _mm_unpacklo_epi64(s, s);
	
	return s;
}

/*
 * [a1a1 a1a1 a2a2 a2a2]
 */
static inline sse2_t aa2v_sse2(uint64_t c)
{
	mmx_t m, r;
	sse2_t s;
	
	r = (mmx_t)c;
	m = (mmx_t)UINT64_C(0xff000000ff000000);
	r = _mm_and_si64(r, m);
	r = _mm_srli_pi32(r, 24);
	m = (mmx_t)UINT64_C(0x0000010000000100);
	r = _mm_sub_pi16(m, r);
	r = _mm_packs_pi32(r, r);
	s = _mm_movpi64_epi64(r);
	s = _mm_unpacklo_epi16(s, s);
	s = _mm_unpacklo_epi32(s, s);
	
	return s;
}

/*
 * [0a 0r 0g 0b 0a 0r 0g 0b]
 */
static inline sse2_t c2v_sse2(uint32_t c)
{
	sse2_t z, r;
		
	r = _mm_cvtsi32_si128(c);
	z = _mm_cvtsi32_si128(0);
	r = _mm_unpacklo_epi8(r, z);
	r = _mm_unpacklo_epi64(r, r);

	return r;
}

/*
 * [0a1 0r1 0g1 0b1 0a2 0r2 0g2 0b2]
 */
static inline sse2_t cc2v_sse2(uint64_t c)
{
	sse2_t r, z;

	r = _mm_movpi64_epi64((mmx_t)c);
	z = _mm_cvtsi32_si128(0);
	r = _mm_unpacklo_epi8(r, z);

	return r;
}

static inline void blend_sse2(uint64_t *d, sse2_t alpha, sse2_t color)
{
	sse2_t r;

	r = cc2v_sse2(*d);
	r = _mm_mullo_epi16(alpha, r);
	r = _mm_srli_epi16(r, 8);
	r = _mm_add_epi16(r, color);
	r = _mm_packus_epi16(r, r);
	*(mmx_t *)d = _mm_movepi64_pi64(r);
}

static void argb8888_sp_color_blend_sse2(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	int r = (len % 2);
	int l = len - r;

	uint32_t *dtmp = d->data.argb8888.plane0;
	uint32_t *end = d->data.argb8888.plane0 + l;
	uint8_t a;

#if 1
	sse2_t r0, r1;
			
	a = color->pixel.argb8888.plane0 >> 24;
	r0 = a2v_sse2(256 - a);
	r1 = c2v_sse2(color->pixel.argb8888.plane0);

	while (dtmp < end)
	{
		blend_sse2((uint64_t *)dtmp, r0, r1);
		dtmp += 2;
	}
#else
	argb8888_sp_color_blend_mmx(d, l, s, color, m);
#endif
#if 0
	if (r)
	{
		mmx_t m0, m1;
		
		m0 = _mm_movepi64_pi64(r0);
		m1 = _mm_movepi64_pi64(r1);
		blend_mmx(++dtmp, m0, m1);
	}
#endif
	_mm_empty();
}

static void argb8888_sp_pixel_blend_argb8888_sse2(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	int r = (len % 2);
	int l = len - r;
	sse2_t r0, r1;
	uint32_t *stmp = s->data.argb8888.plane0;
	uint32_t *dtmp = d->data.argb8888.plane0;
	uint32_t *end = d->data.argb8888.plane0 + l;

	while (dtmp < end)
	{
		r0 = aa2v_sse2(*(uint64_t *)stmp); 
		r1 = cc2v_sse2(*(uint64_t *)stmp);
		
		blend_sse2((uint64_t *)dtmp, r0, r1);
		dtmp += 2;
		stmp += 2;
	}
#if 0
	if (r)
	{
		mmx_t m0, m1;
		uint8_t a;
	
		stmp++;
		dtmp++;

		a = (*stmp) >> 24;
		m0 = a2v_mmx(256 - a);
		m1 = c2v_mmx(*stmp);
		blend_mmx(dtmp, m0, m1);
	}
#endif
	_mm_empty();
}

#undef argb8888_sp_color_blend
#undef argb8888_sp_pixel_blend_argb8888

#define argb8888_sp_color_blend argb8888_sp_color_blend_sse2
#define argb8888_sp_pixel_blend_argb8888 argb8888_sp_pixel_blend_argb8888_sse2

#endif
