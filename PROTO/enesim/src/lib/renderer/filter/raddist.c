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

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Raddist
{
	Enesim_Renderer base;
	Enesim_Surface *src;
	float scale;
	float radius;
	/* the x and y origin of the circle */
	int orx, ory;
} Raddist;

static void _span_identity(Enesim_Renderer *r, int x, int y,
		unsigned int len, uint32_t *dst)
{
	Raddist *rd = (Raddist *)r;
	uint32_t *end = dst + len;
	float r_inv;
	uint32_t *src;
	int sw, sh;
	int sstride;

	/* setup the parameters */
	enesim_surface_size_get(rd->src, &sw, &sh);
	sstride = enesim_surface_stride_get(rd->src);
	src = enesim_surface_data_get(rd->src);
	/* FIXME move this to the setup */
	r_inv = 1.0f / rd->radius;

	x -= rd->orx;
	y -= rd->ory;

	while (dst < end)
	{
		Eina_F16p16 sxx, syy;
		uint32_t p0;
		int sx, sy;
		float r = hypot(x, y);

		r = (((rd->scale * (rd->radius - r)) + r) * r_inv);
		sxx = eina_f16p16_float_from((r * x) + rd->orx);
		syy = eina_f16p16_float_from((r * y) + rd->ory);

		sy = (syy >> 16);
		sx = (sxx >> 16);
		p0 = argb8888_sample_good(src, sstride, sw, sh, sxx, syy, sx, sy);

		*dst++ = p0;
		x++;
	}
}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	r->span = ENESIM_RENDERER_SPAN_DRAW(_span_identity);

	if (r->matrix.type == ENESIM_MATRIX_IDENTITY)
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_identity);
	else
		return EINA_FALSE;
	return EINA_TRUE;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * enesim_renderer_raddist_new(void)
{
	Raddist *rd;
	Enesim_Renderer *r;

	rd = calloc(1, sizeof(Raddist));

	r = (Enesim_Renderer *)rd;
	enesim_renderer_init(r);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);

	return r;
}

EAPI void enesim_renderer_raddist_radius_set(Enesim_Renderer *r, float radius)
{
	Raddist *rd = (Raddist *)r;

	if (!radius)
		radius = 1;
	rd->radius = radius;
}

EAPI void enesim_renderer_raddist_scale_set(Enesim_Renderer *r, float scale)
{
	Raddist *rd = (Raddist *)r;

	if (scale > 1.0)
		scale = 1.0;
	rd->scale = scale;
}

EAPI void enesim_renderer_raddist_src_set(Enesim_Renderer *r, Enesim_Surface *src)
{
	Raddist *rd = (Raddist *)r;

	rd->src = src;
}

EAPI void enesim_renderer_raddist_center_set(Enesim_Renderer *r, int ox, int oy)
{
	Raddist *rd = (Raddist *)r;

	rd->orx = ox;
	rd->ory = oy;
}
