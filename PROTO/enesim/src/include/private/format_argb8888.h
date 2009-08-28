/* ENESIM - Direct Rendering Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FORMAT_ARGB8888_H_
#define FORMAT_ARGB8888_H_

/*============================================================================*
 *                                   Core                                     *
 *============================================================================*/
/* Functions needed by the other subsystems */
static inline unsigned char argb8888_alpha_get(uint32_t plane0)
{
	return (plane0 >> 24);
}

static inline unsigned char argb8888_red_get(unsigned int plane0)
{
	return ((plane0 >> 16) & 0xff);
}

static inline unsigned char argb8888_green_get(unsigned int plane0)
{
	return ((plane0 >> 8) & 0xff);
}

static inline unsigned char argb8888_blue_get(unsigned int plane0)
{
	return (plane0 & 0xff);
}

static inline void argb8888_from_components(unsigned int *plane0, unsigned char a, unsigned char r,
		unsigned char g, unsigned char b)
{
	*plane0 = (a << 24) | (r << 16) | (g << 8) | b;
}

static inline void argb8888_to_components(unsigned int plane0, unsigned char *a, unsigned char *r,
		unsigned char *g, unsigned char *b)
{
	if (a) *a = argb8888_alpha_get(plane0);
	if (r) *r = argb8888_red_get(plane0);
	if (g) *g = argb8888_green_get(plane0);
	if (b) *b = argb8888_blue_get(plane0);
}

static inline void argb8888_to_argb(unsigned int *argb, unsigned int plane0)
{
	*argb = plane0;
}
static inline void argb8888_from_argb(unsigned int argb, unsigned int *plane0)
{
	*plane0 = argb;
}

static inline void argb8888_fill(uint32_t *dplane0, uint32_t splane0)
{
	*dplane0 = splane0;
}
/*============================================================================*
 *                               MMX Helper                                   *
 *============================================================================*/
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

#endif
/*============================================================================*
 *                               SSE2 Helper                                  *
 *============================================================================*/
#ifdef EFL_HAVE_SSE2

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
#endif

/*============================================================================*
 *                             Pixel operations                               *
 *============================================================================*/
#if 0
typedef union
{
	mmx_t mmx;
	uint32_t u32[2];
} mmx_td;
#endif

/* FIXME remove this */
#define INTERP_256 argb8888_interp_256
/*
 * r = (c0 - c1) * a + c1
 * a = 0 => r = c1
 * a = 1 => r = c0
 */
static inline uint32_t argb8888_interp_256(uint16_t a, uint32_t c0, uint32_t c1)
{
	/* Let's disable this until we find the reason of the fp exception */
#ifdef EFL_HAVE_MMX_
	mmx_t rc0, rc1;
	mmx_t ra, ra255;
	uint32_t res;

	ra = a2v_mmx(a);
	ra255 = a2v_mmx(255);
	rc0 = c2v_mmx(c0);
	rc1 = c2v_mmx(c1);

	rc0 = _mm_sub_pi16(rc0, rc1);
	rc0 = _mm_mullo_pi16(ra, rc0);
	rc0 = _mm_srli_pi16(rc0, 8);
	rc0 = _mm_add_pi16(rc0, rc1);
	rc0 = _mm_and_si64(rc0, ra255);
	rc0 = _mm_packs_pu16(rc0, rc0);
	res = _mm_cvtsi64_si32(rc0);
	_mm_empty();

	return res;

#else
	return ( (((((((c0) >> 8) & 0xff00ff) - (((c1) >> 8) & 0xff00ff)) * (a))
			+ ((c1) & 0xff00ff00)) & 0xff00ff00) +
			(((((((c0) & 0xff00ff) - ((c1) & 0xff00ff)) * (a)) >> 8)
			+ ((c1) & 0xff00ff)) & 0xff00ff) );
#endif
}
/*
 * [a1 r1 g1 b1], [a2 r2 g2 b2] => [a1*a2 r1*r2 g1*g2 b1*b2]
 */
static inline uint32_t argb8888_mul4_sym(uint32_t c1, uint32_t c2)
{
	return ( ((((((c1) >> 16) & 0xff00) * (((c2) >> 16) & 0xff00)) + 0xff0000) & 0xff000000) +
	   ((((((c1) >> 8) & 0xff00) * (((c2) >> 16) & 0xff)) + 0xff00) & 0xff0000) +
	   ((((((c1) & 0xff00) * ((c2) & 0xff00)) + 0xff00) >> 16) & 0xff00) +
	   (((((c1) & 0xff) * ((c2) & 0xff)) + 0xff) >> 8) );
}

/*
 *
 */
static inline uint32_t argb8888_mul_256(uint16_t a, uint32_t c)
{
	return  ( (((((c) >> 8) & 0x00ff00ff) * (a)) & 0xff00ff00) +
	(((((c) & 0x00ff00ff) * (a)) >> 8) & 0x00ff00ff) );
}

/*
 *
 */
static inline uint32_t argb8888_mul_sym(uint16_t a, uint32_t c)
{
	return ( (((((c) >> 8) & 0x00ff00ff) * (a) + 0xff00ff) & 0xff00ff00) +
	   (((((c) & 0x00ff00ff) * (a) + 0xff00ff) >> 8) & 0x00ff00ff) );
}

/*
 *
 */
static inline uint32_t argb8888_sample_good(uint32_t *data, int stride, int sw,
		int sh, Eina_F16p16 xx, Eina_F16p16 yy, int x, int y)
{
	if (x < sw && y < sh && x >= 0 && y >= 0)
	{
		uint32_t p0 = 0, p1 = 0, p2 = 0, p3 = 0;

		data = data + (stride * y) + x;

		if ((x > -1) && (y > - 1))
			p0 = *data;

		if ((y > -1) && ((x + 1) < sw))
			p1 = *(data + 1);

		if ((y + 1) < sh)
		{
			if (x > -1)
				p2 = *(data + stride);
			if ((x + 1) < sw)
				p3 = *(data + stride + 1);
		}

		if (p0 | p1 | p2 | p3)
		{
			uint16_t ax, ay;

			ax = 1 + ((xx & 0xffff) >> 8);
			ay = 1 + ((yy & 0xffff) >> 8);

			p0 = argb8888_interp_256(ax, p1, p0);
			p2 = argb8888_interp_256(ax, p3, p2);
			p0 = argb8888_interp_256(ay, p2, p0);
		}
		return p0;
	}
	else
		return 0;
}


#endif /* FORMAT_ARGB8888_H_*/
