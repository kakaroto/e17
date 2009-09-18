/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
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
#ifndef EON_ANIM_MATRIX_H_
#define EON_ANIM_MATRIX_H_
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_ANIMATION_MATRIX_TYPE;
typedef enum Eon_Animation_Matrix_Type
{
	EON_ROTATE,
	EON_SCALE,
	EON_ANIMATION_MATRIX_TYPES,
} Eon_Animation_Matrix_Type;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Animation_Matrix_Private Eon_Animation_Matrix_Private;
struct _Eon_Animation_Matrix
{
	Eon_Animation parent;
	Eon_Animation_Matrix_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type * eon_animation_matrix_type_get(void);
EAPI Eon_Animation_Matrix * eon_animation_matrix_new(void);

#endif /* EON_ANIM_MATRIX_H_ */
