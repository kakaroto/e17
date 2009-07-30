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

/**
 * @brief Possible types of a property.
 */
#define EKEKO_PROPERTY_UNDEFINED 0
#define EKEKO_PROPERTY_INT 1 /**< The value of the property is an integer */          //!< PROPERTY_INT
#define EKEKO_PROPERTY_BOOL 2 /**< The value of the property is a boolean (Bool) */    //!< PROPERTY_BOOL
#define EKEKO_PROPERTY_CHAR 3 /**< The value of the property is a char */              //!< PROPERTY_CHAR
#define EKEKO_PROPERTY_FLOAT 4 /**< The value of the property is a float */             //!< PROPERTY_FLOAT
#define EKEKO_PROPERTY_DOUBLE 5 /**< The value of the property is a double */            //!< PROPERTY_DOUBLE
#define EKEKO_PROPERTY_SHORT 6 /**< The value of the property is a short */             //!< PROPERTY_SHORT
#define EKEKO_PROPERTY_LONG 7 /**< The value of the property is a long */              //!< PROPERTY_LONG
#define EKEKO_PROPERTY_RECTANGLE 8 /**< The value of the property is a rectangle */         //!< PROPERTY_RECTANGLE
#define EKEKO_PROPERTY_POINTER 9 /**< The value of the property is a pointer (void *) */  //!< PROPERTY_POINTER
#define EKEKO_PROPERTY_VALUE 10 /**< The value of the property is value */  //!< PROPERTY_VALUE
#define EKEKO_PROPERTY_STRING 11 /**< The value of the property is a string (char *) */ //!< PROPERTY_STRING
#define EKEKO_PROPERTY_OBJECT 12 /**< The value of the property is an Object */ //!< PROPERTY_OBJECT
#define EKEKO_PROPERTY_LAST EKEKO_PROPERTY_OBJECT

typedef int Ekeko_Value_Type;
/**
 * @brief
 */
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

typedef void * (*Ekeko_Value_Create)(void);
typedef void (*Ekeko_Value_Free)(void *v);
typedef Eina_Bool (*Ekeko_Value_Compare)(void *a, void *b);
typedef void (*Ekeko_Value_Pointer_From)(Ekeko_Value *v, void *val);
typedef void (*Ekeko_Value_Pointer_To)(Ekeko_Value *v, void *ptr);

#define EKEKO_VALUE_CREATE(f) ((Ekeko_Value_Create)(f))
#define EKEKO_VALUE_FREE(f) ((Ekeko_Value_Free)(f))
#define EKEKO_VALUE_CMP(f) ((Ekeko_Value_Compare)(f))
#define EKEKO_VALUE_POINTER_FROM(f) ((Ekeko_Value_Pointer_From)(f))
#define EKEKO_VALUE_POINTER_TO(f) ((Ekeko_Value_Pointer_To)(f))

void ekeko_value_pointer_from(Ekeko_Value *v, Ekeko_Value_Type vtype, void *val);
void ekeko_value_free(Ekeko_Value *v, Ekeko_Value_Type vtype);
int ekeko_value_register(const char *name, Ekeko_Value_Create create,
		Ekeko_Value_Free free, Ekeko_Value_Compare cmp,
		Ekeko_Value_Pointer_From pointer_from,
		Ekeko_Value_Pointer_To pointer_to);

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


static inline void ekeko_value_object_from(Ekeko_Value *v, Ekeko_Object *o)
{
	v->type = EKEKO_PROPERTY_OBJECT;
	v->value.object = o;
}

#endif /* EKEKO_VALUE_H_ */
