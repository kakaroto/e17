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
