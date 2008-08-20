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
/* 
 * 
 * rop functions 
 * pixel
 * color in ARGB format
 * pixel_color
 * mask_color
 * pixel_mask
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
		[ENESIM_SURFACE_RGB565_XA5] = &rgb565_xa5_drawer,
#else
		[ENESIM_SURFACE_RGB565_XA5] = &_unbuilt,
#endif
#ifdef BUILD_SURFACE_RGB565_B1A3
		[ENESIM_SURFACE_RGB565_B1A3] = &rgb565_b1a3_drawer,
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
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
void enesim_drawer_pt_unbuilt(Enesim_Surface_Data *d, Enesim_Surface_Data *s,
		unsigned int color, unsigned char *mask)
{
	//EINA_ERROR_PWARN("Point function not supported\n");
}
void enesim_drawer_span_unbuilt(Enesim_Surface_Data *d, unsigned int len,
		Enesim_Surface_Data *s, unsigned int color, unsigned char *mask)
{
	//EINA_ERROR_PWARN("Span function not supported\n");
}

void enesim_drawer_init(void)
{
	int i;
	int j;

	memset(&_unbuilt, 0, sizeof(Enesim_Drawer));
#if 0
	for (i = 0; i < ENESIM_ROPS; i++)
	{
		int j;
		
		/* sp_color, pt_color */
		_unbuilt.sp_color[i] = &enesim_drawer_span_unbuilt;
		_unbuilt.pt_color[i] = &enesim_drawer_pt_unbuilt;
		/* sp_pixel, sp_pixel_mask, pt_pixel, pt_pixel_mask */
		for (j = 0; j < ENESIM_SURFACE_FORMATS; j++)
		{
			int k;
			
			_unbuilt.sp_pixel[i][j] = &enesim_drawer_span_unbuilt;
			_unbuilt.sp_pixel_mask[i][j] = &enesim_drawer_span_unbuilt;
			_unbuilt.pt_pixel_mask[i][j] = &enesim_drawer_pt_unbuilt;
			_unbuilt.pt_pixel_mask[i][j] = &enesim_drawer_pt_unbuilt;
			/* sp_pixel_color, pt_pixel_color */
			for (k = 0; k < COLOR_TYPES; k++)
			{
				_unbuilt.pt_pixel_color[i][j][k] = enesim_drawer_pt_unbuilt;
				_unbuilt.sp_pixel_color[i][j][k] = enesim_drawer_span_unbuilt;
			}
		}
		/* sp_mask_color, pt_mask_color */
		for (j = 0; j < COLOR_TYPES; j++)
		{
			_unbuilt.pt_mask_color[i][j] = &enesim_drawer_pt_unbuilt;
			_unbuilt.sp_mask_color[i][j] = &enesim_drawer_span_unbuilt;
		}
	}
#endif
}

void enesim_drawer_shutdown(void)
{
	/* do nothing */
}
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * 
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_color_get(Enesim_Rop rop, Enesim_Surface_Format dfmt, unsigned int color)
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
 * 
 */
EAPI Enesim_Drawer_Point enesim_drawer_point_pixel_get(Enesim_Rop rop, Enesim_Surface_Format dfmt, Enesim_Surface_Format sfmt)
{
	return drawer[dfmt]->pt_pixel[rop][sfmt];
}

/* Span functions */

/**
 * Do we need to pass the length here?
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_color_get(Enesim_Rop rop, Enesim_Surface_Format dfmt, unsigned int color)
{
	/* TODO check if the color is opaque */
	return drawer[dfmt]->sp_color[rop];
}

/**
 * 
 */
EAPI Enesim_Drawer_Span enesim_drawer_span_pixel_get(Enesim_Rop rop, Enesim_Surface_Format dfmt, Enesim_Surface_Format sfmt)
{
	/* TODO check if the color is opaque */
	return drawer[dfmt]->sp_pixel[rop][sfmt];
}

//EAPI Enesim_Drawer_Color enesim_drawer_pt_color_get(Enesim_Rop rop)
