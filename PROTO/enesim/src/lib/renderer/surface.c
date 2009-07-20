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
/* TODO
 * + add support for sw and sh
 * + add support for qualities (good scaler, interpolate between the four neighbours)
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Surface
{
	Enesim_Renderer r;
	Enesim_Surface *s;
	int x, y;
	unsigned int w, h;
	int *yoff;
	int *xoff;
} Surface;

static inline void _offsets(unsigned int cs, unsigned int cl, unsigned int sl, int *off)
{
	int c;

	for (c = 0; c < cl; c++)
	{
		off[c] = ((c + cs) * sl) / cl;
	}
}

static void _scale_fast(Surface *s, int x, int y, unsigned int len, uint32_t *dst)
{
	uint32_t sstride;
	uint32_t *src;
	Eina_Rectangle ir, dr;

	eina_rectangle_coords_from(&ir, x, y, len, 1);
	eina_rectangle_coords_from(&dr, s->x, s->y, s->w, s->h);
	if (!eina_rectangle_intersection(&ir, &dr))
		return;


	src = enesim_surface_data_get(s->s);
	sstride = enesim_surface_stride_get(s->s);
	src += sstride * s->yoff[ir.y];

	while (ir.w--)
	{
		*dst = *(src + s->xoff[ir.x++]);
		dst++;
	}
}

static void _noscale(Surface *s, int x, int y, unsigned int len, uint32_t *dst)
{
	uint32_t sstride;
	uint32_t *src;

	src = enesim_surface_data_get(s->s);
	sstride = enesim_surface_stride_get(s->s);
	src += sstride * y + x;

	memcpy(dst, src, len * 4);
}

/* FIXME this should be a state setup, which will set the correct span gen function */
static void _span_draw(Surface *s, int x, int y, unsigned int len, uint32_t *dst)
{
	int sw, sh;

	if (s->xoff)
		free(s->xoff);

	if (s->yoff)
		free(s->yoff);

	enesim_surface_size_get(s->s, &sw, &sh);
	if (sw != s->w && sh != s->h)
	{
		s->xoff = malloc(sizeof(int) * s->w);
		s->yoff = malloc(sizeof(int) * s->h);
		_offsets(s->x, s->w, sw, s->xoff);
		_offsets(s->y, s->h, sh, s->yoff);

		_scale_fast(s, x, y, len, dst);
	}
	else
	{
		_noscale(s, x, y, len, dst);
	}
}

static void _free(Surface *s)
{
	free(s);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * enesim_renderer_surface_new(void)
{
	Surface *s = calloc(1, sizeof(Surface));

	s->r.free = _free;
	s->r.span = _span_draw;
	return s;
}

EAPI void enesim_renderer_surface_x_set(Enesim_Renderer *r, int x)
{
	Surface *s = (Surface *)r;
	s->x = x;
}

EAPI void enesim_renderer_surface_y_set(Enesim_Renderer *r, int y)
{
	Surface *s = (Surface *)r;
	s->y = y;
}

EAPI void enesim_renderer_surface_w_set(Enesim_Renderer *r, int w)
{
	Surface *s = (Surface *)r;

	if (s->w == w)
		return;
	s->w = w;
}

EAPI void enesim_renderer_surface_h_set(Enesim_Renderer *r, int h)
{
	Surface *s = (Surface *)r;

	if (s->h == h)
		return;
	s->h = h;
}

EAPI void enesim_renderer_surface_src_set(Enesim_Renderer *r, Enesim_Surface *src)
{
	Surface *s = (Surface *)r;
	s->s = src;
}
