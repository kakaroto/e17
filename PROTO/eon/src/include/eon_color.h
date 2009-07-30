/*
 * eon_color.h
 *
 *  Created on: 04-mar-2009
 *      Author: jl
 */

#ifndef EON_COLOR_H_
#define EON_COLOR_H_

typedef uint32_t Eon_Color;

static inline void eon_color_set(Eon_Color *c, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	*c = (a << 24) | (r << 16) | (g << 8) | b;
}

#endif /* EON_COLOR_H_ */
