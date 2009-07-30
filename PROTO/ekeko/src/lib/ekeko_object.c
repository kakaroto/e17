/* EKEKO - Object and property system
 * Copyright (C) 2007-2009 Jorge Luis Zapata
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ekeko.h"
#include "ekeko_private.h"
/*
 * TODO avoid allocating a value when a property is set
 * TODO create dynamic callbacks for each property
 * TODO create ids for each property
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(obj) ((Ekeko_Object_Private*)(obj->private))

#define TYPE_NAME "Object"

struct _Ekeko_Object_Private
{
	EINA_INLIST;
	char *id;
	Ekeko_Type *type;
	Eina_Hash *listeners;
	Eina_Hash *user;
	Eina_Inlist *children;
	Ekeko_Object *parent;
	Ekeko_Object *rel; /* when we store an inlist we only store the private no the public memory area */
	/* we need a changed counter, to keep track of every async prop change */
	int changed;
	Eina_List *references; /* list of objects that reference this one */
};

/* We use a container for the object-event in case we need
 * to store more info
 */
typedef struct _Object_Event
{
	Event_Listener el;
	Eina_Bool bubble;
	void *data;
} Object_Event;

static void _ctor(void *instance)
{
	Ekeko_Object *obj;
	Ekeko_Object_Private *prv;

	obj = (Ekeko_Object*) instance;
	obj->private = prv = ekeko_type_instance_private_get(ekeko_object_type_get(), instance);
	prv->listeners = eina_hash_string_superfast_new(NULL);
	prv->user = eina_hash_string_superfast_new(NULL);
	prv->children = NULL;
	prv->parent = NULL;
	prv->changed = 0;
	prv->rel = obj;
	/* Set up the mutation event */
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] ctor %p %p %p\n", obj, obj->private, prv->type);
#endif
}

static void _dtor(void *object)
{
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] dtor %p\n", object);
#endif
}

static void _event_dispatch(const Ekeko_Object *obj, Ekeko_Event *e, Eina_Bool bubble)
{
	Eina_List *listeners;
	Eina_Iterator *it;
	Object_Event *oe;
	Ekeko_Object_Private *prv;

	prv = PRIVATE(obj);
	if (!prv->listeners)
		return;
	listeners = eina_hash_find(prv->listeners, e->type);
	if (!listeners) return;

	it = eina_list_iterator_new(listeners);
	while (eina_iterator_next(it, (void **)&oe))
	{
		if (oe->bubble == bubble)
			oe->el(obj, e, oe->data);
		if (e->stop)
			break;
	}
	eina_iterator_free(it);
}

static void _change_recursive(const Ekeko_Object *obj, int count)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(obj);
	prv->changed += count;
	if (prv->parent)
		_change_recursive(prv->parent, count);
}

static void _unchange_recursive(const Ekeko_Object *obj, int count)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(obj);
	prv->changed -= count;
	if (prv->parent)
		_unchange_recursive(prv->parent, count);

}

static void _dump_recursive(Ekeko_Object *o, Ekeko_Object_Dump dump, int level)
{
	Ekeko_Object *child;

	dump(o, level);
	child = ekeko_object_child_first_get(o);
	while (child)
	{
		_dump_recursive(child, dump, level + 1);
		child = ekeko_object_next(child);
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ekeko_Type * object_private_type_get(Ekeko_Object *object)
{
	return object->private->type;
}

void object_construct(Ekeko_Type *type, void *instance)
{
	Ekeko_Object *object;

	object = (Ekeko_Object*) instance;
	object->private = type_instance_private_get_internal(type, ekeko_object_type_get(), object);
	object->private->type = type;
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] construct %p %p %p\n", object, object->private, object->private->type);
#endif
	/* call all the constructors on the type */
	type_construct(type, instance);
}

void object_event_listener_add(Ekeko_Object *obj, const char *type,
		Event_Listener el, Eina_Bool bubble, void *data)
{
	Ekeko_Object_Private *prv;
	Object_Event *oe;
	Eina_List *events, *lst;

	oe = malloc(sizeof(Object_Event));
	oe->el = el;
	oe->bubble = bubble;
	oe->data = data;

	prv = PRIVATE(obj);
	events = eina_hash_find(prv->listeners, type);
	lst = eina_list_append(events, oe);
	if (!events)
		eina_hash_add(prv->listeners, type, lst);
}

void object_event_listener_remove(Ekeko_Object *obj, const char *type,
		Event_Listener el, Eina_Bool bubble, void *data)
{
	Ekeko_Object_Private *prv;
	Object_Event *oe;
	Eina_List *events;
	Eina_Iterator *it;

	prv = PRIVATE(obj);
	events = eina_hash_find(prv->listeners, type);
	it = eina_list_iterator_new(events);
	while (eina_iterator_next(it, (void **)&oe))
	{
		if ((oe->bubble == bubble) && (oe->data == data) && (oe->el == el))
		{
			eina_list_remove(events, oe);
			return;
		}
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EKEKO_OBJECT_ID;

Ekeko_Type *ekeko_object_type_get(void)
{
	static Ekeko_Type *object_type = NULL;

	if (!object_type)
	{
		object_type = ekeko_type_new(TYPE_NAME, sizeof(Ekeko_Object),
				sizeof(Ekeko_Object_Private), NULL, _ctor, _dtor, NULL);
		EKEKO_OBJECT_ID = EKEKO_TYPE_PROP_SINGLE_ADD(object_type, "id", EKEKO_PROPERTY_STRING, OFFSET(Ekeko_Object_Private, id));
		// TODO register the type's event, with type_event_new
	}

	return object_type;
}
/**
 *
 * @return
 */
EAPI Ekeko_Object * ekeko_object_new(void)
{
	Ekeko_Object *object;

	object = ekeko_type_instance_new(ekeko_object_type_get());

	return object;
}

/**
 *
 * @param object
 * @param name
 */
EAPI void ekeko_object_id_set(Ekeko_Object *object, const char *name)
{
	Ekeko_Value value;

	ekeko_value_str_from(&value, (char *)name);
	ekeko_object_property_value_set(object, "id", &value);
}
/**
 *
 * @param object
 * @return
 */
EAPI const char *ekeko_object_id_get(Ekeko_Object *object)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(object);
	return prv->id;
}
/**
 *
 * @param object
 * @param prop_name
 * @param value
 */
EAPI void ekeko_object_property_value_set(Ekeko_Object *o, char *prop_name, Ekeko_Value *value)
{
	Ekeko_Object_Private *prv;
	Ekeko_Property *prop;
	Ekeko_Value prev_value;
	Ekeko_Event_Mutation evt;
	Ekeko_Value_Type vtype;

	void *curr, *prev;
	char *changed;

	RETURN_IF(o == NULL || prop_name == NULL);

	prv = PRIVATE(o);
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] value_set: %s %p %p %p %d\n", prop_name, o, prv, prv->type, prv->changed);
#endif
	/* FIXME this code isnt good enough */
	prop = type_property_get(prv->type, prop_name);
	if (!prop)
		return;

	vtype = ekeko_property_value_type_get(prop);
	if (vtype != EKEKO_PROPERTY_VALUE && vtype != value->type)
	{
		printf("[Ekeko_Object] ERROR values dont match %s.%s %d %d!!\n", ekeko_object_type_name_get(o), prop_name, vtype, value->type);
		exit(1);
	}
	/* Initialize the type in case the property value type is PROPERTY_VALUE */
	prev_value.type = value->type;
	type_instance_property_pointers_get(prv->type, prop, o, &curr, &prev, &changed);
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] pointers %p %p %p\n", curr, prev, changed);
#endif
	if (property_ptype_get(prop) == EKEKO_PROPERTY_VALUE_DUAL_STATE)
	{
		Eina_Bool changed_bef, changed_now;

		changed_bef = *changed;
		/* here we dont need to malloc the value as it is already stored
		 * on the prev, just copy the data
		 */
		ekeko_value_pointer_from(&prev_value, vtype, prev);
		ekeko_value_pointer_double_to(value, vtype, curr, prev, changed);
		changed_now = *changed;
		if (changed_bef && !changed_now)
		{
			_unchange_recursive(o, 1);
		}
		else if (!changed_bef && changed_now)
		{
			_change_recursive(o, 1);
		}
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Object] %s changed bef = %d, changed now = %d\n", prop_name, changed_bef, changed_now);
#endif
	}
	else
	{
		/* here we do need to malloc the prev and copy it because curr
		 * will overwrite it
		 */
		ekeko_value_create(&prev_value, vtype);
		ekeko_value_pointer_from(&prev_value, vtype, curr);
		ekeko_value_pointer_to(value, vtype, curr);
	}
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] changed = %d\n", prv->changed);
#endif
	/* send the generic event */
	event_mutation_init(&evt, EKEKO_EVENT_PROP_MODIFY, o, o, prop,
			&prev_value, value, EVENT_MUTATION_STATE_CURR);
	ekeko_object_event_dispatch((Ekeko_Object *)o, (Ekeko_Event *)&evt);
	/* send the specific event */
	{
		char evt_name[256];

		strcpy(evt_name, prop_name);
		strcat(evt_name, "Changed");
		event_mutation_init(&evt, evt_name, o, o, prop,
			&prev_value, value, EVENT_MUTATION_STATE_CURR);
		ekeko_object_event_dispatch((Ekeko_Object *)o, (Ekeko_Event *)&evt);
	}
	if (property_ptype_get(prop) != EKEKO_PROPERTY_VALUE_DUAL_STATE)
		ekeko_value_free(&prev_value, vtype);
}
/**
 *
 * @param object
 * @param prop_name
 * @param value
 */
EAPI void ekeko_object_property_value_get(Ekeko_Object *o, char *prop_name, Ekeko_Value *value)
{
	Ekeko_Object_Private *prv;
	Ekeko_Property *prop;
	void *curr, *prev;
	char *changed;
	Ekeko_Value_Type vtype;

	prv = PRIVATE(o);
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] value_get: %s\n", prop_name);
#endif
	prop = type_property_get(prv->type, prop_name);
	if (!prop)
		return;
	type_instance_property_pointers_get(prv->type, prop, o, &curr, &prev, &changed);
	vtype = ekeko_property_value_type_get(prop);
	/* This is a leak, we just want to get the value and set the pointer
	 * dont copy it right?
	 * also we dont want the user to call ekeko_value_free every time
	 */
	ekeko_value_create(value, vtype);
	ekeko_value_pointer_from(value, vtype, curr);
	//ekeko_value_free(value, vtype);

	// FIXME remove the function below
	//type_instance_property_value_get(prv->type, object, prop_name, value);
}
/**
 *
 * @param object
 * @param name
 * @param data
 */
EAPI void ekeko_object_user_data_set(Ekeko_Object *object, const char *name, void *data)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(object);
	eina_hash_add(prv->user, name, data);
}
/**
 *
 * @param object
 * @param name
 */
EAPI void * ekeko_object_user_data_get(Ekeko_Object *object, const char *name)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(object);
	return eina_hash_find(prv->user, name);
}

EAPI void ekeko_object_event_dispatch(const Ekeko_Object *obj, Ekeko_Event *e)
{
	Ekeko_Object_Private *prv;

	/* TODO set the phase on the event */
	prv = PRIVATE(obj);
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] Dispatching event %s\n", e->type);
#endif
	_event_dispatch(obj, e, EINA_FALSE);
	if (e->bubbles == EINA_TRUE)
	{
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Object] Event %s going to bubble %p %p\n", e->type, obj, prv->parent);
#endif
		while (prv->parent)
		{
			Ekeko_Object *parent = prv->parent;
			prv = PRIVATE(parent);
			_event_dispatch(parent, e, EINA_TRUE);
		}
	}
}

EAPI const char * ekeko_object_type_name_get(const Ekeko_Object *obj)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(obj);
	return type_name_get(prv->type);
}

/**
 *
 * @param p
 * @param o
 */
EAPI Eina_Bool ekeko_object_child_append(Ekeko_Object *p, Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;
	Ekeko_Type *t;

	prv = PRIVATE(p);
	t = prv->type;
	if (type_appendable(t, p, o))
	{
		Ekeko_Object_Private *pprv, *oprv;
		Ekeko_Event_Mutation evt;

		pprv = PRIVATE(p);
		oprv = PRIVATE(o);
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Object] Setting the parent of %p (%p) to %p (%p) \n", o, oprv, p, pprv);
#endif
		pprv->children = eina_inlist_append(pprv->children, EINA_INLIST_GET(oprv));
		/* TODO check that there's a parent already
		 * if so, send an event informing that the child has been removed
		 * from that parent
		 */
		/* check if the object has some pending changes */
		if (oprv->changed)
		{
			if (oprv->parent)
				_unchange_recursive(oprv->parent, oprv->changed);
			_change_recursive(p, oprv->changed);
		}
		oprv->parent = p;
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Object] pchanged = %d ochanged = %d\n", pprv->changed, oprv->changed);
#endif
		/* TODO send the EVENT_PARENT_SET event */
		/* send the EVENT_OBJECT_APPEND event */
		event_mutation_init(&evt, EKEKO_EVENT_OBJECT_APPEND, (Ekeko_Object *)o, (Ekeko_Object *)p, NULL, NULL, NULL,
				EVENT_MUTATION_STATE_CURR);
		ekeko_object_event_dispatch((Ekeko_Object *)o, (Ekeko_Event *)&evt);
		return EINA_TRUE;
	}
	else
		return EINA_FALSE;
}

/**
 *
 * @param p
 * @param o
 */
EAPI void ekeko_object_child_remove(Ekeko_Object *p, Ekeko_Object *o)
{

}

/**
 *
 */
EAPI int ekeko_object_child_count(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv = PRIVATE(o);

	return eina_inlist_count(prv->children);
}

EAPI Ekeko_Object * ekeko_object_child_get_at(Ekeko_Object *o, unsigned int index)
{
	Eina_Accessor *a;
	Ekeko_Object_Private *prv = PRIVATE(o);
	Ekeko_Object *child;

	a = eina_inlist_accessor_new(prv->children);
	eina_accessor_data_get(a, index, &child);

	return child;
}

/**
 *
 */
EAPI Ekeko_Object * ekeko_object_child_last_get(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;
	Ekeko_Object_Private *chprv;

	prv = PRIVATE(o);
	chprv = prv->children;
	if (!chprv) return NULL;
	else
	{
		chprv = chprv->__in_list.last;
		return chprv->rel;
	}
}

EAPI Ekeko_Object * ekeko_object_child_first_get(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;
	Ekeko_Object_Private *chprv;

	prv = PRIVATE(o);
	chprv = prv->children;
	if (!chprv) return NULL;
	else return chprv->rel;
}

EAPI Ekeko_Object * ekeko_object_next(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(o);
	prv = (Ekeko_Object_Private *)prv->__in_list.next;
	if (!prv) return NULL;
	else return prv->rel;
}

EAPI Ekeko_Object * ekeko_object_prev(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(o);
	prv = (Ekeko_Object_Private *)prv->__in_list.prev;
	if (!prv) return NULL;
	else return prv->rel;
}

EAPI Ekeko_Object * ekeko_object_parent_get(const Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(o);
	return prv->parent;
}

#if 0
/* old version, this version first checked out the object properties
 * and then the children
 */
EAPI void ekeko_object_process(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;
	Ekeko_Object_Private *in;
	Eina_Iterator *it;
	Property_Iterator *pit;
	Ekeko_Property *prop;
	Ekeko_Event_Mutation evt;

	/* TODO check that the object is actually the top most parent
	 * on the hierarchy?
	 */
	prv = PRIVATE(o);
	/* all childs */
	if (!prv->changed)
	return;
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] [0  Object %p %s changed %d\n", o, ekeko_object_type_name_get(o), prv->changed);
#endif
	/* TODO handle the attributes as they dont have any parent, childs or siblings */
	pit = type_property_iterator_new(prv->type);
	while (type_property_iterator_next(pit, &prop))
	{
		Ekeko_Value prev_value, curr_value;
		Ekeko_Event_Mutation evt;
		void *curr, *prev;
		char *changed;

		if (property_ptype_get(prop) != EKEKO_PROPERTY_VALUE_DUAL_STATE)
		continue;
		type_instance_property_pointers_get(prv->type, prop, o, &curr, &prev, &changed);
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Object] process pointers %p %p %p\n", curr, prev, changed);
#endif
		if (!(*changed))
		continue;
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Object] [1 updating %s %d\n", ekeko_property_name_get(prop), *changed);
#endif

		/* update the property */
		*changed = EINA_FALSE;
		prv->changed--;
		/* send the generic mutation event */
		ekeko_value_pointer_from(&prev_value, ekeko_property_value_type_get(prop), prev);
		ekeko_value_pointer_from(&curr_value, ekeko_property_value_type_get(prop), curr);
		event_mutation_init(&evt, EKEKO_EVENT_PROP_MODIFY, o, o, prop, &prev_value,
				&curr_value, EVENT_MUTATION_STATE_POST);
		ekeko_object_event_dispatch(o, (Ekeko_Event *)&evt);
		/* send the specific event */
		{
			char evt_name[256];

			strcpy(evt_name, ekeko_property_name_get(prop));
			strcat(evt_name, "Changed");
			event_mutation_init(&evt, evt_name, o, o, prop,
					&prev_value, &curr_value, EVENT_MUTATION_STATE_POST);
			ekeko_object_event_dispatch((Ekeko_Object *)o, (Ekeko_Event *)&evt);
		}
		/* update prev */
		ekeko_value_pointer_to(&curr_value, ekeko_property_value_type_get(prop), prev);
		if (!prv->changed)
		{
			type_property_iterator_free(pit);
#ifdef EKEKO_DEBUG
			printf("[Ekeko_Object] 0] Object changed %d (only attributes)\n", prv->changed);
#endif
			goto event;
		}
	}
	/* handle childs */
	it = eina_inlist_iterator_new(prv->children);
	while (eina_iterator_next(it, (void **) &in))
	{
		int changed;

		changed = in->changed;

		if (!changed)
		continue;

		ekeko_object_process(in->rel);
		prv->changed -= changed;
		if (!prv->changed)
		{
			break;
		}
	}
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] 0] Object changed %d\n", prv->changed);
#endif
	/* post condition */
	if (prv->changed)
	{
		printf("WRONG! %p %d\n", o, prv->changed);
		exit(1);
	}
event:
	/* send the event */
	event_mutation_init(&evt, EKEKO_EVENT_OBJECT_PROCESS, o, NULL, NULL, NULL, NULL,
			EVENT_MUTATION_STATE_POST);
	ekeko_object_event_dispatch(o, (Ekeko_Event *)&evt);
}
#endif

/**
 * @brief This function will update every two state attribute in case it has
 * changed
 * @param o
 */
EAPI void ekeko_object_process(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;
	Ekeko_Object_Private *in;
	Eina_Iterator *it;
	Property_Iterator *pit;
	Ekeko_Property *prop;
	Ekeko_Event_Mutation evt;
	int ch;
	Ekeko_Object *parent;

	/* TODO check that the object is actually the top most parent
	 * on the hierarchy?
	 */
	prv = PRIVATE(o);
	/* all childs */
	if (!prv->changed)
		return;
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] [0  Object %p %s changed %d\n", o, ekeko_object_type_name_get(o), prv->changed);
#endif
	/* handle childs */
	it = eina_inlist_iterator_new(prv->children);
	while (eina_iterator_next(it, (void **) &in))
	{
		int changed;

		changed = in->changed;

		if (!changed)
			continue;

		ekeko_object_process(in->rel);
	}
	/* store the old changed here as the childs might have changed some
	 * parent's properties */
	ch = prv->changed;
	/* TODO handle the attributes as they dont have any parent, childs or siblings */
	pit = type_property_iterator_new(prv->type);
	while (type_property_iterator_next(pit, &prop))
	{
		Ekeko_Value prev_value, curr_value;
		Ekeko_Event_Mutation evt;
		void *curr, *prev;
		char *changed;

		if (property_ptype_get(prop) != EKEKO_PROPERTY_VALUE_DUAL_STATE)
			continue;
		type_instance_property_pointers_get(prv->type, prop, o, &curr, &prev, &changed);
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Object] process pointers %p %p %p\n", curr, prev, changed);
#endif
		if (!(*changed))
			continue;
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Object] [1 updating %s %d\n", ekeko_property_name_get(prop), *changed);
#endif

		/* update the property */
		*changed = EINA_FALSE;
		prv->changed--;
		/* send the generic mutation event */
		ekeko_value_pointer_from(&prev_value, ekeko_property_value_type_get(prop), prev);
		ekeko_value_pointer_from(&curr_value, ekeko_property_value_type_get(prop), curr);
		event_mutation_init(&evt, EKEKO_EVENT_PROP_MODIFY, o, o, prop, &prev_value,
				&curr_value, EVENT_MUTATION_STATE_POST);
		ekeko_object_event_dispatch(o, (Ekeko_Event *)&evt);
		/* send the specific event */
		{
			char evt_name[256];

			strcpy(evt_name, ekeko_property_name_get(prop));
			strcat(evt_name, "Changed");
			event_mutation_init(&evt, evt_name, o, o, prop,
				&prev_value, &curr_value, EVENT_MUTATION_STATE_POST);
			ekeko_object_event_dispatch((Ekeko_Object *)o, (Ekeko_Event *)&evt);
		}
		/* update prev */
		ekeko_value_pointer_to(&curr_value, ekeko_property_value_type_get(prop), prev);
		if (!prv->changed)
			break;
	}
	/* FIXME this gives me an error, track it down */
	//type_property_iterator_free(pit);
event:
	/* send the event */
	event_mutation_init(&evt, EKEKO_EVENT_OBJECT_PROCESS, o, NULL, NULL, NULL, NULL,
			EVENT_MUTATION_STATE_POST);
	ekeko_object_event_dispatch(o, (Ekeko_Event *)&evt);

#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] 0] Object %p changed %d\n", o, prv->changed);
#endif
	parent = ekeko_object_parent_get(o);
	if (parent)
	{
		_unchange_recursive(parent, ch);
	}
	/* post condition */
	if (prv->changed)
	{
		printf("WRONG! %p %d\n", o, prv->changed);
		exit(1);
	}
}

EAPI Ekeko_Property * ekeko_object_property_get(Ekeko_Object *o, const char *prop_name)
{
	Ekeko_Object_Private *prv;
	Ekeko_Property *prop;

	prv = PRIVATE(o);
	prop = type_property_get(prv->type, prop_name);
	return prop;
}

EAPI void ekeko_object_dump_printf(Ekeko_Object *o, int level)
{
	int i;

	for (i = 1; i <= level; i++)
		printf("\t");
	printf("> %s (%p) ", ekeko_object_type_name_get(o), o);
	/** TODO foreach attribute, dump it */
	/* some useful properties */
	if (ekeko_type_instance_is_of(o, "Renderable"))
	{
		printf("z-index: %d", ekeko_renderable_zindex_get((Ekeko_Renderable *)o));
	}
	printf("\n");
}

EAPI void ekeko_object_dump(Ekeko_Object *o, Ekeko_Object_Dump dump)
{
	_dump_recursive(o, dump, 0);
}

/* check that the object is of that type */
EAPI Ekeko_Object * ekeko_object_cast(Ekeko_Object *o, Ekeko_Type *t)
{
	if (!o || !t)
		return NULL;
	if (!ekeko_type_instance_is_of_type(o, t))
	{
		EINA_ERROR_PERR("[Ekeko] Object %p is not of type %s\n", o, type_name_get(t));
	}
	return o;
}
