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
/* TODO instead of colors we can also use renderers? for that we'll need the
 * get_pixel() on each renderer
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Checker
{
	Enesim_Renderer base;
	Enesim_Color color1;
	Enesim_Color color2;
	int sw;
	int sh;
} Checker;

static void _span_identity(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Checker *s = (Checker *)r;
	int w2 = s->sw * 2;
	int h2 = s->sh * 2;
	uint32_t color[2] = { s->color1, s->color2};
	uint32_t *end = dst + len;
	int sy;

	/* translate to the origin */
	y -= s->base.oy;
	x -= s->base.ox;
	/* normalize the modulo */
	sy = (y % h2);
	if (sy < 0)
	{
		sy += h2;
	}
	/* swap the colors */
	if (sy >= s->sh)
	{
		color[0] = s->color2;
		color[1] = s->color1;
	}
	while (dst < end)
	{
		int sx;
		uint32_t p0;

		sx = (x % w2);
		if (sx < 0)
		{
			sx += w2;
		}
		/* choose the correct color */
		if (sx >= s->sw)
		{
			p0 = color[0];
		}
		else
		{
			p0 = color[1];
		}
		*dst++ = p0;
		x++;
	}
}

static void _span_affine(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Checker *s = (Checker *)r;
	Eina_F16p16 yy, xx, ww, hh, ww2, hh2;
	uint32_t *end = dst + len;

	renderer_affine_setup(r, x, y, &xx, &yy);
	/* TODO move this to the state setup */
	ww = eina_f16p16_int_from(s->sw);
	ww2 = ww * 2;
	hh = eina_f16p16_int_from(s->sh);
	hh2 = hh * 2;

	while (dst < end)
	{
		Eina_F16p16 syy, sxx;
		uint32_t color[2] = { s->color1, s->color2};
		uint32_t p0;
		int sx, sy;

		/* normalize the modulo */
		syy = (yy % hh2);
		if (syy < 0)
		{
			syy += hh2;
		}
		sxx = (xx % ww2);
		if (sxx < 0)
		{
			sxx += ww2;
		}
		sy = eina_f16p16_int_to(syy);
		sx = eina_f16p16_int_to(sxx);
		/* choose the correct color */
		if (syy >= hh)
		{
			color[0] = s->color2;
			color[1] = s->color1;
		}
		if (sxx >= ww)
		{
			p0 = color[0];

			/* antialias the borders */
			if (sy == 0 || sy == s->sh)
			{
				uint16_t a;

				a = 1 + ((syy & 0xffff) >> 8);
				p0 = INTERP_256(a, p0, color[1]);
			}
			if (sx == 0 || sx == s->sw)
			{
				uint16_t a;

				a = 1 + ((sxx & 0xffff) >> 8);
				p0 = INTERP_256(a, p0, color[1]);
			}
		}
		else
		{
			p0 = color[1];
			/* antialias the borders */
			if (sy == 0 || sy == s->sh)
			{
				uint16_t a;

				a = 1 + ((syy & 0xffff) >> 8);
				p0 = INTERP_256(a, p0, color[0]);
			}
			if (sx == 0 || sx == s->sw)
			{
				uint16_t a;

				a = 1 + ((sxx & 0xffff) >> 8);
				p0 = INTERP_256(a, p0, color[0]);
			}
		}
		yy += r->matrix.values.yx;
		xx += r->matrix.values.xx;
		*dst++ = p0;
	}
}

static void _span_projective(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Checker *s = (Checker *)r;
	Eina_F16p16 yy, xx, ww, hh, ww2, hh2, zz;
	uint32_t *end = dst + len;

	/* translate to the origin */
	renderer_projective_setup(r, x, y, &xx, &yy, &zz);
	/* TODO move this to the state setup */
	ww = eina_f16p16_int_from(s->sw);
	ww2 = ww * 2;
	hh = eina_f16p16_int_from(s->sh);
	hh2 = hh * 2;

	while (dst < end)
	{
		Eina_F16p16 syy, sxx, syyy, sxxx;
		uint32_t color[2] = { s->color1, s->color2};
		uint32_t p0;
		int sx, sy;

		syyy = ((((int64_t)yy) << 16) / zz);
		sxxx = ((((int64_t)xx) << 16) / zz);
		/* normalize the modulo */
		syy = (syyy % hh2);
		if (syy < 0)
		{
			syy += hh2;
		}
		sxx = (sxxx % ww2);
		if (sxx < 0)
		{
			sxx += ww2;
		}
		sy = eina_f16p16_int_to(syy);
		sx = eina_f16p16_int_to(sxx);
		/* choose the correct color */
		if (syy >= hh)
		{
			color[0] = s->color2;
			color[1] = s->color1;
		}
		if (sxx >= ww)
		{
			p0 = color[0];

			/* antialias the borders */
			if (sy == 0 || sy == s->sh)
			{
				uint16_t a;

				a = 1 + ((syy & 0xffff) >> 8);
				p0 = INTERP_256(a, p0, color[1]);
			}
			if (sx == 0 || sx == s->sw)
			{
				uint16_t a;

				a = 1 + ((sxx & 0xffff) >> 8);
				p0 = INTERP_256(a, p0, color[1]);
			}
		}
		else
		{
			p0 = color[1];
			/* antialias the borders */
			if (sy == 0 || sy == s->sh)
			{
				uint16_t a;

				a = 1 + ((syy & 0xffff) >> 8);
				p0 = INTERP_256(a, p0, color[0]);
			}
			if (sx == 0 || sx == s->sw)
			{
				uint16_t a;

				a = 1 + ((sxx & 0xffff) >> 8);
				p0 = INTERP_256(a, p0, color[0]);
			}
		}
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
	Checker *c = (Checker *)r;

	if (r->matrix.type == ENESIM_MATRIX_IDENTITY)
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_identity);
	else if (r->matrix.type == ENESIM_MATRIX_AFFINE)
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_affine);
	else
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_projective);
	return EINA_TRUE;
}

static void _free(Checker *s)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Creates a checker renderer
 * @return The new renderer
 */
EAPI Enesim_Renderer * enesim_renderer_checker_new(void)
{
	Enesim_Renderer *r;
	Checker *s;

	s = calloc(1, sizeof(Checker));
	r = (Enesim_Renderer *)s;

	enesim_renderer_init(r);
	r->free = ENESIM_RENDERER_DELETE(_free);
	r->state_cleanup = ENESIM_RENDERER_STATE_CLEANUP(_state_cleanup);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);

	return r;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_checker_color1_set(Enesim_Renderer *r, Enesim_Color color)
{
	Checker *s = (Checker *)r;

	s->color1 = color;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_checker_color2_set(Enesim_Renderer *r, Enesim_Color color)
{
	Checker *s = (Checker *)r;

	s->color2 = color;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_checker_size_set(Enesim_Renderer *r, int w, int h)
{
	Checker *s = (Checker *)r;

	s->sw = w;
	s->sh = h;
}
