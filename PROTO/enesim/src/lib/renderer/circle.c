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
typedef struct _Circle {
	Enesim_Renderer base;

	float x, y;
	float r;

	struct {
		unsigned int color;
		Enesim_Renderer *r;
		float weight;
	} stroke;

	struct {
		unsigned int color;
		Enesim_Renderer *r;
	} fill;

	int draw_mode;

	int xx0, yy0;
	int rr0, irr0;
	unsigned char do_inner :1;
} Circle;

static void _outlined_fill_paint(Enesim_Renderer *r, int x, int y,
		int len, unsigned int *dst)
{
	Circle *circ = (Circle *)r;
	int axx = r->axx, axy = r->axy, axz = r->axz;
	int ayx = r->ayx, ayy = r->ayy, ayz = r->ayz;
	int do_inner = circ->do_inner;
	unsigned int ocolor = circ->stroke.color;
	unsigned int icolor = circ->fill.color;
	int rr0 = circ->rr0, rr1 = rr0 + 65536;
	int irr0 = circ->irr0, irr1 = irr0 + 65536;
	int rr2 = rr1 * 1.41421357, irr2 = irr1 * 1.41421357; // sqrt(2)
	int xx0 = circ->xx0, yy0 = circ->yy0;
	Enesim_Renderer *fpaint = circ->fill.r;
	unsigned int *d = dst, *e = d + len;
	int xx, yy;

	printf("Rendering the circle %d %g %d %d\n", do_inner, circ->r, x, y);
	if (circ->draw_mode == ENESIM_RENDERER_DRAW_MODE_STROKE)
		icolor = 0;

	if (do_inner)
		fpaint->span(fpaint, x, y, len, dst);

	xx = (axx * x) + (axy * y) + axz - xx0;
	yy = (ayx * x) + (ayy * y) + ayz - yy0;

	while (d < e)
	{
		unsigned int q0 = 0;

		if ((abs(xx) <= rr1) && (abs(yy) <= rr1))
		{
			unsigned int op0 = ocolor, p0;
			int a = 256;

			if (abs(xx) + abs(yy) >= rr0)
			{
				a = 0;
				if (abs(xx) + abs(yy) <= rr2)
				{
					int rr = hypot(xx, yy);

					if (rr < rr1)
					{
						a = 256;
						if (rr > rr0)
							a -= ((rr - rr0) >> 8);
					}
				}
			}

			if (a < 256)
				op0 = argb8888_mul_256(a, op0);

			p0 = op0;
			if (do_inner && (abs(xx) <= irr1) && (abs(yy) <= irr1))
			{
				p0 = *d;
				if (icolor != 0xffffffff)
					p0 = argb8888_mul4_sym(icolor, p0);

				a = 256;
				if (abs(xx) + abs(yy) >= irr0)
				{
					a = 0;
					if (abs(xx) + abs(yy) <= irr2)
					{
						int rr = hypot(xx, yy);

						if (rr < irr1)
						{
							a = 256;
							if (rr > irr0)
								a -= ((rr - irr0) >> 8);
						}
					}
				}

				if (a < 256)
					p0 = INTERP_256(a, p0, op0);
			}
			q0 = p0;
		}
		*d++ = q0;
		xx += axx;
		yy += ayx;
	}
}


static int _state_setup(Enesim_Renderer *r)
{
	Circle *circ = (Circle *)r;
	float rad;
	float sw;

	if (!circ || (circ->r < 1))
		return EINA_FALSE;

	circ->rr0 = 65536 * (circ->r - 1);
	circ->xx0 = 65536 * (circ->x - 0.5);
	circ->yy0 = 65536 * (circ->y - 0.5);

	sw = circ->stroke.weight;
	circ->do_inner = 1;
	if (sw >= (circ->r - 1))
	{
		sw = 0;
		circ->do_inner = 0;
		printf("Entered %g %g\n", circ->r, sw);
	}
	rad = circ->r - 1 - sw;
	if (rad < 0.0039)
		rad = 0;

	circ->irr0 = rad * 65536;
	if (circ->fill.r)
	{
		if (!enesim_renderer_state_setup(circ->fill.r))
			return EINA_FALSE;
		r->span = ENESIM_RENDERER_SPAN_DRAW(_outlined_fill_paint);
	}

	return EINA_TRUE;
}

static void _state_cleanup(Enesim_Renderer *r)
{
	Circle *circ = (Circle *)r;

	if (circ->fill.r)
		enesim_renderer_state_cleanup(circ->fill.r);
}

static void _free(Enesim_Renderer *r)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * enesim_renderer_circle_new(void)
{
	Enesim_Renderer *r;
	Circle *circ;

	circ = calloc(1, sizeof(Circle));
	if (!circ)
		return NULL;
	circ->fill.color = 0xffffffff;
	circ->stroke.color = 0xffffffff;

	r = (Enesim_Renderer *) circ;
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);
	r->state_cleanup = ENESIM_RENDERER_STATE_CLEANUP(_state_cleanup);
	r->free = ENESIM_RENDERER_DELETE(_free);
	r->axx = r->ayy = r->azz = 65536;
	//   if (!circle_setup_state(p, 0)) { free(circ); return NULL; }
	return r;
}

EAPI void enesim_renderer_circle_center_set(Enesim_Renderer *r, float x, float y)
{
	Circle *circ;

	circ = (Circle *)r;
	if (circ->x == x && circ->y == y)
		return;

	circ->x = x;
	circ->y = y;
	r->changed = EINA_TRUE;
}

EAPI void enesim_renderer_circle_radius_set(Enesim_Renderer *r, float radius)
{
	Circle *circ;

	circ = (Circle *)r;
	if (radius < 0.9999999)
		radius = 1;
	if (circ->r == radius)
		return;

	circ->r = radius;
	r->changed = EINA_TRUE;
}

EAPI void enesim_renderer_circle_outline_weight_set(Enesim_Renderer *r, float weight)
{
	Circle *circ;

	circ = (Circle *)r;
	if (weight < 0.000009)
		weight = 0;
	if (circ->stroke.weight == weight)
		return;
	circ->stroke.weight = weight;
	r->changed = EINA_TRUE;
}

EAPI void enesim_renderer_circle_outline_color_set(Enesim_Renderer *r, unsigned int stroke_color)
{
	Circle *circ;

	circ = (Circle *)r;
	circ->stroke.color = stroke_color;
}

EAPI void enesim_renderer_circle_outline_renderer_set(Enesim_Renderer *r, Enesim_Renderer *o)
{
	Circle *circ;

	circ = (Circle *)r;
	circ->stroke.r = o;
}

EAPI void enesim_renderer_circle_fill_color_set(Enesim_Renderer *r, unsigned int fill_color)
{
	Circle *circ;

	circ = (Circle *)r;
	circ->fill.color = fill_color;
}

EAPI void enesim_renderer_circle_fill_renderer_set(Enesim_Renderer *r, Enesim_Renderer *f)
{
	Circle *circ;

	circ = (Circle *)r;
	circ->fill.r = f;
	r->changed = EINA_TRUE;
}

EAPI void enesim_renderer_circle_draw_mode_set(Enesim_Renderer *r, int draw_mode)
{
	Circle *circ;

	circ = (Circle *)r;
	circ->draw_mode = draw_mode;
}
