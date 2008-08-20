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
#ifndef _SURFACE_H
#define _SURFACE_H

/**
 * @todo resize code (enesim_surface_resize())
 * @todo clean this file
 */

typedef enum
{
	ENESIM_SURFACE_NO_SCALE,
	ENESIM_SURFACE_SCALE_X,
	ENESIM_SURFACE_SCALE_Y,
	ENESIM_SURFACE_SCALE_ALL,
	ENESIM_SURFACE_SCALES,
} Enesim_Surface_Scale_Direction;

struct _Enesim_Surface
{
	int w;
	int h;
	Enesim_Surface_Format format;
	int flags;
	Enesim_Surface_Data 		data;
#ifdef DEBUG
	Enesim_Magic magic;
#endif
};

void enesim_surface_premul(Enesim_Surface *s);

#endif
