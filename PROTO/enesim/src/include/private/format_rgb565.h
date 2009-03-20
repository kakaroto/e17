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
#ifndef FORMAT_RGB565_H_
#define FORMAT_RGB565_H_

#define RGB565MASK 0x7E0F81F /* 565565 */
#define FIX_ALPHA 0

static inline unsigned char rgb565_red_get(uint16_t plane0, uint8_t plane1)
{

}

static inline unsigned char rgb565_green_get(uint16_t plane0, uint8_t plane1)
{

}

static inline unsigned char rgb565_blue_get(uint16_t plane0, uint8_t plane1)
{

}

static inline void rgb565_from_components(uint16_t *plane0, unsigned char a, unsigned char r,
		unsigned char g, unsigned char b)
{

}

static inline void rgb565_to_components(uint16_t plane0, unsigned char *r,
		unsigned char *g, unsigned char *b)
{

}

static inline void rgb565_to_argb(unsigned int *argb, uint16_t plane0,
uint8_t alpha)
{
	*argb = (alpha << 24) | ((plane0 & 0x1f) << 3) |
		((plane0 & 0x7e0) << 5) | ((plane0 & 0xf800) << 8);
}

/**
 * the alpha value should be compatible with a value comparision with
 * the green channel
 */
static inline void rgb565_from_argb(unsigned int argb, uint16_t *plane0,
uint8_t alpha)
{
#if FIX_ALPHA
	uint8_t green = argb8888_green_get(argb) >> 2;

	if (green > alpha) green = alpha;

	*plane0 = ((argb8888_red_get(argb) >> 3) << 11) |
		(green << 5) |
		(argb8888_blue_get(argb) >> 3);

#else
	*plane0 = ((argb8888_red_get(argb) >> 3) << 11) |
		((argb8888_green_get(argb) >> 2) << 5) |
		(argb8888_blue_get(argb) >> 3);
#endif
}

static inline void rgb565_blend(uint16_t *dplane0, uint16_t splane0, uint8_t alpha)
{
	uint32_t unpacked;

	unpacked = (*dplane0 | (*dplane0 << 16)) & RGB565MASK;
	*dplane0 = splane0 + ((unpacked * alpha) >> 5);
}

static inline void rgb565_fill(uint16_t *dplane0, uint16_t splane0)
{
	*dplane0 = splane0;
}

#endif /*FORMAT_RGB565_H_*/
