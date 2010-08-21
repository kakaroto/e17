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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(obj) ((Ekeko_Object_Private*)(obj->prv))
#define TYPE_NAME "Object"

struct _Ekeko_Object_Private
{
	EINA_INLIST;
	Ekeko_Type *type;
	Eina_Hash *properties;
	Eina_Hash *listeners;
	Eina_Hash *user;
	Eina_Inlist *children;
	Ekeko_Object *parent;
	Ekeko_Object *rel; /* when we store an inlist we only store the private no the public memory area */
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

typedef struct _Object_Property
{
	const char *name;
	int type;
} Object_Property;

static void _ctor(Ekeko_Object *obj)
{
	Ekeko_Object_Private *prv;

	obj->prv = prv = ekeko_type_instance_private_get(ekeko_object_type_get(), obj);
	prv->listeners = eina_hash_string_superfast_new(NULL);
	prv->user = eina_hash_string_superfast_new(NULL);
	prv->children = NULL;
	prv->parent = NULL;
	prv->rel = obj;
	prv->properties = eina_hash_string_superfast_new(NULL);
	/* Set up the mutation event */
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] ctor %s %p %p %p\n", ekeko_object_type_name_get(obj), obj, obj->prv, prv->type);
#endif
}

static void _dtor(Ekeko_Object *obj)
{
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] dtor %p\n", object);
#endif
}

static void _event_dispatch(Ekeko_Object *obj, Ekeko_Event *e, Eina_Bool bubble)
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
Ekeko_Type * ekeko_object_private_type_get(Ekeko_Object *object)
{
	return object->prv->type;
}

void ekeko_object_construct(Ekeko_Type *type, void *instance)
{
	Ekeko_Object *object;

	object = (Ekeko_Object*) instance;
	object->prv = ekeko_type_instance_private_get_internal(type, ekeko_object_type_get(), object);
	object->prv->type = type;
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] construct %p %p %p\n", object, object->prv, object->prv->type);
#endif
	/* call all the constructors on the type */
	ekeko_type_construct(type, instance);
}

void ekeko_object_event_listener_add(Ekeko_Object *obj, const char *type,
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
	events = lst;
}

void ekeko_object_event_listener_remove(Ekeko_Object *obj, const char *type,
		Event_Listener el, Eina_Bool bubble, void *data)
{
	Ekeko_Object_Private *prv;
	Object_Event *oe;
	Eina_List *events, *l;

	prv = PRIVATE(obj);
	events = eina_hash_find(prv->listeners, type);
	for (l = events; l; l = eina_list_next(l))
	{
		oe = eina_list_data_get(l);
		if ((oe->bubble == bubble) && (oe->data == data) &&
			(oe->el == el))
		{
			Eina_List *lst;

			lst = eina_list_remove_list(events, l);
			if (!lst)
				eina_hash_del(prv->listeners, type, events);

			free(oe);
			return;
		}
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Gets the object type.
 * @return The type definition for an object
 */
Ekeko_Type *ekeko_object_type_get(void)
{
	static Ekeko_Type *object_type = NULL;

	if (!object_type)
	{
		object_type = ekeko_type_new(TYPE_NAME, sizeof(Ekeko_Object),
				sizeof(Ekeko_Object_Private), NULL, _ctor, _dtor, NULL);
		// TODO register the type's event, with type_event_new
	}

	return object_type;
}
/**
 * Deletes an Ekeko_Object
 * @param o The Ekeko_Object to delete
 */
EAPI void ekeko_object_delete(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(o);
	/* before destroying the object first detach it */
	if (prv->parent)
	{
		ekeko_object_child_remove(prv->parent, o);
	}
	/* send the delete event */
	/* FIXME the delete event should bubble? if so, detach first
	 * and then send this?
	 */
	ekeko_type_instance_delete(o);
}
/**
 * Sets a private data pointer on an object
 * @param o The object to set the data to
 * @param name The name to associate this data to
 * @param data The private data to set
 */
EAPI void ekeko_object_user_data_set(Ekeko_Object *o, const char *name, void *data)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(o);
	eina_hash_add(prv->user, name, data);
}
/**
 * Gets a private data pointer from an object previously
 * set with ekeko_object_user_data_set()
 * @param o The object to set the data to
 * @param name The name the data was associated with
 * @return The private user data
 */
EAPI void * ekeko_object_user_data_get(Ekeko_Object *o, const char *name)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(o);
	return eina_hash_find(prv->user, name);
}
/**
 * Dispatch an event
 * @param o The object that will trigger the event
 * @param e The event to dispatch
 */
EAPI void ekeko_object_event_dispatch(Ekeko_Object *o, Ekeko_Event *e)
{
	Ekeko_Object_Private *prv;

	/* TODO set the phase on the event */
	prv = PRIVATE(o);
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Object] Dispatching event %s\n", e->type);
#endif
	_event_dispatch(o, e, EINA_FALSE);
	if (e->bubbles == EINA_TRUE)
	{
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Object] Event %s going to bubble %p %p\n", e->type, o, prv->parent);
#endif
		while (prv->parent)
		{
			Ekeko_Object *parent = prv->parent;
			prv = PRIVATE(parent);
			_event_dispatch(parent, e, EINA_TRUE);
		}
	}
}

/**
 *
 */
EAPI const char * ekeko_object_type_name_get(const Ekeko_Object *obj)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(obj);
	return ekeko_type_name_get(prv->type);
}

/**
 * Appends an object as a child of another
 * @param p The parent object
 * @param o The child object
 * @return EINA_TRUE if the child was succesful appended, EINA_FALSE
 * otherwise
 */
EAPI Eina_Bool ekeko_object_child_append(Ekeko_Object *p, Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;
	Ekeko_Type *t;

	prv = PRIVATE(p);
	t = prv->type;
	if (ekeko_type_appendable(t, p, o))
	{
		Ekeko_Object_Private *pprv, *oprv;
		Ekeko_Event_Mutation evt;

		pprv = PRIVATE(p);
		oprv = PRIVATE(o);
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Object] Setting the parent of %p (%p) to %p (%p) \n", o, oprv, p, pprv);
#endif
		if (oprv->parent == p)
			return;
		if (oprv->parent)
			ekeko_object_child_remove(oprv->parent, o);
#if 0
		/* check if the object has some pending changes */
		if (oprv->changed)
		{
			if (oprv->parent)
				_unchange_recursive(oprv->parent, oprv->changed);
			_change_recursive(p, oprv->changed);
		}
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Object] pchanged = %d ochanged = %d\n", pprv->changed, oprv->changed);
#endif
#endif
		/* send the parent set event */
		ekeko_event_mutation_init(&evt, EKEKO_EVENT_PARENT_SET,
				(Ekeko_Object *)o, (Ekeko_Object *)p, NULL,
				NULL, NULL);
		ekeko_object_event_dispatch((Ekeko_Object *)o,
				(Ekeko_Event *)&evt);
		/* send the chld remove event */
		ekeko_event_mutation_init(&evt, EKEKO_EVENT_OBJECT_APPEND,
				(Ekeko_Object *)p, (Ekeko_Object *)o, NULL,
				NULL, NULL);
		ekeko_object_event_dispatch((Ekeko_Object *)p,
				(Ekeko_Event *)&evt);

		pprv->children = eina_inlist_append(pprv->children, EINA_INLIST_GET(oprv));
		oprv->parent = p;

		return EINA_TRUE;
	}
	else
		return EINA_FALSE;
}

/**
 * Remove a child of object from another
 * @param p The parent object
 * @param o The child object to remove
 */
EAPI void ekeko_object_child_remove(Ekeko_Object *p, Ekeko_Object *o)
{
	Ekeko_Object_Private *pprv, *oprv;
	Ekeko_Event_Mutation evt;

	if (!p || !o)
		return;

	pprv = PRIVATE(p);
	oprv = PRIVATE(o);

	if (oprv->parent != p)
		return;

	/* the child remove event */
	ekeko_event_mutation_init(&evt, EKEKO_EVENT_OBJECT_REMOVE, (Ekeko_Object *)p,
			(Ekeko_Object *)o, NULL, NULL, NULL);
	ekeko_object_event_dispatch(p, (Ekeko_Event *)&evt);
	/* the parent cleanup event */
	ekeko_event_mutation_init(&evt, EKEKO_EVENT_PARENT_UNSET, (Ekeko_Object *)o,
			(Ekeko_Object *)p, NULL, NULL, NULL);
	ekeko_object_event_dispatch(o, (Ekeko_Event *)&evt);

	pprv->children = eina_inlist_remove(pprv->children,
			EINA_INLIST_GET(oprv));
	oprv->parent = NULL;
}

/**
 * Get the number of childs an object has
 * @param o The object to get the number of childs from
 * @return The number of childs
 */
EAPI int ekeko_object_child_count(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv = PRIVATE(o);

	return eina_inlist_count(prv->children);
}
/**
 * Gets the child at a given index
 * @param o The object to get the child from
 * @param index The index of the child to get
 * @return The child
 */
EAPI Ekeko_Object * ekeko_object_child_get_at(Ekeko_Object *o, unsigned int index)
{
	Eina_Accessor *a;
	Ekeko_Object_Private *prv = PRIVATE(o);
	Ekeko_Object *child;

	a = eina_inlist_accessor_new(prv->children);
	eina_accessor_data_get(a, index, (void **)&child);

	return child;
}

/**
 * Gets the last child of an object
 * @param o The object to get the last child from
 * @return The last child
 */
EAPI Ekeko_Object * ekeko_object_child_last_get(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;
	Ekeko_Object_Private *chprv;

	prv = PRIVATE(o);
	chprv = (Ekeko_Object_Private *)prv->children;
	if (!chprv) return NULL;
	else
	{
		chprv = (Ekeko_Object_Private *)chprv->__in_list.last;
		return chprv->rel;
	}
}

/**
 * Gets the first child of an object
 * @param o The object to get the first child from
 * @return The first child
 */
EAPI Ekeko_Object * ekeko_object_child_first_get(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;
	Ekeko_Object_Private *chprv;

	prv = PRIVATE(o);
	chprv = (Ekeko_Object_Private *)prv->children;
	if (!chprv) return NULL;
	else return chprv->rel;
}

/**
 * Gets the next object
 * @param o The object to get the next object from
 * @return The next object
 */
EAPI Ekeko_Object * ekeko_object_next(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(o);
	prv = (Ekeko_Object_Private *)prv->__in_list.next;
	if (!prv) return NULL;
	else return prv->rel;
}

/**
 * Gets the previous object
 * @param o The object to get the previous object from
 * @return The previous object
 */
EAPI Ekeko_Object * ekeko_object_prev(Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(o);
	prv = (Ekeko_Object_Private *)prv->__in_list.prev;
	if (!prv) return NULL;
	else return prv->rel;
}
/**
 * Gets the parent object from another
 * @param o The object to get the parent from
 * @return The parent object
 */
EAPI Ekeko_Object * ekeko_object_parent_get(const Ekeko_Object *o)
{
	Ekeko_Object_Private *prv;

	prv = PRIVATE(o);
	return prv->parent;
}
/**
 * Sets a value for a property of an object
 * @param o The object to set the property to
 * @param name The property name
 * @param value The value to set
 * @return EINA_TRUE if the property was correctly set or EINA_FALSE otherwise
 */
EAPI Eina_Bool ekeko_object_property_value_set(Ekeko_Object *o, char *name, Ekeko_Value *value)
{
	Ekeko_Object_Private *prv;
	Ekeko_Value prev_value;
	Ekeko_Event_Mutation evt;
	Object_Property *prop;

	void *curr, *prev;

	if (!o || !name)
		return EINA_FALSE;

	prv = PRIVATE(o);
	DBG("Setting: %s %p %p %p\n", name, o, prv, prv->type);
	prop = eina_hash_find(prv->properties, name);
	if (!prop)
	{
		WRN("Property %s does not exist", name);
		return EINA_FALSE;
	}
	if (prop->type == EKEKO_PROPERTY_UNDEFINED)
		return EINA_FALSE;

	if (prop->type != EKEKO_PROPERTY_VALUE && prop->type != value->type)
	{
		ERR("Value types dont match %s.%s %d %d\n", ekeko_object_type_name_get(o),
				name, prop->type, value->type);
		return EINA_FALSE;
	}
	/* Initialize the type in case the property value type is PROPERTY_VALUE */
	prev_value.type = value->type;
	/* send the generic event */
	ekeko_event_mutation_init(&evt, EKEKO_EVENT_PROP_MODIFY, o, o, prop->name,
			&prev_value, value);
	ekeko_object_event_dispatch((Ekeko_Object *)o, (Ekeko_Event *)&evt);
#if 0
	/* send the specific event */
	{
		char evt_name[256];

		strcpy(evt_name, name);
		strcat(evt_name, "Changed");
		ekeko_event_mutation_init(&evt, evt_name, o, o, name,
			&prev_value, value);
		ekeko_object_event_dispatch((Ekeko_Object *)o, (Ekeko_Event *)&evt);
	}
#endif
	/* in case the property is an object send the reference event */
	if (prop->type == EKEKO_PROPERTY_OBJECT)
	{
		if ((prev_value.value.object) &&
				(prev_value.value.object != value->value.object))
		{
			ekeko_event_mutation_init(&evt, EKEKO_EVENT_OBJECT_REFERENCED,
					prev_value.value.object, o, NULL, NULL, NULL);
			ekeko_object_event_dispatch(prev_value.value.object,
					(Ekeko_Event *)&evt);

		}
		ekeko_event_mutation_init(&evt, EKEKO_EVENT_OBJECT_REFERENCED,
				value->value.object, o, NULL, NULL, NULL);
		ekeko_object_event_dispatch(value->value.object, (Ekeko_Event *)&evt);
	}
	return EINA_TRUE;
}
/**
 * Gets a value from a property of an object
 * @param o The object to set the property to
 * @param name The property name
 * @param value The value to set
 * @return EINA_TRUE if the property was correctly fetched or EINA_FALSE otherwise
 */
EAPI Eina_Bool ekeko_object_property_value_get(Ekeko_Object *o, char *name, Ekeko_Value *value)
{
	Ekeko_Object_Private *prv;
	Ekeko_Event_Mutation ev;
	Object_Property *prop;

	prv = PRIVATE(o);
	prop = eina_hash_find(prv->properties, name);
	if (!prop)
	{
		WRN("Property %s does not exist", name);
		return EINA_FALSE;
	}
	ekeko_event_mutation_init(&ev, EKEKO_EVENT_VALUE_GET,
			o, NULL, prop->name, NULL, value);
	ekeko_object_event_dispatch(o, &ev);

	return EINA_TRUE;
}
/**
 * Gets the type of the stored value on a property
 * @param o The object to get the property type from
 * @param name The name of the property
 * @return The type of the property
 */
EAPI int ekeko_object_property_type_get(Ekeko_Object *o, const char *name)
{
	Ekeko_Object_Private *prv;
	Object_Property *prop;

	if (!name || !o) return EKEKO_PROPERTY_UNDEFINED;

	prv = PRIVATE(o);
	prop = eina_hash_find(prv->properties, name);
	if (!prop) return EKEKO_PROPERTY_UNDEFINED;
	return prop->type;
}
/**
 * Gets the unique identifier for a name based property of an object
 * @param[in] o The object to get the property from
 * @param[in] name The name of the property
 * @return The unique identifier for this property
 */
EAPI Ekeko_Property_Id ekeko_object_property_get(const Ekeko_Object *o, char *name)
{
	Ekeko_Object_Private *prv;
	Object_Property *prop;

	prv = PRIVATE(o);
	prop = eina_hash_find(prv->properties, name);
	if (!prop)
	{
		WRN("Property %s does not exist", name);
		return NULL;
	}
	return prop->name;
}
/**
 * Add a property to an object
 * @param o The object the add the property to
 * @param name The name of the property
 * @param type The type of the value the property will handle
 * @return The property name
 */
EAPI const char * ekeko_object_property_add(Ekeko_Object *o, const char *name, int type)
{
	Ekeko_Object_Private *prv;
	Object_Property *prop;

	if (!name || !o) return NULL;

	prv = PRIVATE(o);
	prop = eina_hash_find(prv->properties, name);
	if (prop)
	{
		WRN("Property %s already exists", name);
		return prop->name;
	}
	/* create a new property */
	prop = calloc(1, sizeof(Object_Property));
	prop->name = eina_stringshare_add(name);
	prop->type = type;
	eina_hash_add(prv->properties, name, prop);

	return prop->name;
}
/**
 * Deletes a prooperty from an object
 * @param o The object the delete the property from
 * @param name The name of the property
 */
EAPI void ekeko_object_property_del(Ekeko_Object *o, const char *name)
{
	Ekeko_Object_Private *prv;
	Object_Property *prop;

	if (!name) return;
	prv = PRIVATE(o);
	prop = eina_hash_find(prv->properties, name);
	if (!prop)
	{
		WRN("Property %s does not exist", name);
		return;
	}
	eina_hash_del(prv->properties, name, prop);
	eina_stringshare_del(prop->name);
	free(prop);
}

/**
 *
 */
EAPI void ekeko_object_dump_printf(Ekeko_Object *o, int level)
{
	int i;

	for (i = 1; i <= level; i++)
		printf("\t");
	printf("> %s (%p) ", ekeko_object_type_name_get(o), o);
	/** TODO foreach attribute, dump it */
	/* some useful properties */
	printf("\n");
}
/**
 *
 */
EAPI void ekeko_object_dump(Ekeko_Object *o, Ekeko_Object_Dump dump)
{
	_dump_recursive(o, dump, 0);
}

/**
 * check that the object is of that type
 *
 */
EAPI Ekeko_Object * ekeko_object_cast(Ekeko_Object *o, Ekeko_Type *t)
{
	if (!o || !t)
		return NULL;
	if (!ekeko_type_instance_is_of_type(o, t))
	{
		WRN("Object %p is not of type %s but %s\n", o, ekeko_type_name_get(t), ekeko_object_type_name_get(o));
	}
	return o;
}
