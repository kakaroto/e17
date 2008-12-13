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
Enesim_Drawer _unbuilt;

/* this is the main surface format drawer */
extern Enesim_Drawer argb8888_drawer;

#ifdef BUILD_SURFACE_ARGB8888_UNPRE
extern Enesim_Drawer argb8888_unpre_drawer;
#endif

#ifdef BUILD_SURFACE_RGB565_XA5
extern Enesim_Drawer rgb565_xa5_drawer;
#endif

#ifdef BUILD_SURFACE_RGB565_B1A3
extern Enesim_Drawer rgb565_b1a3_drawer;
#endif

Enesim_Drawer *drawer[ENESIM_SURFACE_FORMATS] = {
		[ENESIM_SURFACE_ARGB8888] = &argb8888_drawer,
#ifdef BUILD_SURFACE_ARGB8888_UNPRE
		[ENESIM_SURFACE_ARGB8888_UNPRE] = &argb8888_unpre_drawer,
#else
		[ENESIM_SURFACE_ARGB8888_UNPRE] = &_unbuilt,
#endif
#ifdef BUILD_SURFACE_RGB565_XA5
		[ENESIM_SURFACE_RGB565_XA5] = &_unbuilt, //&rgb565_xa5_drawer,
#else
		[ENESIM_SURFACE_RGB565_XA5] = &_unbuilt,
#endif
#ifdef BUILD_SURFACE_RGB565_B1A3
		[ENESIM_SURFACE_RGB565_B1A3] = &_unbuilt, //&rgb565_b1a3_drawer,
#else
		[ENESIM_SURFACE_RGB565_B1A3] = &_unbuilt,
#endif
#ifdef BUILD_SURFACE_RGB888_A8
#else
		[ENESIM_SURFACE_RGB888_A8] = &_unbuilt,
#endif
#ifdef BUILD_SURFACE_A8
#else
		[ENESIM_SURFACE_A8] = &_unbuilt,
#endif
#ifdef BUILD_SURFACE_B1A3
#else
		[ENESIM_SURFACE_b1A3] = &_unbuilt,
#endif
};

void _pixel_blend(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	switch (d->format)
	{
	case ENESIM_SURFACE_ARGB8888:
		argb8888_pixel_blend(d, p);
		break;
#ifdef BUILD_SURFACE_ARGB8888_UNPRE
	case ENESIM_SURFACE_ARGB8888_UNPRE:
		argb8888_unpre_pixel_blend(d, p);
		break;
#endif
#ifdef BUILD_SURFACE_RGB565_XA5
	case ENESIM_SURFACE_RGB565_XA5:
		break;
#endif
#ifdef BUILD_SURFACE_RGB565_B1A3
	case ENESIM_SURFACE_RGB565_B1A3:
		break;
#endif
	default:
		break;
	}
}

void _pixel_fill(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	switch (d->format)
	{
	case ENESIM_SURFACE_ARGB8888:
		argb8888_pixel_fill(d, p);
		break;
#ifdef BUILD_SURFACE_ARGB8888_UNPRE
	case ENESIM_SURFACE_ARGB8888_UNPRE:
		argb8888_unpre_pixel_fill(d, p);
		break;
#endif
#ifdef BUILD_SURFACE_RGB565_XA5
	case ENESIM_SURFACE_RGB565_XA5:
		break;
#endif
#ifdef BUILD_SURFACE_RGB565_B1A3
	case ENESIM_SURFACE_RGB565_B1A3:
		break;
#endif
	default:
		break;
	}
}
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
/*
 * TODO
 * all the above generic functions should convert the src, dst and mask
 * to argb8888 (internal format) and handle the real operation in that
 * format. This will incredible slow, but works as a generic way
 * to handle this, any surface format implementor should just
 * create the converters from/to argb8888 in case it wants a accelerated drawer
 * he must implement a specifc drawer
 */
void enesim_drawer_pt_color_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	_pixel_blend(d, color);
}
void enesim_drawer_pt_pixel_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	_pixel_blend(d, color);
}

void enesim_drawer_sp_color_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data dtmp = *d;
	int i;
	
	for (i = 0; i < len; i++)
	{
		_pixel_blend(&dtmp, color);
		enesim_surface_data_increment(&dtmp, 1);
	}
}
void enesim_drawer_sp_pixel_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data dtmp = *d, stmp = *s;
	int i;
	
	for (i = 0; i < len; i++)
	{
		Enesim_Surface_Pixel p;
		uint32_t argb;
		
		argb = enesim_surface_data_argb_to(&stmp);
		enesim_surface_pixel_argb_from(&p, dtmp.format, argb);
		_pixel_blend(&dtmp, &p);
		enesim_surface_data_increment(&dtmp, 1);
		enesim_surface_data_increment(&stmp, 1);
	}
}
void enesim_drawer_pt_color_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	_pixel_fill(d, color);
}
void enesim_drawer_pt_pixel_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	Enesim_Surface_Pixel p;

	enesim_surface_pixel_convert(s, &p, d->format);
	_pixel_fill(d, &p);
}

void enesim_drawer_sp_color_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data dtmp = *d;
	int i;
	
	for (i = 0; i < len; i++)
	{
		_pixel_fill(&dtmp, color);
		enesim_surface_data_increment(&dtmp, 1);
	}
}
void enesim_drawer_sp_pixel_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data dtmp = *d, stmp = *s;
	int i;

	for (i = 0; i < len; i++)
	{
		Enesim_Surface_Pixel p;
		uint32_t argb;

		argb = enesim_surface_data_argb_to(&stmp);
		enesim_surface_pixel_argb_from(&p, dtmp.format, argb);
		_pixel_fill(&dtmp, &p);
		enesim_surface_data_increment(&dtmp, 1);
		enesim_surface_data_increment(&stmp, 1);
	}
}
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
	/* TODO check if the color is opaque */
#if 0
	if ((rop == ENESIM_BLEND) && (alpha(color) == 0xff))
	{
		rop = RENESIM_FILL;
	}
#endif
	return drawer[dfmt]->pt_color[rop];
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_pixel_color_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Pixel *src,
		Enesim_Surface_Pixel *color)
{
	return drawer[dfmt]->pt_pixel_color[rop][src->format];
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_mask_color_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *mask)
{
	return drawer[dfmt]->pt_mask_color[rop][mask->format];
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_pixel_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt,
		Enesim_Surface_Pixel *src)
{
	return drawer[dfmt]->pt_pixel[rop][src->format];
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_pixel_mask_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Pixel *src,
		Enesim_Surface_Pixel *mask)
{
	return drawer[dfmt]->pt_pixel_mask[rop][src->format][mask->format];
}

/* Span functions */

/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with color color
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_color_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Pixel *color)
{
	/* TODO check if the color is opaque */
	return drawer[dfmt]->sp_color[rop];
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
	/* TODO check if the color is opaque */
	return drawer[dfmt]->sp_mask_color[rop][mfmt];
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with pixels of format sfmt
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_pixel_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Format sfmt)
{
	return drawer[dfmt]->sp_pixel[rop][sfmt];
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with pixels of format sfmt
 * multypling with color color
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_pixel_color_get(Enesim_Rop rop,
		Enesim_Surface_Format dfmt, Enesim_Surface_Format sfmt)
{
	/* FIXME if the surface is alpha only, use the mask_color */
	/* TODO check if the color is opaque */
	return drawer[dfmt]->sp_pixel_color[rop][sfmt];
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
	return drawer[dfmt]->sp_pixel_mask[rop][sfmt][mfmt];
}
