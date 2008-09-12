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

EAPI Enesim_Transformation * enesim_transformation_new(void);
EAPI void enesim_transformation_delete(Enesim_Transformation *t);
EAPI void enesim_transformation_matrix_set(Enesim_Transformation *t, Enesim_Matrix *m);
EAPI void enesim_transformation_matrix_get(Enesim_Transformation *t, Enesim_Matrix *m);
EAPI void enesim_transformation_rop_set(Enesim_Transformation *t, Enesim_Rop rop);
EAPI Eina_Bool enesim_transformation_apply(Enesim_Transformation *t, Enesim_Surface *s, Eina_Rectangle *sr, Enesim_Surface *d, Eina_Rectangle *dr);
EAPI void enesim_transformation_origin_set(Enesim_Transformation *t, float ox, float oy);
EAPI void enesim_transformation_origin_get(Enesim_Transformation *t, float *ox, float *oy);

#endif /*ENESIM_TRANSFORMER_H_*/
