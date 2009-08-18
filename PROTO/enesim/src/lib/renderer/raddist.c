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
static void _rd_argb8888_fast_argb8888(uint32_t *src, uint32_t spitch,  uint32_t sw, uint32_t sh,
		float x0, float y0, float r0, float scale,
		uint32_t x, uint32_t y, uint32_t dlen,
		uint32_t *dst)
{
	uint32_t *e = dst + dlen;
	float r0_inv = 1.0f/r0;
	int32_t dx = x - x0;
	int32_t dy = y - y0;

	while (dst < e)
	{
		/* first we transform input coords to src coords... */
		unsigned int  p0 = 0;
		int     sxx, syy, sx, sy;
		float   r = hypot(dx, dy);

		r = (((scale * (r0 - r)) + r) * r0_inv);
		sxx = ((r * dx) + x0) * 65536;  sx = (sxx >> 16);
		syy = ((r * dy) + y0) * 65536;  sy = (syy >> 16);
		/* ... then we sample the src. */
		if ( (((unsigned) (sx + 1)) < (sw + 1)) && (((unsigned) (sy + 1)) < (sh + 1)) )
		{
			unsigned int  p3 = 0, p2 = 0, p1 = 0;
			unsigned int *p = src + (sy * spitch) + sx;
#if 0

			if ((sx > -1) && (sy > -1))
				p0 = *p;
			if ((sy > -1) && ((sx + 1) < sw))
				p1 = *(p + 1);
			if ((sy + 1) < sh)
			{
				if (sx > -1)
					p2 = *(p + spitch);
				if ((sx + 1) < sw)
					p3 = *(p + spitch + 1);
			}
			if (p0 | p1 | p2 | p3)
			{
				sx = 1 + ((sxx >> 8) & 0xff);
				sy = 1 + ((syy >> 8) & 0xff);

				p0 = INTERP_256(sx, p1, p0);
				p2 = INTERP_256(sx, p3, p2);
				p0 = INTERP_256(sy, p2, p0);
			}
#else
			if ((sx > -1) && (sy > -1))
				p0 = *p;
#endif
		}
		*dst++ = p0;
		dx++;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * enesim_renderer_raddist_new(void)
{

}

