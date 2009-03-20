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
#ifndef FORMAT_ARGB8888_H_
#define FORMAT_ARGB8888_H_

/*============================================================================*
 *                                   Core                                     *
 *============================================================================*/
static inline void argb8888_data_copy(Enesim_Surface_Data *s, Enesim_Surface_Data *d)
{
	d->plane0 = s->plane0;
}
static inline void argb8888_data_increment(Enesim_Surface_Data *d, unsigned int len)
{
	d->plane0 += len;
}
static inline void argb8888_data_offset(Enesim_Surface_Data *s, Enesim_Surface_Data *d, unsigned int offset)
{
	d->plane0 = s->plane0 + offset;
}
static inline unsigned char argb8888_data_alpha_get(Enesim_Surface_Data *d)
{
	return (*d->plane0 >> 24) & 0xff;
}

static inline uint32_t argb8888_mul_256(uint16_t a, uint32_t c)
{
	return  ( (((((c) >> 8) & 0x00ff00ff) * (a)) & 0xff00ff00) +
	(((((c) & 0x00ff00ff) * (a)) >> 8) & 0x00ff00ff) );
}

static inline uint32_t argb8888_mul_sym(uint16_t a, uint32_t c)
{
	return ( (((((c) >> 8) & 0x00ff00ff) * (a) + 0xff00ff) & 0xff00ff00) +
	   (((((c) & 0x00ff00ff) * (a) + 0xff00ff) >> 8) & 0x00ff00ff) );
}
/*
 * [a1 r1 g1 b1], [a2 r2 g2 b2] => [a1*a2 r1*r2 g1*g2 b1*b2]
 */
static inline uint32_t argb8888_mul4_sym(uint32_t c1, uint32_t c2)
{
	return ( ((((((c1) >> 16) & 0xff00) * (((c2) >> 16) & 0xff00)) + 0xff0000) & 0xff000000) + \
	   ((((((c1) >> 8) & 0xff00) * (((c2) >> 16) & 0xff)) + 0xff00) & 0xff0000) + \
	   ((((((c1) & 0xff00) * ((c2) & 0xff00)) + 0xff00) >> 16) & 0xff00) + \
	   (((((c1) & 0xff) * ((c2) & 0xff)) + 0xff) >> 8) );
}

/* Functions needed by the other subsystems */
static inline unsigned char argb8888_alpha_get(uint32_t plane0)
{
	return (plane0 >> 24);
}

static inline unsigned char argb8888_red_get(unsigned int plane0)
{
	return ((plane0 >> 16) & 0xff);
}

static inline unsigned char argb8888_green_get(unsigned int plane0)
{
	return ((plane0 >> 8) & 0xff);
}

static inline unsigned char argb8888_blue_get(unsigned int plane0)
{
	return (plane0 & 0xff);
}

static inline void argb8888_from_components(unsigned int *plane0, unsigned char a, unsigned char r,
		unsigned char g, unsigned char b)
{
	*plane0 = (a << 24) | (r << 16) | (g << 8) | b;
}

static inline void argb8888_to_components(unsigned int plane0, unsigned char *a, unsigned char *r,
		unsigned char *g, unsigned char *b)
{
	if (a) *a = argb8888_alpha_get(plane0);
	if (r) *r = argb8888_red_get(plane0);
	if (g) *g = argb8888_green_get(plane0);
	if (b) *b = argb8888_blue_get(plane0);
}

static inline void argb8888_to_argb(unsigned int *argb, unsigned int plane0)
{
	*argb = plane0;
}
static inline void argb8888_from_argb(unsigned int argb, unsigned int *plane0)
{
	*plane0 = argb;
}

static inline void argb8888_blend(uint32_t *dplane0, uint16_t a, uint32_t splane0)
{
	*dplane0 = splane0 + argb8888_mul_256(a, *dplane0);
}

static inline void argb8888_fill(uint32_t *dplane0, uint32_t splane0)
{
	*dplane0 = splane0;
}

/* TODO
 * remove this
 */
#define INTERP_256(a, c0, c1) \
 ( (((((((c0) >> 8) & 0xff00ff) - (((c1) >> 8) & 0xff00ff)) * (a)) \
   + ((c1) & 0xff00ff00)) & 0xff00ff00) + \
   (((((((c0) & 0xff00ff) - ((c1) & 0xff00ff)) * (a)) >> 8) \
   + ((c1) & 0xff00ff)) & 0xff00ff) )
#define interp_256 INTERP_256

/* TODO optimize this with MMX */
static inline uint32_t argb8888_interp_256(uint16_t a, uint32_t c0, uint32_t c1)
{
	 return ( (((((((c0) >> 8) & 0xff00ff) - (((c1) >> 8) & 0xff00ff)) * (a)) \
	   + ((c1) & 0xff00ff00)) & 0xff00ff00) + \
	   (((((((c0) & 0xff00ff) - ((c1) & 0xff00ff)) * (a)) >> 8) \
	   + ((c1) & 0xff00ff)) & 0xff00ff) );
}
/*============================================================================*
 *                                Surface                                     *
 *============================================================================*/
#if 0
static inline void argb8888_pixel_components_from(Enesim_Surface_Pixel *color,
		uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	unsigned int alpha = a + 1;

	color->plane0 = (a << 24) | (((r * alpha) >> 8) << 16) | (((g * alpha) >> 8) << 8)
			| ((b * alpha) >> 8);
	//argb8888_from_components(&color->plane0, a, r, g, b);
}

static inline void argb8888_pixel_components_to(Enesim_Surface_Pixel *color,
		uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint32_t argb_unpre;
	uint8_t pa = argb8888_alpha_get(color->plane0);

	if ((pa > 0) && (pa < 255))
	{
		if (a) *a = pa;
		if (r) *r = (argb8888_red_get(color->plane0) * 255) / pa;
		if (g) *g = (argb8888_green_get(color->plane0) * 255) / pa;
		if (b) *b = (argb8888_blue_get(color->plane0) * 255) / pa;
	}
	else
	{
		if (a) *a = pa;
		if (r) *r = argb8888_red_get(color->plane0);
		if (g) *g = argb8888_green_get(color->plane0);
		if (b) *b = argb8888_blue_get(color->plane0);
	}
}
static inline void argb8888_pixel_blend(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	uint16_t a;

	a = 256 - argb8888_alpha_get(p->plane0);
	argb8888_blend(d->plane0, a, p->plane0);
}

static inline void argb8888_pixel_fill(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	argb8888_fill(d->plane0, p->plane0);
}
#endif
#endif /* FORMAT_ARGB8888_H_*/
