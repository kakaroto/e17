
#define INTERP_256(a, c0, c1) \
 ( (((((((c0) >> 8) & 0xff00ff) - (((c1) >> 8) & 0xff00ff)) * (a)) \
   + ((c1) & 0xff00ff00)) & 0xff00ff00) + \
   (((((((c0) & 0xff00ff) - ((c1) & 0xff00ff)) * (a)) >> 8) \
   + ((c1) & 0xff00ff)) & 0xff00ff) )

/* FIXME actually this code is not the "fast" but the "good" version
 * Move the INTERP_256 and the sampling function out of here
 * the transformer also needs this
 * Use eina_f16p16 for the fixed point math
 * Define on eina an Eina_F16p16 eina_f16p16_one_get() function
 */

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
		}
		*dst++ = p0;
		dx++;
	}
}
