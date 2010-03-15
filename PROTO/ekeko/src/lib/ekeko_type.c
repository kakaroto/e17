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
struct _Ekeko_Type
{
	char *name;
	size_t size;
	size_t priv_size;
	Ekeko_Type *parent;

	Ekeko_Type_Constructor ctor;
	Ekeko_Type_Destructor dtor;
	Ekeko_Type_Appendable append;
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

static void type_destruct_internal(Ekeko_Type *type, void *object)
{
	if (!type)
		return;

	if (type->dtor)
		type->dtor(object);

	if (type->parent)
		type_destruct_internal(type->parent, object);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ekeko_type_construct(Ekeko_Type *t, void *instance)
{
	if (!t)
		return;

	if (t->parent)
		ekeko_type_construct(t->parent, instance);

	if (t->ctor)
		t->ctor(instance);
}

void * ekeko_type_instance_private_get_internal(Ekeko_Type *final, Ekeko_Type *t, void *instance)
{
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Type] private get %s (PUB=%d) %s (PRIV_OFF=%d) %p\n", final->name, type_public_size_get(final), t->name, type_private_size_get(t->parent), instance);
#endif
	return (char *)instance + type_public_size_get(final) + type_private_size_get(t->parent);
}

const char * ekeko_type_name_get(Ekeko_Type *t)
{
	return t->name;
}

Eina_Bool ekeko_type_appendable(Ekeko_Type *t, void *instance, void *child)
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
	ekeko_object_construct(type, instance);
	return instance;
}

/**
 *
 * @param instance
 */
void ekeko_type_instance_delete(void *instance)
{
	Ekeko_Type *type;
	Ekeko_Event ev;

	if (!instance)
		return;
	/* first trigger the listeners for the Delete event */
	ekeko_event_init(&ev, EKEKO_EVENT_OBJECT_DELETE, instance, EINA_FALSE);
	ekeko_object_event_dispatch(instance, &ev);

	type = ekeko_object_private_type_get(instance);
	if (!type)
		return;
	type_destruct_internal(type, instance);
}

EAPI void * ekeko_type_instance_private_get(Ekeko_Type *t, void *instance)
{
	Ekeko_Type *final;

	final = ekeko_object_private_type_get(instance);
	return ekeko_type_instance_private_get_internal(final, t, instance);
}

/**
 * TODO make a function that instead of receiving the name
 * should also recieve the type itself to avoid the strcmp()
 */
EAPI Eina_Bool ekeko_type_instance_is_of(void *instance, const char *type)
{
	Ekeko_Type *t;

	t = ekeko_object_private_type_get(instance);
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

	t = ekeko_object_private_type_get(instance);
	do
	{
		if (t == type)
			return EINA_TRUE;
		t = t->parent;
	}
	while (t);

	return EINA_FALSE;
}
