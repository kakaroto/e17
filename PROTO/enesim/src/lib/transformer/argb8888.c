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
#include "_argb8888_c.c"

Enesim_Transformer argb8888_tx = {
	//.mask[ENESIM_SURFACE_ARGB8888][ENESIM_SURFACE_ARGB8888][ENESIM_TRANSFORMATION_AFFINE][ENESIM_GOOD] = 
	//	mask_argb8888_argb8888_affine_good_no_no,
	.normal[ENESIM_SURFACE_ARGB8888][ENESIM_TRANSFORMATION_AFFINE][ENESIM_GOOD] =
		normal_argb8888_argb8888_affine_good_no_no,
	.normal[ENESIM_SURFACE_ARGB8888][ENESIM_TRANSFORMATION_AFFINE][ENESIM_FAST] =
		normal_argb8888_argb8888_affine_fast_no_no,
};

