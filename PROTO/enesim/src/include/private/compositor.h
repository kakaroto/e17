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
#ifndef COMPOSITOR_H_
#define COMPOSITOR_H_

#define ENESIM_COMPOSITOR_SPAN(f) ((Enesim_Compositor_Span)(f))
#define ENESIM_COMPOSITOR_POINT(f) ((Enesim_Compositor_Point)(f))

#define PT_C(f, op) enesim_compositor_##f##_pt_color_##op
#define PT_P(f, sf, op) enesim_compositor_##f##_pt_pixel_##sf##_##op
#define PT_MC(f, mf, op) enesim_compositor_##f##_pt_mask_color_##mf##_##op
#define PT_PC(f, sf, op) enesim_compositor_##f##_pt_pixel_color_##sf##_##op
#define PT_PM(f, sf, mf, op) enesim_compositor_##f##_pt_pixel_mask_##sf##_##mf##_##op

#define SP_C(f, op) enesim_compositor_##f##_sp_color_##op
#define SP_P(f, sf, op) enesim_compositor_##f##_sp_pixel_##sf##_##op
#define SP_MC(f, mf, op) enesim_compositor_##f##_sp_mask_color_##mf##_##op
#define SP_PC(f, sf, op) enesim_compositor_##f##_sp_pixel_color_##sf##_##op
#define SP_PM(f, sf, mf, op) enesim_compositor_##f##_sp_pixel_mask_##sf##_##mf##_##op

void enesim_compositor_init(void);
void enesim_compositor_shutdown(void);

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

static inline void SP_MC(argb8888, a8, fill)(uint32_t *d, uint32_t len,
		uint32_t *s, uint32_t color, uint8_t *m)
{
	uint32_t *end = d + len;
	while (d < end)
	{
		uint16_t a = *m;
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

#endif /* COMPOSITOR_H_*/
