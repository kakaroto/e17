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
#ifndef EON_ANIMATION_H_
#define EON_ANIMATION_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_ANIMATION_START "start"
#define EON_ANIMATION_STOP "stop"
#define EON_ANIMATION_END_CHANGED "endChanged"
#define EON_ANIMATION_BEGIN_CHANGED "beginChanged"
#define EON_ANIMATION_PROPERTY_CHANGED "nameChanged"
#define EON_ANIMATION_REPEAT_CHANGED "repeatChanged"

typedef enum _Eon_Event_Animation_State
{
	EON_EVENT_ANIMATION_START,
	EON_EVENT_ANIMATION_STOP,
} Eon_Event_Animation_State;

typedef struct _Eon_Event_Animation
{
        Ekeko_Event event;
	Eon_Event_Animation_State state;
} Eon_Event_Animation;
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_ANIMATION_END;
extern Ekeko_Property_Id EON_ANIMATION_BEGIN;
extern Ekeko_Property_Id EON_ANIMATION_PROPERTY;
extern Ekeko_Property_Id EON_ANIMATION_REPEAT;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef void (*Eon_Animation_Callback)(Eon_Animation *a, const char *prop,
		Ekeko_Object *o, const Etch_Data *curr, const Etch_Data *prev);

typedef struct _Eon_Animation_Private Eon_Animation_Private;
struct _Eon_Animation
{
	Ekeko_Object parent;
	Eon_Animation_Private *private;
	void (*value_set)(Ekeko_Value *v, Etch_Animation_Keyframe *k);
	void (*callback_set)(Ekeko_Value_Type vtype, Etch_Data_Type *dtype, Eon_Animation_Callback *cb);

};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_animation_type_get(void);
EAPI void eon_animation_end_set(Eon_Animation *a, Ekeko_Object *o, const char *event);
EAPI void eon_animation_begin_set(Eon_Animation *a, Ekeko_Object *o, char *event);
EAPI void eon_animation_duration_set(Eon_Animation *a, Eon_Clock *dur);
EAPI void eon_animation_repeat_set(Eon_Animation *a, int repeat);


#endif /* EON_ANIMATION_H_ */
