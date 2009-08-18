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
static void _span_argb8888_argb8888(uint32_t *native, uint32_t len,
		Enesim_Converter_Data *data)
{
	uint32_t *ssrc = data->argb8888.plane0;

	while (len--)
	{
		uint16_t a = (*ssrc >> 24) + 1;

		if (a != 256)
		{
			*native = (*ssrc & 0xff000000) + (((((*ssrc) >> 8) & 0xff) * a) & 0xff00) +
			(((((*ssrc) & 0x00ff00ff) * a) >> 8) & 0x00ff00ff);
		}
		else
			*native = *ssrc;

		native++;
		ssrc++;
	}
}

static void _span_argb8888_a8(uint32_t *native, uint32_t len,
		Enesim_Converter_Data *data)
{
	uint8_t *ssrc = data->a8.plane0;

	while (len--)
	{
		*native = *ssrc << 24;;

		native++;
		ssrc++;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * enesim_renderer_importer_new(void)
{

}
