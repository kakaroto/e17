/*
 * eon_animation.h
 *
 *  Created on: 09-feb-2009
 *      Author: jl
 */

#ifndef EON_ANIMATION_KEY_H_
#define EON_ANIMATION_KEY_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_ANIMATION_KEY_VALUE_CHANGED "valueChanged"
#define EON_ANIMATION_KEY_START_CHANGED "startChanged"
#define EON_ANIMATION_KEY_CALC_CHANGED "calcChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_ANIMATION_KEY_START;
extern Ekeko_Property_Id EON_ANIMATION_KEY_VALUE;
extern Ekeko_Property_Id EON_ANIMATION_KEY_CALC;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Animation_Key_Private Eon_Animation_Key_Private;
struct _Eon_Animation_Key
{
	Ekeko_Object parent;
	Eon_Animation_Key_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_animation_key_type_get(void);
EAPI void eon_animation_key_start_set(Eon_Animation_Key *k, Eon_Clock *dur);
EAPI void eon_animation_calc_set(Eon_Animation_Key *k, Eon_Calc calc);
EAPI void eon_animation_value_set(Eon_Animation_Key *k, Ekeko_Value *val);

#endif /* EON_ANIMATION_KEY_H_ */
