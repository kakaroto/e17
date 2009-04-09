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
#ifndef DRAWER_H_
#define DRAWER_H_

void enesim_drawer_init(void);
void enesim_drawer_shutdown(void);

/* The above functions are useful for other subsystems */
/* TODO optimize the below functions with MMX/SSE */
static inline uint32_t argb8888_interp_256(uint16_t a, uint32_t c0, uint32_t c1)
{
	 return ( (((((((c0) >> 8) & 0xff00ff) - (((c1) >> 8) & 0xff00ff)) * (a)) \
	   + ((c1) & 0xff00ff00)) & 0xff00ff00) + \
	   (((((((c0) & 0xff00ff) - ((c1) & 0xff00ff)) * (a)) >> 8) \
	   + ((c1) & 0xff00ff)) & 0xff00ff) );
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

/*============================================================================*
 *                                  Fill                                      *
 *============================================================================*/
static inline void enesim_drawer_argb8888_pt_color(uint32_t *d, uint32_t s,
		uint32_t color, uint32_t m)
{
	*d = color;
}
static inline void enesim_drawer_argb8888_pt_mask_color_argb8888(uint32_t *d,
		uint32_t s, uint32_t color, uint32_t m)
{
	uint16_t a = m >> 24;
	switch (a)
	{
		case 0:
		break;

		case 255:
		*d = color;
		break;

		default:
		*d = argb8888_interp_256(a + 1, color, *d);
		break;
	}
}

static inline void enesim_drawer_argb8888_pt_pixel_mask_argb8888_argb8888(
		uint32_t *d, uint32_t s, uint32_t color, uint32_t m)
{
	uint16_t a = m >> 24;
	switch (a)
	{
		case 0:
		break;

		case 255:
		*d = s;
		break;

		default:
		*d = argb8888_interp_256(a + 1, s, *d);
		break;
	}
}

static inline void PT_P(argb8888, argb8888, fill)(uint32_t *d,
		uint32_t s, uint32_t color, uint32_t m)
{
	*d = s;
}

static inline void PT_PC(argb8888, argb8888, fill)(uint32_t *d,
		uint32_t s, uint32_t color, uint32_t m)
{
	*d = argb8888_mul4_sym(color, s);
}


static inline void SP_C(argb8888, fill)(uint32_t *d, uint32_t len,
		uint32_t *s, uint32_t color, uint32_t *m)
{
	uint32_t *end = d + len;
	while (d < end)
	{
		*d = color;
		d++;
	}
}
static inline void SP_P(argb8888, argb8888, fill)(uint32_t *d, uint32_t len,
		uint32_t *s, uint32_t color, uint32_t *m)
{
	uint32_t *end = d + len;
	while (d < end)
	{
		*d = *s;
		d++;
		s++;
	}
}

static inline void SP_PC(argb8888, argb8888, fill)(uint32_t *d, uint32_t len,
		uint32_t *s, uint32_t color, uint32_t *m)
{
	uint32_t *end = d + len;
	while (d < end)
	{
		*d = argb8888_mul4_sym(color, *s);
		d++;
		s++;
	}
}

static inline void SP_MC(argb8888, argb8888, fill)(uint32_t *d, uint32_t len,
		uint32_t *s, uint32_t color, uint32_t *m)
{
	uint32_t *end = d + len;
	while (d < end)
	{
		uint16_t a = *m >> 24;
		switch (a)
		{
			case 0:
			break;

			case 255:
			*d = color;
			break;

			default:
			*d = argb8888_interp_256(a + 1, color, *d);
			break;
		}
		d++;
		m++;
	}
}


static inline void SP_PM(argb8888, argb8888, argb8888, fill)(uint32_t *d,
		uint32_t len, uint32_t *s, uint32_t color,
		uint32_t *m)
{
	uint32_t *end = d + len;
	while (d < end)
	{
		uint16_t a = *m >> 24;
		switch (a)
		{
			case 0:
			break;

			case 255:
			*d = *s;
			break;

			default:
			*d = argb8888_interp_256(a + 1, *s, *d);
			break;
		}
		m++;
		s++;
		d++;
	}
}

#if 0
static void argb8888_sp_pixel_fill_argb8888_mmx(Enesim_Surface_Data *d,
		uint32_t len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	uint32_t *stmp = s->plane0, *dtmp = d->plane0;
	uint32_t *end;
	int l = 0;

	l = (len / 16);
	end = d->plane0 + (len - (len % 16));
	while (dtmp < end)
	{
		mmx_t m0, m1, m2, m3, m4, m5, m6, m7;

		m0 = *((__m64 *)stmp);
		m1 = *((__m64 *)(stmp + 2));
		m2 = *((__m64 *)(stmp + 4));
		m3 = *((__m64 *)(stmp + 6));
		m4 = *((__m64 *)(stmp + 8));
		m5 = *((__m64 *)(stmp + 10));
		m6 = *((__m64 *)(stmp + 12));
		m7 = *((__m64 *)(stmp + 14));
		*(__m64 *)dtmp = m0;
		*(__m64 *)(dtmp + 2) = m1;
		*(__m64 *)(dtmp + 4) = m2;
		*(__m64 *)(dtmp + 6) = m3;
		*(__m64 *)(dtmp + 8) = m4;
		*(__m64 *)(dtmp + 10) = m5;
		*(__m64 *)(dtmp + 12) = m6;
		*(__m64 *)(dtmp + 14) = m7;

		stmp += 16;
		dtmp += 16;
	}
	_mm_empty();
}
#endif

#if 0
/* specific drawers */
Eina_Bool enesim_drawer_generic_init(void);
void enesim_drawer_generic_shutdown(void);
Eina_Bool enesim_drawer_argb8888_init(void);
void enesim_drawer_argb8888_shutdown(void);
#endif

#endif /*DRAWER_H_*/
