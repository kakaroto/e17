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
 * TODO add a surface compositor too, like point (0D), span (1D) but a 2D one :)
 *
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*
 * A drawer should implement functions for every format in case of using
 * pixel source. For color source it should implement the function with
 * opaque value and no opaque.
 */
typedef struct _Enesim_Drawer
{
	/* Scanlines */
	Enesim_Compositor_Span sp_color[ENESIM_ROPS][ENESIM_FORMATS];
	Enesim_Compositor_Span sp_mask_color[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS];
	Enesim_Compositor_Span sp_pixel[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS];
	Enesim_Compositor_Span sp_pixel_color[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS];
	Enesim_Compositor_Span sp_pixel_mask[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS][ENESIM_FORMATS];
	/* Points */
	Enesim_Compositor_Point pt_color[ENESIM_ROPS][ENESIM_FORMATS];
	Enesim_Compositor_Point pt_mask_color[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS];
	Enesim_Compositor_Point pt_pixel[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS];
	Enesim_Compositor_Point pt_pixel_color[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS];
	Enesim_Compositor_Point pt_pixel_mask[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS][ENESIM_FORMATS];
} Enesim_Drawer;

static Enesim_Drawer _comps;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_compositor_init(void)
{
	//memset(&_comps, 0, sizeof(Enesim_Drawer));
	enesim_compositor_argb8888_init();
}
void enesim_compositor_shutdown(void)
{

}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Compositor_Point enesim_compositor_point_color_get(Enesim_Rop rop,
		Enesim_Format dfmt, Enesim_Color color)
{
	/* TODO check if the color is opaque */
	if ((rop == ENESIM_BLEND) && (color & 0xff000000))
	{
		rop = ENESIM_FILL;
	}
	return _comps.pt_color[rop][dfmt];
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Compositor_Point enesim_compositor_point_mask_color_get(Enesim_Rop rop,
		Enesim_Format dfmt, Enesim_Color color, Enesim_Format mfmt)
{
	return _comps.pt_mask_color[rop][dfmt][mfmt];
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Compositor_Point enesim_compositor_point_pixel_get(Enesim_Rop rop,
		Enesim_Format dfmt, Enesim_Format sfmt)
{
	return _comps.pt_pixel[rop][dfmt][sfmt];
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Compositor_Point enesim_compositor_point_pixel_color_get(Enesim_Rop rop,
		Enesim_Format dfmt, Enesim_Format sfmt,
		Enesim_Color color)
{
	if (color == 0xffffffff)
		return _comps.pt_pixel[rop][dfmt][sfmt];
	else
		return _comps.pt_pixel_color[rop][dfmt][sfmt];
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Compositor_Point enesim_compositor_point_pixel_mask_get(Enesim_Rop rop,
		Enesim_Format dfmt, Enesim_Format sfmt, Enesim_Format mfmt)
{
	return _comps.pt_pixel_mask[rop][dfmt][sfmt][mfmt];
}

/* Span functions */

/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with color color
 */
EAPI Enesim_Compositor_Span enesim_compositor_span_color_get(Enesim_Rop rop,
		Enesim_Format dfmt, Enesim_Color color)
{
	/* TODO check if the color is opaque */
	if ((rop == ENESIM_BLEND) && (color & 0xff000000))
	{
		rop = ENESIM_FILL;
	}
	return _comps.sp_color[rop][dfmt];
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with alpha values from the mask
 * and multiplying with color color
 */
EAPI Enesim_Compositor_Span enesim_compositor_span_mask_color_get(Enesim_Rop rop,
		Enesim_Format dfmt, Enesim_Format mfmt, Enesim_Color color)
{
	return _comps.sp_mask_color[rop][dfmt][mfmt];
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with pixels of format sfmt
 */
EAPI Enesim_Compositor_Span enesim_compositor_span_pixel_get(Enesim_Rop rop,
		Enesim_Format dfmt, Enesim_Format sfmt)
{
	return _comps.sp_pixel[rop][dfmt][sfmt];
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with pixels of format sfmt
 * multypling with color color
 */
EAPI Enesim_Compositor_Span enesim_compositor_span_pixel_color_get(Enesim_Rop rop,
		Enesim_Format dfmt, Enesim_Format sfmt, Enesim_Color color)
{
	if (color == 0xffffffff)
		return _comps.sp_pixel[rop][dfmt][sfmt];
	else
		return _comps.sp_pixel_color[rop][dfmt][sfmt];
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with alpha values from the mask
 * and multiplying with the pixel values from sfmt
 * @param
 * @param
 */
EAPI Enesim_Compositor_Span enesim_compositor_span_pixel_mask_get(Enesim_Rop rop,
		Enesim_Format dfmt, Enesim_Format sfmt, Enesim_Format mfmt)
{
	return _comps.sp_pixel_mask[rop][dfmt][sfmt][mfmt];
}

EAPI void enesim_compositor_span_color_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt)
{
	_comps.sp_color[rop][dfmt] = sp;
}
EAPI void enesim_compositor_span_pixel_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt)
{
	_comps.sp_pixel[rop][dfmt][sfmt] = sp;
}
EAPI void enesim_compositor_span_mask_color_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format mfmt)
{
	_comps.sp_mask_color[rop][dfmt][mfmt] = sp;
}
EAPI void enesim_compositor_span_pixel_mask_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt,
		Enesim_Format mfmt)
{
	_comps.sp_pixel_mask[rop][dfmt][sfmt][mfmt] = sp;
}

EAPI void enesim_compositor_span_pixel_color_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt)
{
	_comps.sp_pixel_color[rop][dfmt][sfmt] = sp;
}
