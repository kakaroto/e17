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
/**
 * TODO remove this error handling, use eina's 
 */
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
void enesim_error_set(unsigned int err)
{
	enesim_err = err;
}
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
unsigned int enesim_err;
/**
 * 
 */
EAPI char * enesim_error_to_str(unsigned int err)
{
	switch (err)
	{
	
	case ENESIM_ERROR_HANDLE_INVALID:
		return "ERROR_INVALID_HANDLE";
	
	case ENESIM_ERROR_SCANLINE_NOT_SUPPORTED:
		return "ENESIM_ERROR_SCANLINE_NOT_SUPPORTED";
	
	case ENESIM_ERROR_GEOMETRY_INVALID:
		return "ENESIM_ERROR_GEOMETRY_INVALID";
		
	case ENESIM_ERROR_FROMAT_NOT_SUPPORTED:
		return "ENESIM_ERROR_FROMAT_NOT_SUPPORTED";
	}
	return NULL;
}
