/**
 * parameters:
 * DATA32 *p
 * DATA32 *d
 * enesim_16p16_t x
 * enesim_16p16_t y
 * int sw
 * int sh
 * macros:
 * RENDER_OP_CALL
 */
static inline void
ARGB8888_CONVOLUTION_2X2_PROTO
{
	DATA32 p3 = 0, p2 = 0, p1 = 0, p0 = 0;
	int ax, ay;
	int sx, sy;
	
	/* 8 bits error to alpha */
	ax = 1 + enesim_16p16_fracc_get(x) >> 8;
	ay = 1 + enesim_16p16_fracc_get(y) >> 8;
	/* integer values for the coordinates */
	sx = enesim_16p16_int_to(x);
	sy = enesim_16p16_int_to(y);
	
	if ((sx > -1) && (sy> -1))
		p0 = *p;
	if ((sy> -1) & ((sx + 1) < sw))
		p1 = *(p + 1);
	if ((sy + 1) < sh)
	{
		if (sx> -1)
			p2 = *(p + sw);
		if ((sx + 1) < sw)
			p3 = *(p + sw + 1);
	}

	if (p0 != p1)
		p0 = interp_256(ax, p1, p0);
	if (p2 != p3)
		p2 = interp_256(ax, p3, p2);
	if (p0 != p2)
		p0 = interp_256(ay, p2, p0);
	if (p0)
	{
		RENDER_OP_CALL
	}
}
