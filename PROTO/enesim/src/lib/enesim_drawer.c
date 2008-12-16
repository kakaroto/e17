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
 * TODO
 * rop functions 
 * pixel
 * color in ARGB format
 * pixel_color
 * mask_color
 * pixel_mask
 * TODO
 * color *done*
 * mask_color 
 * pixel *done*
 * pixel_color
 * pixel_mask
 * 
 */
/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
/* this is the main surface format drawer */
extern Enesim_Drawer argb8888_drawer;
extern Enesim_Drawer_Generic generic_drawer;

Enesim_Drawer *drawer[ENESIM_SURFACE_FORMATS] = {
		[ENESIM_SURFACE_ARGB8888] = &argb8888_drawer,
};
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Pixel *src,
		Enesim_Surface_Pixel *color, Enesim_Surface_Pixel *mask)
{
	if (src)
	{
		if (mask)
			return enesim_drawer_point_pixel_mask_get(rop, dfmt, src, mask);
		else if (color)
			return enesim_drawer_point_pixel_color_get(rop, dfmt, src, color);
		else
			return enesim_drawer_point_pixel_get(rop, dfmt, src);
	}
	else
	{
		if (color)
			return enesim_drawer_point_mask_color_get(rop, dfmt, color, mask);
		else
			return enesim_drawer_point_color_get(rop, dfmt, color);
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_color_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Pixel *color)
{
	Enesim_Drawer_Point pt = NULL;
	/* TODO check if the color is opaque */
#if 0
	if ((rop == ENESIM_BLEND) && (alpha(color) == 0xff))
	{
		rop = RENESIM_FILL;
	}
#endif
	if (drawer[dfmt])
	{
		pt = drawer[dfmt]->pt_color[rop]; 
	}
	if (!pt)
		pt = generic_drawer.pt_color[rop]; 
	return pt;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_pixel_color_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Pixel *src,
		Enesim_Surface_Pixel *color)
{
	Enesim_Drawer_Point pt = NULL;

	if (drawer[dfmt])
		pt = drawer[dfmt]->pt_pixel_color[rop][src->format];
	if (!pt)
		pt = generic_drawer.pt_pixel_color[rop]; 
	return pt;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_mask_color_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *mask)
{
	Enesim_Drawer_Point pt = NULL;

	if (drawer[dfmt])
		pt = drawer[dfmt]->pt_mask_color[rop][mask->format];
	if (!pt)
		pt = generic_drawer.pt_mask_color[rop]; 
	return pt;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_pixel_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt,
		Enesim_Surface_Pixel *src)
{
	Enesim_Drawer_Point pt = NULL;

	if (drawer[dfmt])
		pt = drawer[dfmt]->pt_pixel[rop][src->format];
	if (!pt)
		pt = generic_drawer.pt_pixel[rop]; 
	return pt;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_pixel_mask_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Pixel *src,
		Enesim_Surface_Pixel *mask)
{
	Enesim_Drawer_Point pt = NULL;
	
	if (drawer[dfmt])
		pt = drawer[dfmt]->pt_pixel_mask[rop][src->format][mask->format];
	if (!pt)
		pt = generic_drawer.pt_pixel_mask[rop]; 
	return pt;
}

/* Span functions */

/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with color color
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_color_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Pixel *color)
{
	Enesim_Drawer_Span sp = NULL;
	/* TODO check if the color is opaque */
	if (drawer[dfmt])
		sp = drawer[dfmt]->sp_color[rop];
	if (!sp)
		sp = generic_drawer.sp_color[rop]; 
	return sp;
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with alpha values from the mask
 * and multiplying with color color
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_mask_color_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Format mfmt,
		Enesim_Surface_Pixel *color)
{
	Enesim_Drawer_Span sp = NULL;
	/* TODO check if the color is opaque */
	if (drawer[dfmt])
		sp = drawer[dfmt]->sp_mask_color[rop][mfmt];
	if (!sp)
		sp = generic_drawer.sp_mask_color[rop]; 
	return sp;
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with pixels of format sfmt
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_pixel_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Format sfmt)
{
	Enesim_Drawer_Span sp = NULL;

	if (drawer[dfmt])
		sp = drawer[dfmt]->sp_pixel[rop][sfmt];
	if (!sp)
		sp = generic_drawer.sp_pixel[rop]; 
	return sp;
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with pixels of format sfmt
 * multypling with color color
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_pixel_color_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Format sfmt,
		Enesim_Surface_Pixel *color)
{
	Enesim_Drawer_Span sp = NULL;
	/* FIXME if the surface is alpha only, use the mask_color */
	/* TODO check if the color is opaque */
	if (drawer[dfmt])
		sp = drawer[dfmt]->sp_pixel_color[rop][sfmt];
	if (!sp)
		sp = generic_drawer.sp_pixel_color[rop]; 
	return sp;
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with alpha values from the mask
 * and multiplying with the pixel values from sfmt
 * @param
 * @param
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_pixel_mask_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Format sfmt,
		Enesim_Surface_Format mfmt)
{
	Enesim_Drawer_Span sp = NULL;
	if (drawer[dfmt])
		sp = drawer[dfmt]->sp_pixel_mask[rop][sfmt][mfmt];
	if (!sp)
		sp = generic_drawer.sp_pixel_mask[rop]; 
	return sp;
}
