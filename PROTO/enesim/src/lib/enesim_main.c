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

static int _init = 0;

/**
 * 
 */
EAPI int enesim_init(void)
{
	if (!_init++)
	{
		/* initialize the drawer */
		enesim_drawer_init();
	}
	return _init;
}
/**
 * 
 */
EAPI void enesim_shutdown(void)
{
	
}

/**
 * TODO rename this
 */
EAPI void enesim_color_get(Enesim_Color *color, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	unsigned int alpha = a + 1;
	
	*color = (a << 24) | (((r * alpha) >> 8) << 16) | (((g * alpha) >> 8) << 8)
		| ((b * alpha) >> 8);
}
#if 0
/**
 * 
 */
EAPI void enesim_color_components_from(Enesim_Color *color, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	unsigned int alpha = a + 1;
		
	*color = (a << 24) | (((r * alpha) >> 8) << 16) | (((g * alpha) >> 8) << 8)
		| ((b * alpha) >> 8);
}
/**
 * 
 */
EAPI void enesim_color_components_to(Enesim_Color *color, uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b)
{
	
}
#endif
