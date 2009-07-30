/*
 * eon_anim_basic.h
 *
 *  Created on: 22-may-2009
 *      Author: jl
 */

#ifndef EON_ANIM_BASIC_H_
#define EON_ANIM_BASIC_H_
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Animation_Basic_Private Eon_Animation_Basic_Private;
struct _Eon_Animation_Basic
{
	Eon_Animation parent;
	Eon_Animation_Basic_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type * eon_animation_basic_type_get(void);
EAPI Eon_Animation_Basic * eon_animation_basic_new(void);

#endif /* EON_ANIM_BASIC_H_ */
