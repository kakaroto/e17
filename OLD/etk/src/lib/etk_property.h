/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
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

/** @file etk_property.h */
#ifndef _ETK_PROPERTY_H_
#define _ETK_PROPERTY_H_

#include <stdarg.h>

#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Property Etk_Property
 * @brief The property system of Etk is a powerful system to set or to get the state of an object, or to be notified when
 * the state of an object has been changed.
 * @{
 */

/** @brief The type of a property */
typedef enum
{
   ETK_PROPERTY_NONE,           /**< Used when the property is uninitialized */
   ETK_PROPERTY_INT,            /**< The value of the property is an integer */
   ETK_PROPERTY_BOOL,           /**< The value of the property is a boolean (Etk_Bool) */
   ETK_PROPERTY_CHAR,           /**< The value of the property is a char */
   ETK_PROPERTY_FLOAT,          /**< The value of the property is a float */
   ETK_PROPERTY_DOUBLE,         /**< The value of the property is a double */
   ETK_PROPERTY_SHORT,          /**< The value of the property is a short */
   ETK_PROPERTY_LONG,           /**< The value of the property is a long */
   ETK_PROPERTY_OBJECT,         /**< The value of the property is an object (Etk_Object *) */
   ETK_PROPERTY_POINTER,        /**< The value of the property is a pointer (void *) */
   ETK_PROPERTY_STRING,         /**< The value of the property is a string (char *) */
   ETK_PROPERTY_OTHER           /**< Used when none of the above type can be used. The property can't have a default
                                 * value, and can't be accessible with etk_object_property_get/set().
                                 * The only interest of this kind of property is that you can still use etk_object_notify()
                                 * when the value of the property has been modified. You then have to use the API of
                                 * the object to get the new value */
} Etk_Property_Type;

/** @brief Describes if the property is either readable, writable or both.
 * You can also specify if the property needs to be initialized to its default value
 * at the constuction of a new object with ETK_PROPERTY_CONSTRUCT.
 */
typedef enum
{
   ETK_PROPERTY_NO_ACCESS = 1 << 0,     /**< Used when the property type is ETK_PROPERTY_OTHER */
   ETK_PROPERTY_READABLE = 1 << 1,      /**< The value of the property is readable */
   ETK_PROPERTY_WRITABLE = 1 << 2,      /**< The value of the property is writable */
   ETK_PROPERTY_READABLE_WRITABLE =     /**< The value of the property is readable and writable */
      ETK_PROPERTY_READABLE | ETK_PROPERTY_WRITABLE,
   ETK_PROPERTY_CONSTRUCT = 1 << 3      /**< Use this flag to force the default value of the property to be set when
                                         * the object is created */
} Etk_Property_Flags;

/**
 * @brief An Etk_Property is a name/value couple that can be associated to a type.
 * A property has a default value and can be readable, writable or both
 * @structinfo
 */
struct Etk_Property
{
   /* private: */
   char *name;
   int id;
   Etk_Property_Type type;
   Etk_Property_Flags flags;
   Etk_Property_Value *default_value;
};

/**
 * @brief The value of a property. You usually do not need to manipulate it directly, use etk_object_properties_set()
 * and etk_object_properties_get() to set and get the value of a property of an object.
 * @structinfo
 */

struct Etk_Property_Value
{
   /* private: */
   union
   {
      int int_value;
      Etk_Bool bool_value;
      char char_value;
      float float_value;
      double double_value;
      short short_value;
      long long_value;
      Etk_Object *object_value;
      void *pointer_value;
      char *string_value;
   } value;
   Etk_Property_Type type;
};

Etk_Property       *etk_property_new(const char *name, int property_id, Etk_Property_Type type, Etk_Property_Flags flags, Etk_Property_Value *default_value);
void                etk_property_delete(Etk_Property *property);
Etk_Bool            etk_property_default_value_set(Etk_Property *property, Etk_Property_Value *default_value);
Etk_Property_Type   etk_property_type_get(Etk_Property *property);

Etk_Property_Value *etk_property_value_new(void);
void                etk_property_value_delete(Etk_Property_Value *value);
Etk_Property_Type   etk_property_value_type_get(Etk_Property_Value *value);

Etk_Property_Value *etk_property_value_create(Etk_Property_Type type, ...);
Etk_Property_Value *etk_property_value_create_valist(Etk_Property_Type type, va_list *arg);
Etk_Property_Value *etk_property_value_int(int value);
Etk_Property_Value *etk_property_value_bool(Etk_Bool value);
Etk_Property_Value *etk_property_value_char(char value);
Etk_Property_Value *etk_property_value_float(float value);
Etk_Property_Value *etk_property_value_double(double value);
Etk_Property_Value *etk_property_value_short(short value);
Etk_Property_Value *etk_property_value_long(long value);
Etk_Property_Value *etk_property_value_object(Etk_Object *value);
Etk_Property_Value *etk_property_value_pointer(void *value);
Etk_Property_Value *etk_property_value_string(const char *value);

void                etk_property_value_set(Etk_Property_Value *property_value, Etk_Property_Type type, ...);
void                etk_property_value_set_valist(Etk_Property_Value *property_value, Etk_Property_Type type, va_list *arg);
void                etk_property_value_int_set(Etk_Property_Value *property_value, int value);
void                etk_property_value_bool_set(Etk_Property_Value *property_value, Etk_Bool value);
void                etk_property_value_char_set(Etk_Property_Value *property_value, char value);
void                etk_property_value_float_set(Etk_Property_Value *property_value, float value);
void                etk_property_value_double_set(Etk_Property_Value *property_value, double value);
void                etk_property_value_short_set(Etk_Property_Value *property_value, short value);
void                etk_property_value_long_set(Etk_Property_Value *property_value, long value);
void                etk_property_value_object_set(Etk_Property_Value *property_value, Etk_Object *value);
void                etk_property_value_pointer_set(Etk_Property_Value *property_value, void *value);
void                etk_property_value_string_set(Etk_Property_Value *property_value, const char *value);

void                etk_property_value_get(Etk_Property_Value *value, Etk_Property_Type type, void *value_location);
int                 etk_property_value_int_get(Etk_Property_Value *value);
Etk_Bool            etk_property_value_bool_get(Etk_Property_Value *value);
char                etk_property_value_char_get(Etk_Property_Value *value);
float               etk_property_value_float_get(Etk_Property_Value *value);
double              etk_property_value_double_get(Etk_Property_Value *value);
short               etk_property_value_short_get(Etk_Property_Value *value);
long                etk_property_value_long_get(Etk_Property_Value *value);
Etk_Object         *etk_property_value_object_get(Etk_Property_Value *value);
void               *etk_property_value_pointer_get(Etk_Property_Value *value);
const char         *etk_property_value_string_get(Etk_Property_Value *value);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
