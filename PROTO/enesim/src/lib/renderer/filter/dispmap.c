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
	float scale;
} Dispmap;

static void _argb8888_a_b_span_identity(Enesim_Renderer *r, int x, int y,
		unsigned int len, uint32_t *dst)
{
	Dispmap *d = (Dispmap *)r;
	uint32_t *end = dst + len;
	uint32_t *map, *src;
	int mstride;
	int sstride;
	int sw, sh, mw, mh;
	Eina_F16p16 scale, xx, yy;

	/* setup the parameters */
	enesim_surface_size_get(d->src, &sw, &sh);
	enesim_surface_size_get(d->map, &mw, &mh);
	mstride = enesim_surface_stride_get(d->map);
	sstride = enesim_surface_stride_get(d->src);
	map = enesim_surface_data_get(d->map);
	src = enesim_surface_data_get(d->src);

	map = map + (mstride * y) + x;
	/* FIXME put this on the setup */
	yy = eina_f16p16_int_from(y);
	scale = eina_f16p16_float_from(d->scale);

	while (dst < end)
	{
		Eina_F16p16 sxx, syy, vx, vy;
		int sx, sy;
		uint32_t p0 = 0;
		uint16_t m0;
		uint16_t m1;

		if (x < 0 || x >= mw || y < 0 || y >= mh)
			goto next;

		xx = eina_f16p16_int_from(x);

		m0 = *map >> 24;
		m1 = *map & 0xff;
		/* FIXME define fixed(255) as a constant */
		vx = eina_f16p16_int_from(m0 - 127);
		vx = eina_f16p16_mul((((int64_t)(vx) << 16) / eina_f16p16_int_from(255)), scale);

		vy = eina_f16p16_int_from(m1 - 127);
		vy = eina_f16p16_mul((((int64_t)(vy) << 16) / eina_f16p16_int_from(255)), scale);

		sxx = xx + vx;
		syy = yy + vy;

		sx = eina_f16p16_int_to(sxx);
		sy = eina_f16p16_int_to(syy);
		p0 = argb8888_sample_good(src, sstride, sw, sh, sxx, syy, sx, sy);

next:
		*dst++ = p0;
		map++;
		x++;
	}
}

static void _argb8888_a_b_span_affine(Enesim_Renderer *r, int x, int y,
		unsigned int len, uint32_t *dst)
{
	Dispmap *d = (Dispmap *)r;
	uint32_t *end = dst + len;
	uint32_t *map, *src;
	int mstride;
	int sstride;
	int sw, sh, mw, mh;
	Eina_F16p16 scale, xx, yy;

	/* setup the parameters */
	enesim_surface_size_get(d->src, &sw, &sh);
	enesim_surface_size_get(d->map, &mw, &mh);
	mstride = enesim_surface_stride_get(d->map);
	sstride = enesim_surface_stride_get(d->src);
	map = enesim_surface_data_get(d->map);
	src = enesim_surface_data_get(d->src);

	/* TODO move by the origin */
	renderer_affine_setup(r, x, y, &xx, &yy);
	scale = eina_f16p16_float_from(d->scale);

	while (dst < end)
	{
		Eina_F16p16 sxx, syy, vx, vy;
		int sx, sy;
		uint32_t p0 = 0;
		uint16_t m0;
		uint16_t m1;

		x = eina_f16p16_int_to(xx);
		y = eina_f16p16_int_to(yy);

		if (x < 0 || x >= mw || y < 0 || y >= mh)
			goto next;

		m0 = *(map + (mstride * y) + x);
		m1 = m0 & 0xff;
		m0 = m0 >> 24;

		/* FIXME define fixed(255) as a constant */
		vx = eina_f16p16_int_from(m0 - 127);
		vx = eina_f16p16_mul((((int64_t)(vx) << 16) / eina_f16p16_int_from(255)), scale);

		vy = eina_f16p16_int_from(m1 - 127);
		vy = eina_f16p16_mul((((int64_t)(vy) << 16) / eina_f16p16_int_from(255)), scale);

		sxx = xx + vx;
		syy = yy + vy;

		sx = eina_f16p16_int_to(sxx);
		sy = eina_f16p16_int_to(syy);
		p0 = argb8888_sample_good(src, sstride, sw, sh, sxx, syy, sx, sy);

next:
		*dst++ = p0;
		map++;
		yy += r->matrix.values.yx;
		xx += r->matrix.values.xx;
	}
}



static void _state_cleanup(Enesim_Renderer *r)
{

}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	if (r->matrix.type == ENESIM_MATRIX_IDENTITY)
		r->span = ENESIM_RENDERER_SPAN_DRAW(_argb8888_a_b_span_identity);
	else if (r->matrix.type == ENESIM_MATRIX_AFFINE)
		r->span = ENESIM_RENDERER_SPAN_DRAW(_argb8888_a_b_span_affine);
	return EINA_TRUE;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
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

EAPI void enesim_renderer_dispmap_map_set(Enesim_Renderer *r, Enesim_Surface *map)
{
	Dispmap *d = (Dispmap *)r;

	d->map = map;
}


EAPI void enesim_renderer_dispmap_src_set(Enesim_Renderer *r, Enesim_Surface *src)
{
	Dispmap *d = (Dispmap *)r;

	d->src = src;
}

EAPI void enesim_renderer_dispmap_scale_set(Enesim_Renderer *r, float scale)
{
	Dispmap *d = (Dispmap *)r;

	d->scale = scale;
}
