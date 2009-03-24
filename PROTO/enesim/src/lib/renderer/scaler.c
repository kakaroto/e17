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
typedef struct _Renderer_Scaler
{
	Enesim_Renderer r;
	Enesim_Quality quality;
	struct
	{
		Enesim_Surface_Data *data;
		Eina_Rectangle area;
	} src, mask;
	struct
	{
		Eina_Rectangle area;
	} dst;
	/* TODO move the row and offset to src and mask */
	struct {
		int *values;
		int len;
	} offset, row;
} Renderer_Scaler;

/* FIXME use the area.x and area.y for the offsets and rows */
static void _generate_offsets(Renderer_Scaler *s)
{
	int x;

	if (!s->src.area.w || !s->dst.area.w)
		return;
	if (!s->offset.values || s->offset.len < s->dst.area.w)
	{
		s->offset.values = realloc(s->offset.values, sizeof(int) * s->dst.area.w);
		s->offset.len = s->dst.area.w;
	}
	for (x = 0; x < s->dst.area.w; x++)
	{
		s->offset.values[x] = (x * s->src.area.w) / s->dst.area.w;
	}
}

static void _generate_rows(Renderer_Scaler *s)
{
	int y;

	if (!s->src.area.h || !s->dst.area.h)
		return;
	if (!s->row.values || s->row.len < s->dst.area.w)
	{
		s->row.values = realloc(s->row.values, sizeof(int) * s->dst.area.h);
		s->row.len = s->dst.area.w;
	}
	for (y = 0; y < s->dst.area.h; y++)
	{
		s->row.values[y] = (y * s->src.area.h) / s->dst.area.h;
	}
}

/* the destination surface data should be at offset x,y already */
static Eina_Bool _generic(Enesim_Renderer *r, int x, int y, int len, Enesim_Surface_Data *dst)
{
	Renderer_Scaler *s = (Renderer_Scaler *)r;
	Eina_Rectangle slrect;
	Enesim_Drawer_Point pt;
	Enesim_Drawer_Span sp;
	Enesim_Surface_Data sdata, ddata;
	Enesim_Surface_Pixel p;
	int dx = 0;

	/* check that the span actually intersects the destination area */
	eina_rectangle_coords_from(&slrect, x, y, len, 1);
	if (!eina_rectangle_intersection(&slrect, &s->dst.area))
	{
		return EINA_FALSE;
	}
	sdata = *s->src.data;
	ddata = *dst;
	enesim_surface_data_increment(&sdata, s->src.area.x);
	while (dx < slrect.w)
	{
		Enesim_Surface_Data tmp;

		tmp = sdata;
		tmp.plane0 += s->offset.values[dx];
		*ddata.plane0 = *tmp.plane0;
		ddata.plane0++;
		dx++;
	}
	return EINA_TRUE;
}

static Enesim_Renderer_Span _get(Renderer_Scaler *s, Enesim_Format *f)
{
	/* check that the mask and src areas are of the same size */
	return _generic;
}

static void _free(Renderer_Scaler *s)
{
	if (s->offset.values)
		free(s->offset.values);
	if (s->row.values)
		free(s->row.values);
	free(s);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer * enesim_renderer_scaler_new(void)
{
	Renderer_Scaler *s;

	s = calloc(1, sizeof(Renderer_Scaler));
	s->r.free = ENESIM_RENDERER_FREE(_free);
	s->r.get = ENESIM_RENDERER_GET(_get);

	return &s->r;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_scaler_dst_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Renderer_Scaler *s = (Renderer_Scaler *)r;

	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_coords_from(&s->dst.area, x, y, w, h);
	_generate_rows(s);
	_generate_offsets(s);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_scaler_src_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Renderer_Scaler *s = (Renderer_Scaler *)r;

	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_coords_from(&s->src.area, x, y, w, h);
	_generate_rows(s);
	_generate_offsets(s);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_scaler_mask_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Renderer_Scaler *s = (Renderer_Scaler *)r;

	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_coords_from(&s->mask.area, x, y, w, h);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_scaler_src_set(Enesim_Renderer *r, Enesim_Surface_Data *sdata)
{
	Renderer_Scaler *s = (Renderer_Scaler *)r;

	s->src.data = sdata;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_scaler_src_unset(Enesim_Renderer *r)
{
	Renderer_Scaler *s = (Renderer_Scaler *)r;

	s->src.data = NULL;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_scaler_mask_set(Enesim_Renderer *r, Enesim_Surface_Data *mdata)
{
	Renderer_Scaler *s = (Renderer_Scaler *)r;

	s->mask.data = mdata;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_scaler_mask_unset(Enesim_Renderer *r)
{
	Renderer_Scaler *s = (Renderer_Scaler *)r;

	s->mask.data = NULL;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool enesim_renderer_scaler_src_y(Enesim_Renderer *r, int ydst, int *ysrc)
{
	Renderer_Scaler *s = (Renderer_Scaler *)r;

	if (ydst > (s->dst.area.y + s->dst.area.w))
	{
		return EINA_FALSE;
	}
	else
	{
		*ysrc = s->row.values[ydst];
		return EINA_TRUE;
	}
}
