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
#define MUL4_SYM(x, y) \
 ( ((((((x) >> 16) & 0xff00) * (((y) >> 16) & 0xff00)) + 0xff0000) & 0xff000000) + \
   ((((((x) >> 8) & 0xff00) * (((y) >> 16) & 0xff)) + 0xff00) & 0xff0000) + \
   ((((((x) & 0xff00) * ((y) & 0xff00)) + 0xff00) >> 16) & 0xff00) + \
   (((((x) & 0xff) * ((y) & 0xff)) + 0xff) >> 8) )

#define INTERP_65536(a, c0, c1) \
	( ((((((c0 >> 16) & 0xff00) - ((c1 >> 16) & 0xff00)) * a) + \
	  (c1 & 0xff000000)) & 0xff000000) + \
	  ((((((c0 >> 16) & 0xff) - ((c1 >> 16) & 0xff)) * a) + \
	  (c1 & 0xff0000)) & 0xff0000) + \
	  ((((((c0 & 0xff00) - (c1 & 0xff00)) * a) >> 16) + \
	  (c1 & 0xff00)) & 0xff00) + \
	  ((((((c0 & 0xff) - (c1 & 0xff)) * a) >> 16) + \
	  (c1 & 0xff)) & 0xff) )

#define MUL_A_65536(a, c) \
	( ((((c >> 16) & 0xff00) * a) & 0xff000000) + \
	  ((((c >> 16) & 0xff) * a) & 0xff0000) + \
	  ((((c & 0xff00) * a) >> 16) & 0xff00) + \
	  ((((c & 0xff) * a) >> 16) & 0xff) )

typedef struct _Point2D Point2D;
struct _Point2D
{
	float x, y;
};

typedef struct _Polygon_Vertex Polygon_Vertex;
struct _Polygon_Vertex
{
	Point2D v;
	Polygon_Vertex *next;
};

typedef struct _Polygon_Vector Polygon_Vector;
struct _Polygon_Vector
{
	int xx0, yy0, xx1, yy1;
	int a, b, c;
};

typedef struct _Polygon_Edge Polygon_Edge;
struct _Polygon_Edge
{
	int xx0, yy0, xx1, yy1;
	int e, de;
};

typedef struct _Contour_Polygon Contour_Polygon;
struct _Contour_Polygon
{
	Polygon_Vertex *vertices, *last;
	int nverts;

	Contour_Polygon *next;
};

typedef struct _Figure Figure;
struct _Figure
{
	Enesim_Renderer_Shape base;

	Contour_Polygon *polys, *last;
	int npolys;

	Polygon_Vector *vectors;
	int nvectors;

	// ....  geom_transform?

	int lxx, rxx, tyy, byy;

	unsigned char changed :1;
};

static void figure_stroke_fill_paint_affine_simple(Enesim_Renderer *p, int x,
		int y, unsigned int *dst, int len)
{
	Enesim_Renderer_Shape *f = (Enesim_Renderer_Shape *) p;
	Figure *o = (Figure *) p;
	unsigned int fcolor = f->fill.color;
	unsigned int scolor = f->stroke.color;
	Enesim_Renderer *fpaint = f->fill.rend;
	int stroke = 0;
	unsigned int *d = dst, *e = d + len;
	Polygon_Edge *edges, *edge;
	Polygon_Vector *v = o->vectors;
	int nvectors = o->nvectors, n = 0, nedges = 0;

	int axx = p->matrix.values.xx, axy = p->matrix.values.xy, axz =
			p->matrix.values.xz;
	int ayy = p->matrix.values.yy, ayz = p->matrix.values.yz;
	int xx = (axx * x) + (axx >> 1) + (axy * y) + (axy >> 1) + axz - 32768;
	int yy = (ayy * y) + (ayy >> 1) + ayz - 32768;

	if ((((yy >> 16) + 1) < (o->tyy >> 16)) ||
			((yy >> 16) > (1 + (o->byy >> 16))))
	{
get_out:
		while (d < e)
			*d++ = 0;
		return;
	}

	if (f->draw_mode == ENESIM_SHAPE_DRAW_MODE_FILL)
	{
		scolor = fcolor;
		stroke = 0;
		if (fpaint)
			fpaint->span(fpaint, x, y, len, dst);
	}
	if (f->draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE_FILL)
	{
		stroke = 1;
		if (fpaint)
			fpaint->span(fpaint, x, y, len, dst);
	}
	if (f->draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE)
	{
		fcolor = 0;
		fpaint = NULL;
		stroke = 1;
	}

	edges = alloca(nvectors * sizeof(Polygon_Edge));
	edge = edges;
	while (n < nvectors)
	{
		int xx0 = v->xx0, xx1 = v->xx1;
		int yy0 = v->yy0, yy1 = v->yy1;

		if (xx1 < xx0)
		{
			xx0 = xx1;
			xx1 = v->xx0;
		}
		if (yy1 < yy0)
		{
			yy0 = yy1;
			yy1 = v->yy0;
		}
		if ((((yy + 0xffff)) >= (yy0)) && ((yy) <= ((yy1 + 0xffff))))
		{
			edge->xx0 = xx0;
			edge->xx1 = xx1;
			edge->yy0 = yy0;
			edge->yy1 = yy1;
			edge->de = (v->a * (long long int) axx) >> 16;
			edge->e = ((v->a * (long long int) xx) >> 16) +
					((v->b * (long long int) yy) >> 16) +
					v->c;
			edge++;
			nedges++;
		}
		n++;
		v++;
	}
	if (!nedges)
		goto get_out;

	while (d < e)
	{
		unsigned int p0 = 0;
		int count = 0;
		int a = 0;

		n = 0;
		edge = edges;
		while (n < nedges)
		{
			int ee = edge->e;

			if ((yy >= edge->yy0) && (yy < edge->yy1))
			{
				count++;
				if (ee < 0)
					count -= 2;
			}
			if (ee < 0)
				ee = -ee;

			if ((ee < 65536) && ((xx + 0xffff) >= edge->xx0) && (xx
					<= (0xffff + edge->xx1)))
			{
				if (a < 16384)
					a = 65536 - ee;
				else
					a = (a + (65536 - ee)) / 2;
			}

			edge->e += edge->de;
			edge++;
			n++;
		}

		if (count)
		{
			p0 = fcolor;
			if (fpaint)
			{
				p0 = *d;
				if (fcolor != 0xffffffff)
					p0 = MUL4_SYM(fcolor, p0);
			}

			if (stroke && a)
			{
				unsigned int q0 = p0;

				p0 = scolor;
				if (a < 65536)
					p0 = INTERP_65536(a, p0, q0);
			}
		}
		else if (a)
		{
			p0 = scolor;
			if (fpaint && !stroke)
			{
				p0 = *d;
				if (fcolor != 0xffffffff)
					p0 = MUL4_SYM(fcolor, p0);
			}
			if (a < 65536)
				p0 = MUL_A_65536(a, p0);
		}

		*d++ = p0;
		xx += axx;
	}
}

static void figure_stroke_fill_paint_affine(Enesim_Renderer *p, int x, int y,
		int len, unsigned int *dst)
{
	Enesim_Renderer_Shape *f = (Enesim_Renderer_Shape *) p;
	Figure *o = (Figure *) p;
	unsigned int fcolor = f->fill.color;
	unsigned int scolor = f->stroke.color;
	Enesim_Renderer *fpaint = f->fill.rend;
	int stroke = 0;
	unsigned int *d = dst, *e = d + len;
	Polygon_Edge *edges, *edge;
	Polygon_Vector *v = o->vectors;
	int nvectors = o->nvectors, n = 0, nedges = 0;
	int y0, y1;

	int axx = p->matrix.values.xx, axy = p->matrix.values.xy, axz =
			p->matrix.values.xz;
	int ayx = p->matrix.values.yx, ayy = p->matrix.values.yy, ayz =
			p->matrix.values.yz;
	int xx = (axx * x) + (axx >> 1) + (axy * y) + (axy >> 1) + axz - 32768;
	int yy = (ayx * x) + (ayx >> 1) + (ayy * y) + (ayy >> 1) + ayz - 32768;

	if (((ayx <= 0) && ((yy >> 16) + 1 < (o->tyy >> 16))) || ((ayx >= 0)
			&& ((yy >> 16) > 1 + (o->byy >> 16))))
	{
		while (d < e)
			*d++ = 0;
		return;
	}

	len--;
	y0 = yy >> 16;
	y1 = yy + (len * ayx);
	y1 = y1 >> 16;
	if (y1 < y0)
	{
		y0 = y1;
		y1 = yy >> 16;
	}
	edges = alloca(nvectors * sizeof(Polygon_Edge));
	edge = edges;
	while (n < nvectors)
	{
		int xx0, yy0;
		int xx1, yy1;

		xx0 = v->xx0;
		xx1 = v->xx1;
		if (xx1 < xx0)
		{
			xx0 = xx1;
			xx1 = v->xx0;
		}
		yy0 = v->yy0;
		yy1 = v->yy1;
		if (yy1 < yy0)
		{
			yy0 = yy1;
			yy1 = v->yy0;
		}
		if ((y0 <= (yy1 >> 16)) && (y1 >= (yy0 >> 16)))
		{
			edge->xx0 = xx0;
			edge->xx1 = xx1;
			edge->yy0 = yy0;
			edge->yy1 = yy1;
			edge->de = ((v->a * (long long int) axx) >> 16) +
					((v->b * (long long int) ayx) >> 16);
			edge->e = ((v->a * (long long int) xx) >> 16) +
					((v->b * (long long int) yy) >> 16) +
					v->c;
			edge++;
			nedges++;
		}
		n++;
		v++;
	}

	if (f->draw_mode == ENESIM_SHAPE_DRAW_MODE_FILL)
	{
		scolor = fcolor;
		stroke = 0;
		if (fpaint)
			fpaint->span(fpaint, x, y, len, dst);
	}
	if (f->draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE_FILL)
	{
		stroke = 1;
		if (fpaint)
			fpaint->span(fpaint, x, y, len, dst);
	}
	if (f->draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE)
	{
		fcolor = 0;
		fpaint = NULL;
		stroke = 1;
	}

	while (d < e)
	{
		unsigned int p0 = 0;
		int count = 0;
		int a = 0;

		n = 0;
		edge = edges;
		while (n < nedges)
		{
			int ee = edge->e;

			if (((yy + 0xffff) >= edge->yy0) &&
					(yy <= (edge->yy1 + 0xffff)))
			{
				if ((yy >= edge->yy0) && (yy < edge->yy1))
				{
					count++;
					if (ee < 0)
						count -= 2;
				}
				if (ee < 0)
					ee = -ee;
				if ((ee < 65536) &&
						((xx + 0xffff) >= edge->xx0) &&
						(xx <= (0xffff + edge->xx1)))
				{
					if (a < 16384)
						a = 65536 - ee;
					else
						a = (a + (65536 - ee)) / 2;
				}
			}

			edge->e += edge->de;
			edge++;
			n++;
		}

		if (count)
		{
			p0 = fcolor;
			if (fpaint)
			{
				p0 = *d;
				if (fcolor != 0xffffffff)
					p0 = MUL4_SYM(fcolor, p0);
			}

			if (stroke && a)
			{
				unsigned int q0 = p0;

				p0 = scolor;
				if (a < 65536)
					p0 = INTERP_65536(a, p0, q0);
			}
		}
		else if (a)
			p0 = MUL_A_65536(a, scolor);

		*d++ = p0;
		yy += ayx;
		xx += axx;
	}
}

static void figure_stroke_fill_paint_proj(Enesim_Renderer *p, int x, int y,
		int len, unsigned int *dst)
{
	Enesim_Renderer_Shape *f = (Enesim_Renderer_Shape *) p;
	Figure *o = (Figure *) p;
	unsigned int fcolor = f->fill.color;
	unsigned int scolor = f->stroke.color;
	Enesim_Renderer *fpaint = f->fill.rend;
	int stroke = 0;
	unsigned int *d = dst, *e = d + len;
	Polygon_Edge *edges, *edge;
	Polygon_Vector *v = o->vectors;
	int nvectors = o->nvectors, n = 0;

	int axx = p->matrix.values.xx, axy = p->matrix.values.xy, axz =
			p->matrix.values.xz;
	int ayx = p->matrix.values.yx, ayy = p->matrix.values.yy, ayz =
			p->matrix.values.yz;
	int azx = p->matrix.values.zx, azy = p->matrix.values.zy, azz =
			p->matrix.values.zz;
	int xx = (axx * x) + (axx >> 1) + (axy * y) + (axy >> 1) + axz - 32768;
	int yy = (ayx * x) + (ayx >> 1) + (ayy * y) + (ayy >> 1) + ayz - 32768;
	int zz = (azx * x) + (azx >> 1) + (azy * y) + (azy >> 1) + azz;

	edges = alloca(nvectors * sizeof(Polygon_Edge));
	edge = edges;
	while (n < nvectors)
	{
		edge->xx0 = v->xx0;
		edge->xx1 = v->xx1;
		if (edge->xx1 < edge->xx0)
		{
			edge->xx0 = edge->xx1;
			edge->xx1 = v->xx0;
		}
		edge->yy0 = v->yy0;
		edge->yy1 = v->yy1;
		if (edge->yy1 < edge->yy0)
		{
			edge->yy0 = edge->yy1;
			edge->yy1 = v->yy0;
		}
		edge->de = ((v->a * (long long int) axx) >> 16) +
				((v->b * (long long int) ayx) >> 16) +
				((v->c * (long long int) azx) >> 16);
		edge->e = ((v->a * (long long int) xx) >> 16) +
				((v->b * (long long int) yy) >> 16) +
				((v->c * (long long int) zz) >> 16);
		n++;
		v++;
		edge++;
	}

	if (f->draw_mode == ENESIM_SHAPE_DRAW_MODE_FILL)
	{
		scolor = fcolor;
		stroke = 0;
		if (fpaint)
			fpaint->span(fpaint, x, y, len, dst);
	}
	if (f->draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE_FILL)
	{
		stroke = 1;
		if (fpaint)
			fpaint->span(fpaint, x, y, len, dst);
	}
	if (f->draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE)
	{
		fcolor = 0;
		fpaint = NULL;
		stroke = 1;
	}

	while (d < e)
	{
		unsigned int p0 = 0;
		int count = 0;
		int a = 0;
		int sxx, syy;

		n = 0;
		edge = edges;
		if (zz)
		{
			syy = (((long long int) yy) << 16) / zz;
			sxx = (((long long int) xx) << 16) / zz;

			while (n < nvectors)
			{
				int ee = (((long long int) edge->e) << 16) / zz;

				if (((syy + 0xffff) >= edge->yy0) &&
						(syy <= (edge->yy1 + 0xffff)))
				{
					if ((syy >= edge->yy0) &&
							(syy < edge->yy1))
					{
						count++;
						if (ee < 0)
							count -= 2;
					}
					if (ee < 0)
						ee = -ee;
					if ((ee < 65536) &&
							((sxx + 0xffff) >= edge->xx0) &&
							(sxx <= (0xffff + edge->xx1)))
					{
						if (a < 16384)
							a = 65536 - ee;
						else
							a = (a + (65536 - ee)) / 2;
					}
				}
				edge->e += edge->de;
				edge++;
				n++;
			}
			if (count)
			{
				p0 = fcolor;
				if (fpaint)
				{
					p0 = *d;
					if (fcolor != 0xffffffff)
						p0 = MUL4_SYM(fcolor, p0);
				}

				if (stroke && a)
				{
					unsigned int q0 = p0;

					p0 = scolor;
					if (a < 65536)
						p0 = INTERP_65536(a, p0, q0);
				}
			}
			else if (a)
				p0 = MUL_A_65536(a, scolor);
		}
		*d++ = p0;
		xx += axx;
		yy += ayx;
		zz += azx;
	}
}

static void figure_destroy(Enesim_Renderer *p)
{
	Figure *o = (Figure *) p;

	paint_polygon_clear(p);
	free(o);
}

static int _state_setup(Enesim_Renderer *p)
{
	Figure *o = (Figure *) p;
	Enesim_Renderer_Shape *f = (Enesim_Renderer_Shape *) p;

	if (!f || !o)
		return 0;
	if (!o->polys)
		return 0;

	if (o->changed)
	{
		Contour_Polygon *poly;
		int nvectors = 0;
		Polygon_Vector *vec;

		free(o->vectors);
		poly = o->polys;
		while (poly)
		{
			if (!poly->vertices || (poly->nverts < 3))
				return 0;
			nvectors += poly->nverts;
			if ((poly->last->v.x == poly->vertices->v.x) &&
					(poly->last->v.y == poly->vertices->v.y))
				nvectors--;
			poly = poly->next;
		}

		o->vectors = calloc(nvectors, sizeof(Polygon_Vector));
		if (!o->vectors)
			return 0;

		o->nvectors = nvectors;
		poly = o->polys;
		vec = o->vectors;
		o->lxx = 65536;
		o->rxx = -65536;
		o->tyy = 65536;
		o->byy = -65536;
		while (poly)
		{
			Polygon_Vertex *v, *nv;
			double x0, y0, x1, y1;
			double x01, y01;
			double len;
			int n = 0, nverts = poly->nverts;

			if ((poly->last->v.x == poly->vertices->v.x) &&
					(poly->last->v.y == poly->vertices->v.y))
				nverts--;
			v = poly->vertices;
			while (n < nverts)
			{
				nv = v->next;
				if (n == (poly->nverts - 1))
					nv = poly->vertices;
				x0 = v->v.x;
				y0 = v->v.y;
				x1 = nv->v.x;
				y1 = nv->v.y;
				x0 = ((int) (x0 * 256)) / 256.0;
				x1 = ((int) (x1 * 256)) / 256.0;
				y0 = ((int) (y0 * 256)) / 256.0;
				y1 = ((int) (y1 * 256)) / 256.0;
				x01 = x1 - x0;
				y01 = y1 - y0;
				if ((len = hypot(x01, y01)) < (1 / 256.0))
					return 0;
				len *= 1 + (1 / 16.0);
				vec->a = -(y01 * 65536) / len;
				vec->b = (x01 * 65536) / len;
				vec->c = (65536 * ((y1 * x0) - (x1 * y0)))
						/ len;
				vec->xx0 = x0 * 65536;
				vec->yy0 = y0 * 65536;
				vec->xx1 = x1 * 65536;
				vec->yy1 = y1 * 65536;

				if (vec->yy0 < o->tyy)
					o->tyy = vec->yy0;
				if (vec->yy0 > o->byy)
					o->byy = vec->yy0;

				if (vec->xx0 < o->lxx)
					o->lxx = vec->xx0;
				if (vec->xx0 > o->rxx)
					o->rxx = vec->xx0;

				n++;
				vec++;
				v = nv;
			}
			poly = poly->next;
		}
		o->changed = 0;
	}

	if (f->fill.rend &&
			((f->draw_mode == ENESIM_SHAPE_DRAW_MODE_FILL) ||
			(f->draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE_FILL)))
	{
		if (!enesim_renderer_state_setup(f->fill.rend))
			return 0;
	}
	/*
	 if (f->stroke.rend &&
	 ((f->draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE) || (f->draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE_FILL)))
	 {
	 if (!enesim_renderer_state_setup(f->stroke.paint)) return 0;
	 }
	 */

	p->span = figure_stroke_fill_paint_proj;
	if (!p->matrix.type == ENESIM_MATRIX_PROJECTIVE)
	{
		p->span = figure_stroke_fill_paint_affine;
		if (p->matrix.values.yx == 0)
			p->span = figure_stroke_fill_paint_affine_simple;
	}

	return 1;
}

static void _state_cleanup(Enesim_Renderer *p)
{
	Enesim_Renderer_Shape *f = (Enesim_Renderer_Shape *) p;

	/*
	 if (f->stroke.rend &&
			((f->draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE) ||
			(f->draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE_FILL)))
		 enesim_renderer_state_cleanup(f->stroke.paint);
	 */
	if (f->fill.rend &&
			((f->draw_mode == ENESIM_SHAPE_DRAW_MODE_FILL) ||
			(f->draw_mode == ENESIM_SHAPE_DRAW_MODE_STROKE_FILL)))
		enesim_renderer_state_cleanup(f->fill.rend);
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
EAPI Enesim_Renderer * enesim_renderer_figure_new(void)
{
	Enesim_Renderer *p;
	Enesim_Renderer_Shape *f;
	Figure *o;

	o = calloc(1, sizeof(Figure));
	if (!o)
		return NULL;

	p = (Enesim_Renderer *) o;
	enesim_renderer_shape_init(p);
	p->state_setup = _state_setup;
	p->state_cleanup = _state_cleanup;

	return p;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_figure_polygon_add(Enesim_Renderer *p)
{
	Enesim_Renderer_Shape *f;
	Figure *o;
	Contour_Polygon *poly;

	o = (Figure *) p;

	poly = calloc(1, sizeof(Contour_Polygon));
	if (!poly)
		return;

	if (!o->polys)
	{
		o->polys = o->last = poly;
		o->npolys++;
		o->changed = 1;
		return;
	}
	o->last->next = poly;
	o->last = poly;
	o->npolys++;
	o->changed = 1;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_figure_polygon_vertex_add(Enesim_Renderer *p,
		float x, float y)
{
	Enesim_Renderer_Shape *f;
	Figure *o;
	Contour_Polygon *poly;
	Polygon_Vertex *vertex;

	o = (Figure *) p;

	if (!o->polys)
		return; // maybe just add one instead
	poly = o->last;
	if (poly->last &&
			(fabs(poly->last->v.x - x) < (1 / 256.0)) &&
			(fabs(poly->last->v.y - y) < (1 / 256.0)))
		return;
	vertex = calloc(1, sizeof(Polygon_Vertex));
	if (!vertex)
		return;
	vertex->v.x = x;
	vertex->v.y = y;

	if (!poly->vertices)
	{
		poly->vertices = poly->last = vertex;
		poly->nverts++;
		o->changed = 1;
		return;
	}
	poly->last->next = vertex;
	poly->last = vertex;
	poly->nverts++;
	o->changed = 1;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_figure_extents_get(Enesim_Renderer *p, int *lx,
		int *ty, int *rx, int *by)
{
	Enesim_Renderer_Shape *f;
	Figure *o;

	o = (Figure *) p;

	// for now.. but do this better later
	if (!enesim_renderer_state_setup(p))
	{
		if (lx)
			*lx = 0;
		if (rx)
			*rx = 0;
		if (ty)
			*ty = 0;
		if (by)
			*by = 0;
	}
	if (lx)
		*lx = (o->lxx - 0xffff) >> 16;
	if (rx)
		*lx = (o->rxx + 0xffff) >> 16;
	if (ty)
		*ty = (o->tyy - 0xffff) >> 16;
	if (by)
		*by = (o->byy + 0xffff) >> 16;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_figure_clear(Enesim_Renderer *p)
{
	Enesim_Renderer_Shape *f;
	Figure *o;
	Contour_Polygon *c, *nc;
	Polygon_Vertex *v, *nv;

	o = (Figure *) p;

	if (!o->polys)
		return;
	c = o->polys;
	while (c)
	{
		v = c->vertices;
		while (v)
		{
			nv = v->next;
			free(v);
			v = nv;
		}
		nc = c->next;
		free(c);
		c = nc;
	}
	o->polys = o->last = NULL;
	o->npolys = 0;
	free(o->vectors);
	o->vectors = NULL;
	o->changed = 1;
}

#if 0
int
paint_is_polygon(Enesim_Renderer *p)
{
	Enesim_Renderer_Shape *f;

	if (!p || !p->type) return 0;
	if (p->type->id != SHAPE_PAINT) return 0;
	f = (Enesim_Renderer_Shape *)p;
	if (!f || (f->id != POLYGON_PAINT)) return 0;
	return 1;
}
#endif

