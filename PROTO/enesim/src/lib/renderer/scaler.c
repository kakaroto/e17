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
	Enesim_Quality quality;
	struct
	{
		Enesim_Surface_Data *data;
		Eina_Rectangle area;
		int w;
		int h;
	} src, mask;
	struct
	{
		Eina_Rectangle area;
		int w;
		int h;
	} dst;
	int *length;
	int *row;
	Enesim_Drawer_Point pt;
} Renderer_Scaler;

static void _generate(Renderer_Scaler *s)
{
	int xinc;
	int xres;
	int yinc;
	int yres;
	int x, y;
	int res;

	//printf("%d %d %d %d\n", s->src.area.w, s->dst.area.w);
	if (!s->src.area.w || !s->dst.area.w)
		return;
	if (!s->length)
		s->length = malloc(sizeof(int) * s->dst.area.w);
	if (!s->row)
		s->row = malloc(sizeof(int) * s->dst.area.h);

#if 0
	xinc = s->dst.area.w / s->src.area.w;
	xres = res = s->dst.area.w % s->src.area.w;
	for (x = 0; x < s->src.area.w; x++)
	{
		int tmp;
		tmp = res / s->src.area.w;
		res = res % s->src.area.w;
		s->length[x] = xinc + tmp;
		res += xres;
	}
#endif
	//printf("%d %d\n", s->src.area.w, s->dst.area.w);
	for (x = 0; x < s->dst.area.w; x++)
	{
		s->length[x] = (x * s->src.area.w) / s->dst.area.w;
	}
	for (y = 0; y < s->dst.area.h; y++)
	{
		s->row[y] = (y * s->src.area.h) / s->dst.area.h;
	}
}

#define DIRECT 1

/* the destination surface data should be at offset x,y already */
static void _generic(Enesim_Renderer *r, int x, int y, int len, Enesim_Surface_Data *dst)
{
	Renderer_Scaler *s = r->data;
	Enesim_Drawer_Point pt;
	Enesim_Drawer_Span sp;
	Enesim_Surface_Data sdata, ddata;
	Enesim_Surface_Pixel p;
	int dx = 0;
#if 0
	int dx = x;
	int end = x + len;
	int sx = x * s->dst.area.w / s->src.area.w;

	/* Fake an alpha color */
	enesim_surface_pixel_components_from(&p, dst->format, 0xaa, 0xff, 0xaa, 0xff);
	sp = enesim_drawer_span_color_get(ENESIM_FILL, dst->format, &p);
	/* TODO handle the clipping, etc */
	/* TODO create the tables that store the number of pixels per src pixel and the y offsets */
	/* draw a span of this length from y row */
	sdata = *s->src.data;
	printf("entering for line at %d,%d:%d and should be the line %d from src\n", x, y, len, s->row[y]);
	printf("dx %d end %d sx %d\n", dx, end, sx);

	while (dx < end)
	{
		enesim_surface_data_pixel_get(&sdata, &p);
		enesim_surface_data_increment(dst, 1);
		sp(dst, s->length[sx], NULL, &p, NULL);
		dx += s->length[sx];
		sx++;
	}
#endif

	//printf("going to draw\n");

	sdata = *s->src.data;
	ddata = *dst;
#if !DIRECT
	enesim_surface_pixel_components_from(&p, dst->format, 0xaa, 0xff, 0xaa, 0xff);
	pt = enesim_drawer_point_color_get(ENESIM_FILL, dst->format, &p);
#endif
	enesim_surface_data_increment(&sdata, s->src.area.x);
	while (dx < len)
	{
		Enesim_Surface_Data tmp;

		tmp = sdata;
		//printf("position %d %d\n", dx, s->length[dx]);
		tmp.plane0 += s->length[dx];
		//enesim_surface_data_increment(&tmp, s->length[dx]);

		//enesim_surface_data_pixel_get(&tmp, &p);
#if DIRECT
		*ddata.plane0 = *tmp.plane0;
#else
		p.plane0 = *tmp.plane0;
		pt(&ddata, NULL, &p, NULL);
#endif
		ddata.plane0++;
		//enesim_surface_data_increment(&ddata, 1);
		dx++;
	}
#if 0
	{
		int i;
		for (i = 0; i < s->src.area.w; i++)
		{
			printf("LEN = %d\n", s->length[i]);
		}
	}
#endif
}

static Enesim_Renderer_Span _get(Enesim_Renderer *r, Enesim_Format *f)
{
	Renderer_Scaler *s;

	s = r->data;
	/* check that the mask and src areas are of the same size */
	return _generic;
}

static void _free(Enesim_Renderer *r)
{
	free(r->data);
}

static Enesim_Renderer_Func f_func = {
	.get = _get,
	.free = _free,
};
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
	Enesim_Renderer *r;

	s = calloc(1, sizeof(Renderer_Scaler));

	r = enesim_renderer_new();
	r->data = s;
	r->funcs = &f_func;

	return r;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_scaler_dst_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Renderer_Scaler *f;

	assert(r);
	f = r->data;
	assert(f);

	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_coords_from(&f->dst.area, x, y, w, h);
	/* TODO check the dst surface boundings and pick the min */
	/* TODO update the malloced buffer span */
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_scaler_src_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Renderer_Scaler *f;

	assert(r);
	f = r->data;
	assert(f);

	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_coords_from(&f->src.area, x, y, w, h);
	/* TODO update the malloced buffer span */
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_scaler_mask_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Renderer_Scaler *f;

	assert(r);
	f = r->data;
	assert(f);

	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_coords_from(&f->mask.area, x, y, w, h);
}

EAPI void enesim_renderer_scaler_src_set(Enesim_Renderer *r, Enesim_Surface_Data *sdata)
{
	Renderer_Scaler *f;

	f = r->data;
	f->src.data = sdata;
}

EAPI void enesim_renderer_scaler_src_size_set(Enesim_Renderer *r, int sw, int sh)
{
	Renderer_Scaler *f;

	f = r->data;
	f->src.w = sw;
	f->src.h = sh;
	_generate(f);
}

EAPI void enesim_renderer_scaler_mask_size_set(Enesim_Renderer *r, int sw, int sh)
{
	Renderer_Scaler *f;

	f = r->data;
	f->mask.w = sw;
	f->mask.h = sh;
}

EAPI void enesim_renderer_scaler_src_unset(Enesim_Renderer *r)
{
	Renderer_Scaler *f;

	f = r->data;
	f->src.data = NULL;
}


EAPI void enesim_renderer_scaler_mask_set(Enesim_Renderer *r, Enesim_Surface_Data *mdata)
{
	Renderer_Scaler *f;

	f = r->data;
	f->mask.data = mdata;
}

EAPI void enesim_renderer_scaler_mask_unset(Enesim_Renderer *r)
{
	Renderer_Scaler *f;

	f = r->data;
	f->mask.data = NULL;
}

EAPI void enesim_renderer_scaler_src_offset(Enesim_Renderer *r, int y, int *offset)
{
	Renderer_Scaler *f;

	f = r->data;
	*offset = f->src.w * f->row[y];
}

EAPI void enesim_renderer_scaler_mask_offset(Enesim_Renderer *r, int y, int *offset)
{
	Renderer_Scaler *f;

	f = r->data;
	*offset = f->src.w * f->row[y];
}

EAPI Eina_Bool enesim_renderer_scaler_src_y(Enesim_Renderer *r, int ydst, int *ysrc)
{
	Renderer_Scaler *s;

	s = r->data;
	if (ydst > (s->dst.area.y + s->dst.area.w))
	{
		return EINA_TRUE;
	}
	else
	{
		*ysrc = s->row[ydst];
		return EINA_TRUE;
	}
}
