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
/* FIXME make this function on API */
static void _matrix_debug(Enesim_Matrix *m)
{
	printf("XX = %f XY = %f XZ = %f\n", MATRIX_XX(m), MATRIX_XY(m), MATRIX_XZ(m));
	printf("YX = %f YY = %f YZ = %f\n", MATRIX_YX(m), MATRIX_YY(m), MATRIX_YZ(m));
	printf("ZX = %f ZY = %f ZZ = %f\n", MATRIX_ZX(m), MATRIX_ZY(m), MATRIX_ZZ(m));
}
/* FIXME make this function on API */
static inline _quad_dump(Enesim_Quad *q)
{
	printf("Q = %f %f, %f %f, %f %f, %f %f\n", QUAD_X0(q), QUAD_Y0(q), QUAD_X1(q), QUAD_Y1(q), QUAD_X2(q), QUAD_Y2(q), QUAD_X3(q), QUAD_Y3(q), q[8]);
}

/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * 
 */
EAPI Enesim_Matrix * enesim_matrix_new(void)
{
	Enesim_Matrix *m;
	
	m = malloc(sizeof(Enesim_Matrix));
	enesim_matrix_identity(m);
	return m;
}
/**
 * 
 */
EAPI void enesim_matrix_free(Enesim_Matrix *m)
{
	free(m);
}
/**
 * 
 */
EAPI void enesim_matrix_values_set(Enesim_Matrix *m, float a, float b, float c,
		float d, float e, float f, float g, float h, float i)
{
	MATRIX_XX(m) = a;
	MATRIX_XY(m) = b;
	MATRIX_XZ(m) = c;
	MATRIX_YX(m) = d;
	MATRIX_YY(m) = e;
	MATRIX_YZ(m) = f;
	MATRIX_ZX(m) = g;
	MATRIX_ZY(m) = h;
	MATRIX_ZZ(m) = i;	
}
/**
 * 
 */
EAPI void enesim_matrix_values_get(Enesim_Matrix *m, float *a, float *b, float *c,
		float *d, float *e, float *f, float *g, float *h, float *i)
{
	if (a) *a = MATRIX_XX(m);
	if (b) *b = MATRIX_XY(m);
	if (c) *c = MATRIX_XZ(m);
	if (d) *d = MATRIX_YX(m);
	if (e) *e = MATRIX_YY(m);
	if (f) *f = MATRIX_YZ(m);
	if (g) *g = MATRIX_ZX(m);
	if (h) *h = MATRIX_ZY(m);
	if (i) *i = MATRIX_ZZ(m);	
}
/**
 * convert the transformation values to fixed point
 */
EAPI void enesim_matrix_fixed_values_get(Enesim_Matrix *m, Eina_F16p16 *a,
		Eina_F16p16 *b, Eina_F16p16 *c, Eina_F16p16 *d, Eina_F16p16 *e,
		Eina_F16p16 *f, Eina_F16p16 *g, Eina_F16p16 *h, Eina_F16p16 *i)
{
	if (a) *a = eina_f16p16_float_from(MATRIX_XX(m));
	if (b) *b = eina_f16p16_float_from(MATRIX_XY(m));
	if (c) *c = eina_f16p16_float_from(MATRIX_XZ(m));
	if (d) *d = eina_f16p16_float_from(MATRIX_YX(m));
	if (e) *e = eina_f16p16_float_from(MATRIX_YY(m));
	if (f) *f = eina_f16p16_float_from(MATRIX_YZ(m));
	if (g) *g = eina_f16p16_float_from(MATRIX_ZX(m));
	if (h) *h = eina_f16p16_float_from(MATRIX_ZY(m));
	if (i) *i = eina_f16p16_float_from(MATRIX_ZZ(m));
}
/**
 * 
 */
EAPI void enesim_matrix_point_transform(Enesim_Matrix *m, float x, float y, float *xr, float *yr)
{
	_matrix_debug(m);
	if (!MATRIX_ZX(m) && !MATRIX_ZY(m))
	{
		if (xr)
			*xr = (x * MATRIX_XX(m) + y * MATRIX_XY(m) + MATRIX_XZ(m));
		if (yr)
			*yr = (x * MATRIX_YX(m) + y * MATRIX_YY(m) + MATRIX_YZ(m));
	}
	else
	{
		if (xr)
			*xr = (x * MATRIX_XX(m) + y * MATRIX_XY(m) + MATRIX_XZ(m)) /
				(x * MATRIX_ZX(m) + y * MATRIX_ZY(m) + 1);
		if (yr)
			*yr = (x * MATRIX_YX(m) + y * MATRIX_YY(m) + MATRIX_YZ(m)) /
				(x * MATRIX_ZX(m) + y * MATRIX_ZY(m) + 1);
	}
}
/**
 * 
 */
EAPI void enesim_matrix_adjoint(Enesim_Matrix *m, Enesim_Matrix *a)
{
	float a11, a12, a13, a21, a22, a23, a31, a32, a33;
	
	/* cofactor */
	a11 = (MATRIX_YY(m) * MATRIX_ZZ(m)) - (MATRIX_YZ(m) * MATRIX_ZY(m));
	a12 = -1 * ((MATRIX_YX(m) * MATRIX_ZZ(m)) - (MATRIX_YZ(m) * MATRIX_ZX(m)));
	a13 = (MATRIX_YX(m) * MATRIX_ZY(m)) - (MATRIX_YY(m) * MATRIX_ZX(m));
	
	a21 = -1 * ((MATRIX_XY(m) * MATRIX_ZZ(m)) - (MATRIX_XZ(m) * MATRIX_ZY(m)));
	a22 = (MATRIX_XX(m) * MATRIX_ZZ(m)) - (MATRIX_XZ(m) * MATRIX_ZX(m));
	a23 = -1 * ((MATRIX_XX(m) * MATRIX_ZY(m)) - (MATRIX_XY(m) * MATRIX_ZX(m)));
	
	a31 = (MATRIX_XY(m) * MATRIX_YZ(m)) - (MATRIX_XZ(m) * MATRIX_YY(m));
	a32 = -1 * ((MATRIX_XX(m) * MATRIX_YZ(m)) - (MATRIX_XZ(m) * MATRIX_YX(m)));
	a33 = (MATRIX_XX(m) * MATRIX_YY(m)) - (MATRIX_XY(m) * MATRIX_YX(m));
	/* transpose */
	MATRIX_XX(a) = a11;
	MATRIX_XY(a) = a21;
	MATRIX_XZ(a) = a31;
	
	MATRIX_YX(a) = a12;
	MATRIX_YY(a) = a22;
	MATRIX_YZ(a) = a32;
	
	MATRIX_ZX(a) = a13;
	MATRIX_ZY(a) = a23;
	MATRIX_ZZ(a) = a33;
	
}
/**
 * 
 */
EAPI float enesim_matrix_determinant(Enesim_Matrix *m)
{
	float det;
	
	det = MATRIX_XX(m) * ((MATRIX_YY(m) * MATRIX_ZZ(m)) - (MATRIX_YZ(m) * MATRIX_ZY(m)));
	det -= MATRIX_XY(m) * ((MATRIX_YX(m) * MATRIX_ZZ(m)) - (MATRIX_YZ(m) * MATRIX_ZX(m)));
	det += MATRIX_XZ(m) * ((MATRIX_YX(m) * MATRIX_ZY(m)) - (MATRIX_YY(m) * MATRIX_ZX(m)));
	
	return det;
}
/**
 * 
 */
EAPI void enesim_matrix_divide(Enesim_Matrix *m, float scalar)
{
	MATRIX_XX(m) /= scalar;
	MATRIX_XY(m) /= scalar;
	MATRIX_XZ(m) /= scalar;
	
	MATRIX_YX(m) /= scalar;
	MATRIX_YY(m) /= scalar;
	MATRIX_YZ(m) /= scalar;
	
	MATRIX_ZX(m) /= scalar;
	MATRIX_ZY(m) /= scalar;
	MATRIX_ZZ(m) /= scalar;
}
/**
 * 
 */
EAPI void enesim_matrix_inverse(Enesim_Matrix *m, Enesim_Matrix *m2)
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
EAPI void enesim_matrix_compose(Enesim_Matrix *m1, Enesim_Matrix *m2, Enesim_Matrix *dst)
{
	float a11, a12, a13, a21, a22, a23, a31, a32, a33;

	a11 = (MATRIX_XX(m1) * MATRIX_XX(m2)) + (MATRIX_XY(m1) * MATRIX_YX(m2)) + (MATRIX_XZ(m1) * MATRIX_ZX(m2));
	a12 = (MATRIX_XX(m1) * MATRIX_XY(m2)) + (MATRIX_XY(m1) * MATRIX_YY(m2)) + (MATRIX_XZ(m1) * MATRIX_ZY(m2));
	a13 = (MATRIX_XX(m1) * MATRIX_XZ(m2)) + (MATRIX_XY(m1) * MATRIX_YZ(m2)) + (MATRIX_XZ(m1) * MATRIX_ZZ(m2));
	
	a21 = (MATRIX_YX(m1) * MATRIX_XX(m2)) + (MATRIX_YY(m1) * MATRIX_YX(m2)) + (MATRIX_YZ(m1) * MATRIX_ZX(m2));
	a22 = (MATRIX_YX(m1) * MATRIX_XY(m2)) + (MATRIX_YY(m1) * MATRIX_YY(m2)) + (MATRIX_YZ(m1) * MATRIX_ZY(m2));
	a23 = (MATRIX_YX(m1) * MATRIX_XZ(m2)) + (MATRIX_YY(m1) * MATRIX_YZ(m2)) + (MATRIX_YZ(m1) * MATRIX_ZZ(m2));

	a31 = (MATRIX_ZX(m1) * MATRIX_XX(m2)) + (MATRIX_ZY(m1) * MATRIX_YX(m2)) + (MATRIX_ZZ(m1) * MATRIX_ZX(m2));
	a32 = (MATRIX_ZX(m1) * MATRIX_XY(m2)) + (MATRIX_ZY(m1) * MATRIX_YY(m2)) + (MATRIX_ZZ(m1) * MATRIX_ZY(m2));
	a33 = (MATRIX_ZX(m1) * MATRIX_XZ(m2)) + (MATRIX_ZY(m1) * MATRIX_YZ(m2)) + (MATRIX_ZZ(m1) * MATRIX_ZZ(m2));
	
	MATRIX_XX(dst) = a11;
	MATRIX_XY(dst) = a12;
	MATRIX_XZ(dst) = a13;
	MATRIX_YX(dst) = a21;
	MATRIX_YY(dst) = a22;
	MATRIX_YZ(dst) = a23;
	MATRIX_ZX(dst) = a31;
	MATRIX_ZY(dst) = a32;
	MATRIX_ZZ(dst) = a33;
}
/**
 * 
 */
EAPI void enesim_matrix_translate(Enesim_Matrix *m, float tx, float ty)
{
	MATRIX_XX(m) = 1;
	MATRIX_XY(m) = 0;
	MATRIX_XZ(m) = tx;
	MATRIX_YX(m) = 0;
	MATRIX_YY(m) = 1;
	MATRIX_YZ(m) = ty;
	MATRIX_ZX(m) = 0;
	MATRIX_ZY(m) = 0;
	MATRIX_ZZ(m) = 1;	
}
/**
 * 
 */
EAPI void enesim_matrix_scale(Enesim_Matrix *m, float sx, float sy)
{
	MATRIX_XX(m) = sx;
	MATRIX_XY(m) = 0;
	MATRIX_XZ(m) = 0;
	MATRIX_YX(m) = 0;
	MATRIX_YY(m) = sy;
	MATRIX_YZ(m) = 0;
	MATRIX_ZX(m) = 0;
	MATRIX_ZY(m) = 0;
	MATRIX_ZZ(m) = 1;
}
/**
 * 
 */
EAPI void enesim_matrix_rotate(Enesim_Matrix *m, float rad)
{
	float c = cos(rad);
	float s = sin(rad);
	
	MATRIX_XX(m) = c;
	MATRIX_XY(m) = -s;
	MATRIX_XZ(m) = 0;
	MATRIX_YX(m) = s;
	MATRIX_YY(m) = c;
	MATRIX_YZ(m) = 0;
	MATRIX_ZX(m) = 0;
	MATRIX_ZY(m) = 0;
	MATRIX_ZZ(m) = 1;	
}
/**
 * 
 */
EAPI void enesim_matrix_identity(Enesim_Matrix *m)
{
	MATRIX_XX(m) = 1;
	MATRIX_XY(m) = 0;
	MATRIX_XZ(m) = 0;
	MATRIX_YX(m) = 0;
	MATRIX_YY(m) = 1;
	MATRIX_YZ(m) = 0;
	MATRIX_ZX(m) = 0;
	MATRIX_ZY(m) = 0;
	MATRIX_ZZ(m) = 1;
}
/**
 * 
 */
EAPI Enesim_Quad * enesim_quad_new(void)
{
	Enesim_Quad *q;
	
	q = malloc(sizeof(Enesim_Quad));
	return q;
}
/**
 * 
 */
EAPI void enesim_quad_free(Enesim_Quad *q)
{
	free(q);
}
/**
 * 
 */
EAPI void enesim_quad_rectangle_from(Enesim_Quad *q,
		Eina_Rectangle *r)
{
	QUAD_X0(q) = r->x;
	QUAD_Y0(q) = r->y;
	QUAD_X1(q) = r->x + r->w;
	QUAD_Y1(q) = r->y;
	QUAD_X2(q) = r->x + r->w;
	QUAD_Y2(q) = r->y + r->h;
	QUAD_X3(q) = r->x;
	QUAD_Y3(q) = r->y + r->h;
}
/**
 * 
 */
EAPI void enesim_quad_coords_get(Enesim_Quad *q, float *x1, float *y1,
		float *x2, float *y2, float *x3, float *y3, float *x4,
		float *y4)
{
	if (x1) *x1 = q->x0;
	if (y1) *y1 = q->y0;
	if (x2) *x2 = q->x1;
	if (y2) *y2 = q->y1;
	if (x3) *x3 = q->x2;
	if (y3) *y3 = q->y2;
	if (x4) *x4 = q->x3;
	if (y4) *y4 = q->y3;
}
/**
 * 
 */
EAPI void enesim_quad_coords_set(Enesim_Quad *q, float x1,
		float y1, float x2, float y2, float x3, float y3, float x4,
		float y4)
{
	QUAD_X0(q) = x1;
	QUAD_Y0(q) = y1;
	QUAD_X1(q) = x2;
	QUAD_Y1(q) = y2;
	QUAD_X2(q) = x3;
	QUAD_Y2(q) = y3;
	QUAD_X3(q) = x4;
	QUAD_Y3(q) = y4;
}
/**
 * 
 */
EAPI Eina_Bool enesim_matrix_square_quad_to(Enesim_Matrix *m,
		Enesim_Quad *q)
{
	float ex = QUAD_X0(q) - QUAD_X1(q) + QUAD_X2(q) - QUAD_X3(q); // x0 - x1 + x2 - x3
	float ey = QUAD_Y0(q) - QUAD_Y1(q) + QUAD_Y2(q) - QUAD_Y3(q); // y0 - y1 + y2 - y3 
	
	/* paralellogram */
	if (!ex && !ey)
	{
		/* create the affine matrix */
		MATRIX_XX(m) = QUAD_X1(q) - QUAD_X0(q);
		MATRIX_XY(m) = QUAD_X2(q) - QUAD_X1(q);
		MATRIX_XZ(m) = QUAD_X0(q);
		
		MATRIX_YX(m) = QUAD_Y1(q) - QUAD_Y0(q);
		MATRIX_YY(m) = QUAD_Y2(q) - QUAD_Y1(q);
		MATRIX_YZ(m) = QUAD_Y0(q);
		
		MATRIX_ZX(m) = 0;
		MATRIX_ZY(m) = 0;
		MATRIX_ZZ(m) = 1;
		
		return EINA_TRUE;
	}
	else
	{
		float dx1 = QUAD_X1(q) - QUAD_X2(q); // x1 - x2
		float dx2 = QUAD_X3(q) - QUAD_X2(q); // x3 - x2
		float dy1 = QUAD_Y1(q) - QUAD_Y2(q); // y1 - y2
		float dy2 = QUAD_Y3(q) - QUAD_Y2(q); // y3 - y2
		float den = (dx1 * dy2) - (dx2 * dy1);
		
		if (!den)
			return EINA_FALSE;
		
		MATRIX_ZX(m) = ((ex * dy2) - (dx2 * ey)) / den;
		MATRIX_ZY(m) = ((dx1 * ey) - (ex * dy1)) / den;
		MATRIX_ZZ(m) = 1;
		MATRIX_XX(m) = QUAD_X1(q) - QUAD_X0(q) + (MATRIX_ZX(m) * QUAD_X1(q));
		MATRIX_XY(m) = QUAD_X3(q) - QUAD_X0(q) + (MATRIX_ZY(m) * QUAD_X3(q));
		MATRIX_XZ(m) = QUAD_X0(q);
		MATRIX_YX(m) = QUAD_Y1(q) - QUAD_Y0(q) + (MATRIX_ZX(m) * QUAD_Y1(q));
		MATRIX_YY(m) = QUAD_Y3(q) - QUAD_Y0(q) + (MATRIX_ZY(m) * QUAD_Y3(q));
		MATRIX_YZ(m) = QUAD_Y0(q);

		return EINA_TRUE;
	}
}

/**
 * 
 */
EAPI Eina_Bool enesim_matrix_quad_square_to(Enesim_Matrix *m,
		Enesim_Quad *q)
{
	Enesim_Matrix tmp;
	
	/* compute square to quad */
	if (!enesim_matrix_square_quad_to(&tmp, q))
		return EINA_FALSE;

	enesim_matrix_inverse(&tmp, m);
	/* make the projective matrix always have 1 on zz */
	if (MATRIX_ZZ(m) != 1)
	{
		enesim_matrix_divide(m, MATRIX_ZZ(m));
	}
	
	return EINA_TRUE;
}

/**
 * Creates a projective matrix that maps a quadrangle to a quadrangle
 */
EAPI Eina_Bool enesim_matrix_quad_quad_to(Enesim_Matrix *m,
		Enesim_Quad *src, Enesim_Quad *dst)
{
	Enesim_Matrix tmp;

	/* TODO check that both are actually quadrangles */
	if (!enesim_matrix_quad_square_to(m, src))
		return EINA_FALSE;
	if (!enesim_matrix_square_quad_to(&tmp, dst))
		return EINA_FALSE;
	enesim_matrix_compose(&tmp, m, m);
	
	return EINA_TRUE;
}
