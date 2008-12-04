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
/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static int _init = 0;
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * 
 */
EAPI int enesim_init(void)
{
	if (!_init++)
	{
		/* TODO Dump the information about SIMD extensions
		 * get the cpuid for this
		 */
		eina_init();
#ifdef __MMX__
		EINA_ERROR_PINFO("MMX Drawer available\n");
#endif
#ifdef __SSE2__
		EINA_ERROR_PINFO("SSE2 Drawer available\n");
#endif
	}
	return _init;
}
/**
 * 
 */
EAPI void enesim_shutdown(void)
{
	eina_shutdown();
}

