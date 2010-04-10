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
#include "Enesim.h"
#include "enesim_private.h"
/*
 * P'(x,y) <- P(x + scale * (XC(x,y) - .5), y + scale * (YC(x,y) - .5))
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Dispamp
{
	Enesim_Renderer base;
	Enesim_Surface *map;
	Enesim_Surface *src;
	Enesim_Channel x_channel;
	Enesim_Channel y_channel;
	float scale;
	/* The state variables */
	Eina_F16p16 s_scale;
} Dispmap;

/* TODO Move this to a common header */
static inline uint8_t _argb8888_alpha(uint32_t argb8888)
{
	return (argb8888 >> 24);
}

static inline uint8_t _argb8888_red(uint32_t argb8888)
{
	return (argb8888 >> 16) & 0xff;
}

static inline uint8_t _argb8888_green(uint32_t argb8888)
{
	return (argb8888 >> 8) & 0xff;
}

static inline uint8_t _argb8888_blue(uint32_t argb8888)
{
	return argb8888 & 0xff;
}

static inline Eina_F16p16 _displace(Eina_F16p16 coord, uint8_t distance, Eina_F16p16 scale)
{
	Eina_F16p16 vx;

	/* FIXME define fixed(255) as a constant */
	vx = eina_f16p16_int_from(distance - 127);
	vx = eina_f16p16_mul((((int64_t)(vx) << 16) / eina_f16p16_int_from(255)), scale);

	return vx + coord;
}

/*----------------------------------------------------------------------------*
 *          X alpha channel, Y blue channel, Identity transformation          *
 *----------------------------------------------------------------------------*/
static void _argb8888_a_b_span_identity(Enesim_Renderer *r, int x, int y,
		unsigned int len, uint32_t *dst)
{
	Dispmap *d = (Dispmap *)r;
	uint32_t *end = dst + len;
	uint32_t *map, *src;
	int mstride;
	int sstride;
	int sw, sh, mw, mh;
	Eina_F16p16 xx, yy;

	/* setup the parameters */
	enesim_surface_size_get(d->src, &sw, &sh);
	enesim_surface_size_get(d->map, &mw, &mh);
	mstride = enesim_surface_stride_get(d->map);
	sstride = enesim_surface_stride_get(d->src);
	map = enesim_surface_data_get(d->map);
	src = enesim_surface_data_get(d->src);

	map = map + (mstride * y) + x;
	yy = eina_f16p16_int_from(y);

	while (dst < end)
	{
		Eina_F16p16 sxx, syy;
		int sx, sy;
		uint32_t p0 = 0;
		uint16_t m0;
		uint16_t m1;

		if (x < 0 || x >= mw || y < 0 || y >= mh)
			goto next;

		xx = eina_f16p16_int_from(x);

		m0 = *map >> 24;
		m1 = *map & 0xff;
		sxx = _displace(xx, m0, d->s_scale);
		syy = _displace(yy, m1, d->s_scale);

		sx = eina_f16p16_int_to(sxx);
		sy = eina_f16p16_int_to(syy);
		p0 = argb8888_sample_good(src, sstride, sw, sh, sxx, syy, sx, sy);

next:
		*dst++ = p0;
		map++;
		x++;
	}
}

static void _argb8888_r_g_span_identity(Enesim_Renderer *r, int x, int y,
		unsigned int len, uint32_t *dst)
{
	Dispmap *d = (Dispmap *)r;
	uint32_t *end = dst + len;
	uint32_t *map, *src;
	int mstride;
	int sstride;
	int sw, sh, mw, mh;
	Eina_F16p16 xx, yy;

	/* setup the parameters */
	enesim_surface_size_get(d->src, &sw, &sh);
	enesim_surface_size_get(d->map, &mw, &mh);
	mstride = enesim_surface_stride_get(d->map);
	sstride = enesim_surface_stride_get(d->src);
	map = enesim_surface_data_get(d->map);
	src = enesim_surface_data_get(d->src);

	map = map + (mstride * y) + x;
	yy = eina_f16p16_int_from(y);

	while (dst < end)
	{
		Eina_F16p16 sxx, syy;
		int sx, sy;
		uint32_t p0 = 0;
		uint16_t m0;
		uint16_t m1;

		if (x < 0 || x >= mw || y < 0 || y >= mh)
			goto next;

		xx = eina_f16p16_int_from(x);

		m0 = (*map >> 16) & 0xff;
		m1 = (*map >> 8) & 0xff;
		sxx = _displace(xx, m0, d->s_scale);
		syy = _displace(yy, m1, d->s_scale);

		sx = eina_f16p16_int_to(sxx);
		sy = eina_f16p16_int_to(syy);
		p0 = argb8888_sample_good(src, sstride, sw, sh, sxx, syy, sx, sy);

next:
		*dst++ = p0;
		map++;
		x++;
	}
}

#define DISPMAP_AFFINE(xch, ych, xfunction, yfunction) \
static void _argb8888_##xch##_##ych##_span_affine(Enesim_Renderer *r, int x,	\
		int y, unsigned int len, uint32_t *dst)				\
{										\
	Dispmap *d = (Dispmap *)r;						\
	uint32_t *end = dst + len;						\
	uint32_t *map, *src;							\
	int mstride;								\
	int sstride;								\
	int sw, sh, mw, mh;							\
	Eina_F16p16 xx, yy;							\
										\
	/* setup the parameters */						\
	enesim_surface_size_get(d->src, &sw, &sh);				\
	enesim_surface_size_get(d->map, &mw, &mh);				\
	mstride = enesim_surface_stride_get(d->map);				\
	sstride = enesim_surface_stride_get(d->src);				\
	map = enesim_surface_data_get(d->map);					\
	src = enesim_surface_data_get(d->src);					\
										\
	/* TODO move by the origin */						\
	renderer_affine_setup(r, x, y, &xx, &yy);				\
										\
	while (dst < end)							\
	{									\
		Eina_F16p16 sxx, syy;						\
		int sx, sy;							\
		uint32_t p0 = 0;						\
		uint16_t m0;							\
		uint16_t m1;							\
										\
		x = eina_f16p16_int_to(xx);					\
		y = eina_f16p16_int_to(yy);					\
										\
		if (x < 0 || x >= mw || y < 0 || y >= mh)			\
			goto next;						\
										\
		m0 = *(map + (mstride * y) + x);				\
		m0 = xfunction(m0);						\
		m1 = yfunction(m1);						\
										\
		sxx = _displace(xx, m0, d->s_scale);				\
		syy = _displace(yy, m1, d->s_scale);				\
										\
		sx = eina_f16p16_int_to(sxx);					\
		sy = eina_f16p16_int_to(syy);					\
		p0 = argb8888_sample_good(src, sstride, sw, sh, sxx, syy, sx,	\
				sy);						\
										\
next:										\
		*dst++ = p0;							\
		map++;								\
		yy += r->matrix.values.yx;					\
		xx += r->matrix.values.xx;					\
	}									\
}

DISPMAP_AFFINE(r, g, _argb8888_red, _argb8888_green);
DISPMAP_AFFINE(a, b, _argb8888_alpha, _argb8888_blue);

static Enesim_Renderer_Span_Draw _spans[ENESIM_CHANNELS][ENESIM_CHANNELS][ENESIM_MATRIX_TYPES] = {
	[ENESIM_CHANNEL_ALPHA][ENESIM_CHANNEL_BLUE][ENESIM_MATRIX_IDENTITY] = _argb8888_a_b_span_identity,
	[ENESIM_CHANNEL_ALPHA][ENESIM_CHANNEL_BLUE][ENESIM_MATRIX_AFFINE] = _argb8888_a_b_span_affine,
	[ENESIM_CHANNEL_RED][ENESIM_CHANNEL_GREEN][ENESIM_MATRIX_IDENTITY] = _argb8888_r_g_span_identity,
	[ENESIM_CHANNEL_RED][ENESIM_CHANNEL_GREEN][ENESIM_MATRIX_AFFINE] = _argb8888_r_g_span_affine,
};

static void _state_cleanup(Enesim_Renderer *r)
{

}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	Dispmap *d = (Dispmap *)r;

	d->s_scale = eina_f16p16_float_from(d->scale);
	r->span = _spans[d->x_channel][d->y_channel][r->matrix.type];
	if (!r->span)
		return EINA_FALSE;
	return EINA_TRUE;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Creates a new displacement map renderer
 *
 * @return The renderer
 */
EAPI Enesim_Renderer * enesim_renderer_dispmap_new(void)
{
	Dispmap *d;
	Enesim_Renderer *r;

	d = calloc(1, sizeof(Dispmap));

	r = (Enesim_Renderer *)d;
	enesim_renderer_init(r);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);

	return r;
}
/**
 * Sets the channel to use as the x coordinate displacement
 * @param[in] r The displacement map renderer 
 * @param[in] channel The channel to use
 */
EAPI void enesim_renderer_dispmap_x_channel_set(Enesim_Renderer *r,
	Enesim_Channel channel)
{
	Dispmap *d = (Dispmap *)r;

	d->x_channel = channel;
}
/**
 * Sets the channel to use as the y coordinate displacement
 * @param[in] r The displacement map renderer 
 * @param[in] channel The channel to use
 */
EAPI void enesim_renderer_dispmap_y_channel_set(Enesim_Renderer *r,
	Enesim_Channel channel)
{
	Dispmap *d = (Dispmap *)r;

	d->y_channel = channel;
}
/**
 *
 */
EAPI void enesim_renderer_dispmap_map_set(Enesim_Renderer *r, Enesim_Surface *map)
{
	Dispmap *d = (Dispmap *)r;

	d->map = map;
}
/**
 *
 */
EAPI Enesim_Surface * enesim_renderer_dispmap_map_get(Enesim_Renderer *r)
{
	Dispmap *d = (Dispmap *)r;

	return d->map;
}
/**
 *
 */
EAPI void enesim_renderer_dispmap_src_set(Enesim_Renderer *r, Enesim_Surface *src)
{
	Dispmap *d = (Dispmap *)r;

	d->src = src;
}
/**
 *
 */
EAPI Enesim_Surface * enesim_renderer_dispmap_src_get(Enesim_Renderer *r)
{
	Dispmap *d = (Dispmap *)r;

	return d->src;
}
/**
 *
 */
EAPI void enesim_renderer_dispmap_scale_set(Enesim_Renderer *r, float scale)
{
	Dispmap *d = (Dispmap *)r;

	d->scale = scale;
}
/**
 *
 */
EAPI float enesim_renderer_dispmap_scale_get(Enesim_Renderer *r)
{
	Dispmap *d = (Dispmap *)r;

	return d->scale;
}

