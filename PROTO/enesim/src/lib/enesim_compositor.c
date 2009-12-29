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
 * TODO add a surface compositor too, like point (0D), span (1D) but a 2D one :)
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*
 * A drawer should implement functions for every format in case of using
 * pixel source. For color source it should implement the function with
 * opaque value and no opaque.
 */
typedef struct _Enesim_Compositor
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
} Enesim_Compositor;

static Enesim_Compositor _comps;

/* FIXME all this getters still need a way to *set* the dfmt */
/* point functions */
static Enesim_Compositor_Point _point_color_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Color color)
{
	/* TODO check if the color is opaque */
	if ((rop == ENESIM_BLEND) && (color & 0xff000000))
	{
		rop = ENESIM_FILL;
	}
	return _comps.pt_color[rop][*dfmt];
}

static Enesim_Compositor_Point _point_mask_color_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Color color, Enesim_Format mfmt)
{
	return _comps.pt_mask_color[rop][*dfmt][mfmt];
}

static Enesim_Compositor_Point _point_pixel_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt)
{
	return _comps.pt_pixel[rop][*dfmt][sfmt];
}

static Enesim_Compositor_Point _point_pixel_color_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt,
		Enesim_Color color)
{
	if (color == ENESIM_COLOR_FULL)
		return _comps.pt_pixel[rop][*dfmt][sfmt];
	else
		return _comps.pt_pixel_color[rop][*dfmt][sfmt];
}

static Enesim_Compositor_Point _point_pixel_mask_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt, Enesim_Format mfmt)
{
	return _comps.pt_pixel_mask[rop][*dfmt][sfmt][mfmt];
}

/* Span functions */

/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with color color
 */
static Enesim_Compositor_Span _span_color_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Color color)
{
	/* TODO check if the color is opaque */
	if ((rop == ENESIM_BLEND) && (color & 0xff000000))
	{
		rop = ENESIM_FILL;
	}
	return _comps.sp_color[rop][*dfmt];
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with alpha values from the mask
 * and multiplying with color color
 */
static Enesim_Compositor_Span _span_mask_color_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format mfmt, Enesim_Color color)
{
	return _comps.sp_mask_color[rop][*dfmt][mfmt];
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with pixels of format sfmt
 */
static Enesim_Compositor_Span _span_pixel_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt)
{
	return _comps.sp_pixel[rop][*dfmt][sfmt];
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with pixels of format sfmt
 * multypling with color color
 */
static Enesim_Compositor_Span _span_pixel_color_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt, Enesim_Color color)
{
	if (color == ENESIM_COLOR_FULL)
		return _comps.sp_pixel[rop][*dfmt][sfmt];
	else
		return _comps.sp_pixel_color[rop][*dfmt][sfmt];
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with alpha values from the mask
 * and multiplying with the pixel values from sfmt
 * @param
 * @param
 */
static Enesim_Compositor_Span _span_pixel_mask_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt, Enesim_Format mfmt)
{
	return _comps.sp_pixel_mask[rop][*dfmt][sfmt][mfmt];
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_compositor_init(void)
{
	enesim_compositor_argb8888_init();
}
void enesim_compositor_shutdown(void)
{

}

void enesim_compositor_span_color_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt)
{
	_comps.sp_color[rop][dfmt] = sp;
}

void enesim_compositor_span_pixel_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt)
{
	_comps.sp_pixel[rop][dfmt][sfmt] = sp;
}

void enesim_compositor_span_mask_color_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format mfmt)
{
	_comps.sp_mask_color[rop][dfmt][mfmt] = sp;
}

void enesim_compositor_span_pixel_mask_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt,
		Enesim_Format mfmt)
{
	_comps.sp_pixel_mask[rop][dfmt][sfmt][mfmt] = sp;
}

void enesim_compositor_span_pixel_color_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt)
{
	_comps.sp_pixel_color[rop][dfmt][sfmt] = sp;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Compositor_Span enesim_compositor_span_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt, Enesim_Color color,
		Enesim_Format mfmt)
{
	if (!dfmt)
		return NULL;
	if (*dfmt <= ENESIM_FORMAT_NONE && *dfmt > ENESIM_FORMATS)
		return NULL;

	if (!sfmt && !mfmt)
	{
		return _span_color_get(rop, dfmt, color);
	}
	if (sfmt && mfmt)
	{
		return _span_pixel_mask_get(rop, dfmt, sfmt, mfmt);
	}
	if (sfmt)
	{
		return _span_pixel_color_get(rop, dfmt, sfmt, color);
	}
	if (mfmt)
	{
		return _span_mask_color_get(rop, dfmt, mfmt, color);
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Compositor_Point enesim_compositor_point_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt, Enesim_Color color,
		Enesim_Format mfmt)
{
	if (!dfmt)
		return NULL;
	if (*dfmt <= ENESIM_FORMAT_NONE && *dfmt > ENESIM_FORMATS)
		return NULL;

	if (!sfmt && !mfmt)
	{
		return _point_color_get(rop, dfmt, color);
	}
	if (sfmt && mfmt)
	{
		return _point_pixel_mask_get(rop, dfmt, sfmt, mfmt);
	}
	if (sfmt)
	{
		return _point_pixel_color_get(rop, dfmt, sfmt, color);
	}
	if (mfmt)
	{
		return _point_mask_color_get(rop, dfmt, mfmt, color);
	}
}
