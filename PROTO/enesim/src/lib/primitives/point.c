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

EAPI void enesim_primitive_point(Enesim_Surface *dst, unsigned int x, unsigned int y, Enesim_Drawer_Context *c)
{
	Enesim_Surface_Data sdata;
	Enesim_Drawer_Point point;
	int w;
	
	//enesim_drawer_context_mul_color_get
	//point = enesim_drawer_pt_
	enesim_surface_data_get(dst, &sdata);
	enesim_surface_size_get(dst, &w, NULL);
	enesim_surface_data_increment(&sdata, (w * y) + x);
	//point(sdata, ..., ..., ...);
}


