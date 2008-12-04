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
#ifndef SURFACE_ARGB8888_ROP_H_
#define SURFACE_ARGB8888_ROP_H_

/*============================================================================*
 *                                   Core                                     *
 *============================================================================*/
static inline void argb8888_data_copy(Enesim_Surface_Data *s, Enesim_Surface_Data *d)
{
	d->data.argb8888.plane0 = s->data.argb8888.plane0;
}
static inline void argb8888_data_increment(Enesim_Surface_Data *d, unsigned int len)
{
	d->data.argb8888.plane0 += len;
}
static inline void argb8888_data_offset(Enesim_Surface_Data *s, Enesim_Surface_Data *d, unsigned int offset)
{
	d->data.argb8888.plane0 = s->data.argb8888.plane0 + offset;
}
static inline unsigned char argb8888_data_alpha_get(Enesim_Surface_Data *d)
{
	return (*d->data.argb8888.plane0 >> 24) & 0xff;
}

#define MUL_256(a, c) \
 ( (((((c) >> 8) & 0x00ff00ff) * (a)) & 0xff00ff00) + \
   (((((c) & 0x00ff00ff) * (a)) >> 8) & 0x00ff00ff) )


static inline unsigned int mul_256(unsigned char a, unsigned int c)
{
	//printf("%x %x\n", a, c);
	return  ( (((((c) >> 8) & 0x00ff00ff) * (a)) & 0xff00ff00) +
	(((((c) & 0x00ff00ff) * (a)) >> 8) & 0x00ff00ff) );
}
/* Functions needed by the other subsystems */
static inline unsigned char argb8888_alpha_get(unsigned int plane0)
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

static inline void argb8888_blend(unsigned int *dplane0, unsigned int splane0)
{
	unsigned char a;
	
	a = 256 - argb8888_alpha_get(splane0);
	*dplane0 = splane0 + mul_256(a, *dplane0);
}

static inline void argb8888_fill(unsigned int *dplane0, unsigned int splane0)
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

/*============================================================================*
 *                                Surface                                     *
 *============================================================================*/
static inline void argb8888_pixel_components_from(Enesim_Surface_Pixel *color,
		uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	unsigned int alpha = a + 1;
			
	color->pixel.argb8888.plane0 = (a << 24) | (((r * alpha) >> 8) << 16) | (((g * alpha) >> 8) << 8)
			| ((b * alpha) >> 8);
	//argb8888_from_components(&color->pixel.argb8888.plane0, a, r, g, b);
}
static inline void argb8888_pixel_blend(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	argb8888_blend(d->data.argb8888.plane0, p->pixel.argb8888.plane0);
}

static inline void argb8888_pixel_fill(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	argb8888_fill(d->data.argb8888.plane0, p->pixel.argb8888.plane0);
}

#endif /*SURFACE_ARGB8888_ROP_H_*/
