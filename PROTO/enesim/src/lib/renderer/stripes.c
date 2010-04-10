/* ENESIM - Direct Rendering Library
 * Copyright (C) 2007-2010 Jorge Luis Zapata
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
typedef struct _Stripes Stripes;
struct _Stripes {
	Enesim_Renderer base;

	struct {
		Enesim_Color color;
		int thickness;
	} s0, s1;

	int hh0, hh;
};

static void _span_projective(Enesim_Renderer *p, int x, int y,
		int len, unsigned int *dst)
{
	Stripes *st = (Stripes *) p;
	int hh = st->hh, hh0 = st->hh0, h0 = hh0 >> 16;
	unsigned int c0 = st->s0.color, c1 = st->s1.color;
	unsigned int *d = dst, *e = d + len;
	Eina_F16p16 yy, xx, zz;

	//yy = (ayx * x) + (ayy * y) + ayz;
	//xx = (axx * x) + (axy * y) + axz;
	renderer_projective_setup(p, x, y, &xx, &yy, &zz);

	while (d < e)
	{
		Eina_F16p16 syy, syyy;
		unsigned int p0 = c0;
		int sy;

		syyy = ((((int64_t)yy) << 16) / zz);
		syy = (syyy % hh);

		if (syy < 0)
			syy += hh;
		sy = syy >> 16;

		if (sy == 0)
		{
			int a = 1 + ((syy & 0xffff) >> 8);

			p0 = INTERP_256(a, c0, c1);
		}
		if (syy >= hh0)
		{
			p0 = c1;
			if (sy == h0)
			{
				int a = 1 + ((syy & 0xffff) >> 8);

				p0 = INTERP_256(a, c1, c0);
			}
		}
		*d++ = p0;
		yy += p->matrix.values.yx;
		zz += p->matrix.values.zx;
	}
}

static void _span_affine(Enesim_Renderer *p, int x, int y,
		int len, unsigned int *dst)
{
	Stripes *st = (Stripes *) p;
	int ayx = p->matrix.values.yx, ayy = p->matrix.values.yy, ayz = p->matrix.values.yz;
	int hh = st->hh, hh0 = st->hh0, h0 = hh0 >> 16;
	unsigned int c0 = st->s0.color, c1 = st->s1.color;
	unsigned int *d = dst, *e = d + len;
	int yy;

	yy = (ayx * x) + (ayy * y) + ayz;

	while (d < e)
	{
		unsigned int p0 = c0;
		int syy = (yy % hh), sy;

		if (syy < 0)
			syy += hh;
		sy = syy >> 16;

		if (sy == 0)
		{
			int a = 1 + ((syy & 0xffff) >> 8);

			p0 = INTERP_256(a, c0, c1);
		}
		if (syy >= hh0)
		{
			p0 = c1;
			if (sy == h0)
			{
				int a = 1 + ((syy & 0xffff) >> 8);

				p0 = INTERP_256(a, c1, c0);
			}
		}
		*d++ = p0;
		yy += ayx;
	}
}

static int _setup_state(Enesim_Renderer *r, int len)
{
	Stripes *st = (Stripes *)r;

	if (!st)
		return EINA_FALSE;
	st->hh0 = st->s0.thickness * 65536;
	st->hh = st->hh0 + (st->s1.thickness * 65536);

	if (r->matrix.type == ENESIM_MATRIX_IDENTITY)
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_affine);
	else if (r->matrix.type == ENESIM_MATRIX_AFFINE)
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_affine);
	else
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_projective);
	return EINA_TRUE;
}

static void _cleanup_state(Enesim_Renderer *p)
{
}

static void _destroy(Enesim_Renderer *p)
{
}


/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Creates a stripe renderer
 * @return The new renderer
 */
EAPI Enesim_Renderer * enesim_renderer_stripes_new(void)
{
	Enesim_Renderer *r;
	Stripes *st;

	st = calloc(1, sizeof(Stripes));
	if (!st)
		return NULL;
	r = (Enesim_Renderer *)st;
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_setup_state);
	enesim_renderer_init(r);
	return r;
}
/**
 * Sets the color of the even stripes
 * @param[in] r The stripes renderer
 * @param[in] color The even stripes color
 */
EAPI void enesim_renderer_stripes_even_color_set(Enesim_Renderer *r,
		Enesim_Color color)
{
	Stripes *st;

	st = (Stripes *)r;
	st->s0.color = color;
}
/**
 * Gets the color of the even stripes
 * @param[in] r The stripes renderer
 * @return The even stripes color
 */
EAPI Enesim_Color enesim_renderer_stripes_even_color_get(Enesim_Renderer *r)
{
	Stripes *st;

	st = (Stripes *)r;
	return st->s0.color;
}
/**
 * Sets the thickness of the even stripes
 * @param[in] r The stripes renderer
 * @param[in] thickness The even stripes thickness
 */
EAPI void enesim_renderer_stripes_even_thickness_set(Enesim_Renderer *r,
		float thickness)
{
	Stripes *st;

	st = (Stripes *)r;
	if (thickness < 0.99999)
		thickness = 1;
	st->s0.thickness = thickness;
}
/**
 * Gets the thickness of the even stripes
 * @param[in] r The stripes renderer
 * @param[in] thickness The even stripes thickness
 */
EAPI float enesim_renderer_stripes_even_thickness_get(Enesim_Renderer *r)
{
	Stripes *st;

	st = (Stripes *)r;
	return st->s0.thickness;
}
/**
 * Sets the color of the odd stripes
 * @param[in] r The stripes renderer
 * @param[in] color The odd stripes color
 */
EAPI void enesim_renderer_stripes_odd_color_set(Enesim_Renderer *r,
		Enesim_Color color)
{
	Stripes *st;

	st = (Stripes *)r;
	st->s1.color = color;
}
/**
 * Gets the color of the odd stripes
 * @param[in] r The stripes renderer
 * @return The odd stripes color
 */
EAPI Enesim_Color enesim_renderer_stripes_odd_color_get(Enesim_Renderer *r)
{
	Stripes *st;

	st = (Stripes *)r;
	return st->s1.color;
}
/**
 * Sets the thickness of the odd stripes
 * @param[in] r The stripes renderer
 * @param[in] thickness The odd stripes thickness
 */
EAPI void enesim_renderer_stripes_odd_thickness_set(Enesim_Renderer *r,
		float thickness)
{
	Stripes *st;

	st = (Stripes *)r;
	if (thickness < 0.99999)
		thickness = 1;
	st->s1.thickness = thickness;
}
/**
 * Gets the thickness of the odd stripes
 * @param[in] r The stripes renderer
 * @param[in] thickness The odd stripes thickness
 */
EAPI float enesim_renderer_stripes_odd_thickness_get(Enesim_Renderer *r)
{
	Stripes *st;

	st = (Stripes *)r;
	return st->s1.thickness;
}

