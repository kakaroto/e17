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
	Enesim_Renderer r;
	unsigned int w;
	unsigned int h;
	Enesim_Renderer *lrend;
	Enesim_Renderer *rrend;
	float step;
} Hswitch;


static void _generic(Hswitch *hs, int x, int y, unsigned int len, uint32_t *dst)
{
	int mx;
	Eina_Rectangle ir, dr;

	eina_rectangle_coords_from(&ir, x, y, len, 1);
	eina_rectangle_coords_from(&dr, 0, 0, hs->w, hs->h);
	if (!eina_rectangle_intersection(&ir, &dr))
		return;

	mx = hs->w * hs->step;
	if (mx == 0)
	{
		enesim_renderer_span_fill(hs->lrend, ir.x, ir.y, ir.w, dst);
	}
	else if (mx == hs->w)
	{
		enesim_renderer_span_fill(hs->rrend, ir.x, ir.y, ir.w, dst);
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
	enesim_renderer_delete(hs->lrend);
	enesim_renderer_delete(hs->rrend);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * enesim_renderer_hswitch_new(void)
{
	Hswitch *h = calloc(1, sizeof(Hswitch));

	h->r.free = _free;
	h->r.span = _generic;
	h->lrend = enesim_renderer_surface_new();
	h->rrend = enesim_renderer_surface_new();

	return h;
}

EAPI void enesim_renderer_hswitch_w_set(Enesim_Renderer *r, int w)
{
	Hswitch *hs = (Hswitch *)r;

	if (hs->w == w)
		return;
	hs->w = w;
	enesim_renderer_surface_w_set(hs->lrend, w);
	enesim_renderer_surface_w_set(hs->rrend, w);
}

EAPI void enesim_renderer_hswitch_h_set(Enesim_Renderer *r, int h)
{
	Hswitch *hs = (Hswitch *)r;

	if (hs->h == h)
		return;
	hs->h = h;
	enesim_renderer_surface_h_set(hs->lrend, h);
	enesim_renderer_surface_h_set(hs->rrend, h);
}

EAPI void enesim_renderer_hswitch_left_set(Enesim_Renderer *r,
		Enesim_Surface *s)
{
	Hswitch *hs = (Hswitch *)r;

	enesim_renderer_surface_src_set(hs->lrend, s);
}

EAPI void enesim_renderer_hswitch_right_set(Enesim_Renderer *r,
		Enesim_Surface *s)
{
	Hswitch *hs = (Hswitch *)r;

	enesim_renderer_surface_src_set(hs->rrend, s);
}

EAPI void enesim_renderer_hswitch_step_set(Enesim_Renderer *r, float step)
{
	Hswitch *hs = (Hswitch *)r;

	if (step < 0)
		step = 0;
	else if (step > 1)
		step = 1;
	hs->step = step;
}
