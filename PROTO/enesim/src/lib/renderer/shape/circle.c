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
	Enesim_Renderer_Shape base;

	float x, y;
	float r;

	int xx0, yy0;
	int rr0, irr0;
	unsigned char do_inner :1;
} Circle;

static void _outlined_fill_paint(Enesim_Renderer *r, int x, int y,
		int len, unsigned int *dst)
{
	Circle *circ = (Circle *)r;
	int axx = r->matrix.values.xx, axy = r->matrix.values.xy, axz = r->matrix.values.xz;
	int ayx = r->matrix.values.yx, ayy = r->matrix.values.yy, ayz = r->matrix.values.yz;
	int do_inner = circ->do_inner;
	unsigned int ocolor = circ->base.stroke.color;
	unsigned int icolor = circ->base.fill.color;
	int rr0 = circ->rr0, rr1 = rr0 + 65536;
	int irr0 = circ->irr0, irr1 = irr0 + 65536;
	int rr2 = rr1 * 1.41421357, irr2 = irr1 * 1.41421357; // sqrt(2)
	int xx0 = circ->xx0, yy0 = circ->yy0;
	Enesim_Renderer *fpaint = circ->base.fill.rend;
	unsigned int *d = dst, *e = d + len;
	int xx, yy;
	int fill_only = 0;

	if (circ->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE)
	{
		icolor = 0;
		fpaint = NULL;
	}

	if (circ->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_FILL)
	{
		ocolor = icolor;
		fill_only = 1;
		do_inner = 0;
		if (fpaint)
			fpaint->span(fpaint, x, y, len, dst);
	}
	if ((circ->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE_FILL) && do_inner
			&& fpaint)
		fpaint->span(fpaint, x, y, len, dst);

#if 0
        renderer_affine_setup(r, x, y, &xx, &yy);
	xx -= xx0;
	yy -= yy0;
#else
	xx = (axx * x) + (axy * y) + axz - xx0;
	yy = (ayx * x) + (ayy * y) + ayz - yy0;
#endif
	while (d < e)
	{
		unsigned int q0 = 0;

		if ((abs(xx) <= rr1) && (abs(yy) <= rr1))
		{
			unsigned int op0 = ocolor, p0;
			int a = 256;

			if (fill_only && fpaint)
				op0 = argb8888_mul4_sym(*d, op0);

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
				p0 = icolor;
				if (fpaint)
				{
					p0 = *d;
					if (icolor != 0xffffffff)
						p0 = argb8888_mul4_sym(icolor, p0);
				}
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
					p0 = argb8888_interp_256(a, p0, op0);
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

	sw = circ->base.stroke.weight;
	circ->do_inner = 1;
	if (sw >= (circ->r - 1))
	{
		sw = 0;
		circ->do_inner = 0;
	}
	rad = circ->r - 1 - sw;
	if (rad < 0.0039)
		rad = 0;

	circ->irr0 = rad * 65536;
	if (circ->base.fill.rend)
	{
		if (!enesim_renderer_state_setup(circ->base.fill.rend))
			return EINA_FALSE;

	}
	r->span = ENESIM_RENDERER_SPAN_DRAW(_outlined_fill_paint);

	return EINA_TRUE;
}

static void _state_cleanup(Enesim_Renderer *r)
{
	Circle *circ = (Circle *)r;

	if (circ->base.fill.rend)
		enesim_renderer_state_cleanup(circ->base.fill.rend);
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
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer * enesim_renderer_circle_new(void)
{
	Enesim_Renderer *r;
	Circle *circ;

	circ = calloc(1, sizeof(Circle));
	if (!circ)
		return NULL;

	r = (Enesim_Renderer *)circ;
	enesim_renderer_shape_init(r);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);
	r->state_cleanup = ENESIM_RENDERER_STATE_CLEANUP(_state_cleanup);
	r->free = ENESIM_RENDERER_DELETE(_free);

	return r;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_circle_center_set(Enesim_Renderer *r, float x, float y)
{
	Circle *circ;

	circ = (Circle *)r;
	circ->x = x;
	circ->y = y;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_circle_radius_set(Enesim_Renderer *r, float radius)
{
	Circle *circ;

	circ = (Circle *)r;
	if (radius < 1)
		radius = 1;
	circ->r = radius;
}
