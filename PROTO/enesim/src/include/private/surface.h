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

struct _Enesim_Surface
{
#ifdef DEBUG
	Enesim_Magic magic;
#endif
	int w;
	int h;
	int flags;
	/* TODO make all code use this */
	int stride; /* number of bytes per line */
	Enesim_Surface_Data sdata;;
	Eina_Bool external; /* in case the user created this surface externally */
	void *user; /* user provided data */
};

#endif
