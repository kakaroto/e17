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
 * Initialize the library
 * You must call this function before calling any other function
 * @return The number of times the library has been initialized
 */
EAPI int enesim_init(void)
{
	if (!_init++)
	{
		/* TODO Dump the information about SIMD extensions
		 * get the cpuid for this
		 */
		eina_init();
		enesim_compositor_init();
		enesim_converter_init();
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
 * Shutdowns the library
 * You must call this function after you finish using the library
 */
EAPI void enesim_shutdown(void)
{
	enesim_compositor_shutdown();
	enesim_converter_shutdown();
	eina_shutdown();
}

/**
 * Gets the string based name of a format
 * @param f The format to get the name from
 * @return The name of the format
 */
EAPI const char * enesim_format_name_get(Enesim_Format f)
{
	switch (f)
	{
		case ENESIM_FORMAT_ARGB8888:
		return "argb8888";

		case ENESIM_FORMAT_XRGB8888:
		return "xrgb8888";

		case ENESIM_FORMAT_ARGB8888_SPARSE:
		return "argb8888sp";

		case ENESIM_FORMAT_A8:
		return "a8";

		default:
		return NULL;

	}
}

/**
 * Get the total size of bytes for a given a format and a size
 * @param f The format
 * @param w The width
 * @param h The height
 */
EAPI size_t enesim_format_bytes_calc(Enesim_Format f, uint32_t w, uint32_t h)
{
	switch (f)
	{
		case ENESIM_FORMAT_ARGB8888:
		case ENESIM_FORMAT_XRGB8888:
		case ENESIM_FORMAT_ARGB8888_SPARSE:
		return w * h * sizeof(uint32_t);

		case ENESIM_FORMAT_A8:
		return w * h * sizeof(uint8_t);

		default:
		return 0;

	}
}
