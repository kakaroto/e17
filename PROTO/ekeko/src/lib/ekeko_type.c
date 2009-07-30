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
#include "Ekeko.h"
#include "ekeko_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/**
 * @brief
 */
struct _Ekeko_Type
{
	char *name;
	size_t size;
	size_t priv_size;
	Ekeko_Type *parent;

	Ekeko_Type_Constructor ctor;
	Ekeko_Type_Destructor dtor;
	Ekeko_Type_Appendable append;

	Eina_Hash *properties;
};

static size_t type_public_size_get(Ekeko_Type *type)
{
	return type->size;
}

static size_t type_private_size_get(Ekeko_Type *type)
{
	size_t parent_size = 0;

	if (!type) return 0;
	if (type->parent) parent_size = type_private_size_get(type->parent);

	return type->priv_size + parent_size;
}

static size_t type_size_get(Ekeko_Type *type)
{
	size_t parent_size = 0;

	if (!type) return 0;
	if (type->parent) parent_size = type_size_get(type->parent);

	return type->size + type->priv_size + parent_size;
}

static inline void * _instance_property_offset_get(Ekeko_Type *type, Ekeko_Property *prop,
		ssize_t poffset, void *instance)
{
	Ekeko_Type *pt = property_type_get(prop);

	int offset = type_public_size_get(type) + type_private_size_get(pt->parent);
	return (char *)instance + offset + poffset;
}

static inline void * _instance_property_curr_ptr_get(Ekeko_Type *type, Ekeko_Property *prop, void *instance)
{
	ssize_t poffset = property_curr_offset_get(prop);

	return _instance_property_offset_get(type, prop, poffset, instance);
}

static inline void * _instance_property_prev_ptr_get(Ekeko_Type *type, Ekeko_Property *prop, void *instance)
{
	ssize_t poffset = property_prev_offset_get(prop);

	return _instance_property_offset_get(type, prop, poffset, instance);
}

static inline void * _instance_property_changed_ptr_get(Ekeko_Type *type, Ekeko_Property *prop, void *instance)
{
	ssize_t poffset = property_changed_offset_get(prop);

	return _instance_property_offset_get(type, prop, poffset, instance);
}

static void type_destruct_internal(Ekeko_Type *type, void *object)
{
	if (!type)
		return;

	if (type->dtor)
		type->dtor(object);

	if (type->parent)
		type_destruct_internal(type->parent, object);
}

/**
 *
 * @param type
 * @param prop_name
 * @return
 */
static Ekeko_Property *_property_get(Ekeko_Type *type, const char *prop_name)
{
	Ekeko_Property *property = NULL;

	do
	{
		property = eina_hash_find(type->properties, prop_name);
		if (!property)
			type = type->parent;
	} while (!property && type);

	return property;
}

/* TODO should we register types per document?
 * Note that type_new_name_from wont work until the that type has been
 * added i.e first calling ekeko_type_new()
 */
static Eina_Hash *_types = NULL;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void type_construct(Ekeko_Type *t, void *instance)
{
	if (!t)
		return;

	if (t->parent)
		type_construct(t->parent, instance);

	if (t->ctor)
		t->ctor(instance);
}

void * type_instance_private_get_internal(Ekeko_Type *final, Ekeko_Type *t, void *instance)
{
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Type] private get %s (PUB=%d) %s (PRIV_OFF=%d) %p\n", final->name, type_public_size_get(final), t->name, type_private_size_get(t->parent), instance);
#endif
	return (char *)instance + type_public_size_get(final) + type_private_size_get(t->parent);
}

Ekeko_Property * type_property_get(Ekeko_Type *t, const char *name)
{
	return _property_get(t, name);
}

void type_instance_property_value_get(Ekeko_Type *type, void *instance, char *prop_name, Ekeko_Value *v)
{
	Ekeko_Property *property;
	void *curr;

	if (!type || !instance || !prop_name)
		return;
	property = _property_get(type, prop_name);
	if (!property)
		return;
	curr = _instance_property_curr_ptr_get(type, property, instance);
	ekeko_value_pointer_from(v, ekeko_property_value_type_get(property), curr);
}

const char * type_name_get(Ekeko_Type *t)
{
	return t->name;
}

void type_instance_property_pointers_get(Ekeko_Type *t, Ekeko_Property *prop, void *instance,
		void **curr, void **prev, char **changed)
{
	Ekeko_Type *pt = property_type_get(prop);
	ssize_t coffset = property_curr_offset_get(prop);
	ssize_t poffset = property_prev_offset_get(prop);
	ssize_t choffset = property_changed_offset_get(prop);

	int offset = type_public_size_get(t) + type_private_size_get(pt->parent);
	*curr = (char *)instance + offset + coffset;
	*prev = (char *)instance + offset + poffset;
	*changed = (char *)instance + offset + choffset;
}

struct _Property_Iterator
{
	Eina_Iterator *it;
	Ekeko_Type *t;
};

Property_Iterator * type_property_iterator_new(Ekeko_Type *t)
{
	Property_Iterator *pit = malloc(sizeof(Property_Iterator));
	pit->t = t;
	pit->it = eina_hash_iterator_data_new(t->properties);

	return pit;
}

Eina_Bool type_property_iterator_next(Property_Iterator *pit, Ekeko_Property **prop)
{
	/* return false on no more properties */
	if (!eina_iterator_next(pit->it, (void **)prop))
	{
		do
		{
			pit->t = pit->t->parent;
			eina_iterator_free(pit->it);
			if (!pit->t)
				return EINA_FALSE;
			pit->it = eina_hash_iterator_data_new(pit->t->properties);
		} while (!eina_iterator_next(pit->it, (void **)prop));

		return EINA_TRUE;
	}
	return EINA_TRUE;
}

void type_property_iterator_free(Property_Iterator *pit)
{
	eina_iterator_free(pit->it);
	free(pit);
}

Eina_Bool type_appendable(Ekeko_Type *t, void *instance, void *child)
{
	if (!t->append)
		return EINA_FALSE;
	return t->append(instance, child);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Creates a new Ekeko_Type.
 *
 * @param name the type's unique name.
 * @param size the type's size.
 * @param parent the type's parent, or <code>null</code> if it doesnt have one.
 * @param ctor the type's constuctor.
 * @param dtor the type's destructor.
 * @return the newly created type.
 */
Ekeko_Type * ekeko_type_new(char *name, size_t size, size_t priv_size, Ekeko_Type *parent,
		Ekeko_Type_Constructor ctor, Ekeko_Type_Destructor dtor, Ekeko_Type_Appendable append)
{
	Ekeko_Type *type;

	type = malloc(sizeof(Ekeko_Type));
	if (!type) return NULL;

	/* initialize the type */
	type->name = strdup(name);
	type->size = size;
	type->priv_size = priv_size;
	type->parent = parent;
	type->ctor = ctor;
	type->dtor = dtor;
	type->append = append;
	type->properties = eina_hash_string_superfast_new(NULL);
	/* add the type */
	if (!_types) _types = eina_hash_string_superfast_new(NULL);
	eina_hash_add(_types, name, type);

	return type;
}

/**
 * Creates a new instance of the given type.
 *
 * @param type the Ekeko_Type to instantiate.
 * @return the instantiated type.
 */
void * ekeko_type_instance_new(Ekeko_Type *type)
{
	void *instance;

	if (!type)
		return NULL;
	instance = calloc(1, type_size_get(type));
	if (!instance)
		return NULL;
	object_construct(type, instance);
	return instance;
}

void * ekeko_type_instance_new_name_from(const char *name)
{
	Ekeko_Type *t;

	t = eina_hash_find(_types, name);
	return ekeko_type_instance_new(t);
}
/**
 *
 * @param instance
 */
void ekeko_type_instance_delete(void *instance)
{
	Ekeko_Type *type;

	if (!instance)
		return;
	type = object_private_type_get(instance);
	if (!type)
		return;
	type_destruct_internal(type, instance);
}

/**
 *
 * @param type
 * @param prop_name
 * @param prop_type
 * @param value_type
 * @param process_cb
 */
Ekeko_Property_Id ekeko_type_property_new(Ekeko_Type *type, char *prop_name,
		Type_Property_Type prop_type, Ekeko_Value_Type value_type,
		ssize_t curr_offset, ssize_t prev_offset, ssize_t changed_offset)
{
	Ekeko_Property *property;

	/* How to handle the changed thing?
	 * usually you do foo_changed:1
	 * but is impossible to get offset of a bit field (man offsetof)
	 * why dont use an uint32/64_t as a bit mask?
	 *
	 * We dont need the process function, just a generic callback handler
	 * for a change after the curr == prev comparision in case it has changed
	 *
	 */
	if (!type || !prop_name)
		return 0;

	property = property_new(type, prop_name, prop_type, value_type,
			curr_offset, prev_offset, changed_offset);
	eina_hash_add(type->properties, prop_name, property);

	return ekeko_property_id_get(property);
}

EAPI void * ekeko_type_instance_private_get(Ekeko_Type *t, void *instance)
{
	Ekeko_Type *final;

	final = object_private_type_get(instance);
	return type_instance_private_get_internal(final, t, instance);
}

/**
 * TODO make a function that instead of receiving the name
 * should also recieve the type itself to avoid the strcmp()
 */
EAPI Eina_Bool ekeko_type_instance_is_of(void *instance, const char *type)
{
	Ekeko_Type *t;

	t = object_private_type_get(instance);
	do
	{
		if (!strcmp(t->name, type))
			return EINA_TRUE;
		t = t->parent;
	}
	while (t);

	return EINA_FALSE;
}

/* TODO normalize this and the above function into one */
EAPI Eina_Bool ekeko_type_instance_is_of_type(void *instance, Ekeko_Type *type)
{
	Ekeko_Type *t;

	t = object_private_type_get(instance);
	do
	{
		if (t == type)
			return EINA_TRUE;
		t = t->parent;
	}
	while (t);

	return EINA_FALSE;
}
