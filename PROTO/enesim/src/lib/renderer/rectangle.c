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
typedef struct _Rectangle {
	Enesim_Renderer base;

	int w, h;
	struct {
		float radius;
	//       int     style;
	//       unsigned char tl : 1;
	//       unsigned char tr : 1;
	//       unsigned char bl : 1;
	//       unsigned char br : 1;
	} corner;

	struct {
		unsigned int color;
		Enesim_Renderer *paint;
		int weight;
	} stroke;

	struct {
		unsigned int color;
		Enesim_Renderer *paint;
	} fill;

	int draw_mode;

	int lxx0, rxx0;
	int tyy0, byy0;
	int rr0, irr0;
	int sw;
	unsigned char do_inner :1;
} Rectangle;

#define EVAL_SRC_ROUND_OUTER_CORNERS(c0,c1,c2,c3) \
		if (lxx < 0) \
		  { \
		    if (tyy < 0) \
		      { \
			if ((-xx - yy) >= rr0) \
			  { \
			    int rr = hypot(lxx, tyy); \
 \
			    ca = 0; \
			    if (rr < rr1) \
			      { \
				ca = 256; \
				if (rr > rr0) \
				   ca = 256 - ((rr - rr0) >> 8); \
			      } \
			  } \
 \
			if (sx < 0) \
			  { \
			    if (c1 != c3) \
				c1 = INTERP_256(ay, c3, c1); \
			    c0 = c2 = c3 = c1; \
			  } \
			if (sy < 0) \
			  { \
			    if (c2 != c3) \
				c2 = INTERP_256(ax, c3, c2); \
			    c0 = c1 = c3 = c2; \
			  } \
		      } \
		    if (byy > 0) \
		      { \
			if ((-xx + yy) >= rr0) \
			  { \
			    int rr = hypot(lxx, byy); \
 \
			    ca = 0; \
			    if (rr < rr1) \
			      { \
				ca = 256; \
				if (rr > rr0) \
				   ca = 256 - ((rr - rr0) >> 8); \
			      } \
			  } \
 \
			if (sx < 0) \
			  { \
			    if (c1 != c3) \
				c1 = INTERP_256(ay, c3, c1); \
			    c0 = c2 = c3 = c1; \
			  } \
			if ((sy + 1) == sh) \
			  { \
			    if (c0 != c1) \
				c0 = INTERP_256(ax, c1, c0); \
			    c1 = c2 = c3 = c0; \
			  } \
		      } \
		  } \
		if (rxx > 0) \
		  { \
		    if (tyy < 0) \
		      { \
			if ((rxx - tyy) >= rr0) \
			  { \
			    int rr = hypot(rxx, tyy); \
 \
			    ca = 0; \
			    if (rr < rr1) \
			      { \
				ca = 256; \
				if (rr > rr0) \
				   ca = 256 - ((rr - rr0) >> 8); \
			      } \
			  } \
 \
			if ((sx + 1) == sw) \
			  { \
			    if (c0 != c2) \
				c0 = INTERP_256(ay, c2, c0); \
			    c1 = c2 = c3 = c0; \
			  } \
			if (sy < 0) \
			  { \
			    if (c2 != c3) \
				c2 = INTERP_256(ax, c3, c2); \
			    c0 = c1 = c3 = c2; \
			  } \
		      } \
		    if (byy > 0) \
		      { \
			if ((rxx + byy) >= rr0) \
			  { \
			    int rr = hypot(rxx, byy); \
 \
			    ca = 0; \
			    if (rr < rr1) \
			      { \
				ca = 256; \
				if (rr > rr0) \
				   ca = 256 - ((rr - rr0) >> 8); \
			      } \
			  } \
 \
			if ((sx + 1) == sw) \
			  { \
			    if (c0 != c2) \
				c0 = INTERP_256(ay, c2, c0); \
			    c1 = c2 = c3 = c0; \
			  } \
			if ((sy + 1) == sh) \
			  { \
			    if (c0 != c1) \
				c0 = INTERP_256(ax, c1, c0); \
			    c1 = c2 = c3 = c0; \
			  } \
		      } \
		  }

#define EVAL_SRC_ROUND_INNER_CORNERS(c0,c1,c2,c3) \
		if (lxx < 0) \
		  { \
		    if (tyy < 0) \
		      { \
			if ((-lxx - tyy) >= irr0) \
			  { \
			    int rr = hypot(lxx, tyy); \
 \
			    ca = 0; \
			    if (rr < irr1) \
			      { \
				ca = 256; \
				if (rr > irr0) \
				   ca = 256 - ((rr - irr0) >> 8); \
			      } \
			  } \
 \
			if (sx < stw) \
			  { \
			    if (c1 != c3) \
				c1 = INTERP_256(ay, c3, c1); \
			    c0 = c2 = c3 = c1; \
			  } \
			if (sy < stw) \
			  { \
			    if (c2 != c3) \
				c2 = INTERP_256(ax, c3, c2); \
			    c0 = c1 = c3 = c2; \
			  } \
		      } \
		    if (byy > 0) \
		      { \
			if ((-lxx + byy) >= irr0) \
			  { \
			    int rr = hypot(lxx, byy); \
 \
			    ca = 0; \
			    if (rr < irr1) \
			      { \
				ca = 256; \
				if (rr > irr0) \
				   ca = 256 - ((rr - irr0) >> 8); \
			      } \
			  } \
 \
			if (sx < stw) \
			  { \
			    if (c1 != c3) \
				c1 = INTERP_256(ay, c3, c1); \
			    c0 = c2 = c3 = c1; \
			  } \
			if ((sy + 1 + stw) == sh) \
			  { \
			    if (c0 != c1) \
				c0 = INTERP_256(ax, c1, c0); \
			    c1 = c2 = c3 = c0; \
			  } \
		      } \
		  } \
		if (rxx > 0) \
		  { \
		    if (tyy < 0) \
		      { \
			if ((rxx - tyy) >= irr0) \
			  { \
			    int rr = hypot(rxx, tyy); \
 \
			    ca = 0; \
			    if (rr < irr1) \
			      { \
				ca = 256; \
				if (rr > irr0) \
				   ca = 256 - ((rr - irr0) >> 8); \
			      } \
			  } \
 \
			if ((sx + 1 + stw) == sw) \
			  { \
			    if (c0 != c2) \
				c0 = INTERP_256(ay, c2, c0); \
			    c1 = c2 = c3 = c0; \
			  } \
			if (sy < stw) \
			  { \
			    if (c2 != c3) \
				c2 = INTERP_256(ax, c3, c2); \
			    c0 = c1 = c3 = c2; \
			  } \
		      } \
		    if (byy > 0) \
		      { \
			if ((rxx + byy) >= irr0) \
			  { \
			    int rr = hypot(rxx, byy); \
 \
			    ca = 0; \
			    if (rr < irr1) \
			      { \
				ca = 256; \
				if (rr > irr0) \
				   ca = 256 - ((rr - irr0) >> 8); \
			      } \
			  } \
 \
			if ((sx + 1 + stw) == sw) \
			  { \
			    if (c0 != c2) \
				c0 = INTERP_256(ay, c2, c0); \
			    c1 = c2 = c3 = c0; \
			  } \
			if ((sy + 1 + stw) == sh) \
			  { \
			    if (c0 != c1) \
				c0 = INTERP_256(ax, c1, c0); \
			    c1 = c2 = c3 = c0; \
			  } \
		      } \
		  }

static void rect_rounded_outlined_fill_paint(Enesim_Renderer *p, int x, int y,
		int len, unsigned int *dst)
{
	Rectangle *rect = (Rectangle *) p;
	int sw = rect->w, sh = rect->h;
	int axx = p->axx, axy = p->axy, axz = p->axz;
	int ayx = p->ayx, ayy = p->ayy, ayz = p->ayz;
	int do_inner = rect->do_inner;
	unsigned int ocolor = rect->stroke.color;
	unsigned int icolor = rect->fill.color;
	int stw = rect->sw;
	int rr0 = rect->rr0, rr1 = rr0 + 65536;
	int irr0 = rect->irr0, irr1 = irr0 + 65536;
	int lxx0 = rect->lxx0, rxx0 = rect->rxx0;
	int tyy0 = rect->tyy0, byy0 = rect->byy0;
	Enesim_Renderer *fpaint = rect->fill.paint;
	unsigned int *d = dst, *e = d + len;
	int xx, yy;

	if (rect->draw_mode == ENESIM_RENDERER_DRAW_MODE_STROKE)
		icolor = 0;

	if (do_inner)
		fpaint->span(fpaint, x, y, len, dst);

	xx = (axx * x) + (axy * y) + axz;
	yy = (ayx * x) + (ayy * y) + ayz;

	while (d < e)
	{
		int sx = (xx >> 16);
		int sy = (yy >> 16);
		unsigned int q0 = 0;

		if ((((unsigned) (sx + 1)) < (sw + 1)) && (((unsigned) (sy + 1)) < (sh
				+ 1)))
		{
			int ca = 256;
			unsigned int op3 = 0, op2 = 0, op1 = 0, op0 = 0, p0;
			int ax = 1 + ((xx & 0xffff) >> 8);
			int ay = 1 + ((yy & 0xffff) >> 8);

			int lxx = xx - lxx0, rxx = xx - rxx0;
			int tyy = yy - tyy0, byy = yy - byy0;

			if ((sx > -1) & (sy > -1))
				op0 = ocolor;
			if ((sy > -1) & ((sx + 1) < sw))
				op1 = ocolor;
			if ((sy + 1) < sh)
			{
				if (sx > -1)
					op2 = ocolor;
				if ((sx + 1) < sw)
					op3 = ocolor;
			}

			EVAL_SRC_ROUND_OUTER_CORNERS(op0,op1,op2,op3)

			if (op0 != op1)
				op0 = INTERP_256(ax, op1, op0);
			if (op2 != op3)
				op2 = INTERP_256(ax, op3, op2);
			if (op0 != op2)
				op0 = INTERP_256(ay, op2, op0);

			if (ca < 256)
				op0 = argb8888_mul_256(ca, op0);

			p0 = op0;
			if (do_inner && ((((unsigned) (sx - stw + 1))
					< (sw - (2 * stw) + 1)) && (((unsigned) (sy - stw + 1))
					< (sh - (2 * stw) + 1))))
			{
				unsigned int p3 = p0, p2 = p0, p1 = p0;
				unsigned int color = *d;

				if (icolor != 0xffffffff)
					color = argb8888_mul4_sym(icolor, color);

				ca = 256;
				if ((sx > (stw - 1)) & (sy > (stw - 1)))
					p0 = color;
				if ((sy > (stw - 1)) & ((sx + 1 + stw) < sw))
					p1 = color;
				if ((sy + 1 + stw) < sh)
				{
					if (sx > (stw - 1))
						p2 = color;
					if ((sx + 1 + stw) < sw)
						p3 = color;
				}

				EVAL_SRC_ROUND_INNER_CORNERS(p0,p1,p2,p3)

				if (p0 != p1)
					p0 = INTERP_256(ax, p1, p0);
				if (p2 != p3)
					p2 = INTERP_256(ax, p3, p2);
				if (p0 != p2)
					p0 = INTERP_256(ay, p2, p0);

				if (ca < 256)
					p0 = INTERP_256(ca, p0, op0);
			}
			q0 = p0;
		}
		*d++ = q0;
		xx += axx;
		yy += ayx;
	}
}

static Eina_Bool _state_setup(Enesim_Renderer *p)
{
	Rectangle *rect = (Rectangle *) p;
	float rad;
	int sw;

	if (!rect || (rect->w < 1) || (rect->h < 1))
		return EINA_FALSE;

	rad = rect->corner.radius;
	if (rad > (rect->w / 2.0))
		rad = rect->w / 2.0;
	if (rad > (rect->h / 2.0))
		rad = rect->h / 2.0;

	rect->rr0 = rad * 65536;
	rect->lxx0 = rect->tyy0 = rect->rr0;
	rect->rxx0 = (rect->w - rad - 1) * 65536;
	rect->byy0 = (rect->h - rad - 1) * 65536;

	sw = rect->stroke.weight;
	rect->do_inner = 1;
	if ((sw >= (rect->w / 2.0)) || (sw >= (rect->h / 2.0)))
	{
		sw = 0;
		rect->do_inner = 0;
	}
	rad = rad - sw;
	if (rad < 0.0039)
		rad = 0;

	rect->irr0 = rad * 65536;
	rect->sw = sw;
	if (rect->fill.paint)
	{
		if (!enesim_renderer_state_setup(rect->fill.paint))
			return EINA_FALSE;
		p->span = ENESIM_RENDERER_SPAN_DRAW(rect_rounded_outlined_fill_paint);
	} else
		p->span = NULL; //rect_rounded_outlined_fill;

	return EINA_TRUE;
}

static void _state_cleanup(Enesim_Renderer *p)
{
	Rectangle *rect = (Rectangle *) p;

	if (rect->fill.paint)
		enesim_renderer_state_cleanup(rect->fill.paint);
}

static void _free(Enesim_Renderer *p)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * enesim_renderer_rectangle_new(void)
{
	Enesim_Renderer *p;
	Rectangle *rect;

	rect = calloc(1, sizeof(Rectangle));
	if (!rect)
		return NULL;
	rect->fill.color = 0xffffffff;
	rect->stroke.color = 0xffffffff;
	p = (Enesim_Renderer *) rect;
	p->axx = p->ayy = p->azz = 65536;
	p->free = ENESIM_RENDERER_DELETE(_free);
	p->state_cleanup = ENESIM_RENDERER_STATE_CLEANUP(_state_cleanup);
	p->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);
	//   if (!rectangle_setup_state(p, 0)) { free(rect); return NULL; }
	return p;
}

EAPI void enesim_renderer_rectangle_size_set(Enesim_Renderer *p, int w, int h)
{
	Rectangle *rect;

	rect = (Rectangle *) p;
	if (w < 0)
		w = 0;
	if (h < 0)
		h = 0;
	rect->w = w;
	rect->h = h;
}

EAPI void enesim_renderer_rectangle_corner_radius_set(Enesim_Renderer *p, float radius)
{
	Rectangle *rect;

	rect = (Rectangle *) p;
	if (radius < 0)
		radius = 0;
	rect->corner.radius = radius;
}

EAPI void enesim_renderer_rectangle_outline_weight_set(Enesim_Renderer *p, int weight)
{
	Rectangle *rect;

	rect = (Rectangle *) p;
	if (weight < 1)
		weight = 0;
	rect->stroke.weight = weight;
}

EAPI void enesim_renderer_rectangle_outline_color_set(Enesim_Renderer *p, unsigned int stroke_color)
{
	Rectangle *rect;

	rect = (Rectangle *) p;
	rect->stroke.color = stroke_color;
}

EAPI void enesim_renderer_rectangle_outline_paint_set(Enesim_Renderer *p, Enesim_Renderer *paint)
{
	Rectangle *rect;

	rect = (Rectangle *) p;
	rect->stroke.paint = paint;
}

EAPI void enesim_renderer_rectangle_fill_color_set(Enesim_Renderer *p, unsigned int fill_color)
{
	Rectangle *rect;

	rect = (Rectangle *) p;
	rect->fill.color = fill_color;
}

EAPI void enesim_renderer_rectangle_fill_paint_set(Enesim_Renderer *p, Enesim_Renderer *paint)
{
	Rectangle *rect;

	rect = (Rectangle *) p;
	rect->fill.paint = paint;
}

EAPI void enesim_renderer_rectangle_draw_mode_set(Enesim_Renderer *p, int draw_mode)
{
	Rectangle *rect;

	rect = (Rectangle *) p;
	rect->draw_mode = draw_mode;
}
