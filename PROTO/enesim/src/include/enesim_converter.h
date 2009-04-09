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
#ifndef ENESIM_CONVERTER_H_
#define ENESIM_CONVERTER_H_

/**
 * @defgroup Enesim_Converter_Group Converter
 * @{
 *
 *
 * ENESIM_CONVERTER_FORMAT_A8
 * +---------------+----------------+
 * |     Alpha     |      Alpha     |
 * +---------------+----------------+
 *         8                8
 * <------P0------>.<------P1------>.
 *
 * ENESIM_CONVERTER_FORMAT_b1A3
 * +-------+-------+--------+-------+
 * | Blink | Alpha |  Blink | Alpha |
 * +-------+-------+--------+-------+
 *     1       3        1        3
 * <------P0------>.<------P1------>.
 */
/* TODO
 * define a surface_data similar to what enesim had before
 * but for the destination surfaces, there are different multi plane
 * formats for different display controllers, like the a1_rgb565 found
 * on davinci processors.
 */
typedef enum _Enesim_Converter_Format
{
	ENESIM_CONVERTER_RGB565,
	ENESIM_CONVERTER_ARGB888,
	ENESIM_CONVERTER_FORMATS
} Enesim_Converter_Format;

typedef void (*Enesim_Converter_2D)(uint32_t *src, uint32_t sw, uint32_t sh,
		uint32_t spitch, uint16_t *dst, uint32_t dw, uint32_t dh,
		uint32_t dpitch);

typedef void (*Enesim_Converter_1D)(uint32_t *src, uint32_t len, uint32_t *d);

#define ENESIM_CONVERTER_1D(f) ((Enesim_Converter_1D)(f))
#define ENESIM_CONVERTER_2D(f) ((Enesim_Converter_2D)(f))

EAPI Eina_Bool enesim_converter_1d_get(Enesim_Operator *op, Enesim_Cpu *cpu, Enesim_Format sfmt,
		Enesim_Converter_Format dfmt);
EAPI Eina_Bool enesim_converter_2d_get(Enesim_Operator *op, Enesim_Cpu *cpu, Enesim_Format sfmt,
		Enesim_Rotator_Angle angle, Enesim_Converter_Format dfmt);
EAPI void enesim_converter_1d_register(Enesim_Converter_1D cnv, Enesim_Cpu *cpu,
		Enesim_Format sfmt, Enesim_Converter_Format dfmt);
EAPI void enesim_converter_2d_register(Enesim_Converter_2D cnv, Enesim_Cpu *cpu,
		Enesim_Format sfmt, Enesim_Rotator_Angle angle, Enesim_Converter_Format dfmt);
EAPI Enesim_Converter_Format enesim_converter_format_get(uint8_t aoffset, uint8_t alen,
		uint8_t roffset, uint8_t rlen, uint8_t goffset, uint8_t glen,
		uint8_t boffset, uint8_t blen);

/** @} */
#endif /*ENESIM_CONVERTER_H_*/
