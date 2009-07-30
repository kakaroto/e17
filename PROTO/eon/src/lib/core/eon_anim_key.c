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
#include "Eon.h"
#include "eon_private.h"

/* TODO add more variables to define a bezier cubic and quadratic calcs */
#define EON_ANIMATION_DEBUG 1
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Animation_Key_Private *)((Eon_Animation_Key *)(d))->private)
struct _Eon_Animation_Key_Private
{
	Ekeko_Value value;
	Eon_Clock start;
	Eon_Calc calc;
	Etch_Animation_Keyframe *k;
	Etch_Animation *anim;
};

static inline Etch_Animation_Type _calc_to_etch(Eon_Calc c)
{
	switch (c)
	{
		case EON_CALC_LINEAR:
		return ETCH_ANIMATION_LINEAR;
		break;

		case EON_CALC_DISCRETE:
		return ETCH_ANIMATION_DISCRETE;
		break;

		default:
#if EON_ANIMATION_DEBUG
		printf("ERRRRRRRRRRRRRRORRRRRRR\n");
#endif
		break;
	}
	return ETCH_ANIMATION_LINEAR;
}

static void _value_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Animation_Key_Private *prv = PRIVATE(o);
	Eon_Animation *a;

	a = (Eon_Animation *)ekeko_object_parent_get(o);
	if (!a)
		return;
	if (!prv->anim)
		return;
	if (!prv->k)
		prv->k = etch_animation_keyframe_add(prv->anim);
	if (a->value_set)
		a->value_set(&prv->value, prv->k);
}

static void _start_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Clock *clock;
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Object *parent;
	Eon_Animation_Key_Private *prv = PRIVATE(o);
	Etch_Animation_Keyframe *k;

	if (!prv->anim)
		return;
	if (!prv->k)
		return;

	clock = em->curr->value.pointer_value;
#if EON_ANIMATION_DEBUG
	printf("[Eon_Animation_Key] Setting new clock to %d %d\n", clock->seconds, clock->micro);
#endif
	printf("%p\n", prv->k);
	etch_animation_keyframe_time_set(prv->k, clock->seconds, clock->micro);
}

static void _calc_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Object *parent;
	Eina_Iterator *it;
	Eon_Animation_Key_Private *prv = PRIVATE(o);
	Etch_Animation_Type atype;

	if (!prv->anim)
		return;
	if (!prv->k)
		return;
#if EON_ANIMATION_DEBUG
	printf("[Eon_Animation_Key] Calc mode changed %d\n", em->curr->value.int_value);
#endif
	atype = _calc_to_etch(em->curr->value.int_value);
	etch_animation_keyframe_type_set(prv->k, atype);
}

static void _child_append_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Animation_Key_Private *prv = PRIVATE(o);
	Eon_Animation *a;

	printf("INSIDE %p!!!\n", em->related);
	a = (Eon_Animation *)em->related;
	prv->anim = eon_animation_etch_animation_get(a);
}

static void _ctor(void *instance)
{
	Eon_Animation_Key *k;
	Eon_Animation_Key_Private *prv;

	k = (Eon_Animation*) instance;
	k->private = prv = ekeko_type_instance_private_get(eon_animation_key_type_get(), instance);
	ekeko_event_listener_add((Ekeko_Object *)k, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)k, EON_ANIMATION_KEY_START_CHANGED, _start_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)k, EON_ANIMATION_KEY_VALUE_CHANGED, _value_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)k, EON_ANIMATION_KEY_CALC_CHANGED, _calc_change, EINA_FALSE, NULL);
}

static void _dtor(void *rect)
{

}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_ANIMATION_KEY_START;
Ekeko_Property_Id EON_ANIMATION_KEY_VALUE;
Ekeko_Property_Id EON_ANIMATION_KEY_CALC;

EAPI Ekeko_Type *eon_animation_key_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_ANIMATION_KEY, sizeof(Eon_Animation_Key),
				sizeof(Eon_Animation_Key_Private), ekeko_object_type_get(),
				_ctor, _dtor, NULL);
		EON_ANIMATION_KEY_START = EKEKO_TYPE_PROP_SINGLE_ADD(type, "start", EON_PROPERTY_CLOCK, OFFSET(Eon_Animation_Key_Private, start));
		EON_ANIMATION_KEY_CALC = EKEKO_TYPE_PROP_SINGLE_ADD(type, "calc", EKEKO_PROPERTY_INT, OFFSET(Eon_Animation_Key_Private, calc));
		EON_ANIMATION_KEY_VALUE = EKEKO_TYPE_PROP_SINGLE_ADD(type, "value", EKEKO_PROPERTY_VALUE, OFFSET(Eon_Animation_Key_Private, value));
	}

	return type;
}

EAPI Eon_Animation_Key * eon_animation_key_new(Eon_Animation *a)
{
	Eon_Animation_Key *k;

	k = ekeko_type_instance_new(eon_animation_key_type_get());
	ekeko_object_child_append((Ekeko_Object *)a, (Ekeko_Object *)k);
	return k;
}


EAPI void eon_animation_key_value_set(Eon_Animation_Key *a, Ekeko_Value *from)
{
	ekeko_object_property_value_set((Ekeko_Object *)a, "value", from);
}

EAPI void eon_animation_key_start_set(Eon_Animation_Key *a, Eon_Clock *dur)
{
	Ekeko_Value val;

	eon_value_clock_from(&val, dur);
	ekeko_object_property_value_set((Ekeko_Object *)a, "start", &val);
}

EAPI void eon_animation_key_calc_set(Eon_Animation_Key *a, Eon_Calc calc)
{
	Ekeko_Value val;

	ekeko_value_int_from(&val, calc);
	ekeko_object_property_value_set((Ekeko_Object *)a, "calc", &val);
}
