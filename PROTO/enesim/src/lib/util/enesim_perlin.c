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
/* FIXME
 * Very naive implementation of a perlin noise, this needs to be cleaned up,
 * optimized and validated.
 * http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_F16p16 noise(int x, int y)
{
	int n;
	Eina_F16p16 tmp;

	n = x + y * 57;
	n = (n << 13) ^ n;
	/* clamp to 0x7fffffff */
	n = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
	/* rescale to -1:1 */
	tmp = (65536 - (((int64_t)(n) << 16) / 1073741824));
	return tmp;
}

#define FP16 1048576
#define FP8 524288
#define FP4 262144
#define FP2 131072
#define FP1 65536

static Eina_F16p16 smoothnoise(int x, int y)
{
	Eina_F16p16 corners, sides, center;

	/* corners */
	corners = noise(x - 1, y - 1) + noise(x + 1, y - 1) + noise(x - 1, y + 1)
			+ noise(x + 1, y + 1);
	corners = (((int64_t) (corners) << 16) / FP16);

	sides = noise(x - 1, y) + noise(x + 1, y) + noise(x, y - 1)
			+ noise(x, y + 1);
	sides = (((int64_t) (sides) << 16) / FP8);

	center = (((int64_t) (noise(x, y)) << 16) / FP4);
	return corners + sides + center;
}


static Eina_F16p16 interpolate(Eina_F16p16 x, Eina_F16p16 y, Eina_F16p16 r)
{
	return eina_f16p16_mul((x - y), r) + y;
}

static Eina_F16p16 interpolatenoise(Eina_F16p16 x, Eina_F16p16 y)
{
	Eina_F16p16 v1, v2, v3, v4;
	Eina_F16p16 fx, fy;
	int ix, iy;

	fx = eina_f16p16_fracc_get(x);
	fy = eina_f16p16_fracc_get(y);
	ix = eina_f16p16_int_to(x);
	iy = eina_f16p16_int_to(y);
#if 0
	v1 = smoothnoise(ix, iy);
	v2 = smoothnoise(ix + 1, iy);
	v3 = smoothnoise(ix, iy + 1);
	v4 = smoothnoise(ix + 1, iy + 1);
#else
	v1 = noise(ix, iy);
	v2 = noise(ix + 1, iy);
	v3 = noise(ix, iy + 1);
	v4 = noise(ix + 1, iy + 1);
#endif
	v1 = interpolate(v1, v2, fx);
	v2 = interpolate(v3, v4, fx);
	//return interpolate(v1, v2, fy);
	return smoothnoise(ix, iy);
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Eina_F16p16 enesim_perlin(Eina_F16p16 xx, Eina_F16p16 yy, unsigned int octaves,
		Eina_F16p16 *freq, Eina_F16p16 *ampl)
{
	int i;
	Eina_F16p16 total = 0;

	for (i = 0; i < octaves; i++)
	{
		Eina_F16p16 x, y, res;

		//printf("%d %d %d\n", i, freq[i], ampl[i]);
		x = eina_f16p16_mul(xx, freq[i]);
		y = eina_f16p16_mul(yy, freq[i]);
		res = interpolatenoise(x, y);
		//printf("%d\n", res);
		total = total + eina_f16p16_mul(res, ampl[i]);
	}
	/* rescale to 0:2? */
	//total += 65536;
	total = abs(total);
	return total;
}
