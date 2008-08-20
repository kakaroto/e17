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
#ifndef ENESIM_DRAWER_H_
#define ENESIM_DRAWER_H_

/*
 * TODO, the mask can be a surface by itself, we just need to call the
 * FORMAT_alpha_get on that surface
 */

/**
 * Function to draw a point
 * d = Destination surface data
 * s = In case of using the surface as pixel source
 * color = To draw with a color or as a multiplier color in case of using s
 * mask = in case of using a mask
 */
typedef void (*Enesim_Drawer_Point)(Enesim_Surface_Data *d, Enesim_Surface_Data *s,
		Enesim_Color color, Enesim_Surface_Data *m);
/**
 * 
 */
typedef void (*Enesim_Drawer_Span)(Enesim_Surface_Data *d, unsigned int len,
		Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m);

EAPI Enesim_Drawer_Point enesim_drawer_point_color_get(Enesim_Rop rop, Enesim_Surface_Format dfmt, unsigned int color);
EAPI Enesim_Drawer_Span enesim_drawer_span_color_get(Enesim_Rop rop, Enesim_Surface_Format dfmt, unsigned int color);
EAPI Enesim_Drawer_Span enesim_drawer_span_pixel_get(Enesim_Rop rop, Enesim_Surface_Format dfmt, Enesim_Surface_Format sfmt);

#endif /*ENESIM_DRAWER_H_*/
