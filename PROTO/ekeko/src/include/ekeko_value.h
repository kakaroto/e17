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
#ifndef EKEKO_VALUE_H_
#define EKEKO_VALUE_H_

typedef int Ekeko_Value_Type;
typedef struct _Ekeko_Value
{
	Ekeko_Value_Type type;
	union
	{
		int int_value;
		Eina_Bool bool_value;
		char char_value;
		float float_value;
		double double_value;
		short short_value;
		long long_value;
		void *pointer_value;
		char *string_value;
		Eina_Rectangle rect;
		Ekeko_Object *object;
	} value;
} Ekeko_Value;

typedef Eina_Bool (*Ekeko_Value_Compare)(Ekeko_Value *v1, Ekeko_Value *v2);
typedef Eina_Bool (*Ekeko_Value_String_From)(Ekeko_Value *v, const char *str);
typedef char * (*Ekeko_Value_String_To)(Ekeko_Value *v);

#define EKEKO_VALUE_CMP(f) ((Ekeko_Value_Compare)(f))
#define EKEKO_VALUE_STRING_FROM(f) ((Ekeko_Value_String_From)(f))
#define EKEKO_VALUE_STRING_TO(f) ((Ekeko_Value_String_To)(f))

extern Ekeko_Value_Type EKEKO_PROPERTY_UNDEFINED; /**< The value of the property is underfined */
extern Ekeko_Value_Type EKEKO_PROPERTY_INT; /**< The value of the property is an Ekeko_Value_Typeeger */
extern Ekeko_Value_Type EKEKO_PROPERTY_BOOL; /**< The value of the property is a boolean (Bool) */
extern Ekeko_Value_Type EKEKO_PROPERTY_CHAR; /**< The value of the property is a char */
extern Ekeko_Value_Type EKEKO_PROPERTY_FLOAT; /**< The value of the property is a float */
extern Ekeko_Value_Type EKEKO_PROPERTY_DOUBLE; /**< The value of the property is a double */
extern Ekeko_Value_Type EKEKO_PROPERTY_SHORT; /**< The value of the property is a short */
extern Ekeko_Value_Type EKEKO_PROPERTY_LONG; /**< The value of the property is a long */
extern Ekeko_Value_Type EKEKO_PROPERTY_RECTANGLE; /**< The value of the property is a rectangle */
extern Ekeko_Value_Type EKEKO_PROPERTY_POINTER; /**< The value of the property is a poEkeko_Value_Typeer (void *) */
extern Ekeko_Value_Type EKEKO_PROPERTY_VALUE; /**< The value of the property is value */
extern Ekeko_Value_Type EKEKO_PROPERTY_STRING; /**< The value of the property is a string (char *) */
extern Ekeko_Value_Type EKEKO_PROPERTY_OBJECT; /**< The value of the property is an Object */

EAPI Ekeko_Value_Type ekeko_value_register(const char *name, Ekeko_Value_Compare cmp,
		Ekeko_Value_String_From string_from,
		Ekeko_Value_String_To string_to);
EAPI Eina_Bool ekeko_value_compare(Ekeko_Value *v1, Ekeko_Value *v2);
EAPI char * ekeko_value_string_to(Ekeko_Value *v);
EAPI Eina_Bool ekeko_value_string_from(Ekeko_Value *v, Ekeko_Value_Type type, const char *str);

static inline void ekeko_value_int_from(Ekeko_Value *v, int i)
{
	v->type = EKEKO_PROPERTY_INT;
	v->value.int_value = i;
}

static inline void ekeko_value_float_from(Ekeko_Value *v, float f)
{
	v->type = EKEKO_PROPERTY_FLOAT;
	v->value.float_value = f;
}

static inline void ekeko_value_str_from(Ekeko_Value *v, char *str)
{
	v->type = EKEKO_PROPERTY_STRING;
	v->value.string_value = str;
}

static inline void ekeko_value_rectangle_from(Ekeko_Value *v, Eina_Rectangle *rect)
{
	v->type = EKEKO_PROPERTY_RECTANGLE;
	v->value.rect = *rect;
}

static inline void ekeko_value_rectangle_coords_from(Ekeko_Value *v, int x, int y, int w, int h)
{
	v->type = EKEKO_PROPERTY_RECTANGLE;
	v->value.rect.x = x;
	v->value.rect.y = y;
	v->value.rect.w = w;
	v->value.rect.h = h;
}

static inline void ekeko_value_bool_from(Ekeko_Value *v, Eina_Bool b)
{
	v->type = EKEKO_PROPERTY_BOOL;
	v->value.bool_value = b;
}

static inline void ekeko_value_pointer_from(Ekeko_Value *v, void *p)
{
	v->type = EKEKO_PROPERTY_POINTER;
	v->value.pointer_value = p;
}


static inline void ekeko_value_object_from(Ekeko_Value *v, Ekeko_Object *o)
{
	v->type = EKEKO_PROPERTY_OBJECT;
	v->value.object = o;
}

#endif /* EKEKO_VALUE_H_ */
