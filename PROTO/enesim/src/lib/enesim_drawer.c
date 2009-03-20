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
typedef struct _Enesim_Drawer_Span_Container
{
	EINA_INLIST;
	Enesim_Drawer_Span func;
	Enesim_Format *dfmt;
	Enesim_Format *sfmt;
	Enesim_Format *mfmt;
	Eina_Bool color;
} Enesim_Drawer_Span_Container;

typedef struct _Enesim_Drawer_Point_Container
{
	EINA_INLIST;
	Enesim_Drawer_Point func;
	Enesim_Format *dfmt;
	Enesim_Format *sfmt;
	Enesim_Format *mfmt;
	Eina_Bool color;
} Enesim_Drawer_Point_Container;

static Eina_Inlist *_pt[ENESIM_ROPS] = { NULL };
static Eina_Inlist *_sp[ENESIM_ROPS] = { NULL };

static Enesim_Drawer_Point_Container *_drawer_point_get(Enesim_Rop rop, Enesim_Format *dfmt,
		Enesim_Format *sfmt, Eina_Bool color, Enesim_Format *mfmt)
{
	Enesim_Drawer_Point_Container *ctn;

	EINA_INLIST_FOREACH(_pt[rop], ctn)
	{
		if ((ctn->dfmt == dfmt) && (ctn->sfmt == sfmt) &&
				(ctn->mfmt == mfmt) && (ctn->color == color))
		{
			return ctn;
		}
	}
	return NULL;
}
static Enesim_Drawer_Span_Container *_drawer_span_get(Enesim_Rop rop, Enesim_Format *dfmt,
		Enesim_Format *sfmt, Eina_Bool color, Enesim_Format *mfmt)
{
	Enesim_Drawer_Span_Container *ctn;

	EINA_INLIST_FOREACH(_sp[rop], ctn)
	{
		if ((ctn->dfmt == dfmt) && (ctn->sfmt == sfmt) &&
				(ctn->mfmt == mfmt) && (ctn->color == color))
		{
			return ctn;
		}
	}
	return NULL;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_drawer_init(void)
{
	/* first the generic drawer */
	enesim_drawer_generic_init();
	enesim_drawer_argb8888_init();

}
void enesim_drawer_shutdown(void)
{
	/* last the generic drawer */
	enesim_drawer_generic_shutdown();
	enesim_drawer_argb8888_shutdown();
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Surface_Pixel *src,
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
		if (mask)
			return enesim_drawer_point_mask_color_get(rop, dfmt, color, mask);
		else
			return enesim_drawer_point_color_get(rop, dfmt, color);
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Surface *src,
		Enesim_Surface_Pixel *color, Enesim_Surface *mask)
{
	if (src)
	{
		Enesim_Format *sfmt;

		sfmt = enesim_surface_format_get(src);
		if (mask)
		{
			Enesim_Format *mfmt;

			mfmt = enesim_surface_format_get(mask);
			return enesim_drawer_span_pixel_mask_get(rop, dfmt, sfmt, mfmt);
		}
		else if (color)
			return enesim_drawer_span_pixel_color_get(rop, dfmt, sfmt, color);
		else
			return enesim_drawer_span_pixel_get(rop, dfmt, sfmt);
	}
	else
	{
		if (mask)
		{
			Enesim_Format *mfmt;

			mfmt = enesim_surface_format_get(mask);
			return enesim_drawer_span_mask_color_get(rop, dfmt, mfmt, color);
		}
		else
			return enesim_drawer_span_color_get(rop, dfmt, color);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_color_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Surface_Pixel *color)
{
	Enesim_Drawer_Point_Container *cnt;
	/* TODO check if the color is opaque */
#if 0
	if ((rop == ENESIM_BLEND) && (alpha(color) == 0xff))
	{
		rop = RENESIM_FILL;
	}
#endif
	cnt = _drawer_point_get(rop, dfmt, NULL, EINA_TRUE, NULL);
	if (cnt)
		return cnt->func;
	else
		return NULL;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_pixel_color_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Surface_Pixel *src,
		Enesim_Surface_Pixel *color)
{
	Enesim_Drawer_Point_Container *cnt;
	cnt = _drawer_point_get(rop, dfmt, src->format, EINA_TRUE, NULL);
	if (cnt)
		return cnt->func;
	else
		return NULL;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_mask_color_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *mask)
{
	Enesim_Drawer_Point_Container *cnt;
	cnt = _drawer_point_get(rop, dfmt, NULL, EINA_TRUE, mask->format);
	if (cnt)
		return cnt->func;
	else
		return NULL;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_pixel_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Surface_Pixel *src)
{
	Enesim_Drawer_Point_Container *cnt;
	cnt = _drawer_point_get(rop, dfmt, src->format, EINA_FALSE, NULL);
	if (cnt)
		return cnt->func;
	else
		return NULL;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_pixel_mask_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Surface_Pixel *src,
		Enesim_Surface_Pixel *mask)
{
	Enesim_Drawer_Point_Container *cnt;

	cnt = _drawer_point_get(rop, dfmt, src->format, EINA_FALSE, mask->format);
	if (cnt)
		return cnt->func;
	else
		return NULL;
}

/* Span functions */

/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with color color
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_color_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Surface_Pixel *color)
{
	Enesim_Drawer_Span_Container *cnt;
	/* TODO check if the color is opaque */
	cnt = _drawer_span_get(rop, dfmt, NULL, EINA_TRUE, NULL);
	if (cnt)
		return cnt->func;
	else
		return NULL;
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with alpha values from the mask
 * and multiplying with color color
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_mask_color_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format *mfmt,
		Enesim_Surface_Pixel *color)
{
	Enesim_Drawer_Span_Container *cnt;
	/* TODO check if the color is opaque */
	cnt = _drawer_span_get(rop, dfmt, NULL, EINA_TRUE, mfmt);
	if (cnt)
		return cnt->func;
	else
		return NULL;
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with pixels of format sfmt
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_pixel_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format *sfmt)
{
	Enesim_Drawer_Span_Container *cnt;
	cnt = _drawer_span_get(rop, dfmt, sfmt, EINA_FALSE, NULL);
	if (cnt)
		return cnt->func;
	else
		return NULL;
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with pixels of format sfmt
 * multypling with color color
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_pixel_color_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format *sfmt,
		Enesim_Surface_Pixel *color)
{
	Enesim_Drawer_Span_Container *cnt;
	/* FIXME if the surface is alpha only, use the mask_color */
	/* TODO check if the color is opaque */
	cnt = _drawer_span_get(rop, dfmt, sfmt, EINA_TRUE, NULL);
	if (cnt)
		return cnt->func;
	else
		return NULL;
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with alpha values from the mask
 * and multiplying with the pixel values from sfmt
 * @param
 * @param
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_pixel_mask_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format *sfmt,
		Enesim_Format *mfmt)
{
	Enesim_Drawer_Span_Container *cnt;
	cnt = _drawer_span_get(rop, dfmt, sfmt, EINA_FALSE, mfmt);
	if (cnt)
		return cnt->func;
	else
		return NULL;
}


EAPI Eina_Bool enesim_drawer_point_register(Enesim_Rop rop, Enesim_Drawer_Point pt,
		Enesim_Format *dfmt, Enesim_Format *sfmt, Eina_Bool color, Enesim_Format *mfmt)
{
	Enesim_Drawer_Point_Container *cnt;

	if (!pt || !dfmt)
		return EINA_FALSE;

	/* Replace the drawer if it already exists */
	cnt = _drawer_point_get(rop, dfmt, sfmt, color, mfmt);
	if (cnt)
	{
		cnt->func = pt;
	}
	else
	{
		cnt = calloc(1, sizeof(Enesim_Drawer_Point_Container));
		cnt->color = color;
		cnt->dfmt = dfmt;
		cnt->sfmt = sfmt;
		cnt->mfmt = mfmt;
		cnt->func = pt;

		_pt[rop] = eina_inlist_append(_pt[rop], EINA_INLIST_GET(cnt));
	}
	return EINA_TRUE;
}
EAPI Eina_Bool enesim_drawer_span_register(Enesim_Rop rop, Enesim_Drawer_Span sp,
		Enesim_Format *dfmt, Enesim_Format *sfmt, Eina_Bool color, Enesim_Format *mfmt)
{
	Enesim_Drawer_Span_Container *cnt;
	if (!sp || !dfmt)
		return EINA_FALSE;
	/* Replace the drawer if it already exists */
	cnt = _drawer_span_get(rop, dfmt, sfmt, color, mfmt);
	if (cnt)
	{
		cnt->func = sp;
	}
	else
	{
		cnt = calloc(1, sizeof(Enesim_Drawer_Point_Container));
		cnt->color = color;
		cnt->dfmt = dfmt;
		cnt->sfmt = sfmt;
		cnt->mfmt = mfmt;
		cnt->func = sp;

		EINA_ERROR_PINFO("Span drawer registered %s %s %s %s\n",
				enesim_format_name_get(dfmt),
				sfmt ? enesim_format_name_get(sfmt) : "none",
				color ? "true" : "false",
				mfmt ? enesim_format_name_get(mfmt) : "none");
		_sp[rop] = eina_inlist_append(_sp[rop], EINA_INLIST_GET(cnt));
	}
	return EINA_TRUE;
}
