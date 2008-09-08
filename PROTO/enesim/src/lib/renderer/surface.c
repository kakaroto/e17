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
#include "enesim_common.h"
#include "Enesim.h"
#include "enesim_private.h"

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
typedef struct _Renderer_Surface
{
	Enesim_Surface *s;
	int mode;
	Eina_Rectangle sarea;
	Eina_Rectangle darea;
} Renderer_Surface;

#define DRECT f->darea
#define SRECT f->sarea

static inline void _draw_alias(Enesim_Renderer *r, Enesim_Scanline_Alias *sl, Enesim_Surface *dst)
{
	Enesim_Drawer_Span spfnc;
	Eina_Rectangle slrect, dr;
	Enesim_Surface_Data sdata, ddata;
	Renderer_Surface *f = r->data;
	int ssw, ssh; /* source surface width and height */
	int dsw, dsh; /* destination surface width and height */
	int paw, pah; /* pattern area width and height */

	if (!f->s)
		return;
	
	/* check if the scanline intersects with the destination rectangle */
	if (!(f->mode & ENESIM_SURFACE_REPEAT_X))
	{
		paw = MIN(SRECT.w, DRECT.w);
	}
	else
	{
		paw = DRECT.w;
	}
	if (!(f->mode & ENESIM_SURFACE_REPEAT_Y))
	{
		pah = MIN(SRECT.h, DRECT.h);
	}
	else
	{
		pah = DRECT.h;
	}
	eina_rectangle_coords_from(&slrect, sl->x, sl->y, sl->w, 1);
	eina_rectangle_coords_from(&dr, DRECT.x, DRECT.y, paw, pah);
	if (!eina_rectangle_intersection(&slrect, &dr))
	{
			return;
	}
	/* setup */
	spfnc = enesim_drawer_span_pixel_get(r->rop, dst->format, f->s->format);
	/* the source surface data should be at offset SRECT.x and sl->y */
	enesim_surface_size_get(f->s, &ssw, &ssh);
	enesim_surface_data_get(f->s, &sdata);
	enesim_surface_data_increment(&sdata, f->s->format, (((slrect.y - DRECT.y) % SRECT.h) * ssw) + SRECT.x);
	/* the desitination surface data should be at offset sl->x and sl->y */ 
	enesim_surface_size_get(dst, &dsw, &dsh);
	enesim_surface_data_get(dst, &ddata);
	enesim_surface_data_increment(&ddata, dst->format, (slrect.y * dsw) + slrect.x);
	
	/* left */
	//printf("1 %d %d %d %d\n", slrect.x, slrect.y, slrect.w, slrect.h);
	if (slrect.x > DRECT.x)
	{
		Enesim_Surface_Data tmp;
		int len;
		
		tmp = sdata;
		enesim_surface_data_increment(&tmp, f->s->format, slrect.x - DRECT.x);
		len = MIN(slrect.w, SRECT.w - slrect.x);
		spfnc(&ddata, len, &tmp, /* mul_color */0, NULL);
		enesim_surface_data_increment(&ddata, dst->format, len);
		//printf("left %d\n", len);
		slrect.x += len;
		slrect.w -= len;
	}
	/* we dont have anything left to draw */
	if (!slrect.w)
		return;
	
	//printf("2 %d %d %d %d\n", slrect.x, slrect.y, slrect.w, slrect.h);
	/* middle */
	{
		int len = MIN(paw, SRECT.w);
		int count = slrect.w / len; 
		int total = count * len;
		int i;
		
		//printf("middle = %d %d\n", count, len);
		for (i = 0; i < count; i++)
		{
			spfnc(&ddata, len, &sdata, /* mul_color */0, NULL);
			enesim_surface_data_increment(&ddata, dst->format, len);
		}
		slrect.w -= total;
		slrect.x += total;
	}
	//printf("3 %d %d %d %d\n", slrect.x, slrect.y, slrect.w, slrect.h);
	/* right */
	if (slrect.w)
	{
		//printf("right %d\n", slrect.w);
		spfnc(&ddata, slrect.w, &sdata, /* mul_color */0, NULL);
	}
}

static Eina_Bool _draw(Enesim_Renderer *r, int type, void *sl, Enesim_Surface *dst)
{
	if (type == ENESIM_SCANLINE_ALIAS)
		_draw_alias(r, sl, dst);
	
	return EINA_TRUE;
}

static void _free(Enesim_Renderer *r)
{
	free(r->data);
}

static Enesim_Renderer_Func f_func = {
	.draw 	= _draw,
	.free 	= _free,
};
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer * enesim_renderer_surface_new(void)
{
	Renderer_Surface *f;
	Enesim_Renderer *r;

	f = calloc(1, sizeof(Renderer_Surface));
	
	r = enesim_renderer_new();
	r->data = f;
	r->funcs = &f_func;

	return r;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_surface_surface_set(Enesim_Renderer *r, Enesim_Surface *s)
{
	Renderer_Surface *f;

	assert(r);
	assert(s);
	f = r->data;
	f->s = s;
	/* TODO check the dst area boundings and pick the min */
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_surface_mode_set(Enesim_Renderer *r, int mode)
{
	Renderer_Surface *f;
	
	assert(r);
	f = r->data;
	f->mode = mode;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_surface_dst_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Renderer_Surface *f;
	
	assert(r);
	f = r->data;
	assert(f);
	
	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_coords_from(&f->darea, x, y, w, h);
	/* TODO check the dst surface boundings and pick the min */
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_surface_src_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Renderer_Surface *f;
	
	assert(r);
	f = r->data;
	assert(f);
	
	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_coords_from(&f->sarea, x, y, w, h);
}
