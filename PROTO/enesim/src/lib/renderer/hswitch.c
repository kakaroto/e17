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
typedef struct _Hswitch
{
	Enesim_Renderer base;
	unsigned int w;
	unsigned int h;
	Enesim_Renderer *lrend;
	Enesim_Renderer *rrend;
	float step;
} Hswitch;


static void _generic_good(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Hswitch *hs = (Hswitch *)r;
	uint32_t *end = dst + len;
	Eina_F16p16 mmx;
	int mx;

	mmx = eina_f16p16_float_from(hs->w - (float)(hs->w * hs->step));
	mx = eina_f16p16_int_to(mmx);
	while (dst < end)
	{
		uint32_t p0;

		if (x > mx)
		{
			enesim_renderer_span_fill(hs->rrend, x, y, 1, &p0);
		}
		else if (x < mx)
		{
			enesim_renderer_span_fill(hs->lrend, x, y, 1, &p0);
		}
		else
		{
			uint32_t p1;
			uint16_t a;

			a = 1 + ((mmx & 0xffff) >> 8);
			enesim_renderer_span_fill(hs->lrend, x, y, 1, &p0);
			enesim_renderer_span_fill(hs->rrend, 0, y, 1, &p1);
			p0 = argb8888_interp_256(a, p0, p1);
		}
		*dst++ = p0;
		x++;
	}
}

static void _affine_good(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Hswitch *hs = (Hswitch *)r;
	uint32_t *end = dst + len;
	Eina_F16p16 mmx, xx, yy;
	int mx;

	yy = eina_f16p16_int_from(y);
	xx = eina_f16p16_int_from(x);
	yy = eina_f16p16_mul(r->matrix.values.yx, xx) +
			eina_f16p16_mul(r->matrix.values.yy, yy) + r->matrix.values.yz;
	xx = eina_f16p16_mul(r->matrix.values.xx, xx) +
			eina_f16p16_mul(r->matrix.values.xy, yy) + r->matrix.values.xz;

	/* FIXME put this on the state setup */
	mmx = eina_f16p16_float_from(hs->w - (float)(hs->w * hs->step));
	mx = eina_f16p16_int_to(mmx);
	while (dst < end)
	{
		uint32_t p0;

		x = eina_f16p16_int_to(xx);
		y = eina_f16p16_int_to(yy);
		if (x > mx)
		{
			enesim_renderer_span_fill(hs->rrend, x, y, 1, &p0);
		}
		else if (x < mx)
		{
			enesim_renderer_span_fill(hs->lrend, x, y, 1, &p0);
		}
		/* FIXME, what should we use here? mmx or xx?
		 * or better use a subpixel center?
		 */
		else
		{
			uint32_t p1;
			uint16_t a;

			a = 1 + ((xx & 0xffff) >> 8);
			enesim_renderer_span_fill(hs->lrend, x, y, 1, &p0);
			enesim_renderer_span_fill(hs->rrend, 0, y, 1, &p1);
			p0 = argb8888_interp_256(a, p1, p0);
		}
		*dst++ = p0;
		xx += r->matrix.values.xx;
		yy += r->matrix.values.yx;
	}
}

static void _generic_fast(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Hswitch *hs = (Hswitch *)r;
	int mx;
	Eina_Rectangle ir, dr;

	eina_rectangle_coords_from(&ir, x, y, len, 1);
	eina_rectangle_coords_from(&dr, 0, 0, hs->w, hs->h);
	if (!eina_rectangle_intersection(&ir, &dr))
		return;

	mx = hs->w - (hs->w * hs->step);
	if (mx == 0)
	{
		enesim_renderer_span_fill(hs->rrend, ir.x, ir.y, ir.w, dst);
	}
	else if (mx == hs->w)
	{
		enesim_renderer_span_fill(hs->lrend, ir.x, ir.y, ir.w, dst);
	}
	else
	{
		if (ir.x > mx)
		{
			enesim_renderer_span_fill(hs->rrend, ir.x, ir.y, ir.w, dst);
		}
		else if (ir.x + ir.w < mx)
		{
			enesim_renderer_span_fill(hs->lrend, ir.x, ir.y, ir.w, dst);
		}
		else
		{
			int w;

			w = mx - ir.x;
			enesim_renderer_span_fill(hs->lrend, ir.x, ir.y, w, dst);
			dst += w;
			enesim_renderer_span_fill(hs->rrend, 0, ir.y, ir.w + ir.x - mx , dst);
		}
	}
}

static void _free(Hswitch *hs)
{

}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	Hswitch *h = (Hswitch *)r;

	if (!h->lrend || !h->rrend)
		return EINA_FALSE;
	if (!enesim_renderer_state_setup(h->lrend))
		return EINA_FALSE;
	if (!enesim_renderer_state_setup(h->rrend))
		return EINA_FALSE;

	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer * enesim_renderer_hswitch_new(void)
{
	Enesim_Renderer *r;
	Hswitch *h;

	h = calloc(1, sizeof(Hswitch));
	r = (Enesim_Renderer *)h;
	enesim_renderer_init(r);
	r->free = ENESIM_RENDERER_DELETE(_free);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);
	r->span = ENESIM_RENDERER_SPAN_DRAW(_affine_good);

	return r;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_hswitch_w_set(Enesim_Renderer *r, int w)
{
	Hswitch *hs = (Hswitch *)r;

	if (hs->w == w)
		return;
	hs->w = w;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_hswitch_h_set(Enesim_Renderer *r, int h)
{
	Hswitch *hs = (Hswitch *)r;

	if (hs->h == h)
		return;
	hs->h = h;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_hswitch_left_set(Enesim_Renderer *r,
		Enesim_Renderer *left)
{
	Hswitch *hs = (Hswitch *)r;

	hs->lrend = left;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_hswitch_right_set(Enesim_Renderer *r,
		Enesim_Renderer *right)
{
	Hswitch *hs = (Hswitch *)r;

	hs->rrend = right;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_hswitch_step_set(Enesim_Renderer *r, float step)
{
	Hswitch *hs = (Hswitch *)r;

	if (step < 0)
		step = 0;
	else if (step > 1)
		step = 1;
	hs->step = step;
}
