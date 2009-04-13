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

/* FIXME this code should be xported somehow */
#if 0
#define SAMPLE_FAST
#define SAMPLE_GOOD

static inline void argb8888_surface_sample(uint32_t *src,
		uint32_t sw, uint32_t sh, uint32_t spitch,
		uint32_t sx, uint32_t sy, uint32_t *dst)
{
	uint32_t *ssrc = src + (sy * spitch) + sx;
#ifdef SAMPLE_GOOD
	uint32_t p3 = 0, p2 = 0, p1 = 0;
#endif

#ifdef SAMPLE_FAST
	*dst = *ssrc;
#elif SAMPLE_GOOD
	*p0 = *ssrc;
	if ((sy > -1) && ((sx + 1) < sw))
	{
		p1 = *(ssrc + 1);
	}
	if ((sy + 1) < sh)
	{
		if (sx > -1)
		{
			p2 = *(ssrc + spitch);
		}
		if ((sx + 1) < w)
		{
			p3 = *(ssrc + spitch + 1);
		}
	}
	if (p0 | p1 | p2 | p3)
	{
		uint16 ax, ay;

		ax = 1 + ((sxx >> 8) & 0xff);
		ay = 1 + ((syy >> 8) & 0xff);

		p0 = argb8888_interp_256(ax, p1, p0);
		p2 = argb8888_interp_256(ax, p3, p2);
		p0 = argb8888_interp_256(ay, p2, p0);
	}
	*dst = p0;
#endif
}
#endif
#include "_affine.c"
#include "_projective.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_transformer_argb8888_init(Enesim_Cpu *cpu)
{
	/* TODO check if the cpu is the host */
	enesim_transformer_1d_register(cpu,
			_tx_argb8888_affine_fast_argb8888,
			ENESIM_FORMAT_ARGB8888, ENESIM_MATRIX_AFFINE,
			ENESIM_FAST, ENESIM_FORMAT_ARGB8888);
	enesim_transformer_1d_register(cpu,
			_tx_argb8888_projective_fast_argb8888,
			ENESIM_FORMAT_ARGB8888, ENESIM_MATRIX_PROJECTIVE,
			ENESIM_FAST, ENESIM_FORMAT_ARGB8888);
}
