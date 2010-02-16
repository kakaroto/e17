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
typedef struct _Stripes Stripes;
struct _Stripes {
	Enesim_Renderer base;

	struct {
		Enesim_Color color;
		int thickness;
	} s0, s1;

	int hh0, hh;
};

static void stripes_span_affine(Enesim_Renderer *p, int x, int y,
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

static int stripes_setup_state(Enesim_Renderer *p, int len)
{
	Stripes *st = (Stripes *) p;

	if (!st)
		return 0;
	st->hh0 = st->s0.thickness * 65536;
	st->hh = st->hh0 + (st->s1.thickness * 65536);
	p->span = ENESIM_RENDERER_SPAN_DRAW(stripes_span_affine);
	return 1;
}

static void stripes_cleanup_state(Enesim_Renderer *p)
{
}

static void stripes_destroy(Enesim_Renderer *p)
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
	Enesim_Renderer *p;
	Stripes *st;

	st = calloc(1, sizeof(Stripes));
	if (!st)
		return NULL;
	p = (Enesim_Renderer *) st;
	p->state_setup = ENESIM_RENDERER_STATE_SETUP(stripes_setup_state);
	enesim_renderer_init(p);
	return p;
}
/**
 * Sets the color of the stripes
 * @param[in] p The stripes renderer
 * @param[in] c0 The even stripes color
 * @param[in] c1 The odd stripes color
 */
EAPI void enesim_renderer_stripes_color_set(Enesim_Renderer *p,
		Enesim_Color c0, Enesim_Color c1)
{
	Stripes *st;

	st = (Stripes *) p;
	st->s0.color = c0;
	st->s1.color = c1;
}
/**
 * Sets the thickness of the stripes
 * @param[in] p The stripes renderer
 * @param[in] thickness_0 The even stripes thickness
 * @param[in] thickness_0 The odd stripes thickness
 */
EAPI void enesim_renderer_stripes_thickness_set(Enesim_Renderer *p,
		float thickness_0, float thickness_1)
{
	Stripes *st;

	st = (Stripes *) p;
	if (thickness_0 < 0.99999)
		thickness_0 = 1;
	if (thickness_1 < 0.99999)
		thickness_1 = 1;
	st->s0.thickness = thickness_0;
	st->s1.thickness = thickness_1;
}
