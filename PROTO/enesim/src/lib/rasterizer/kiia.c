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
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Rasterizer * enesim_rasterizer_kiia_new(Enesim_Rasterizer_Kiia_Count
		count, Eina_Rectangle boundaries)
{
	Enesim_Rasterizer *r = NULL;
	switch (count)
	{
	case ENESIM_RASTERIZER_KIIA_COUNT_8:
		r = enesim_rasterizer_kiia8_new(boundaries);
		break;
	case ENESIM_RASTERIZER_KIIA_COUNT_16:
		r = enesim_rasterizer_kiia16_new(boundaries);
		break;
	case ENESIM_RASTERIZER_KIIA_COUNT_32:
		break;
	}
	return r;
}
