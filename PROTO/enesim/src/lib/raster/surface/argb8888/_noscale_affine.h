/**
 * parameters:
 * Enesim_Surface *ss
 * Enesim_Rectangle *srect
 * Enesim_Surface *ds
 * Enesim_Rectangle *drect
 * enesim_16p16_t *t
 * macros:
 * ARGB8888_CONVOLUTION_CALL
 * 
 */
/* TODO fix this to only use one rectangle, as both are equal?. */
void 
ARGB8888_NOSCALE_AFFINE_ITERATOR_PROTO
{
	DATA32 *dp;
	DATA32 *sp;
	enesim_16p16_t sx, sy;
	int h;
	
	sx = enesim_16p16_mul(t[MATRIX_XX], drect->x) + enesim_16p16_mul(t[MATRIX_XY],drect->y) + t[MATRIX_XZ];
	sy = enesim_16p16_mul(t[MATRIX_YX], drect->x) + enesim_16p16_mul(t[MATRIX_YY],drect->y) + t[MATRIX_YZ];
	
	dp = ds->data.argb8888.data + (drect->y * ds->w) + drect->x;
	h = drect->h;
	while (h--)
	{
		DATA32 *d = dp;
		DATA32 *e = d + drect->w;
		enesim_16p16_t sxx, syy;
		
		sxx = sx;
		syy = sy;

		while (d < e)
		{

			int six, siy;
			
			six = enesim_16p16_int_to(sxx);
			siy = enesim_16p16_int_to(syy);
			
			/* check that we are inside the source rectangle */
			if ((enesim_rectangle_xcoord_inside(srect, six)) && (enesim_rectangle_ycoord_inside(srect, siy)))
			{
				/* use 2x2 convolution kernel to interpolate */
				sp = ss->data.argb8888.data + (siy * ss->w) + six;
				ARGB8888_CONVOLUTION_CALL
			}
			d++;
			sxx += t[MATRIX_XX];
			syy += t[MATRIX_YX];
		}
		dp += ds->w;
		sx += t[MATRIX_XY];
		sy += t[MATRIX_YY];
	}
}
