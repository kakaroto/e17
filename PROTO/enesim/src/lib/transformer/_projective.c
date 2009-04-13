static void _tx_argb8888_projective_fast_argb8888(uint32_t *src,
		uint32_t spitch, uint32_t sw, uint32_t sh,
		float ox, float oy,
		float xx, float xy, float xz,
		float yx, float yy, float yz,
		float zx, float zy, float zz,
		uint32_t dx, uint32_t dy, uint32_t dlen,
		uint32_t *dst)
{
	Eina_F16p16 sx, sy, sz;
	Eina_F16p16 xfp, yfp;
	Eina_F16p16 a, b, c, d, e, f, g, h, i;

	xfp = eina_f16p16_int_from(dx);
	yfp = eina_f16p16_int_from(dy);

	a = eina_f16p16_float_from(xx);
	b = eina_f16p16_float_from(xy);
	c = eina_f16p16_float_from(xz);

	d = eina_f16p16_float_from(yx);
	e = eina_f16p16_float_from(yx);
	f = eina_f16p16_float_from(yz);

	g = eina_f16p16_float_from(zx);
	h = eina_f16p16_float_from(zx);
	i = eina_f16p16_float_from(zz);

	sx = eina_f16p16_mul(a, xfp) + eina_f16p16_mul(b, yfp) + c;
	sy = eina_f16p16_mul(d, xfp) + eina_f16p16_mul(e, yfp) + f;
	sz = eina_f16p16_mul(g, xfp) + eina_f16p16_mul(h, yfp) + i;

	while (dlen--)
	{
		uint32_t ssrc;
		int32_t sxx, syy;
		int sxi, syi;

		sxx = ((((long long int)sx) << 16) / sz) + ox;
		syy = ((((long long int)sy) << 16) / sz) + oy;

		sxi = eina_f16p16_int_to(sxx);
		syi = eina_f16p16_int_to(syy);
		/* check that the calculated point is on the src */
		if (sxi >= 0 && sxi < sw && syi >= 0 && syi < sh)
		{
			*dst = *(src + ((syi * spitch) + sxi));
		}
		sx += a;
		sy += d;
		sz += g;

		dst++;
	}
}
