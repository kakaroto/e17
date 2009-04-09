/*
 * rect.c
 *
 *  Created on: 12-feb-2009
 *      Author: jl
 */
#include "Enesim.h"
#include "enesim_private.h"
/* TODO optimize this to be argb8888 specific */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void enesim_rect_draw(Enesim_Surface *s, Enesim_Context *c,
		uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
	int i;
	int sw;
	Eina_Rectangle r;
	Enesim_Surface_Data dtmp;
	Enesim_Drawer_Span span;
	Enesim_Surface_Pixel pixel;
	Enesim_Format sfmt;

	sfmt = enesim_surface_format_get(s);
	enesim_surface_size_get(s, &sw, NULL);
	/* convert the argb into a pixel */
	enesim_surface_pixel_argb_from(&pixel, sfmt, c->color);
	span = enesim_drawer_span_get(c->rop, sfmt, c->src, &pixel, c->mask);
	/* get the span function */
	eina_rectangle_coords_from(&r, x, y, w, h);
	/* TODO handle the clipping */
	if (c->clip.used)
	{
		eina_rectangle_intersection(&r, &c->clip.r);
	}
	if (eina_rectangle_is_empty(&r))
		return;
	enesim_surface_data_get(s, &dtmp);
	enesim_surface_data_increment(&dtmp, (y * sw) + x);
	for (i = 0; i < h; i++)
	{
		span(&dtmp, w, NULL, &pixel, NULL);
		enesim_surface_data_increment(&dtmp, sw);
		/* TODO handle the mask incrementation and the src incrementation */
	}
}
