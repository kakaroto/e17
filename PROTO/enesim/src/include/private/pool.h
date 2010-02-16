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
#ifndef POOL_H_
#define POOL_H_

typedef void (*Enesim_Pool_Surface_Alloc)(Enesim_Pool *p);
typedef void (*Enesim_Pool_Surface_Free)(Enesim_Pool *p);
typedef void (*Enesim_Pool_Free)(Enesim_Pool *p);

struct _Enesim_Pool
{
	EINA_MAGIC;
	Enesim_Pool_Surface_Alloc surface_alloc;
	Enesim_Pool_Surface_Free surface_free;
	Enesim_Pool_Free free;
};

#endif
