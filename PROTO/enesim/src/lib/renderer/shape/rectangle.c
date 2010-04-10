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
	Enesim_Renderer_Shape base;

	int w, h;
	struct {
		float radius;
	//       int     style;
	       unsigned char tl : 1;
	       unsigned char tr : 1;
	       unsigned char bl : 1;
	       unsigned char br : 1;
	} corner;

	int lxx0, rxx0;
	int tyy0, byy0;
	int rr0, irr0;
	int sw;
	unsigned char do_inner :1;
} Rectangle;

#define EVAL_ROUND_OUTER_CORNERS(c0,c1,c2,c3) \
		if (lxx < 0) \
		{ \
			if (tl && (tyy < 0)) \
			{ \
				if ((-lxx - tyy) >= rr0) \
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
 \
			if (bl && (byy > 0)) \
			{ \
				if ((-lxx + byy) >= rr0) \
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
 \
		if (rxx > 0) \
		{ \
			if (tr && (tyy < 0)) \
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
 \
			if (br && (byy > 0)) \
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


#define EVAL_ROUND_INNER_CORNERS(c0,c1,c2,c3) \
		if (lxx < 0) \
		{ \
			if (tl && (tyy < 0)) \
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
 \
			if (bl && (byy > 0)) \
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
 \
		if (rxx > 0) \
		{ \
			if (tr && (tyy < 0)) \
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
 \
			if (br && (byy > 0)) \
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

static void _span_norounded_nooutlined_paint_filled_identity(Enesim_Renderer *r, int x,
		int y, unsigned int len, uint32_t *dst)
{
	Rectangle *rect = (Rectangle *)r;

}

static void _span_rounded_color_outlined_paint_filled_affine(Enesim_Renderer *p, int x, int y,
		unsigned int len, uint32_t *dst)
{
	Rectangle *rect = (Rectangle *) p;
	int sw = rect->w, sh = rect->h;
	int axx = p->matrix.values.xx, axy = p->matrix.values.xy, axz = p->matrix.values.xz;
	int ayx = p->matrix.values.yx, ayy = p->matrix.values.yy, ayz = p->matrix.values.yz;
	int do_inner = rect->do_inner;
	unsigned int ocolor = rect->base.stroke.color;
	unsigned int icolor = rect->base.fill.color;
	int stw = rect->sw;
	int rr0 = rect->rr0, rr1 = rr0 + 65536;
	int irr0 = rect->irr0, irr1 = irr0 + 65536;
	int lxx0 = rect->lxx0, rxx0 = rect->rxx0;
	int tyy0 = rect->tyy0, byy0 = rect->byy0;
	Enesim_Renderer *fpaint = rect->base.fill.rend;
	unsigned int *d = dst, *e = d + len;
	int xx, yy;
	int fill_only = 0;
	char bl = rect->corner.bl, br = rect->corner.br, tl = rect->corner.tl, tr = rect->corner.tr;

	if (rect->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE)
	  {
		icolor = 0;
		fpaint = NULL;
	  }
	if (rect->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_FILL)
	  {
		ocolor = icolor;
		fill_only = 1;
		do_inner = 0;
		if (fpaint)
			fpaint->span(fpaint, x, y, len, dst);
	  }
	if ((rect->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE_FILL) && do_inner && fpaint)
		fpaint->span(fpaint, x, y, len, dst);

#if 1
        renderer_affine_setup(p, x, y, &xx, &yy);

#else
	xx = (axx * x) + (axy * y) + axz;
	yy = (ayx * x) + (ayy * y) + ayz;
#endif

	while (d < e)
	{
		unsigned int q0 = 0;
		int sx = (xx >> 16);
		int sy = (yy >> 16);

		if ((((unsigned) (sx + 1)) < (sw + 1)) && (((unsigned) (sy + 1)) < (sh + 1)))
		{
			int ca = 256;
			unsigned int op3 = 0, op2 = 0, op1 = 0, op0 = 0, p0;
			int ax = 1 + ((xx & 0xffff) >> 8);
			int ay = 1 + ((yy & 0xffff) >> 8);

			int lxx = xx - lxx0, rxx = xx - rxx0;
			int tyy = yy - tyy0, byy = yy - byy0;

			if (fill_only && fpaint)
			{
				ocolor = *d;
				if (icolor != 0xffffffff)
					ocolor = argb8888_mul4_sym(icolor, ocolor);
			}

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

			EVAL_ROUND_OUTER_CORNERS(op0,op1,op2,op3)

			if (op0 != op1)
				op0 = INTERP_256(ax, op1, op0);
			if (op2 != op3)
				op2 = INTERP_256(ax, op3, op2);
			if (op0 != op2)
				op0 = INTERP_256(ay, op2, op0);

			if (ca < 256)
				op0 = argb8888_mul_256(ca, op0);

			p0 = op0;
			if (do_inner && ((((unsigned) (sx - stw + 1)) < (sw - (2 * stw) + 1))
				 && (((unsigned) (sy - stw + 1)) < (sh - (2 * stw) + 1))))
			{
				unsigned int p3 = p0, p2 = p0, p1 = p0;
				unsigned int color = icolor;

				if (fpaint)
				  {
					color = *d;
					if (icolor != 0xffffffff)
						color = argb8888_mul4_sym(icolor, color);
				  }

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

				EVAL_ROUND_INNER_CORNERS(p0,p1,p2,p3)

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

static void _span_rounded_color_outlined_paint_filled_proj(Enesim_Renderer *p, int x, int y,
		unsigned int len, uint32_t *dst)
{
	Rectangle *rect = (Rectangle *) p;
	int sw = rect->w, sh = rect->h;
	int axx = p->matrix.values.xx, axy = p->matrix.values.xy, axz = p->matrix.values.xz;
	int ayx = p->matrix.values.yx, ayy = p->matrix.values.yy, ayz = p->matrix.values.yz;
	int azx = p->matrix.values.zx, azy = p->matrix.values.zy, azz = p->matrix.values.zz;
	int do_inner = rect->do_inner;
	unsigned int ocolor = rect->base.stroke.color;
	unsigned int icolor = rect->base.fill.color;
	int stw = rect->sw;
	int rr0 = rect->rr0, rr1 = rr0 + 65536;
	int irr0 = rect->irr0, irr1 = irr0 + 65536;
	int lxx0 = rect->lxx0, rxx0 = rect->rxx0;
	int tyy0 = rect->tyy0, byy0 = rect->byy0;
	Enesim_Renderer *fpaint = rect->base.fill.rend;
	unsigned int *d = dst, *e = d + len;
	int xx, yy, zz;
	int fill_only = 0;
	char bl = rect->corner.bl, br = rect->corner.br, tl = rect->corner.tl, tr = rect->corner.tr;

	if (rect->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE)
	{
		icolor = 0;
		fpaint = NULL;
	}
	if (rect->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_FILL)
	{
		ocolor = icolor;
		fill_only = 1;
		do_inner = 0;
		if (fpaint)  fpaint->span(fpaint, x, y, len, dst);
	}
	if ((rect->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE_FILL) && do_inner && fpaint)
		fpaint->span(fpaint, x, y, len, dst);
#if 1
	renderer_projective_setup(p, x, y, &xx, &yy, &zz);
#else
	xx = (axx * x) + (axy * y) + axz;
	yy = (ayx * x) + (ayy * y) + ayz;
	zz = (azx * x) + (azy * y) + azz;
#endif
	while (d < e)
	{
		unsigned int q0 = 0;

		if (zz)
		{
			int sxx = (((long long int)xx) << 16) / zz;
			int sx = sxx >> 16;
			int syy = (((long long int)yy) << 16) / zz;
			int sy = syy >> 16;

			if ((((unsigned) (sx + 1)) < (sw + 1)) && (((unsigned) (sy + 1)) < (sh + 1)))
			{
				int ca = 256;
				unsigned int op3 = 0, op2 = 0, op1 = 0, op0 = 0, p0;
				int ax = 1 + ((sxx & 0xffff) >> 8);
				int ay = 1 + ((syy & 0xffff) >> 8);

				int lxx = sxx - lxx0, rxx = sxx - rxx0;
				int tyy = syy - tyy0, byy = syy - byy0;

				if (fill_only && fpaint)
				{
					ocolor = *d;
					if (icolor != 0xffffffff)
						ocolor = argb8888_mul4_sym(ocolor, icolor);
				}

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

				EVAL_ROUND_OUTER_CORNERS(op0,op1,op2,op3)

				if (op0 != op1)
					op0 = INTERP_256(ax, op1, op0);
				if (op2 != op3)
					op2 = INTERP_256(ax, op3, op2);
				if (op0 != op2)
					op0 = INTERP_256(ay, op2, op0);

				if (ca < 256)
					op0 = argb8888_mul_256(ca, op0);

				p0 = op0;
				if (do_inner && ((((unsigned) (sx - stw + 1)) < (sw - (2 * stw) + 1))
					 && (((unsigned) (sy - stw + 1)) < (sh - (2 * stw) + 1))))
				{
					unsigned int p3 = p0, p2 = p0, p1 = p0;
					unsigned int color = icolor;

					if (fpaint)
					{
						color = *d;
						if (icolor != 0xffffffff)
							color = argb8888_mul4_sym(icolor, color);
					}

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

					EVAL_ROUND_INNER_CORNERS(p0,p1,p2,p3)

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
		}
		*d++ = q0;
		xx += axx;
		yy += ayx;
		zz += azx;
	}
}


static Eina_Bool _state_setup(Enesim_Renderer *p)
{
	Rectangle *rect = (Rectangle *) p;

	if (!rect || (rect->w < 1) || (rect->h < 1))
		return EINA_FALSE;

	if (1)
	{
		float rad;
		int sw;

		rad = rect->corner.radius;
		if (rad > (rect->w / 2.0))
			rad = rect->w / 2.0;
		if (rad > (rect->h / 2.0))
			rad = rect->h / 2.0;

		rect->rr0 = rad * 65536;
		rect->lxx0 = rect->tyy0 = rect->rr0;
		rect->rxx0 = (rect->w - rad - 1) * 65536;
		rect->byy0 = (rect->h - rad - 1) * 65536;

		sw = rect->base.stroke.weight;
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
	}

	if (rect->base.fill.rend &&
	    ((rect->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_FILL) ||
	     (rect->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE_FILL)))
	{
		if (!enesim_renderer_state_setup(rect->base.fill.rend))
			return EINA_FALSE;
	}

	p->span = ENESIM_RENDERER_SPAN_DRAW(_span_rounded_color_outlined_paint_filled_proj);
	if (p->matrix.type == ENESIM_MATRIX_AFFINE || p->matrix.type == ENESIM_MATRIX_IDENTITY)
		p->span = ENESIM_RENDERER_SPAN_DRAW(_span_rounded_color_outlined_paint_filled_affine);

	p->changed = EINA_FALSE;
	return EINA_TRUE;
}

static void _state_cleanup(Enesim_Renderer *p)
{
	Rectangle *rect = (Rectangle *) p;

	if (rect->base.fill.rend &&
	    ((rect->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_FILL) ||
	     (rect->base.draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE_FILL)))
		enesim_renderer_state_cleanup(rect->base.fill.rend);
//	if (rect->stroke.paint)
//		enesim_renderer_state_cleanup(rect->stroke.paint);
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
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer * enesim_renderer_rectangle_new(void)
{
	Enesim_Renderer *p;
	Rectangle *rect;

	rect = calloc(1, sizeof(Rectangle));
	if (!rect)
		return NULL;

	p = (Enesim_Renderer *) rect;
	p->type_id = RECTANGLE_RENDERER;
	enesim_renderer_shape_init(p);
	p->free = ENESIM_RENDERER_DELETE(_free);
	p->state_cleanup = ENESIM_RENDERER_STATE_CLEANUP(_state_cleanup);
	p->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);
	p->changed = EINA_TRUE;
	//   if (!rectangle_setup_state(p, 0)) { free(rect); return NULL; }
	return p;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_rectangle_size_set(Enesim_Renderer *p, int w, int h)
{
	Rectangle *rect;

	rect = (Rectangle *) p;
	if (!rect) return;
	if (w < 0)
		w = 0;
	if (h < 0)
		h = 0;
	if ((rect->w == w) && (rect->h == h))
		return;
	rect->w = w;
	rect->h = h;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_rectangle_corner_radius_set(Enesim_Renderer *p, float radius)
{
	Rectangle *rect;

	rect = (Rectangle *) p;
	if (!rect) return;
	if (radius < 0)
		radius = 0;
	if (rect->corner.radius == radius)
		return;
	rect->corner.radius = radius;
	p->changed = EINA_TRUE;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_rectangle_corners_set(Enesim_Renderer *p, int tl, int tr, int bl, int br)
{
	Rectangle  *rect;

	rect = (Rectangle *)p;
	if (!rect) return;
	tl = !!tl;  tr = !!tr;  bl = !!bl;  br = !!br;
	if ((rect->corner.tl == tl) && (rect->corner.tr == tr) &&
	     (rect->corner.bl == bl) && (rect->corner.br == br))
		return;
	rect->corner.tl = tl;  rect->corner.tr = tr;
	rect->corner.bl = bl;  rect->corner.br = br;
}
