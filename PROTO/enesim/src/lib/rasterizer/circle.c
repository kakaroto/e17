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
/*
 * References:
 * http://www.b500.com/~hplus/graphics/RasterizeCircle.html
 * http://www.cs.unc.edu/~mcmillan/comp136/Lecture7/circle.html
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef void (*Circle_Plot)(int cx, int cy, int x, int y, Enesim_Scanline_Callback cb, void *data);

typedef struct _Circle
{
	Enesim_Rasterizer r;
	float radius;
	Enesim_Rasterizer_Fill_Policy policy;
	float cx, cy;
	Circle_Plot cp;
} Circle;

static void _point_plot(int cx, int cy, int x, int y, Enesim_Scanline_Callback cb, void *data)
{
	Enesim_Scanline sl;

	sl.type = ENESIM_SCANLINE_ALIAS;
	sl.data.alias.w = 1;

	if (x == 0)
	{
		sl.data.alias.x = cx;
		sl.data.alias.y = cy + y;
		cb(&sl, data);
		sl.data.alias.x = cx;
		sl.data.alias.y = cy - y;
		cb(&sl, data);
		sl.data.alias.x = cx + y;
		sl.data.alias.y = cy;
		cb(&sl, data);
		sl.data.alias.x = cx - y;
		sl.data.alias.y = cy;
		cb(&sl, data);
	}
	else if (x == y)
	{
		sl.data.alias.x = cx + x;
		sl.data.alias.y = cy + y;
		cb(&sl, data);
		sl.data.alias.x = cx - x;
		sl.data.alias.y = cy + y;
		cb(&sl, data);
		sl.data.alias.x = cx + x;
		sl.data.alias.y = cy - y;
		cb(&sl, data);
		sl.data.alias.x = cx - x;
		sl.data.alias.y = cy - y;
		cb(&sl, data);
	}
	else if (x < y)
	{
		sl.data.alias.x = cx + x;
		sl.data.alias.y = cy + y;
		cb(&sl, data);
		sl.data.alias.x = cx - x;
		sl.data.alias.y = cy + y;
		cb(&sl, data);
		sl.data.alias.x = cx + x;
		sl.data.alias.y = cy - y;
		cb(&sl, data);
		sl.data.alias.x = cx - x;
		sl.data.alias.y = cy - y;
		cb(&sl, data);

		sl.data.alias.x = cx + y;
		sl.data.alias.y = cy + x;
		cb(&sl, data);
		sl.data.alias.x = cx - y;
		sl.data.alias.y = cy + x;
		cb(&sl, data);
		sl.data.alias.x = cx + y;
		sl.data.alias.y = cy - x;
		cb(&sl, data);
		sl.data.alias.x = cx - y;
		sl.data.alias.y = cy - x;
		cb(&sl, data);

	}
}

static void _span_plot(int cx, int cy, int x, int y, Enesim_Scanline_Callback cb, void *data)
{
	Enesim_Scanline sl;

	sl.type = ENESIM_SCANLINE_ALIAS;

	if (x == 0)
	{
		sl.data.alias.x = cx;
		sl.data.alias.y = cy + y;
		sl.data.alias.w = 1;
		cb(&sl, data);
		sl.data.alias.x = cx;
		sl.data.alias.y = cy - y;
		cb(&sl, data);

		sl.data.alias.x = cx - y;
		sl.data.alias.y = cy;
		sl.data.alias.w = y << 1;
		cb(&sl, data);
	}
	else if (x == y)
	{
		sl.data.alias.x = cx - x;
		sl.data.alias.y = cy + y;
		sl.data.alias.w = x << 1;
		cb(&sl, data);

		sl.data.alias.x = cx - x;
		sl.data.alias.y = cy - y;
		cb(&sl, data);
	}
	else if (x < y)
	{
		sl.data.alias.x = cx - x;
		sl.data.alias.y = cy + y;
		sl.data.alias.w = x << 1;
		cb(&sl, data);
		sl.data.alias.x = cx - x;
		sl.data.alias.y = cy - y;
		cb(&sl, data);

		sl.data.alias.x = cx - y;
		sl.data.alias.y = cy + x;
		sl.data.alias.w = y << 1;
		cb(&sl, data);
		sl.data.alias.x = cx - y;
		sl.data.alias.y = cy - x;
		cb(&sl, data);

	}
}

void _rasterizer_vertex_add(Circle *c, float x, float y)
{
	c->cx = x;
	c->cy = y;
}

static void _rasterizer_generate(Circle *c, Eina_Rectangle *rect, Enesim_Scanline_Callback cb, void *data)
{
	int rs2 = c->radius * c->radius * 4;
	int xs2 = 0;
	int ys2m1 = rs2 - 2 * c->radius + 1;
	int x = 0;
	int y = c->radius;
	int ycs2;

	c->cp(c->cx, c->cy, x, y, cb, data);
	while (x <= y)
	{
		/* advance to the right */
		xs2 = xs2 + 8* x + 4;
		++x;

		/* calculate new Yc */
		ycs2 = rs2 - xs2;
		if (ycs2 < ys2m1)
		{
			ys2m1 = ys2m1 - 8 * y + 4;
			y--;
		}
		c->cp(c->cx, c->cy, x, y, cb, data);
	}
}
static void _rasterizer_delete(Circle *c)
{
	free(c);
}

static void _rasterizer_reset(Circle *c)
{
	c->cx = 0;
	c->cy = 0;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void enesim_rasterizer_circle_radius_set(Enesim_Rasterizer *r, float radius)
{
	Circle *c = r->data;

	c->radius = radius;
}


EAPI Enesim_Rasterizer * enesim_rasterizer_circle_new(void)
{
	Circle *c;

	c = calloc(1, sizeof(Circle));

	enesim_rasterizer_init(&c->r);
	c->cp = _point_plot;
	/* setup the rasterizer */
	c->r.data = c;
	c->r.reset = ENESIM_RASTERIZER_RESET(_rasterizer_reset);
	c->r.vertex_add = ENESIM_RASTERIZER_VERTEX_ADD(_rasterizer_vertex_add);
	c->r.generate = ENESIM_RASTERIZER_GENERATE(_rasterizer_generate);
	c->r.delete = ENESIM_RASTERIZER_DELETE(_rasterizer_delete);

	return &c->r;
}

EAPI void enesim_rasterizer_circle_fill_policy_set(Enesim_Rasterizer *r, Enesim_Rasterizer_Fill_Policy po)
{
	Circle *c = r->data;
	c->policy = po;
	if (po == ENESIM_RASTERIZER_FILL_POLICY_BORDER)
		c->cp = _point_plot;
	else
		c->cp = _span_plot;
}
