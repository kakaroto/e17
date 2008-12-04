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
typedef struct _Renderer_Color
{
#ifdef DEBUG
	unsigned int magic;
#endif
	Enesim_Surface_Pixel color;
} Renderer_Color;

static Eina_Bool _draw_alias(Enesim_Renderer *r, Enesim_Scanline_Alias *sl, Enesim_Surface *dst)
{
	Renderer_Color *f;
	Enesim_Drawer_Span cfnc;
	Enesim_Surface_Data ddata;
	Enesim_Surface_Format sfmt;
	int nsl;
	int offset;
	int i;

	f = r->data;
	
	sfmt = enesim_surface_format_get(dst);
	//cfnc = enesim_drawer_span_color_get(r->rop, sfmt, f->color);
	assert(cfnc);
	offset = (dst->w * sl->y) + sl->x;
	enesim_surface_data_get(dst, &ddata);
	enesim_surface_data_increment(&ddata, offset);
	cfnc(&ddata, sl->w, NULL, &(f->color), NULL);
	
	return EINA_TRUE;
}

static Eina_Bool _draw_mask(Enesim_Renderer *r, Enesim_Scanline_Mask *sl, Enesim_Surface *dst)
{
#if 0
	Renderer_Color *f;
	Span_Color_Mask_Func cfnc;
	int nsl;
	int offset;
	int i;

	f = r->data;
		
	cfnc = enesim_surface_span_color_mask_func_get(dst, r->rop);
	offset = (dst->w * sl->y) + sl->x;
	cfnc(&dst->data, offset, f->color, sl->w, sl->coverages);
#endif
	return EINA_TRUE;
}

static Eina_Bool _draw(Enesim_Renderer *r, int type, void *sl, Enesim_Surface *dst)
{
	Eina_Bool ret;
	
	ENESIM_MAGIC_CHECK(((Renderer_Color *)(r->data)), ENESIM_RENDERER_FILLCOLOR_MAGIC);
	if (type == ENESIM_SCANLINE_ALIAS)
		ret = _draw_alias(r, sl, dst);
	else if (type == ENESIM_SCANLINE_MASK)
		ret = _draw_mask(r, sl, dst);
	else
	{
//		ENESIM_ERROR(ENESIM_ERROR_HANDLE_INVALID);
	}
	return ret;
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
EAPI Enesim_Renderer * enesim_renderer_color_new(void)
{
	Renderer_Color *f;
	Enesim_Renderer *r;

	f = calloc(1, sizeof(Renderer_Color));
	
	r = enesim_renderer_new();
	r->data = f;
	r->funcs = &f_func;
	ENESIM_MAGIC_SET(f, ENESIM_RENDERER_FILLCOLOR_MAGIC);

	return r;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_color_color_set(Enesim_Renderer *r, uint32_t color)
{
	Renderer_Color *f;
	int i;
	
	i = ENESIM_RENDERER_MAGIC;

	ENESIM_ASSERT(r, ENESIM_ERROR_HANDLE_INVALID);
	
	ENESIM_MAGIC_CHECK(r, ENESIM_RENDERER_MAGIC);
	f = r->data;
	ENESIM_MAGIC_CHECK(f, ENESIM_RENDERER_FILLCOLOR_MAGIC);
	//f->color = color;
}
