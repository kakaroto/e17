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
/*
  very simple path struct..
  should maybe keep path commands
  so that we can defer subdiv approx
  to the setup function.
*/
typedef struct _Path Path;
struct _Path
{
   Enesim_Renderer_Shape   base;

   Enesim_Renderer   *figure;
   float   last_x, last_y;
   float   last_ctrl_x, last_ctrl_y;

// ....  geom_transform?
};

static void _quadratic_to(Enesim_Renderer *p, float ctrl_x, float ctrl_y,
		float x, float y);
static void _cubic_to(Enesim_Renderer *p, float ctrl_x0, float ctrl_y0,
		float ctrl_x, float ctrl_y, float x, float y);

static void _move_to(Enesim_Renderer *p, float x, float y)
{
	Enesim_Renderer_Shape *f;
	Path *o;

	o = (Path *) p;

	enesim_renderer_figure_polygon_add(o->figure);
	enesim_renderer_figure_polygon_vertex_add(o->figure, x, y);
	o->last_x = x;
	o->last_y = y;
	o->last_ctrl_x = x;
	o->last_ctrl_y = y;
}

static void _line_to(Enesim_Renderer *p, float x, float y)
{
	Enesim_Renderer_Shape *f;
	Path *o;

	o = (Path *) p;

	enesim_renderer_figure_polygon_vertex_add(o->figure, x, y);
	o->last_ctrl_x = o->last_x;
	o->last_ctrl_y = o->last_y;
	o->last_x = x;
	o->last_y = y;
}

/* these subdiv approximations need to be done more carefully */
static void _quadratic_to(Enesim_Renderer *p, float ctrl_x, float ctrl_y,
		float x, float y)
{
	Enesim_Renderer_Shape *f;
	Path *o;
	double x0, y0, x1, y1, x01, y01;
	double sm = 1 / 92.0;

	o = (Path *) p;

	x0 = o->last_x;
	y0 = o->last_y;
	x01 = (x0 + x) / 2;
	y01 = (y0 + y) / 2;
	if ((((x01 - ctrl_x) * (x01 - ctrl_x)) + ((y01 - ctrl_y) * (y01
			- ctrl_y))) <= sm)
	{
		enesim_renderer_figure_polygon_vertex_add(o->figure, x, y);
		o->last_x = x;
		o->last_y = y;
		o->last_ctrl_x = ctrl_x;
		o->last_ctrl_y = ctrl_y;
		return;
	}

	x0 = (ctrl_x + x0) / 2;
	y0 = (ctrl_y + y0) / 2;
	x1 = (ctrl_x + x) / 2;
	y1 = (ctrl_y + y) / 2;
	x01 = (x0 + x1) / 2;
	y01 = (y0 + y1) / 2;

	_quadratic_to(p, x0, y0, x01, y01);
	o->last_x = x01;
	o->last_y = y01;
	o->last_ctrl_x = x0;
	o->last_ctrl_y = y0;

	_quadratic_to(p, x1, y1, x, y);
	o->last_x = x;
	o->last_y = y;
	o->last_ctrl_x = x1;
	o->last_ctrl_y = y1;
}

static void _squadratic_to(Enesim_Renderer *p, float x, float y)
{
	Enesim_Renderer_Shape *f;
	Path *o;
	double x0, y0, cx0, cy0;

	o = (Path *) p;

	x0 = o->last_x;
	y0 = o->last_y;
	cx0 = o->last_ctrl_x;
	cy0 = o->last_ctrl_y;
	cx0 = (2 * x0) - cx0;
	cy0 = (2 * y0) - cy0;

	_quadratic_to(p, cx0, cy0, x, y);
	o->last_x = x;
	o->last_y = y;
	o->last_ctrl_x = cx0;
	o->last_ctrl_y = cy0;
}

static void _scubic_to(Enesim_Renderer *p, float ctrl_x, float ctrl_y, float x,
		float y)
{
	Enesim_Renderer_Shape *f;
	Path *o;
	double x0, y0, cx0, cy0;

	o = (Path *) p;

	x0 = o->last_x;
	y0 = o->last_y;
	cx0 = o->last_ctrl_x;
	cy0 = o->last_ctrl_y;
	cx0 = (2 * x0) - cx0;
	cy0 = (2 * y0) - cy0;

	_cubic_to(p, cx0, cy0, ctrl_x, ctrl_y, x, y);
	o->last_x = x;
	o->last_y = y;
	o->last_ctrl_x = ctrl_x;
	o->last_ctrl_y = ctrl_y;
}

static void _cubic_to(Enesim_Renderer *p, float ctrl_x0, float ctrl_y0,
		float ctrl_x, float ctrl_y, float x, float y)
{
	Enesim_Renderer_Shape *f;
	Path *o;
	double x0, y0, x01, y01, x23, y23;
	double xa, ya, xb, yb, xc, yc;
	double sm = 1 / 64.0;

	o = (Path *) p;

	x0 = o->last_x;
	y0 = o->last_y;
	x01 = (x0 + x) / 2;
	y01 = (y0 + y) / 2;
	x23 = (ctrl_x0 + ctrl_x) / 2;
	y23 = (ctrl_y0 + ctrl_y) / 2;

	if ((((x01 - x23) * (x01 - x23)) + ((y01 - y23) * (y01 - y23))) <= sm)
	{
		enesim_renderer_figure_polygon_vertex_add(o->figure, x, y);
		o->last_x = x;
		o->last_y = y;
		o->last_ctrl_x = ctrl_x;
		o->last_ctrl_y = ctrl_y;
		return;
	}

	x01 = (x0 + ctrl_x0) / 2;
	y01 = (y0 + ctrl_y0) / 2;
	xc = x23;
	yc = y23;
	x23 = (x + ctrl_x) / 2;
	y23 = (y + ctrl_y) / 2;
	xa = (x01 + xc) / 2;
	ya = (y01 + yc) / 2;
	xb = (x23 + xc) / 2;
	yb = (y23 + yc) / 2;
	xc = (xa + xb) / 2;
	yc = (ya + yb) / 2;

	_cubic_to(p, x01, y01, xa, ya, xc, yc);
	o->last_x = xc;
	o->last_y = yc;
	o->last_ctrl_x = xa;
	o->last_ctrl_y = ya;

	_cubic_to(p, xb, yb, x23, y23, x, y);
	o->last_x = x;
	o->last_y = y;
	o->last_ctrl_x = x23;
	o->last_ctrl_y = y23;
}

static void _span(Enesim_Renderer *p, int x, int y, int len, unsigned int *dst)
{
	Path *o = (Path *) p;

	o->figure->span(o->figure, x, y, len, dst);
}

static int _state_setup(Enesim_Renderer *p)
{
	Path *o = (Path *) p;
	Enesim_Renderer_Shape *f = (Enesim_Renderer_Shape *) p;

	if (!f || !o)
		return 0;
	if (!o->figure)
		return 0; // should just not draw

	enesim_renderer_shape_outline_weight_set(o->figure, f->stroke.weight);
	enesim_renderer_shape_outline_color_set(o->figure, f->stroke.color);
	enesim_renderer_shape_outline_renderer_set(o->figure, f->stroke.rend);
	enesim_renderer_shape_fill_color_set(o->figure, f->fill.color);
	enesim_renderer_shape_fill_renderer_set(o->figure, f->fill.rend);
	enesim_renderer_shape_draw_mode_set(o->figure, f->draw_mode);

	o->figure->matrix = p->matrix;

	if (!enesim_renderer_state_setup(o->figure))
		return 0;

	p->span = _span;

	return 1;
}

static void _state_cleanup(Enesim_Renderer *p)
{
	Path *o = (Path *) p;

	enesim_renderer_state_cleanup(o->figure);
}

#if 0
static void path_destroy(Enesim_Renderer *p)
{
	Path *o = (Path *)p;

	paint_free(o->figure);
	free(o);
}
#endif

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
EAPI Enesim_Renderer * enesim_renderer_path_new(void)
{
	Enesim_Renderer *p;
	Enesim_Renderer_Shape *f;
	Path *o;

	o = calloc(1, sizeof(Path));
	if (!o)
		return NULL;

	o->figure = enesim_renderer_figure_new();
	if (!o->figure)
	{
		free(o);
		return NULL;
	}

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
EAPI void enesim_renderer_path_move_to(Enesim_Renderer *p, float x, float y)
{
	x = ((int) (2* x + 0.5)) / 2.0;
	y = ((int) (2* y + 0.5)) / 2.0;
	_move_to(p, x, y);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_path_line_to(Enesim_Renderer *p, float x, float y)
{
	x = ((int) (2* x + 0.5)) / 2.0;
	y = ((int) (2* y + 0.5)) / 2.0;
	_line_to(p, x, y);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_path_squadratic_to(Enesim_Renderer *p, float x,
		float y)
{
	x = ((int) (2* x + 0.5)) / 2.0;
	y = ((int) (2* y + 0.5)) / 2.0;
	_squadratic_to(p, x, y);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_path_quadratic_to(Enesim_Renderer *p, float ctrl_x,
		float ctrl_y, float x, float y)
{
	x = ((int) (2* x + 0.5)) / 2.0;
	y = ((int) (2* y + 0.5)) / 2.0;
	_quadratic_to(p, ctrl_x, ctrl_y, x, y);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_path_cubic_to(Enesim_Renderer *p, float ctrl_x0,
		float ctrl_y0, float ctrl_x, float ctrl_y, float x, float y)
{
	x = ((int) (2* x + 0.5)) / 2.0;
	y = ((int) (2* y + 0.5)) / 2.0;
	_cubic_to(p, ctrl_x0, ctrl_y0, ctrl_x, ctrl_y, x, y);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_path_scubic_to(Enesim_Renderer *p, float ctrl_x,
		float ctrl_y, float x, float y)
{
	x = ((int) (2* x + 0.5)) / 2.0;
	y = ((int) (2* y + 0.5)) / 2.0;
	_scubic_to(p, ctrl_x, ctrl_y, x, y);
}
/**
 * To be documented
 * FIXME do this more efficiently.. but for now:
 */
EAPI void enesim_renderer_path_extents_get(Enesim_Renderer *p, int *lx,
		int *ty, int *rx, int *by)
{
	Enesim_Renderer_Shape *f;
	Path *o;

	o = (Path *) p;

	enesim_renderer_figure_extents_get(o->figure, lx, ty, rx, by);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_path_clear(Enesim_Renderer *p)
{
	Enesim_Renderer_Shape *f;
	Path *o;

	o = (Path *) p;

	enesim_renderer_figure_clear(o->figure);
	o->last_x = 0;
	o->last_y = 0;
	o->last_ctrl_x = 0;
	o->last_ctrl_y = 0;
}

#if 0
int
paint_is_path(Enesim_Renderer *p)
{
	Enesim_Renderer_Shape *f;

	if (!p || !p->type) return 0;
	if (p->type->id != SHAPE_PAINT) return 0;
	f = (Enesim_Renderer_Shape *)p;
	if (!f || (f->id != PATH_PAINT)) return 0;
	return 1;
}
#endif

