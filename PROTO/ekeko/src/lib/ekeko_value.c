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
typedef struct _Ekeko_Value_Impl
{
	const char *name;
	Ekeko_Value_Compare cmp;
	Ekeko_Value_String_From string_from;
	Ekeko_Value_String_To string_to;
} Ekeko_Value_Impl;

static Eina_Array *_implementations = NULL;

static char * _int_string_to(Ekeko_Value *v)
{
	char *str;

	asprintf(&str, "%d", v->value.int_value);
	return str;
}

static Eina_Bool _int_string_from(Ekeko_Value *v, const char *str)
{
	v->value.int_value = strtol(str, NULL, 10);
	return EINA_TRUE;
}

static Eina_Bool _int_cmp(Ekeko_Value *v1, Ekeko_Value *v2)
{
	if (v1->value.int_value == v2->value.int_value)
		return EINA_FALSE;
	return EINA_TRUE;
}

static char * _bool_string_to(Ekeko_Value *v)
{
	char *str;

	asprintf(&str, "%d", v->value.bool_value);
	return str;
}

static Eina_Bool _bool_string_from(Ekeko_Value *v, const char *str)
{
	/* TODO check false/true strings */
	v->value.bool_value = strtol(str, NULL, 10) > 1 ? EINA_TRUE : EINA_FALSE;
	return EINA_TRUE;
}

static Eina_Bool _bool_cmp(Ekeko_Value *v1, Ekeko_Value *v2)
{
	if (v1->value.bool_value == v2->value.bool_value)
		return EINA_FALSE;
	return EINA_TRUE;
}

static char * _char_string_to(Ekeko_Value *v)
{
	char *str;

	asprintf(&str, "%c", v->value.char_value);
	return str;
}

static Eina_Bool _char_string_from(Ekeko_Value *v, const char *str)
{
	v->value.char_value = *str;
	return EINA_TRUE;
}

static Eina_Bool _char_cmp(Ekeko_Value *v1, Ekeko_Value *v2)
{
	if (v1->value.char_value == v2->value.char_value)
		return EINA_FALSE;
	return EINA_TRUE;
}

static char * _float_string_to(Ekeko_Value *v)
{
	char *str;

	asprintf(&str, "%f", v->value.float_value);
	return str;
}

static Eina_Bool _float_string_from(Ekeko_Value *v, const char *str)
{
	v->value.float_value = strtof(str, NULL);
	return EINA_TRUE;
}

static Eina_Bool _float_cmp(Ekeko_Value *v1, Ekeko_Value *v2)
{
	if (v1->value.float_value == v2->value.float_value)
		return EINA_FALSE;
	return EINA_TRUE;
}

static char * _double_string_to(Ekeko_Value *v)
{
	char *str;

	asprintf(&str, "%f", v->value.double_value);
	return str;
}

static Eina_Bool _double_string_from(Ekeko_Value *v, const char *str)
{
	v->value.double_value = strtod(str, NULL);
	return EINA_TRUE;
}

static Eina_Bool _double_cmp(Ekeko_Value *v1, Ekeko_Value *v2)
{
	if (v1->value.double_value == v2->value.double_value)
		return EINA_FALSE;
	return EINA_TRUE;
}

static char * _rectangle_string_to(Ekeko_Value *v)
{
	char *str;

	asprintf(&str, "%d %d %d %d",
			v->value.rect.x,
			v->value.rect.y,
			v->value.rect.w,
			v->value.rect.h);
	return str;
}

static Eina_Bool _rectangle_string_from(Ekeko_Value *v, const char *str)
{
	if (sscanf(str, "%d %d %d %d",
			&v->value.rect.x,
			&v->value.rect.y,
			&v->value.rect.w,
			&v->value.rect.h) < 4)
		return EINA_FALSE;
	return EINA_TRUE;
}

static Eina_Bool _rectangle_cmp(Ekeko_Value *v1, Ekeko_Value *v2)
{
	if (v1->value.rect.x == v2->value.rect.x &&
			v1->value.rect.y == v2->value.rect.y &&
			v1->value.rect.w == v2->value.rect.w &&
			v1->value.rect.h == v2->value.rect.h)
		return EINA_FALSE;
	return EINA_TRUE;
}

static char * _string_string_to(Ekeko_Value *v)
{
	return strdup(v->value.string_value);
}

static Eina_Bool _string_string_from(Ekeko_Value *v, const char *str)
{
	v->value.string_value = strdup(str);
	return EINA_TRUE;
}

static Eina_Bool _string_cmp(Ekeko_Value *v1, Ekeko_Value *v2)
{
	if (!strcmp(v1->value.string_value, v2->value.string_value))
		return EINA_TRUE;
	return EINA_FALSE;

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ekeko_value_init(void)
{
	_implementations = eina_array_new(5);
	EKEKO_PROPERTY_INT = ekeko_value_register("int",
		_int_cmp, _int_string_from, _int_string_to);
	EKEKO_PROPERTY_BOOL = ekeko_value_register("bool",
		_bool_cmp, _bool_string_from, _bool_string_to);
	EKEKO_PROPERTY_CHAR = ekeko_value_register("char",
		_char_cmp, _char_string_from, _char_string_to);
	EKEKO_PROPERTY_STRING = ekeko_value_register("string",
		_string_cmp, _string_string_from, _string_string_to);
}

void ekeko_value_shutdown(void)
{
	eina_array_free(_implementations);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Value_Type EKEKO_PROPERTY_UNDEFINED; 
Ekeko_Value_Type EKEKO_PROPERTY_INT; 
Ekeko_Value_Type EKEKO_PROPERTY_BOOL; 
Ekeko_Value_Type EKEKO_PROPERTY_CHAR; 
Ekeko_Value_Type EKEKO_PROPERTY_FLOAT;
Ekeko_Value_Type EKEKO_PROPERTY_DOUBLE;
Ekeko_Value_Type EKEKO_PROPERTY_SHORT;
Ekeko_Value_Type EKEKO_PROPERTY_LONG;
Ekeko_Value_Type EKEKO_PROPERTY_RECTANGLE; 
Ekeko_Value_Type EKEKO_PROPERTY_POINTER;
Ekeko_Value_Type EKEKO_PROPERTY_VALUE;
Ekeko_Value_Type EKEKO_PROPERTY_STRING;
Ekeko_Value_Type EKEKO_PROPERTY_OBJECT;

EAPI Ekeko_Value_Type ekeko_value_register(const char *name, Ekeko_Value_Compare cmp,
		Ekeko_Value_String_From string_from,
		Ekeko_Value_String_To string_to)
{
	static Ekeko_Value_Type count = 0;
	Ekeko_Value_Impl *impl;

	impl = malloc(sizeof(Ekeko_Value_Impl));
	impl->name = strdup(name);
	impl->cmp = cmp;
	impl->string_to = string_to;
	impl->string_from = string_from;

	eina_array_push(_implementations, impl);
	return ++count;
}

EAPI Eina_Bool ekeko_value_compare(Ekeko_Value *v1, Ekeko_Value *v2)
{
	Ekeko_Value_Impl *impl;

	impl = eina_array_data_get(_implementations, v1->type);
	return impl->cmp(v1, v2);
}

EAPI char * ekeko_value_string_to(Ekeko_Value *v)
{
	Ekeko_Value_Impl *impl;

	impl = eina_array_data_get(_implementations, v->type);
	return impl->string_to(v);
}

EAPI Eina_Bool ekeko_value_string_from(Ekeko_Value *v, Ekeko_Value_Type type, const char *str)
{
	Ekeko_Value_Impl *impl;

	impl = eina_array_data_get(_implementations, type);
	return impl->string_from(v, str);
}
