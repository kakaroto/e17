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
#ifndef ENESIM_TRANSFORMER_H_
#define ENESIM_TRANSFORMER_H_

/**
 * @todo
 * + Add a function to compose the transformations
 * + Add a function to setup the quality
 */
typedef struct _Enesim_Transformation Enesim_Transformation;

/* TODO
 * Create all this Macros
 * Replace all float * with Enesim_Matrix
 */
#define ENESIM_MATRIX_XX(m) m[0]

typedef float Enesim_Matrix[9];
typedef float Enesim_Quad[8];

EAPI Enesim_Transformation * enesim_transformation_new(void);
EAPI void enesim_transformation_set(Enesim_Transformation *t, float *tx);
EAPI void enesim_transformation_get(Enesim_Transformation *t, float *tx);
EAPI void enesim_transformation_rop_set(Enesim_Transformation *t, Enesim_Rop rop);
EAPI Eina_Bool enesim_transformation_apply(Enesim_Transformation *t, Enesim_Surface *s, Eina_Rectangle *sr, Enesim_Surface *d, Eina_Rectangle *dr);
EAPI void enesim_transformation_origin_set(Enesim_Transformation *t, float ox, float oy);
EAPI void enesim_transformation_origin_get(Enesim_Transformation *t, float *ox, float *oy);

EAPI void enesim_matrix_compose(float *m1, float *m2, float *dst);
EAPI void enesim_matrix_translate(float *t, float tx, float ty);
EAPI void enesim_matrix_scale(float *t, float sx, float sy);
EAPI void enesim_matrix_rotate(float *t, float rad);
EAPI void enesim_matrix_identity(float *t);
EAPI float enesim_matrix_determinant(float *m);
EAPI void enesim_matrix_divide(float *m, float scalar);
EAPI void enesim_matrix_inverse(float *m, float *m2);
EAPI void enesim_matrix_point_transform(float *m, float x, float y, float *xr, float *yr);
EAPI Eina_Bool enesim_matrix_square_quad_to(float *m, float *q);
EAPI Eina_Bool enesim_matrix_quad_square_to(float *m, float *q);
EAPI void enesim_matrix_adjoint(float *m, float *a);
EAPI void enesim_quad_coords_from(float *q, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);


#endif /*ENESIM_TRANSFORMER_H_*/
