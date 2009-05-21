
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
#define FIXED 0

/* P'(x,y) <- P( x + scale * (XC(x,y) - .5), y + scale * (YC(x,y) - .5)) */
static void _dm_argb8888_fast_argb8888(uint32_t *src, uint32_t spitch,  uint32_t sw, uint32_t sh,
		float scale, uint32_t *map,
		uint32_t dx, uint32_t dy, uint32_t dlen,
		uint32_t *dst)
{
	uint32_t *e = dst + dlen;

	while (dst < e)
	{
		uint32_t p0 = 0;
		uint16_t a;
		int32_t sx, sy;
		int32_t sxx, syy;
#if FIXED
		int32_t vx, vy;
#else
		float vx, vy;
#endif
		/* first we transform input coords to src coords... */
		if (dx >= sw || dy >= sh)
			goto next;
		a = *(map) >> 24;
		/*
		{
			uint32_t tmp = *map;
			a = tmp >> 24;
			if (a > 0 && a < 255)
				a = ((tmp >> 16) & 0xff) * 255 / a;
			else
				a = a;

		}*/
#if FIXED
		vx = ((a - 127) * 65536) / (255 * 65536);
		vy = ((a - 127) * 65536) / (255 * 65536);
		sxx = (dx * 65535) + (scale * 65536 * vx);
		syy = (dy * 65535) + (scale * 65536 * vy);
#else
		vx = (a / 255.0) - 0.5;
		vy = (a / 255.0) - 0.5;
		sxx = (dx + (scale * vx)) * 65536;
		syy = (dy + (scale * vy)) * 65536;

#endif
		sx = (sxx >> 16);
		sy = (syy >> 16);
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
			//printf("moving %d %d (%d)\n", vx >> 16, vy >> 16, a);
			//printf("moving %g %g (%d)\n", vx, vy, a);
			//printf("from %d %d to %d %d\n", dx, dy, sx, sy);
		}
next:
		*dst++ = p0;
		map++;
		dx++;
	}
}
