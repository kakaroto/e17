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
typedef Enesim_Converter_1D Enesim_Converter_1D_Lut[ENESIM_CONVERTER_FORMATS][ENESIM_ANGLES][ENESIM_FORMATS];
typedef Enesim_Converter_2D Enesim_Converter_2D_Lut[ENESIM_CONVERTER_FORMATS][ENESIM_ANGLES][ENESIM_FORMATS];

Enesim_Converter_1D_Lut _converters_1d;
Enesim_Converter_2D_Lut _converters_2d;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_converter_init(void)
{
	enesim_converter_argb8888_init();
	enesim_converter_rgb565_init();
}
void enesim_converter_shutdown(void)
{
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_converter_span_register(Enesim_Converter_1D cnv,
		Enesim_Converter_Format dfmt, Enesim_Angle angle, Enesim_Format sfmt)
{

	_converters_1d[dfmt][angle][sfmt] = cnv;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_converter_surface_register(Enesim_Converter_2D cnv,
		Enesim_Converter_Format dfmt, Enesim_Angle angle, Enesim_Format sfmt)
{
	_converters_2d[dfmt][angle][sfmt] = cnv;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Converter_1D enesim_converter_span_get(Enesim_Converter_Format dfmt,
		Enesim_Angle angle, Enesim_Format f)
{
	return _converters_1d[dfmt][angle][f];
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Converter_2D enesim_converter_surface_get(Enesim_Converter_Format dfmt,
		Enesim_Angle angle, Enesim_Format f)
{
	return _converters_2d[dfmt][angle][f];
}
/**
 * To be documented
 * FIXME: To be fixed
 * FIXME how to handle the gray?
 */
EAPI Enesim_Converter_Format enesim_converter_rgb_format_get(uint8_t aoffset, uint8_t alen,
		uint8_t roffset, uint8_t rlen, uint8_t goffset, uint8_t glen,
		uint8_t boffset, uint8_t blen, Eina_Bool premul)
{
	if ((boffset == 0) && (blen == 5) && (goffset == 5) && (glen == 6) &&
			(roffset == 11) && (rlen == 5) && (aoffset == 0) && (alen == 0))
		return ENESIM_CONVERTER_RGB565;

	if ((boffset == 0) && (blen == 8) && (goffset == 8) && (glen == 8) &&
			(roffset == 16) && (rlen == 8) && (aoffset == 24) && (alen == 8))
	{
		if (premul)
			return ENESIM_CONVERTER_ARGB8888_PRE;
		else
			return ENESIM_CONVERTER_ARGB8888;
	}

	if ((boffset == 0) && (blen == 0) && (goffset == 0) && (glen == 0) &&
			(roffset == 0) && (rlen == 0) && (aoffset == 0) && (alen == 8))
		return ENESIM_CONVERTER_A8;
}

/**
 * Gets the pixel depth of the converter format
 * @param fmt The converter format to get the depth from
 * @return The depth in bits per pixel
 */
EAPI uint8_t enesim_converter_format_depth_get(Enesim_Converter_Format fmt)
{
	switch (fmt)
	{
		case ENESIM_CONVERTER_RGB565:
		return 16;

		case ENESIM_CONVERTER_ARGB8888:
		case ENESIM_CONVERTER_ARGB8888_PRE:
		return 32;

		case ENESIM_CONVERTER_A8:
		case ENESIM_CONVERTER_GRAY:
		return 8;

		case ENESIM_CONVERTER_RGB888:
		return 24;

		default:
		return 0;
	}
}
