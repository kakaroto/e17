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
/* FIXME rename the callbacks, get/set isnt enough to describe what they do
 * FIXME check the PROPERTY_VALUE
 * pointer_from -> double: dont allocate just copy the vars, pointers or whatever
 *                 single: same
 * create -> double: dont called
 *           single: alloc the pointer malloc(ssize_t impl->size)
 * pointer_to -> strdup for string
 * free -> free the malloced area and free the char * or any other pointer
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Hash *_values = NULL;

typedef struct _Ekeko_Value_Impl
{
	const char *name;
	Ekeko_Value_Create create;
	Ekeko_Value_Compare cmp;
	Ekeko_Value_Free free;
	Ekeko_Value_Pointer_From pointer_from;
	Ekeko_Value_Pointer_From pointer_to;
} Ekeko_Value_Impl;

static inline Ekeko_Value_Impl * _implementation_get(int id)
{
	Ekeko_Value_Impl *impl;

	impl = eina_hash_find(_values, &id);
	return impl;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ekeko_value_create(Ekeko_Value *value, Ekeko_Value_Type type)
{
	switch (type)
	{
		case EKEKO_PROPERTY_UNDEFINED:
		printf("[Ekeko_Value] creating an undefined value?\n");
		//exit(1);
		break;

		/* there's no need to allocate anything for this property types */
		case EKEKO_PROPERTY_INT:
		case EKEKO_PROPERTY_BOOL:
		case EKEKO_PROPERTY_CHAR:
		case EKEKO_PROPERTY_FLOAT:
		case EKEKO_PROPERTY_DOUBLE:
		case EKEKO_PROPERTY_SHORT:
		case EKEKO_PROPERTY_LONG:
		case EKEKO_PROPERTY_RECTANGLE:
		case EKEKO_PROPERTY_STRING:
		case EKEKO_PROPERTY_OBJECT:
		break;

		case EKEKO_PROPERTY_VALUE:
#ifndef EKEKO_DEBUG
		printf("[Ekeko_Value] value create %d %d\n", type, value->type);
#endif
		ekeko_value_create(value, value->type);
		break;

		default:
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Value] value create %d\n", type);
#endif
		{
			Ekeko_Value_Impl *impl;

			impl = _implementation_get(type);
			value->value.pointer_value = impl->create();
		}
		break;
	}
}
void ekeko_value_pointer_double_to(Ekeko_Value *value, Ekeko_Value_Type type, void *ptr,
		void *prev, char *changed)
{
	*changed = EINA_FALSE;
	switch (type)
	{
		case EKEKO_PROPERTY_INT:
		*((int *)ptr) = value->value.int_value;
		if (*((int *)ptr) != *((int *)prev))
			*changed = EINA_TRUE;
		break;

		/* FIXME check the real difference < minimal float difference */
		case EKEKO_PROPERTY_FLOAT:
		*((float *)ptr) = value->value.float_value;
		if (*((float *)ptr) != *((float *)prev))
			*changed = EINA_TRUE;
		break;

		case EKEKO_PROPERTY_STRING:
		/* FIXME fix this mess */
		*((char **)ptr) = strdup(value->value.string_value);
		if (!*((char **)prev))
			*changed = EINA_TRUE;
		else if (!strcmp(*((char **)ptr), *((char **)prev)))
			*changed = EINA_TRUE;
		break;

		case EKEKO_PROPERTY_RECTANGLE:

		{
			Eina_Rectangle *c = (Eina_Rectangle *)ptr;
			Eina_Rectangle *p;

			*c = value->value.rect;
			p = (Eina_Rectangle *)prev;
			if ((c->x != p->x) || (c->y != p->y) || (c->w != p->w) || (c->h != p->h))
				*changed = EINA_TRUE;
		}
		break;

		case EKEKO_PROPERTY_BOOL:
		*((Eina_Bool *)ptr) = value->value.bool_value;
		if (*((Eina_Bool *)ptr) != *((Eina_Bool *)prev))
			*changed = EINA_TRUE;
		break;

		case EKEKO_PROPERTY_VALUE:
#ifndef EKEKO_DEBUG
		printf("[Ekeko_Value] Pointer double property value set %d %d\n", type, value->type);
#endif
		ekeko_value_pointer_double_to(value, value->type, ptr,
				prev, changed);
		break;

		default:
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Value] Pointer double set %d\n", type);
#endif
		{
			Ekeko_Value_Impl *impl;

			impl = _implementation_get(type);
			impl->pointer_to(value, ptr);
			if (impl->cmp(ptr, prev))
				*changed = EINA_TRUE;
		}
		break;
	}
}

/*
 * ptr points to the memory area where the values are stored
 */
void ekeko_value_pointer_to(Ekeko_Value *value, Ekeko_Value_Type vtype, void *ptr)
{
	switch (vtype)
	{
		case EKEKO_PROPERTY_UNDEFINED:
		printf("[Ekeko_Value] pointer to undefined value?\n");
		break;

		case EKEKO_PROPERTY_INT:
		*((int *)ptr) = value->value.int_value;
		break;

		case EKEKO_PROPERTY_FLOAT:
		*((float *)ptr) = value->value.float_value;
		break;

		case EKEKO_PROPERTY_STRING:
		*((char **)ptr) = strdup(value->value.string_value);
		break;

		case EKEKO_PROPERTY_RECTANGLE:
		*((Eina_Rectangle *)ptr) = value->value.rect;
		break;

		case EKEKO_PROPERTY_BOOL:
		*((Eina_Bool *)ptr) = value->value.bool_value;
		break;

		case EKEKO_PROPERTY_OBJECT:
		*((Ekeko_Object **)ptr) = value->value.object;
		break;

		case EKEKO_PROPERTY_VALUE:
		{
			Ekeko_Value *v = ptr;


			/* FIXME malloc the pointer */
			printf("[Ekeko_Value] value pointer to %p\n", v->value.pointer_value);
			v->type = value->type;
			/* TODO if the pointer already has a property allocated
			 * also free it
			 */
			if (v->value.pointer_value)
				ekeko_value_free(v, v->type);
			ekeko_value_create(v, v->type);
			ekeko_value_pointer_to(value, value->type, v->value.pointer_value ? v->value.pointer_value : &v->value);
		}
		break;

		default:
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Value] Pointer to %d\n", vtype);
#endif
		{
			Ekeko_Value_Impl *impl;

			impl = _implementation_get(vtype);
			impl->pointer_to(value, ptr);
		}
		break;
	}
}

void ekeko_value_init(void)
{
	_values = eina_hash_int32_new(NULL);
}

void ekeko_value_shutdown(void)
{
	/* TODO */
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
int ekeko_value_register(const char *name, Ekeko_Value_Create create,
		Ekeko_Value_Free free, Ekeko_Value_Compare cmp,
		Ekeko_Value_Pointer_From pointer_from,
		Ekeko_Value_Pointer_To pointer_to)
{
	static int _curr = EKEKO_PROPERTY_LAST; // the last internal property
	Ekeko_Value_Impl *impl;

	impl = malloc(sizeof(Ekeko_Value_Impl));
	impl->name = strdup(name);
	impl->cmp = cmp;
	impl->create = create;
	impl->free = free;
	impl->pointer_to = pointer_to;
	impl->pointer_from = pointer_from;

	++_curr;
	eina_hash_add(_values, &_curr, impl);
	return _curr;
}

void ekeko_value_free(Ekeko_Value *v, Ekeko_Value_Type vtype)
{
	switch (vtype)
	{
		case EKEKO_PROPERTY_UNDEFINED:
		printf("[Ekeko_Value] freeing an undefined value?\n");
		break;

		case EKEKO_PROPERTY_INT:
		case EKEKO_PROPERTY_BOOL:
		case EKEKO_PROPERTY_CHAR:
		case EKEKO_PROPERTY_FLOAT:
		case EKEKO_PROPERTY_DOUBLE:
		case EKEKO_PROPERTY_SHORT:
		case EKEKO_PROPERTY_LONG:
		case EKEKO_PROPERTY_RECTANGLE:
		break;

		case EKEKO_PROPERTY_STRING:
		free(v->value.string_value);
		break;

		case EKEKO_PROPERTY_VALUE:
		ekeko_value_free(v, v->type);
		break;

		case EKEKO_PROPERTY_OBJECT:
		/* FIXME just unref the object */
		break;

		default:
		{
			Ekeko_Value_Impl *impl;

			impl = _implementation_get(vtype);
			impl->free(v->value.pointer_value);
			v->value.pointer_value = NULL;
		}
	}
}

void ekeko_value_pointer_from(Ekeko_Value *v, Ekeko_Value_Type vtype, void *ptr)
{
	switch (vtype)
	{
		case EKEKO_PROPERTY_UNDEFINED:
		v->type = EKEKO_PROPERTY_UNDEFINED;
		printf("[Ekeko_Value] pointer from undefined value?\n");
		break;

		case EKEKO_PROPERTY_STRING:
		v->type = EKEKO_PROPERTY_STRING;
		v->value.string_value = *(char **)ptr;
		break;

		case EKEKO_PROPERTY_INT:
		v->type = EKEKO_PROPERTY_INT;
		v->value.int_value = *(int *)ptr;
		break;

		case EKEKO_PROPERTY_FLOAT:
		v->type = EKEKO_PROPERTY_FLOAT;
		v->value.float_value = *(float *)ptr;
		break;

		case EKEKO_PROPERTY_RECTANGLE:
		v->type = EKEKO_PROPERTY_RECTANGLE;
		v->value.rect = *(Eina_Rectangle *)ptr;
		break;

		case EKEKO_PROPERTY_BOOL:
		v->type = EKEKO_PROPERTY_BOOL;
		v->value.bool_value = *(Eina_Bool *)ptr;
		break;

		case EKEKO_PROPERTY_OBJECT:
		v->type = EKEKO_PROPERTY_OBJECT;
		v->value.object = *(Ekeko_Object **)ptr;
		break;

		case EKEKO_PROPERTY_VALUE:
		{
			Ekeko_Value *val = ptr;
#ifndef EKEKO_DEBUG
			printf("[Ekeko_Value] value pointer from %p %p\n", ptr, ((Ekeko_Value *)ptr)->value.pointer_value);
#endif
			ekeko_value_pointer_from(v, val->type, &val->value);
		}
		break;

		default:
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Value] Pointer from %p %p\n", v, ptr);
#endif
		{
			Ekeko_Value_Impl *impl;

			v->type = vtype;
			impl = _implementation_get(vtype);
			impl->pointer_from(v, ptr);
		}
		break;
	}
}
