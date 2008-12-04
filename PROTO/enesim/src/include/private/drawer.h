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
#ifndef DRAWER_H_
#define DRAWER_H_

/**
 * An ARGB color can be opaque or transparent, depending on the alpha value.
 * To optimize the functions we should define one for each case
 */
enum Color_Type
{
	COLOR_OPAQUE,
	COLOR_TRANSPARENT,
	COLOR_TYPES,
};

/**
 * Generic (and very slow) drawer functions.
 */
void enesim_drawer_pt_color_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m);
void enesim_drawer_pt_pixel_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m);
void enesim_drawer_sp_color_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m);
void enesim_drawer_sp_pixel_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m);
void enesim_drawer_pt_color_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m);
void enesim_drawer_pt_pixel_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m);
void enesim_drawer_sp_color_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m);
void enesim_drawer_sp_pixel_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m);
/* 
 * A drawer should implement functions for every format in case of using
 * pixel source. For color source it should implement the function with
 * opaque value and no opaque.
 */
typedef struct _Enesim_Drawer
{
	/* Scanlines */
	Enesim_Drawer_Span sp_color[ENESIM_ROPS];
	Enesim_Drawer_Span sp_mask_color[ENESIM_ROPS][COLOR_TYPES];
	Enesim_Drawer_Span sp_pixel[ENESIM_ROPS][ENESIM_SURFACE_FORMATS];
	Enesim_Drawer_Span sp_pixel_color[ENESIM_ROPS][ENESIM_SURFACE_FORMATS][COLOR_TYPES];	
	Enesim_Drawer_Span sp_pixel_mask[ENESIM_ROPS][ENESIM_SURFACE_FORMATS];
	/* Points */
	Enesim_Drawer_Point pt_color[ENESIM_ROPS];
	Enesim_Drawer_Point pt_mask_color[ENESIM_ROPS][COLOR_TYPES];
	Enesim_Drawer_Point pt_pixel[ENESIM_ROPS][ENESIM_SURFACE_FORMATS];
	Enesim_Drawer_Point pt_pixel_color[ENESIM_ROPS][ENESIM_SURFACE_FORMATS][COLOR_TYPES];
	Enesim_Drawer_Point pt_pixel_mask[ENESIM_ROPS][ENESIM_SURFACE_FORMATS];
} Enesim_Drawer;

#endif /*DRAWER_H_*/
