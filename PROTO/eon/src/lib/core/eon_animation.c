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

#define EON_ANIMATION_DEBUG 0
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Animation_Private *)((Eon_Animation *)(d))->private)
struct _Eon_Animation_Private
{
	char *name;
	Eon_Trigger begin;
	Eon_Trigger end;
	int repeat;
	Etch_Animation *anim;
};

typedef struct _Eon_Animation_Callback_Container
{
	Eon_Animation *a;
	Eon_Animation_Callback cb;
} Eon_Animation_Callback_Container;



static inline Eon_Document * _document_get(Ekeko_Object *o)
{
	Eon_Document *doc;

	/* canvas */
	if (ekeko_type_instance_is_of(o, EON_TYPE_CANVAS))
	{
		doc = eon_canvas_document_get((Eon_Canvas *)o);
	}
	/* shape */
	else
	{
		Ekeko_Object *parent;

		parent = ekeko_object_parent_get(o);
		if (!parent) return NULL;
		doc = _document_get(parent);
	}
	return doc;
}

static void _etch_callback(const Etch_Data *curr, const Etch_Data *prev, void *data)
{
	Eon_Animation_Callback_Container *acc = data;
	Eon_Animation *a = acc->a;
	Ekeko_Object *rel;
	Eon_Animation_Private *prv = PRIVATE(a);

#if EON_ANIMATION_DEBUG
	printf("[Eon_Animation] Entering animation callback\n");
#endif
	rel = ekeko_object_parent_get((Ekeko_Object *)a);
	acc->cb(a, prv->name, rel, curr, prev);
}


static void _event_set(Eon_Event_Animation *ev, const Ekeko_Object *o, const char *type, Eon_Event_Animation_State state)
{
	ekeko_event_init((Ekeko_Event *)ev, type, o, EINA_FALSE);
	ev->state = state;
	ekeko_event_dispatch((Ekeko_Event *)ev);
}

static void _etch_start_cb(Etch_Animation *ea, void *data)
{
	Eon_Animation_Callback_Container *acc = data;
	Eon_Event_Animation ev;

	_event_set(&ev, (Ekeko_Object *)acc->a, EON_ANIMATION_START, EON_EVENT_ANIMATION_START);
}

static void _etch_stop_cb(Etch_Animation *ea, void *data)
{
	Eon_Animation_Callback_Container *acc = data;
	Eon_Event_Animation ev;

	_event_set(&ev, (Ekeko_Object *)acc->a, EON_ANIMATION_STOP, EON_EVENT_ANIMATION_STOP);
}

static inline void _property_animate(Eon_Animation *a, Ekeko_Object *parent)
{
	Ekeko_Property *p;
	Eon_Document *doc;
	Eon_Animation_Private *prv = PRIVATE(a);
	Etch *etch;
	Ekeko_Value_Type vtype;
	Etch_Data_Type dtype;
	Eon_Animation_Callback cb = NULL;
	Eon_Animation_Callback_Container *acc;
	Etch_Animation_Type atype;

	/* get the property */
	if (!prv->name)
		return;

	p = ekeko_object_property_get(parent, prv->name);
	if (!p)
		return;

	doc = _document_get(parent);
	if (!doc)
		return;

	etch = eon_document_etch_get(doc);
	vtype = ekeko_property_value_type_get(p);
#ifndef EON_ANIMATION_DEBUG
	printf("[Eon_Animation] Trying to animate property %s of type %d\n", prv->name, vtype);
#endif

	if (!a->callback_set)
		return;
	a->callback_set(vtype, &dtype, &cb);
	if (!cb)
		return;

	acc = malloc(sizeof(Eon_Animation_Callback_Container));
	acc->a = a;
	acc->cb = cb;
	/* TODO check if the animation already has an anim */
	prv->anim = etch_animation_add(etch, dtype, _etch_callback,
			_etch_start_cb, _etch_stop_cb, acc);
	etch_animation_repeat_set(prv->anim, prv->repeat);

	/* If we have some trigger event disable the animation for now */
	if (prv->begin.obj)
		etch_animation_disable(prv->anim);
	else
		etch_animation_enable(prv->anim);
}

static void _trigger_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Animation_Private *prv = PRIVATE(data);

	printf("[Eon_Animation] Trigger callback %p\n", prv->anim);

	if (!etch_animation_enabled(prv->anim))
	{
		Etch *e;
		unsigned long secs, usecs;

		printf("[Eon_Animation] Begin trigger callback called!\n");
		/* change every keyframe to curr offset */
		e = etch_animation_etch_get(prv->anim);
		etch_timer_get(e, &secs, &usecs);
		etch_animation_offset_add(prv->anim, secs, usecs);
		/* Enable the animation */
		etch_animation_enable(prv->anim);
	}
	else if (prv->end.obj)
	{
		printf("[Eon_Animation] End trigger callback called!\n");
		etch_animation_disable(prv->anim);
	}
}

static void _prop_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Animation *a = (Eon_Animation *)o;
	Ekeko_Object *parent;

	if (!(parent = ekeko_object_parent_get(o)))
		return;
	_property_animate(a, parent);
}

static void _begin_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Animation_Private *prv = PRIVATE(o);
	Eon_Trigger *t;
	Ekeko_Object *parent;

	/* TODO  we should have a default event that will trigger
	 * the animation start
	 */

	if (!(parent = ekeko_object_parent_get(o)))
		return;

	/* register an event listener to this event */
	t = em->curr->value.pointer_value;
	printf("[Eon_Animation] Begin changed %p %s\n", t->obj, t->event);
	if (!prv->end.obj)
	{
		printf("[Eon_Animation] event listener!!!!!\n");
		/* FIXME change that is not the same! */
		ekeko_event_listener_add(t->obj, t->event, _trigger_cb, EINA_FALSE, o);
	}
	if (!prv->anim)
		return;
	/* disable the animation */
	etch_animation_disable(prv->anim);

}

static void _end_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Animation_Private *prv = PRIVATE(o);
	Eon_Trigger *t;
	Ekeko_Object *parent;

	if (!(parent = ekeko_object_parent_get(o)))
		return;

	/* register an event listener to this event */
	t = em->curr->value.pointer_value;
	printf("[Eon_Animation] End changed %p %s\n", t->obj, t->event);
	if (!prv->begin.obj)
	{
		/* FIXME change that is not the same! */
		ekeko_event_listener_add(t->obj, t->event, _trigger_cb, EINA_FALSE, o);
	}
}

static void _repeat_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Animation_Private *prv = PRIVATE(o);
	Ekeko_Object *parent;

	if (!(parent = ekeko_object_parent_get(o)))
		return;

	if (prv->anim)
		etch_animation_repeat_set(prv->anim, em->curr->value.int_value);
}

static void _child_append_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Animation *a = (Eon_Animation *)o;
	Eon_Animation_Private *prv = PRIVATE(a);

	if (!prv->name)
		return;
	_property_animate(a, em->related);
}

static void _ctor(void *instance)
{
	Eon_Animation *a;
	Eon_Animation_Private *prv;

	a = (Eon_Animation*) instance;
	a->private = prv = ekeko_type_instance_private_get(eon_animation_type_get(), instance);
	/* default values */
	prv->repeat = 1;
	ekeko_event_listener_add((Ekeko_Object *)a, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)a, EON_ANIMATION_END_CHANGED, _end_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)a, EON_ANIMATION_BEGIN_CHANGED, _begin_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)a, EON_ANIMATION_PROPERTY_CHANGED, _prop_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)a, EON_ANIMATION_REPEAT_CHANGED, _repeat_change, EINA_FALSE, NULL);
}

static void _dtor(void *rect)
{

}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Etch_Animation * eon_animation_etch_animation_get(Eon_Animation *a)
{
	Eon_Animation_Private *prv = PRIVATE(a);

	return prv->anim;
}

Ekeko_Value_Type eon_animation_prop_type_get(Eon_Animation *a)
{
	Eon_Animation_Private *prv = PRIVATE(a);
	Ekeko_Property *p;
	Ekeko_Object *parent;

	parent = ekeko_object_parent_get((Ekeko_Object *)a);
	if (!parent)
		return 0;
	p = ekeko_object_property_get(parent, prv->name);
	if (!p)
		return 0;

	return ekeko_property_value_type_get(p);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_ANIMATION_END;
Ekeko_Property_Id EON_ANIMATION_BEGIN;
Ekeko_Property_Id EON_ANIMATION_PROPERTY;
Ekeko_Property_Id EON_ANIMATION_REPEAT;

EAPI Ekeko_Type *eon_animation_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_ANIMATION, sizeof(Eon_Animation),
				sizeof(Eon_Animation_Private), ekeko_object_type_get(),
				_ctor, _dtor, NULL);
		EON_ANIMATION_PROPERTY = EKEKO_TYPE_PROP_SINGLE_ADD(type, "name", EKEKO_PROPERTY_STRING, OFFSET(Eon_Animation_Private, name));
		EON_ANIMATION_REPEAT = EKEKO_TYPE_PROP_SINGLE_ADD(type, "repeat", EKEKO_PROPERTY_INT, OFFSET(Eon_Animation_Private, repeat));
		EON_ANIMATION_BEGIN = EKEKO_TYPE_PROP_SINGLE_ADD(type, "begin", EON_PROPERTY_TRIGGER, OFFSET(Eon_Animation_Private, begin));
	}

	return type;
}

EAPI void eon_animation_property_set(Eon_Animation *a, const char *prop)
{
	Ekeko_Value val;

	ekeko_value_str_from(&val, prop);
	ekeko_object_property_value_set((Ekeko_Object *)a, "name", &val);
}

/*
 * Whenever the event is triggered the animation should start relative to the begin
 * attribute
 */
EAPI void eon_animation_begin_set(Eon_Animation *a, Ekeko_Object *o, char *event)
{
	Ekeko_Value val;
	Eon_Trigger trigger;

	trigger.event = event;
	trigger.obj = o;
	eon_value_trigger_from(&val, &trigger);
	ekeko_object_property_value_set((Ekeko_Object *)a, "begin", &val);
}

EAPI void eon_animation_end_set(Eon_Animation *a, Ekeko_Object *o, const char *event)
{
	Ekeko_Value val;
	Eon_Trigger trigger;

	trigger.event = (char *)event;
	trigger.obj = o;
	eon_value_trigger_from(&val, &trigger);
	ekeko_object_property_value_set((Ekeko_Object *)a, "end", &val);
}

EAPI void eon_animation_repeat_set(Eon_Animation *a, int repeat)
{
	Ekeko_Value val;

	ekeko_value_int_from(&val, repeat);
	ekeko_object_property_value_set((Ekeko_Object *)a, "repeat", &val);
}
