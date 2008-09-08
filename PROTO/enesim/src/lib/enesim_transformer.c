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
#include "enesim_common.h"
#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static Enesim_Transformation_Type _transformation_get(float *t)
{
	if ((t[MATRIX_ZX] != 0) || (t[MATRIX_ZY] != 0) || (t[MATRIX_ZZ] != 1))
	            return ENESIM_TRANSFORMATION_PROJECTIVE;
	else
	{
		if ((t[MATRIX_XX] == 1) && (t[MATRIX_XY] == 0) && (t[MATRIX_XZ] == 0) &&
				(t[MATRIX_YX] == 0) && (t[MATRIX_YY] == 1) && (t[MATRIX_YZ] == 0))
			return ENESIM_TRANSFORMATION_IDENTITY;
		else
			return ENESIM_TRANSFORMATION_AFFINE;
	}
}

/* convert the transformation values to fixed point */
static void _transformation_to_fixed(float *t, Eina_F16p16 *td)
{
	td[0] = eina_f16p16_float_from(t[0]);
	td[1] = eina_f16p16_float_from(t[1]);
	td[2] = eina_f16p16_float_from(t[2]);
	td[3] = eina_f16p16_float_from(t[3]);
	td[4] = eina_f16p16_float_from(t[4]);
	td[5] = eina_f16p16_float_from(t[5]);
	td[6] = eina_f16p16_float_from(t[6]);
	td[7] = eina_f16p16_float_from(t[7]);
	td[8] = eina_f16p16_float_from(t[8]);
}

static void _matrix_debug(float *m)
{
	printf("XX = %f XY = %f XZ = %f\n", m[MATRIX_XX], m[MATRIX_XY], m[MATRIX_XZ]);
	printf("YX = %f YY = %f YZ = %f\n", m[MATRIX_YX], m[MATRIX_YY], m[MATRIX_YZ]);
	printf("ZX = %f ZY = %f ZZ = %f\n", m[MATRIX_ZX], m[MATRIX_ZY], m[MATRIX_ZZ]);
}

static void _transformation_debug(Enesim_Transformation *t)
{
	
	printf("Transformation with rop = %d\n", t->rop);
	printf("Floating point matrix\n");
	_matrix_debug(t->matrix);
	printf("Fixed point matrix (16p16 format)\n");
	printf("XX = %u XY = %u XZ = %u\n", t->matrix_fixed[MATRIX_XX], t->matrix_fixed[MATRIX_XY], t->matrix_fixed[MATRIX_XZ]);
	printf("YX = %u YY = %u YZ = %u\n", t->matrix_fixed[MATRIX_YX], t->matrix_fixed[MATRIX_YY], t->matrix_fixed[MATRIX_YZ]);
	printf("ZX = %u ZY = %u ZZ = %u\n", t->matrix_fixed[MATRIX_ZX], t->matrix_fixed[MATRIX_ZY], t->matrix_fixed[MATRIX_ZZ]);
}

/* TODO
 * remove this
 */
#define INTERP_256(a, c0, c1) \
 ( (((((((c0) >> 8) & 0xff00ff) - (((c1) >> 8) & 0xff00ff)) * (a)) \
   + ((c1) & 0xff00ff00)) & 0xff00ff00) + \
   (((((((c0) & 0xff00ff) - ((c1) & 0xff00ff)) * (a)) >> 8) \
   + ((c1) & 0xff00ff)) & 0xff00ff) )
#define interp_256 INTERP_256

/*
 * TODO
 * this function is very expensive because we have convert every time to argb
 * in case of argb8888_pre means several muls and divs
 */
static unsigned int convolution2x2(Enesim_Surface_Data *data, 
		Enesim_Surface_Format fmt, Eina_F16p16 x, Eina_F16p16 y,
		unsigned w, unsigned int h)
{
	Enesim_Surface_Data tmp;
	unsigned int p3 = 0, p2 = 0, p1 = 0, p0 = 0;
	int ax, ay;
	int sx, sy;

	/* 8 bits error to alpha */
	ax = 1 + (eina_f16p16_fracc_get(x) >> 8);
	ay = 1 + (eina_f16p16_fracc_get(y) >> 8);
	/* integer values for the coordinates */
	sx = eina_f16p16_int_to(x);
	sy = eina_f16p16_int_to(y);

	if ((sx > -1) && (sy > -1))
	{
		p0 = enesim_surface_data_to_argb(data, fmt);
	}
	if ((sy > -1) && ((sx + 1) < w))
	{
		tmp = *data;
		
		enesim_surface_data_increment(&tmp, fmt, 1);
		p1 = enesim_surface_data_to_argb(&tmp, fmt);
	}
	if ((sy + 1) < h)
	{
		if (sx > -1)
		{
			tmp = *data;
			enesim_surface_data_increment(&tmp, fmt, w);
			p2 = enesim_surface_data_to_argb(&tmp, fmt);
		}
		if ((sx + 1) < w)
		{
			tmp = *data;
			enesim_surface_data_increment(&tmp, fmt, w + 1);
			p3 = enesim_surface_data_to_argb(&tmp, fmt);
		}
	}
	if (p0 != p1)
		p0 = interp_256(ax, p1, p0);
	if (p2 != p3)
		p2 = interp_256(ax, p3, p2);
	if (p0 != p2)
		p0 = interp_256(ay, p2, p0);
	return p0;
}

static void transformer_identity_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	Enesim_Surface_Data sdata, ddata;
	Enesim_Drawer_Span spfnc;
	int h;
	
	/* TODO, pixel or pixel_color */
	spfnc = enesim_drawer_span_pixel_get(t->rop, ds->format, ss->format);
	if (!spfnc)
		return;
	
	enesim_surface_data_get(ss, &sdata);
	enesim_surface_data_get(ds, &ddata);
	enesim_surface_data_increment(&sdata, ss->format, (srect->y * ss->w) + srect->x);
	enesim_surface_data_increment(&ddata, ds->format, (drect->y * ds->w) + drect->x);
	h = drect->h;
	
	while (h--)
	{
		spfnc(&ddata, drect->w, &sdata, /* mul_color */0, NULL);
		enesim_surface_data_increment(&sdata, ss->format, ss->w);
		enesim_surface_data_increment(&ddata, ds->format, ds->w);
	}	
}

static void transformer_affine_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	Enesim_Surface_Data sdata, ddata;
	Eina_F16p16 sx, sy;
	Enesim_Drawer_Point ptfnc;
	int h;

	/* force an alpha color, return if we dont have a point function */
	ptfnc = enesim_drawer_point_color_get(t->rop, ds->format, 0x55555555);
	if (!ptfnc)
		return;
	sx = eina_f16p16_mul(t->matrix_fixed[MATRIX_XX], drect->x) + 
		eina_f16p16_mul(t->matrix_fixed[MATRIX_XY], drect->y) +
		t->matrix_fixed[MATRIX_XZ];
	sy = eina_f16p16_mul(t->matrix_fixed[MATRIX_YX], drect->x) +
		eina_f16p16_mul(t->matrix_fixed[MATRIX_YY], drect->y) +
		t->matrix_fixed[MATRIX_YZ];
	
	enesim_surface_data_get(ds, &ddata);
	enesim_surface_data_get(ss, &sdata);
	enesim_surface_data_increment(&ddata, ds->format, (drect->y * ds->w) + drect->x);

	h = drect->h;
	while (h--)
	{
		Enesim_Surface_Data ddata2;
		int w;
		Eina_F16p16 sxx, syy;
		
		ddata2 = ddata;
		sxx = sx;
		syy = sy;

		w = drect->w;
		while (w--)
		{
			int six, siy;
			Enesim_Surface_Data sdata2;
			
			sdata2 = sdata;
			six = eina_f16p16_int_to(sxx);
			siy = eina_f16p16_int_to(syy);
			
			/* check that we are inside the source rectangle */
			if ((eina_rectangle_xcoord_inside(srect, six)) && (eina_rectangle_ycoord_inside(srect, siy)))
			{
				unsigned int argb;
				
				/* use 2x2 convolution kernel to interpolate */
				enesim_surface_data_increment(&sdata2, ss->format, (siy * ss->w) + six);
				argb = convolution2x2(&sdata2, ss->format, sxx, syy, ss->w, ss->h);
				ptfnc(&ddata2, NULL, argb, NULL);
			}
			enesim_surface_data_increment(&ddata2, ds->format, 1);
			sxx += t->matrix_fixed[MATRIX_XX];
			syy += t->matrix_fixed[MATRIX_YX];
		}
		enesim_surface_data_increment(&ddata, ds->format, ds->w);
		sx += t->matrix_fixed[MATRIX_XY];
		sy += t->matrix_fixed[MATRIX_YY];
	}
}

static void transformer_projective_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	Enesim_Surface_Data sdata, ddata;
	Eina_F16p16 sx, sy, sz;
	Enesim_Drawer_Point ptfnc;
	int h;

	/* force an alpha color, return if we dont have a point function */
	ptfnc = enesim_drawer_point_color_get(t->rop, ds->format, 0x55555555);
	if (!ptfnc)
		return;
	
	sx = eina_f16p16_mul(t->matrix_fixed[MATRIX_XX], drect->x) + 
			eina_f16p16_mul(t->matrix_fixed[MATRIX_XY], drect->y) +
			t->matrix_fixed[MATRIX_XZ];
	sy = eina_f16p16_mul(t->matrix_fixed[MATRIX_YX], drect->x) +
			eina_f16p16_mul(t->matrix_fixed[MATRIX_YY], drect->y) +
			t->matrix_fixed[MATRIX_YZ];
	sz = eina_f16p16_mul(t->matrix_fixed[MATRIX_ZX], drect->x) +
			eina_f16p16_mul(t->matrix_fixed[MATRIX_ZY], drect->y) +
			t->matrix_fixed[MATRIX_ZZ];
	
	enesim_surface_data_get(ss, &sdata);
	enesim_surface_data_get(ds, &ddata);
	enesim_surface_data_increment(&sdata, ss->format, (srect->y * ss->w) + srect->x);
	enesim_surface_data_increment(&ddata, ds->format, (drect->y * ds->w) + drect->x);
	
	h = drect->h;
	while (h--)
	{
		Enesim_Surface_Data ddata2;
		Eina_F16p16 sxx, syy, szz;
		int w;
			
		ddata2 = ddata;
		sxx = sx;
		syy = sy;
		szz = sz;

		w = drect->w;
		while (w--)
		{
			int six, siy;
			Enesim_Surface_Data sdata2;
			Eina_F16p16 sxxx, syyy;
						
			sdata2 = sdata;
			/* TODO set the destination to zero? */
			if (!szz)
				goto iterate;
				
			sxxx = ((((long long int)sxx) << 16) / szz); // + x origin
			syyy = ((((long long int)syy) << 16) / szz); // + y origin
			six = eina_f16p16_int_to(sxxx);
			siy = eina_f16p16_int_to(syyy);
			/* check that we are inside the source rectangle */
			if ((eina_rectangle_xcoord_inside(srect, six)) && (eina_rectangle_ycoord_inside(srect, siy)))
			{
				unsigned int argb;
							
				/* use 2x2 convolution kernel to interpolate */
				enesim_surface_data_increment(&sdata2, ss->format, (siy * ss->w) + six);
				argb = convolution2x2(&sdata2, ss->format, sxxx, syyy, ss->w, ss->h);
				ptfnc(&ddata2, NULL, argb, NULL);
			}
iterate:
			enesim_surface_data_increment(&ddata2, ds->format, 1);
			sxx += t->matrix_fixed[MATRIX_XX];
			syy += t->matrix_fixed[MATRIX_YX];
			szz += t->matrix_fixed[MATRIX_ZX];
		}
		enesim_surface_data_increment(&ddata, ds->format, ds->w);
		sx += t->matrix_fixed[MATRIX_XY];
		sy += t->matrix_fixed[MATRIX_YY];
		sz += t->matrix_fixed[MATRIX_ZY];
	}
}

static Enesim_Transformer_Func _functions[ENESIM_TRANSFORMATIONS] = {
		[ENESIM_TRANSFORMATION_AFFINE] = &transformer_affine_no_no,
		[ENESIM_TRANSFORMATION_IDENTITY] = &transformer_identity_no_no,
		[ENESIM_TRANSFORMATION_PROJECTIVE] = &transformer_projective_no_no,
};

/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * 
 */
EAPI void enesim_matrix_point_transform(float *m, float x, float y, float *xr, float *yr)
{
	_matrix_debug(m);
	if (!m[MATRIX_ZX] && !m[MATRIX_ZY])
	{
		if (xr)
			*xr = (x * m[MATRIX_XX] + y * m[MATRIX_XY] + m[MATRIX_XZ]);
		if (yr)
			*yr = (x * m[MATRIX_YX] + y * m[MATRIX_YY] + m[MATRIX_YZ]);
	}
	else
	{
		if (xr)
			*xr = (x * m[MATRIX_XX] + y * m[MATRIX_XY] + m[MATRIX_XZ]) /
				(x * m[MATRIX_ZX] + y * m[MATRIX_ZY] + 1);
		if (yr)
			*yr = (x * m[MATRIX_YX] + y * m[MATRIX_YY] + m[MATRIX_YZ]) /
				(x * m[MATRIX_ZX] + y * m[MATRIX_ZY] + 1);
	}
}
/**
 * 
 */
EAPI void enesim_matrix_adjoint(float *m, float *a)
{
	float a11, a12, a13, a21, a22, a23, a31, a32, a33;
	
	/* cofactor */
	a11 = (m[MATRIX_YY] * m[MATRIX_ZZ]) - (m[MATRIX_YZ] * m[MATRIX_ZY]);
	a12 = -1 * ((m[MATRIX_YX] * m[MATRIX_ZZ]) - (m[MATRIX_YZ] * m[MATRIX_ZX]));
	a13 = (m[MATRIX_YX] * m[MATRIX_ZY]) - (m[MATRIX_YY] * m[MATRIX_ZX]);
	
	a21 = -1 * ((m[MATRIX_XY] * m[MATRIX_ZZ]) - (m[MATRIX_XZ] * m[MATRIX_ZY]));
	a22 = (m[MATRIX_XX] * m[MATRIX_ZZ]) - (m[MATRIX_XZ] * m[MATRIX_ZX]);
	a23 = -1 * ((m[MATRIX_XX] * m[MATRIX_ZY]) - (m[MATRIX_XY] * m[MATRIX_ZX]));
	
	a31 = (m[MATRIX_XY] * m[MATRIX_YZ]) - (m[MATRIX_XZ] * m[MATRIX_YY]);
	a32 = -1 * ((m[MATRIX_XX] * m[MATRIX_YZ]) - (m[MATRIX_XZ] * m[MATRIX_YX]));
	a33 = (m[MATRIX_XX] * m[MATRIX_YY]) - (m[MATRIX_XY] * m[MATRIX_YX]);
	/* transpose */
	a[MATRIX_XX] = a11;
	a[MATRIX_XY] = a21;
	a[MATRIX_XZ] = a31;
	
	a[MATRIX_YX] = a12;
	a[MATRIX_YY] = a22;
	a[MATRIX_YZ] = a32;
	
	a[MATRIX_ZX] = a13;
	a[MATRIX_ZY] = a23;
	a[MATRIX_ZZ] = a33;
	
}
/**
 * 
 */
EAPI float enesim_matrix_determinant(float *m)
{
	float det;
	
	det = m[MATRIX_XX] * ((m[MATRIX_YY] * m[MATRIX_ZZ]) - (m[MATRIX_YZ] * m[MATRIX_ZY]));
	det -= m[MATRIX_XY] * ((m[MATRIX_YX] * m[MATRIX_ZZ]) - (m[MATRIX_YZ] * m[MATRIX_ZX]));
	det += m[MATRIX_XZ] * ((m[MATRIX_YX] * m[MATRIX_ZY]) - (m[MATRIX_YY] * m[MATRIX_ZX]));
	
	return det;
}
/**
 * 
 */
EAPI void enesim_matrix_divide(float *m, float scalar)
{
	m[MATRIX_XX] /= scalar;
	m[MATRIX_XY] /= scalar;
	m[MATRIX_XZ] /= scalar;
	
	m[MATRIX_YX] /= scalar;
	m[MATRIX_YY] /= scalar;
	m[MATRIX_YZ] /= scalar;
	
	m[MATRIX_ZX] /= scalar;
	m[MATRIX_ZY] /= scalar;
	m[MATRIX_ZZ] /= scalar;
}
/**
 * 
 */
EAPI void enesim_matrix_inverse(float *m, float *m2)
{
	float scalar;
	
	/* determinant */
	scalar = enesim_matrix_determinant(m);
	/* do its adjoint */
	enesim_matrix_adjoint(m, m2);
	/* divide */
	enesim_matrix_divide(m2, scalar);
}
/**
 * 
 */
EAPI void enesim_matrix_compose(float *m1, float *m2, float *dst)
{
	int i;
	float tmp[MATRIX_SIZE];
	
	tmp[MATRIX_XX] = (m1[MATRIX_XX] * m2[MATRIX_XX]) + (m1[MATRIX_XY] * m2[MATRIX_YX]) + (m1[MATRIX_XZ] * m2[MATRIX_ZX]);
	tmp[MATRIX_XY] = (m1[MATRIX_XX] * m2[MATRIX_XY]) + (m1[MATRIX_XY] * m2[MATRIX_YY]) + (m1[MATRIX_XZ] * m2[MATRIX_ZY]);
	tmp[MATRIX_XZ] = (m1[MATRIX_XX] * m2[MATRIX_XZ]) + (m1[MATRIX_XY] * m2[MATRIX_YZ]) + (m1[MATRIX_XZ] * m2[MATRIX_ZZ]);
	
	tmp[MATRIX_YX] = (m1[MATRIX_YX] * m2[MATRIX_XX]) + (m1[MATRIX_YY] * m2[MATRIX_YX]) + (m1[MATRIX_YZ] * m2[MATRIX_ZX]);
	tmp[MATRIX_YY] = (m1[MATRIX_YX] * m2[MATRIX_XY]) + (m1[MATRIX_YY] * m2[MATRIX_YY]) + (m1[MATRIX_YZ] * m2[MATRIX_ZY]);
	tmp[MATRIX_YZ] = (m1[MATRIX_YX] * m2[MATRIX_XZ]) + (m1[MATRIX_YY] * m2[MATRIX_YZ]) + (m1[MATRIX_YZ] * m2[MATRIX_ZZ]);

	tmp[MATRIX_ZX] = (m1[MATRIX_ZX] * m2[MATRIX_XX]) + (m1[MATRIX_ZY] * m2[MATRIX_YX]) + (m1[MATRIX_ZZ] * m2[MATRIX_ZX]);
	tmp[MATRIX_ZY] = (m1[MATRIX_ZX] * m2[MATRIX_XY]) + (m1[MATRIX_ZY] * m2[MATRIX_YY]) + (m1[MATRIX_ZZ] * m2[MATRIX_ZY]);
	tmp[MATRIX_ZZ] = (m1[MATRIX_ZX] * m2[MATRIX_XZ]) + (m1[MATRIX_ZY] * m2[MATRIX_YZ]) + (m1[MATRIX_ZZ] * m2[MATRIX_ZZ]);

	for (i = 0; i < MATRIX_SIZE; i++)
		dst[i] = tmp[i];
}
/**
 * 
 */
EAPI void enesim_matrix_translate(float *t, float tx, float ty)
{
	t[MATRIX_XX] = 1;
	t[MATRIX_XY] = 0;
	t[MATRIX_XZ] = tx;
	t[MATRIX_YX] = 0;
	t[MATRIX_YY] = 1;
	t[MATRIX_YZ] = ty;
	t[MATRIX_ZX] = 0;
	t[MATRIX_ZY] = 0;
	t[MATRIX_ZZ] = 1;	
}
/**
 * 
 */
EAPI void enesim_matrix_scale(float *t, float sx, float sy)
{
	t[MATRIX_XX] = sx;
	t[MATRIX_XY] = 0;
	t[MATRIX_XZ] = 0;
	t[MATRIX_YX] = 0;
	t[MATRIX_YY] = sy;
	t[MATRIX_YZ] = 0;
	t[MATRIX_ZX] = 0;
	t[MATRIX_ZY] = 0;
	t[MATRIX_ZZ] = 1;
}
/**
 * 
 */
EAPI void enesim_matrix_rotate(float *t, float rad)
{
	float c = cos(rad);
	float s = sin(rad);
	
	t[MATRIX_XX] = c;
	t[MATRIX_XY] = -s;
	t[MATRIX_XZ] = 0;
	t[MATRIX_YX] = s;
	t[MATRIX_YY] = c;
	t[MATRIX_YZ] = 0;
	t[MATRIX_ZX] = 0;
	t[MATRIX_ZY] = 0;
	t[MATRIX_ZZ] = 1;	
}
/**
 * 
 */
EAPI void enesim_matrix_identity(float *t)
{
	t[MATRIX_XX] = 1;
	t[MATRIX_XY] = 0;
	t[MATRIX_XZ] = 0;
	t[MATRIX_YX] = 0;
	t[MATRIX_YY] = 1;
	t[MATRIX_YZ] = 0;
	t[MATRIX_ZX] = 0;
	t[MATRIX_ZY] = 0;
	t[MATRIX_ZZ] = 1;
}
/**
 * 
 */
EAPI void enesim_quad_rectangle_from(float *q,
		Eina_Rectangle *r)
{
	q[0] = r->x;
	q[1] = r->y;
	q[2] = r->x + r->w;
	q[3] = r->y;
	q[4] = r->x + r->w;
	q[5] = r->y + r->h;
	q[6] = r->x;
	q[7] = r->y + r->h;
}
/**
 * 
 */
EAPI void enesim_quad_coords_from(float *q, float x1,
		float y1, float x2, float y2, float x3, float y3, float x4,
		float y4)
{
	q[0] = x1;
	q[1] = y1;
	q[2] = x2;
	q[3] = y2;
	q[4] = x3;
	q[5] = y3;
	q[6] = x4;
	q[7] = y4;
}

static inline quad_dump(float *q)
{
	printf("Q = %f %f, %f %f, %f %f, %f %f\n", q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], q[8]);
}

/**
 * 
 */
EAPI Eina_Bool enesim_matrix_square_quad_to(float *m,
		float *q)
{
	float ex = q[0] - q[2] + q[4] - q[6]; // x0 - x1 + x2 - x3
	float ey = q[1] - q[3] + q[5] - q[7]; // y0 - y1 + y2 - y3 
	
	/* paralellogram */
	if (!ex && !ey)
	{
		/* create the affine matrix */
		m[MATRIX_XX] = q[2] - q[0];
		m[MATRIX_XY] = q[4] - q[2];
		m[MATRIX_XZ] = q[0];
		
		m[MATRIX_YX] = q[3] - q[1];
		m[MATRIX_YY] = q[5] - q[3];
		m[MATRIX_YZ] = q[1];
		
		m[MATRIX_ZX] = 0;
		m[MATRIX_ZY] = 0;
		m[MATRIX_ZZ] = 1;
		
		return EINA_TRUE;
	}
	else
	{
		float dx1 = q[2] - q[4]; // x1 - x2
		float dx2 = q[6] - q[4]; // x3 - x2
		float dy1 = q[3] - q[5]; // y1 - y2
		float dy2 = q[7] - q[5]; // y3 - y2
		float den = (dx1 * dy2) - (dx2 * dy1);
		
		if (!den)
			return EINA_FALSE;
		
		m[MATRIX_ZX] = ((ex * dy2) - (dx2 * ey)) / den;
		m[MATRIX_ZY] = ((dx1 * ey) - (ex * dy1)) / den;
		m[MATRIX_ZZ] = 1;
		m[MATRIX_XX] = q[2] - q[0] + (m[MATRIX_ZX] * q[2]);
		m[MATRIX_XY] = q[6] - q[0] + (m[MATRIX_ZY] * q[6]);
		m[MATRIX_XZ] = q[0];
		m[MATRIX_YX] = q[3] - q[1] + (m[MATRIX_ZX] * q[3]);
		m[MATRIX_YY] = q[7] - q[1] + (m[MATRIX_ZY] * q[7]);
		m[MATRIX_YZ] = q[1];

		return EINA_TRUE;
	}
}

/**
 * 
 */
EAPI Eina_Bool enesim_matrix_quad_square_to(float *m,
		float *q)
{
	float tmp[9];
	
	/* compute square to quad */
	if (!enesim_matrix_square_quad_to(tmp, q))
		return EINA_FALSE;

	enesim_matrix_inverse(tmp, m);
	/* make the projective matrix always have 1 on zz */
	if (m[MATRIX_ZZ] != 1)
	{
		enesim_matrix_divide(m, m[MATRIX_ZZ]);
	}
	
	return EINA_TRUE;
}

/**
 * Creates a projective matrix that maps a quadrangle to a quadrangle
 */
EAPI Eina_Bool enesim_matrix_quad_quad_to(float *m,
		float *src, float *dst)
{
	float tmp[9];
	
	/* TODO check that both are actually quadrangles */
	if (!enesim_matrix_quad_square_to(m, src))
		return EINA_FALSE;
	if (!enesim_matrix_square_quad_to(tmp, dst))
		return EINA_FALSE;
	enesim_matrix_compose(tmp, m, m);
	
	return EINA_TRUE;
}
/**
 * 
 */
EAPI Enesim_Transformation * enesim_transformation_new(void)
{
	Enesim_Transformation *t;
	
	t = calloc(1, sizeof(Enesim_Transformation));
	/* setup the identity matrix */
	t->matrix[MATRIX_XX] = 1;
	t->matrix[MATRIX_YY] = 1;
	t->matrix[MATRIX_ZZ] = 1;
	_transformation_to_fixed(t->matrix, t->matrix_fixed);
	t->type = ENESIM_TRANSFORMATION_IDENTITY;
	ENESIM_MAGIC_SET(t, ENESIM_TRANSFORMATION_MAGIC);
	
	return t;
}
/**
 * 
 */
EAPI void enesim_transformation_set(Enesim_Transformation *t, float *tx)
{
	int i;
	
	ENESIM_ASSERT(t, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_MAGIC_CHECK(t, ENESIM_TRANSFORMATION_MAGIC);
	
	for (i = 0; i < MATRIX_SIZE; i++)
	{
		t->matrix[i] = tx[i];
	}
	/* TODO the type should on the fixed or on the float */
	_transformation_to_fixed(t->matrix, t->matrix_fixed);
	t->type = _transformation_get(t->matrix);
}
/**
 * 
 */
EAPI void enesim_transformation_get(Enesim_Transformation *t, float *tx)
{
	int i;

	for (i = 0; i < MATRIX_SIZE; i++)
	{
		tx[i] = t->matrix[i];
	}
}
/**
 * 
 */
EAPI void enesim_transformation_rop_set(Enesim_Transformation *t, Enesim_Rop rop)
{
	t->rop = rop;
}
/**
 * 
 */
EAPI Eina_Bool enesim_transformation_apply(Enesim_Transformation *t,
		Enesim_Surface *s, Eina_Rectangle *sr, Enesim_Surface *d,
		Eina_Rectangle *dr)
{
	Eina_Rectangle csr, cdr;
	Enesim_Transformer_Func tfunc;
	Enesim_Scale xscale, yscale;
	
	ENESIM_ASSERT(t, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_ASSERT(s, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_ASSERT(d, ENESIM_ERROR_HANDLE_INVALID);
	
	ENESIM_MAGIC_CHECK(t, ENESIM_TRANSFORMATION_MAGIC);
	ENESIM_MAGIC_CHECK(s, ENESIM_SURFACE_MAGIC);
	ENESIM_MAGIC_CHECK(d, ENESIM_SURFACE_MAGIC);
	
	xscale = ENESIM_SCALE_NO;
	yscale = ENESIM_SCALE_NO;
	
	/* TODO check if we are out of bounds */
	/* setup the destination clipping */
	cdr.x = 0;
	cdr.y = 0;
	cdr.w = d->w;
	cdr.h = d->h;
	if (sr)
	{
		/* TODO check the return value of the intersection */
		if (eina_rectangle_intersection(&cdr, dr) == EINA_FALSE)
			return EINA_FALSE;
		if (eina_rectangle_is_empty(&cdr))
		{
			//ENESIM_ERROR(ENESIM_ERROR_DSTRECT_INVALID);
			return EINA_FALSE;
		}
	}
	/* setup the source clipping */
	csr.x = 0;
	csr.y = 0;
	csr.w = s->w;
	csr.h = s->h;
	if (dr)
	{
		/* TODO check the return value of the intersection */
		if (eina_rectangle_intersection(&csr, sr) == EINA_FALSE)
			return EINA_FALSE;
		if (eina_rectangle_is_empty(&csr))
		{
			//ENESIM_ERROR(ENESIM_ERROR_SRCRECT_INVALID);
			return EINA_FALSE;
		}
	}
	/* check if we are going to scale */
	/* x scaling */
	if (cdr.w > csr.w)
		xscale = ENESIM_SCALE_UP; 
	else if (cdr.w < csr.w)
		xscale = ENESIM_SCALE_DOWN;
	/* y scaling */
	if (cdr.h > csr.h)
		yscale = ENESIM_SCALE_UP;
	else if (cdr.h < csr.h)
		yscale = ENESIM_SCALE_DOWN;
	/* get the correct transfomer function */
	/* TODO use xscale and yscale */
	if (!(tfunc = _functions[t->type]))
	{
		//ENESIM_ERROR(ENESIM_ERROR_TRANSFORMATION_NOT_SUPPORTED);
		return EINA_FALSE;
	}
	tfunc(t, s, sr, d, dr);
	return EINA_TRUE;
}
/**
 * 
 */
EAPI void enesim_transformation_origin_set(Enesim_Transformation *t, float ox, float oy)
{
	t->ox = ox;
	t->oy = oy;
}
/**
 * 
 */
EAPI void enesim_transformation_origin_get(Enesim_Transformation *t, float *ox, float *oy)
{
	if (ox) *ox = t->ox;
	if (oy) *oy = t->oy;
}
