/*
 * eon_anim_matrix.h
 *
 *  Created on: 22-may-2009
 *      Author: jl
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
