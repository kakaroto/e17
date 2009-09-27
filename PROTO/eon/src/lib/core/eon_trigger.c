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
#define PRIVATE(d) ((Eon_Trigger_Private *)((Eon_Trigger_Object *)(d))->private)

static Ekeko_Type *_type;
struct _Eon_Trigger_Private
{
	char *event;
};

static void _handler_cb(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Trigger_Object *t = (Eon_Trigger_Object *)data;
	Eon_Trigger_Private *prv = PRIVATE(t);

	/* for every child call the needed enable() */
	/* as childs we can have setters for now */
}

static void _unset(Ekeko_Object *o, Ekeko_Object *rel)
{

}


static void _set(Ekeko_Object *o, Ekeko_Object *rel)
{

}

#if 0
static void _target_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Trigger_Object *t = (Eon_Trigger_Object *)o;
	Eon_Trigger_Private *prv = PRIVATE(t);
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Object *target;

	if (!prv->event)
		return;
	/* remove current event */
	target = em->prev->value.object;
	if (target)
	{
		ekeko_event_listener_remove(target, prv->event,
				_handler_cb, EINA_FALSE, o);

	}
	/* set the handler */
	ekeko_event_listener_add(em->curr->value.object, prv->event,
			_handler_cb, EINA_FALSE, o);
}

static void _event_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Trigger_Object *t = (Eon_Trigger_Object *)o;
	Eon_Trigger_Private *prv = PRIVATE(t);
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	char *event;

	/* TODO get the style, get the references and remove the listeners
	 * on them
	 */
	if (!prv->target)
		return;
	/* remove current event */
	event = em->prev->value.string_value;
	if (event)
	{
		ekeko_event_listener_remove(prv->target, event,
				_handler_cb, EINA_FALSE, o);
	}
	/* set the handler */
	ekeko_event_listener_add(prv->target, em->curr->value.string_value,
			_handler_cb, EINA_FALSE, o);
}
#endif

static void _ctor(Ekeko_Object *o)
{
	Eon_Trigger_Object *t;
	Eon_Trigger_Private *prv;

	t = (Eon_Trigger_Object *)o;
	t->private = prv = ekeko_type_instance_private_get(_type, o);
	/* default values */
	t->parent.set = _set;
	t->parent.unset = _unset;
}

static void _dtor(Ekeko_Object *o)
{

}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_trigger_init(void)
{
	_type = ekeko_type_new(EON_TYPE_TRIGGER, sizeof(Eon_Trigger_Object),
			sizeof(Eon_Trigger_Private), ekeko_object_type_get(),
			_ctor, _dtor, NULL);
	eon_type_register(_type, EON_TYPE_TRIGGER);
}

void eon_trigger_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_TRIGGER_EVENT;

Eon_Trigger_Object * eon_trigger_new(Eon_Document *d)
{
	Eon_Trigger_Object *t;

	t = eon_document_object_new(d, EON_TYPE_TRIGGER);

	return t;
}

EAPI void eon_trigger_event_set(Eon_Trigger_Object *t, const char *event)
{
	Ekeko_Value v;

	ekeko_value_str_from(&v, event);
	ekeko_object_property_value_set((Ekeko_Object *)t, "event", &v);
}

