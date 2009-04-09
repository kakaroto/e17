/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
static Eina_Bool _affine(Renderer_Transformer *t, int x, int y, int len, Enesim_Surface_Data *dst)
{
	Eina_F16p16 sx, sy;
	Eina_F16p16 xfp, yfp;
	Enesim_Surface_Data ddata = *dst;
	Enesim_Surface_Data sdata;
	int dx = 0;
	int sw; /* TODO replace this with pitch */
	int sh;
	Eina_F16p16 a, b, c, d, e, f, g, h, i;


	enesim_surface_data_get(t->src, &sdata);
	enesim_surface_size_get(t->src, &sw, &sh);
	xfp = eina_f16p16_int_from(x);
	yfp = eina_f16p16_int_from(y);
	a = t->matrix.fixed[0];
	b = t->matrix.fixed[1];
	c = t->matrix.fixed[2];
	d = t->matrix.fixed[3];
	e = t->matrix.fixed[4];
	f = t->matrix.fixed[5];
	/* projective
	 * g = t->matrix.fixed[6];
	 * h = t->matrix.fixed[7];
	 * i = t->matrix.fixed[8];
	 */

	sx = eina_f16p16_mul(a, xfp) + eina_f16p16_mul(b, yfp) + c;
	sy = eina_f16p16_mul(d, xfp) + eina_f16p16_mul(e, yfp) + f;
	while (dx < len)
	{
		Enesim_Surface_Pixel spixel;
		int sxi, syi;

		/* projective
		 * sxx = ((((long long int)sx) << 16) / sz) + ox;
		 * syy = ((((long long int)sy) << 16) / sz) + oy;
		 */
		sxi = eina_f16p16_int_to(sx);
		syi = eina_f16p16_int_to(sy);
		/* check that the calculated point is on the src */
		//printf("x = %d y = %d\n", sxi, syi);
		if (sxi < 0 || sxi > sw || syi < 0 || syi > sh)
		{
			spixel.plane0 = 0;
		}
		else
		{
			spixel.plane0 = *(sdata.plane0 + ((syi * sw) + sxi));
		}
		/* fill */
		*ddata.plane0 = spixel.plane0;
		sx += a;
		sy += d;
		/* projective
		 * sz += g;
		 */
		ddata.plane0++;
		dx++;
	}
	return EINA_TRUE;
}
