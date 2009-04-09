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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _init = 0;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI int enesim_init(void)
{
	if (!_init++)
	{
		/* TODO Dump the information about SIMD extensions
		 * get the cpuid for this
		 */
		eina_init();
		enesim_cpu_init();
		enesim_drawer_init();
		enesim_converter_init();
		/* TODO initialize all the transformers */
		/* TODO initialize all the converters */
#ifdef EFL_HAVE_MMX
		EINA_ERROR_PINFO("MMX Drawer available\n");
#endif
#ifdef EFL_HAVE_SSE2
		EINA_ERROR_PINFO("SSE2 Drawer available\n");
#endif
	}
	return _init;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_shutdown(void)
{
	enesim_drawer_shutdown();
	enesim_converter_shutdown();
	eina_shutdown();
}

