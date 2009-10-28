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
typedef struct _Grid
{
	Enesim_Renderer base;
	struct {
		Enesim_Color color;
		unsigned int w;
		unsigned int h;
	} inside, outside;
	/* the state */
	int wt;
	int ht;
	Eina_F16p16 wi;
	Eina_F16p16 hi;
	Eina_F16p16 wwt;
	Eina_F16p16 hht;
} Grid;

static inline uint32_t _grid(Grid *g, Eina_F16p16 yy, Eina_F16p16 xx)
{
	Eina_F16p16 syy;
	int sy;
	uint32_t p0;

	/* normalize the modulo */
	syy = (yy % g->hht);
	if (syy < 0)
		syy += g->hht;
	/* simplest case, we are on the grid border
	 * the whole line will be outside color */
	sy = eina_f16p16_int_to(syy);
	if (syy >= g->hi)
	{
		p0 = g->outside.color;
	}
	else
	{
		Eina_F16p16 sxx;
		int sx;

		/* normalize the modulo */
		sxx = (xx % g->wwt);
		if (sxx < 0)
			sxx += g->wwt;

		sx = eina_f16p16_int_to(sxx);
		if (sxx >= g->wi)
		{
			p0 = g->outside.color;
		}
		/* totally inside */
		else
		{
			p0 = g->inside.color;
			/* antialias the inner square */
			if (sx == 0)
			{
				uint16_t a;

				a = 1 + ((sxx & 0xffff) >> 8);
				p0 = INTERP_256(a, p0, g->outside.color);
			}
			else if (sx == (g->inside.w - 1))
			{
				uint16_t a;

				a = 1 + ((sxx & 0xffff) >> 8);
				p0 = INTERP_256(a, g->outside.color, p0);

			}
			if (sy == 0)
			{
				uint16_t a;

				a = 1 + ((syy & 0xffff) >> 8);
				p0 = INTERP_256(a, p0, g->outside.color);
			}
			else if (sy == (g->inside.h - 1))
			{
				uint16_t a;

				a = 1 + ((syy & 0xffff) >> 8);
				p0 = INTERP_256(a, g->outside.color, p0);
			}
		}
	}
	return p0;
}


static void _span_identity(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Grid *g = (Grid *)r;
	uint32_t *end = dst + len;
	int sy;

	sy = (y % g->ht);
	if (sy < 0)
	{
		sy += g->ht;
	}
	/* simplest case, all the span is outside */
	if (sy >= g->inside.h)
	{
		while (dst < end)
			*dst++ = g->outside.color;
	}
	/* we swap between the two */
	else
	{
		while (dst < end)
		{
			int sx;

			sx = (x % g->wt);
			if (sx < 0)
				sx += g->wt;

			if (sx >= g->inside.w)
				*dst = g->outside.color;
			else
				*dst = g->inside.color;

			dst++;
			x++;
		}
	}
}

static void _span_affine(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Grid *g = (Grid *)r;
	uint32_t *end = dst + len;
	int sy;
	Eina_F16p16 yy, xx;

	renderer_affine_setup(r, x, y, &xx, &yy);

	while (dst < end)
	{
		Eina_F16p16 syy;
		int sy;
		uint32_t p0;

		p0 = _grid(g, yy, xx);

		yy += r->matrix.values.yx;
		xx += r->matrix.values.xx;
		*dst++ = p0;
	}
}

static void _span_projective(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Grid *g = (Grid *)r;
	uint32_t *end = dst + len;
	int sy;
	Eina_F16p16 yy, xx, zz;

	renderer_projective_setup(r, x, y, &xx, &yy, &zz);

	while (dst < end)
	{
		Eina_F16p16 syy, sxx, syyy, sxxx;
		uint32_t p0;

		syyy = ((((int64_t)yy) << 16) / zz);
		sxxx = ((((int64_t)xx) << 16) / zz);

		p0 = _grid(g, syyy, sxxx);

		yy += r->matrix.values.yx;
		xx += r->matrix.values.xx;
		zz += r->matrix.values.zx;
		*dst++ = p0;
	}
}

static void _state_cleanup(Enesim_Renderer *r)
{

}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	Grid *g = (Grid *)r;

	g->ht = g->inside.h + g->outside.h;
	g->wt = g->inside.w + g->outside.w;
	g->hht = eina_f16p16_int_from(g->ht);
	g->wwt = eina_f16p16_int_from(g->wt);

	if (r->matrix.type == ENESIM_MATRIX_IDENTITY)
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_identity);
	else if (r->matrix.type == ENESIM_MATRIX_AFFINE)
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_affine);
	else
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_projective);
	return EINA_TRUE;
}

static void _free(Grid *g)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * enesim_renderer_grid_new(void)
{
	Enesim_Renderer *r;
	Grid *g;

	g = calloc(1, sizeof(Grid));
	r = (Enesim_Renderer *)g;

	enesim_renderer_init(r);
	r->free = ENESIM_RENDERER_DELETE(_free);
	r->state_cleanup = ENESIM_RENDERER_STATE_CLEANUP(_state_cleanup);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);

	return r;
}

EAPI void enesim_renderer_grid_inside_size_set(Enesim_Renderer *r, unsigned int w, unsigned int h)
{
	Grid *g = (Grid *)r;

	g->inside.w = w;
	g->inside.h = h;
	g->wi = eina_f16p16_int_from(w);
	g->hi = eina_f16p16_int_from(h);
}

EAPI void enesim_renderer_grid_inside_color_set(Enesim_Renderer *r, Enesim_Color color)
{
	Grid *g = (Grid *)r;

	g->inside.color = color;
}

EAPI void enesim_renderer_grid_outside_size_set(Enesim_Renderer *r, unsigned int w, unsigned int h)
{
	Grid *g = (Grid *)r;

	g->outside.w = w;
	g->outside.h = h;
}

EAPI void enesim_renderer_grid_outside_color_set(Enesim_Renderer *r, Enesim_Color color)
{
	Grid *g = (Grid *)r;

	g->outside.color = color;
}

