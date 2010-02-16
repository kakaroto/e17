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
#ifndef ENESIM_MATRIX_H_
#define ENESIM_MATRIX_H_
/**
 * @defgroup Enesim_Matrix_Group Matrix
 * @{
 * @todo
 * - Create all this Macros
 * - Replace all float * with Enesim_Matrix
 */
#define ENESIM_MATRIX_XX(m) m[0]

typedef enum
{
	ENESIM_MATRIX_IDENTITY,
	ENESIM_MATRIX_AFFINE,
	ENESIM_MATRIX_PROJECTIVE,
	ENESIM_MATRIX_TYPES
} Enesim_Matrix_Type;

typedef struct _Enesim_Matrix
{
	float xx, xy, xz;
	float yx, yy, yz;
	float zx, zy, zz;
} Enesim_Matrix; /**< Floating point matrix handler */

typedef struct _Enesim_F16p16_Matrix
{
	Eina_F16p16 xx, xy, xz;
	Eina_F16p16 yx, yy, yz;
	Eina_F16p16 zx, zy, zz;
} Enesim_F16p16_Matrix; /**< Fixed point matrix handler */

typedef struct _Enesim_Quad
{
	float x0, y0;
	float x1, y1;
	float x2, y2;
	float x3, y3;
} Enesim_Quad; /**< Quadrangle handler */

EAPI Enesim_Matrix_Type enesim_matrix_type_get(Enesim_Matrix *m);
EAPI void enesim_matrix_values_set(Enesim_Matrix *m, float a, float b, float c,
		float d, float e, float f, float g, float h, float i);
EAPI void enesim_matrix_values_get(Enesim_Matrix *m, float *a, float *b,
		float *c, float *d, float *e, float *f, float *g, float *h,
		float *i);
EAPI void enesim_matrix_fixed_values_get(Enesim_Matrix *m, Eina_F16p16 *a,
		Eina_F16p16 *b, Eina_F16p16 *c, Eina_F16p16 *d, Eina_F16p16 *e,
		Eina_F16p16 *f, Eina_F16p16 *g, Eina_F16p16 *h, Eina_F16p16 *i);
EAPI void enesim_matrix_f16p16_matrix_to(Enesim_Matrix *m,
		Enesim_F16p16_Matrix *fm);

EAPI void enesim_matrix_compose(Enesim_Matrix *m1, Enesim_Matrix *m2,
		Enesim_Matrix *dst);
EAPI void enesim_f16p16_matrix_compose(Enesim_F16p16_Matrix *m1,
		Enesim_F16p16_Matrix *m2, Enesim_F16p16_Matrix *dst);
EAPI void enesim_matrix_translate(Enesim_Matrix *t, float tx, float ty);
EAPI void enesim_matrix_scale(Enesim_Matrix *t, float sx, float sy);
EAPI void enesim_matrix_rotate(Enesim_Matrix *t, float rad);
EAPI void enesim_matrix_identity(Enesim_Matrix *t);

EAPI float enesim_matrix_determinant(Enesim_Matrix *m);
EAPI void enesim_matrix_divide(Enesim_Matrix *m, float scalar);
EAPI void enesim_matrix_inverse(Enesim_Matrix *m, Enesim_Matrix *m2);
EAPI void enesim_matrix_adjoint(Enesim_Matrix *m, Enesim_Matrix *a);

EAPI void enesim_matrix_point_transform(Enesim_Matrix *m, float x, float y, float *xr, float *yr);
EAPI void enesim_matrix_rect_transform(Enesim_Matrix *m, Eina_Rectangle *r, Enesim_Quad *q);

EAPI Eina_Bool enesim_matrix_quad_quad_to(Enesim_Matrix *m, Enesim_Quad *src, Enesim_Quad *dst);
EAPI Eina_Bool enesim_matrix_square_quad_to(Enesim_Matrix *m, Enesim_Quad *q);
EAPI Eina_Bool enesim_matrix_quad_square_to(Enesim_Matrix *m, Enesim_Quad *q);

EAPI void enesim_quad_rectangle_to(Enesim_Quad *q,
		Eina_Rectangle *r);
EAPI void enesim_quad_rectangle_from(Enesim_Quad *q,
		Eina_Rectangle *r);
EAPI void enesim_quad_coords_set(Enesim_Quad *q, float x1, float y1, float x2,
		float y2, float x3, float y3, float x4, float y4);
EAPI void enesim_quad_coords_get(Enesim_Quad *q, float *x1, float *y1,
		float *x2, float *y2, float *x3, float *y3, float *x4,
		float *y4);
/**
 * @}
 */
#endif /*ENESIM_MATRIX_H_*/
