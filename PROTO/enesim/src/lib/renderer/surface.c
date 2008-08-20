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

static inline void _draw_alias_sl(Renderer_Surface *f, Enesim_Scanline_Alias *sl, Enesim_Surface *dst)
{
	int offset;
	int w = DRECT.w;
	int h;

	if (!(f->mode & ENESIM_SURFACE_REPEAT_X))
	{
		w = MIN(SRECT.w, DRECT.w);
	}
	if (!(f->mode & ENESIM_SURFACE_REPEAT_Y))
	{
		h = MIN(SRECT.h, DRECT.h);
		offset = ((sl->y - DRECT.y) + SRECT.y) * f->s->w + SRECT.x;
	}
	else
	{
		h = DRECT.h;
		offset = (((sl->y - DRECT.y) % SRECT.h) + SRECT.y) * f->s->w + SRECT.x;
	}
	if (!enesim_rects_intersect(sl->x, sl->y, sl->w, 1, DRECT.x, DRECT.y, w,
			h))
	{
		// FIXME fill base color ?
	}
	if (f->mode & ENESIM_SURFACE_REPEAT_X)
	{
		
	}
	else
	{
		
	}
}

static inline void _draw_alias(Enesim_Renderer *r, Scanline_Alias *sl, Enesim_Surface *dst)
{
	Renderer_Surface *f;
	Scanline_Alias_Sl *s;
	int nsl;	
	int i;

	f = r->data;
	nsl = sl->num_sls;
	s = sl->sls;
	
	for (i = 0; i < nsl; i++)
	{
		_draw_alias_sl(f, s, dst);
		s++;
	}
}

static void _draw(Enesim_Renderer *r, Enesim_Scanline *sl, Enesim_Surface *dst)
{
	_draw_alias(r, sl->data, dst);
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
EAPI Enesim_Renderer * enesim_fill_surface_new(void)
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
	
	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_from_coords(&f->darea, x, y, w, h);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_surface_src_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Renderer_Surface *f;
	assert(r);
	
	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_from_coords(&f->sarea, x, y, w, h);
}
