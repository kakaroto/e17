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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Animation_Basic_Private *)((Eon_Animation_Basic *)(d))->private)
struct _Eon_Animation_Basic_Private
{

};

static void _animation_float_callback(Eon_Animation *a, const char *prop,
		Ekeko_Object *o, const Etch_Data *curr, const Etch_Data *prev)
{
	Ekeko_Value v;

	ekeko_value_float_from(&v, curr->data.f);
	ekeko_object_property_value_set(o, prop, &v);
}

static void _animation_int32_callback(Eon_Animation *a, const char *prop,
		Ekeko_Object *o, const Etch_Data *curr, const Etch_Data *prev)
{
	Ekeko_Value v;

	ekeko_value_int_from(&v, curr->data.i32);
	ekeko_object_property_value_set(o, prop, &v);
}

static void _animation_coord_callback(Eon_Animation *a, const char *prop,
		Ekeko_Object *o, const Etch_Data *curr, const Etch_Data *prev)
{
	Eon_Coord coord;
	Ekeko_Value v;

	/* we need to fetch it in order to know if it is relative or absolute */
	ekeko_object_property_value_get(o, prop, &v);
#if EON_ANIMATION_DEBUG
	printf("[Eon_Animation] Animation %p %p called %d -> %d\n", a, v.value.pointer_value, prev->data.i32, curr->data.i32);
#endif
	eon_coord_set(&coord, curr->data.i32, ((Eon_Coord *)v.value.pointer_value)->type);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set(o, prop, &v);
}

/* object, propname, animation */
static void _animation_color_callback(Eon_Animation *a, const char *prop,
		Ekeko_Object *o, const Etch_Data *curr, const Etch_Data *prev)
{
	Ekeko_Value v;

	eon_value_color_from(&v, curr->data.argb);
	ekeko_object_property_value_set(o, prop, &v);
}

static void _value_set(Ekeko_Value *v, Etch_Animation_Keyframe *k)
{
	switch (v->type)
	{
		/* basic types */
		case EKEKO_PROPERTY_INT:
		etch_animation_keyframe_value_set(k, v->value.int_value);
		break;

		case EKEKO_PROPERTY_FLOAT:
		etch_animation_keyframe_value_set(k, v->value.float_value);
		break;

		default:
		if (v->type == EON_PROPERTY_COORD)
		{
			Eon_Coord *c = v->value.pointer_value;

			etch_animation_keyframe_value_set(k, c->value);
#if EON_ANIMATION_DEBUG
			printf("[Eon_Animation] Setting coord to %d\n", c->value);
#endif
		}
		else if (v->type == EON_PROPERTY_COLOR)
		{
			Eon_Color c = v->value.int_value;

			etch_animation_keyframe_value_set(k, c);
#if EON_ANIMATION_DEBUG
			printf("[Eon_Animation] Setting color to %08x\n", c);
#endif
		}
		else
		{
			printf("[Eon_Animation] Unsupported type on the animation %d\n", v->type);
		}
	}
}

static void _callback_set(Ekeko_Value_Type vtype, Etch_Data_Type *dtype, Eon_Animation_Callback *cb)
{
	switch (vtype)
	{
		/* basic types */
		case EKEKO_PROPERTY_INT:
		*dtype = ETCH_INT32;
		*cb = _animation_int32_callback;
		break;

		case EKEKO_PROPERTY_FLOAT:
		*dtype = ETCH_FLOAT;
		*cb = _animation_float_callback;
		break;

		default:
		if (vtype == EON_PROPERTY_COORD)
		{
			*dtype = ETCH_UINT32;
			*cb = _animation_coord_callback;
		}
		else if (vtype == EON_PROPERTY_COLOR)
		{

			*dtype = ETCH_ARGB;
			*cb = _animation_color_callback;
		}
		else
		{
			printf("[Eon_Animation] Unsupported type on the animation %d\n", vtype);
		}
		break;
	}
}


static void _ctor(void *instance)
{
	Eon_Animation_Basic *a;
	Eon_Animation_Basic_Private *prv;

	a = (Eon_Animation_Basic *) instance;
	a->private = prv = ekeko_type_instance_private_get(eon_animation_basic_type_get(), instance);
	/* default values */
	a->parent.callback_set = _callback_set;
	a->parent.value_set = _value_set;
}

static void _dtor(void *rect)
{

}

static Eina_Bool _appendable(void *instance, void *child)
{
	if (!strcmp(ekeko_object_type_name_get(child), EON_TYPE_ANIMATION_KEY))
		return EINA_TRUE;
	else
		return EINA_FALSE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ekeko_Type * eon_animation_basic_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_ANIMATION_BASIC, sizeof(Eon_Animation_Basic),
				sizeof(Eon_Animation_Basic_Private), eon_animation_type_get(),
				_ctor, _dtor, _appendable);
	}

	return type;
}

EAPI Eon_Animation_Basic * eon_animation_basic_new(void)
{
	Eon_Animation_Basic *a;

	a = ekeko_type_instance_new(eon_animation_basic_type_get());

	return a;
}


