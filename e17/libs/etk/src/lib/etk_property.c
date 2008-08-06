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

/** @file etk_property.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_property.h"
#include <stdlib.h>
#include <string.h>
#include "etk_utils.h"

/**
 * @addtogroup Etk_Property
 * @{
 */

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Creates a new property
 * @param name the name of the new property
 * @param property_id the id of the new property
 * @param type the type of the property
 * @param flags describes if the property value should be writable (ETK_PROPERTY_WRITABLE),
 * readable (ETK_PROPERTY_READABLE) or both (ETK_PROPERTY_WRITABLE | ETK_PROPERTY_READABLE)
 * @param default_value the default value of the property
 * @return Returns the new property on success, or NULL on failure
 * @warning The property will have to be freed with @a etk_property_delete()
 */
Etk_Property *etk_property_new(const char *name, int property_id, Etk_Property_Type type, Etk_Property_Flags flags, Etk_Property_Value *default_value)
{
   Etk_Property *new_property;

   if (!name)
      return NULL;

   new_property = malloc(sizeof(Etk_Property));
   new_property->name = strdup(name);
   new_property->id = property_id;
   new_property->type = type;
   new_property->flags = flags;
   new_property->default_value = default_value;

   return new_property;
}

/**
 * @brief Deletes the property
 * @param property the property to delete
 */
void etk_property_delete(Etk_Property *property)
{
   if (!property)
      return;

   etk_property_value_delete(property->default_value);
   free(property->name);
   free(property);
}

/**
 * @brief Gets the type of the property
 * @param property a property
 * @return Returns the type of the property
 */
Etk_Property_Type etk_property_type_get(Etk_Property *property)
{
   if (!property)
      return ETK_PROPERTY_NONE;

   return property->type;
}

/**
 * @brief Clears a property value: the current value is freed and the type of the property is set to ETK_PROPERTY_NONE
 * @param value the property value to clear
 */
void etk_property_value_clear(Etk_Property_Value *value)
{
   if (!value)
      return;

   if (value->type == ETK_PROPERTY_STRING)
      free(value->value.string_value);
   value->type = ETK_PROPERTY_NONE;
}

/**
 * @brief Deletes a property value
 * @param value the property value to delete
 */
void etk_property_value_delete(Etk_Property_Value *value)
{
   etk_property_value_clear(value);
   free(value);
}

/**
 * @brief Gets the type of the value
 * @param value a property value
 * @return Returns the type of the value
 */
Etk_Property_Type etk_property_value_type_get(Etk_Property_Value *value)
{
   if (!value)
      return ETK_PROPERTY_NONE;
   return value->type;
}

/**
 * @brief Creates a new property value with no type
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_new(void)
{
   Etk_Property_Value *new_value;
   new_value = malloc(sizeof(Etk_Property_Value));
   new_value->type = ETK_PROPERTY_NONE;
   return new_value;
}

/**
 * @brief Creates a new property value of type @a type
 * @param type the type of the new Etk_Property_Value
 * @param ... the value of the new Etk_Property_Value
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_create(Etk_Property_Type type, ...)
{
   Etk_Property_Value *new_value;
   va_list arg;

   va_start(arg, type);
   new_value = etk_property_value_create_valist(type, &arg);
   va_end(arg);

   return new_value;
}

/**
 * @brief Creates a new property value of type @a type
 * @param type the type of the new property value
 * @param arg the value of the new property value
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_create_valist(Etk_Property_Type type, va_list *arg)
{
   Etk_Property_Value *new_value;
   new_value = etk_property_value_new();
   etk_property_value_set_valist(new_value, type, arg);
   return new_value;
}



/**
 * @brief Creates a new integer property value
 * @param value the value of the new property value
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_int(int value)
{
   Etk_Property_Value *new_value;
   new_value = etk_property_value_new();
   etk_property_value_int_set(new_value, value);
   return new_value;
}

/**
 * @brief Creates a new boolean property value
 * @param value the value of the new property value
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_bool(Etk_Bool value)
{
   Etk_Property_Value *new_value;
   new_value = etk_property_value_new();
   etk_property_value_bool_set(new_value, value);
   return new_value;
}

/**
 * @brief Creates a new char property value
 * @param value the value of the new property value
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_char(char value)
{
   Etk_Property_Value *new_value;
   new_value = etk_property_value_new();
   etk_property_value_char_set(new_value, value);
   return new_value;
}

/**
 * @brief Creates a new float property value
 * @param value the value of the new property value
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_float(float value)
{
   Etk_Property_Value *new_value;
   new_value = etk_property_value_new();
   etk_property_value_float_set(new_value, value);
   return new_value;
}

/**
 * @brief Creates a new double property value
 * @param value the value of the new property value
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_double(double value)
{
   Etk_Property_Value *new_value;
   new_value = etk_property_value_new();
   etk_property_value_double_set(new_value, value);
   return new_value;
}

/**
 * @brief Creates a new short property value
 * @param value the value of the new property value
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_short(short value)
{
   Etk_Property_Value *new_value;
   new_value = etk_property_value_new();
   etk_property_value_short_set(new_value, value);
   return new_value;
}

/**
 * @brief Creates a new long property value
 * @param value the value of the new property value
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_long(long value)
{
   Etk_Property_Value *new_value;
   new_value = etk_property_value_new();
   etk_property_value_long_set(new_value, value);
   return new_value;
}

/**
 * @brief Creates a new object property value
 * @param value the value of the new property value
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_object(Etk_Object *value)
{
   Etk_Property_Value *new_value;
   new_value = etk_property_value_new();
   etk_property_value_object_set(new_value, value);
   return new_value;
}

/**
 * @brief Creates a new pointer property value
 * @param value the value of the new property value
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_pointer(void *value)
{
   Etk_Property_Value *new_value;
   new_value = etk_property_value_new();
   etk_property_value_pointer_set(new_value, value);
   return new_value;
}

/**
 * @brief Creates a new string property value
 * @param value the value of the new property value
 * @return Returns the new property value
 */
Etk_Property_Value *etk_property_value_string(const char *value)
{
   Etk_Property_Value *new_value;
   new_value = etk_property_value_new();
   etk_property_value_string_set(new_value, value);
   return new_value;
}

/**
 * @brief Sets the value of a property value
 * @param property_value a property value
 * @param type the type of the value
 * @param ... the value to set (must be compatible with @a type)
 */
void etk_property_value_set(Etk_Property_Value *property_value, Etk_Property_Type type, ...)
{
   va_list arg;

   if (!property_value)
      return;

   va_start(arg, type);
   etk_property_value_set_valist(property_value, type, &arg);
   va_end(arg);
}

/**
 * @brief Sets the value of a property value
 * @param property_value a property value
 * @param type the type of the value
 * @param arg the value to set (must be compatible with @a type)
 */
void etk_property_value_set_valist(Etk_Property_Value *property_value, Etk_Property_Type type, va_list *arg)
{
   if (!property_value || !arg)
      return;

   switch (type)
   {
      case ETK_PROPERTY_INT:
      {
         int value;
         value = va_arg(*arg, int);
         etk_property_value_int_set(property_value, value);
         break;
      }
      case ETK_PROPERTY_BOOL:
      {
         Etk_Bool value;
         value = va_arg(*arg, int);
         etk_property_value_bool_set(property_value, value);
         break;
      }
      case ETK_PROPERTY_CHAR:
      {
         char value;
         value = va_arg(*arg, int);
         etk_property_value_char_set(property_value, value);
         break;
      }
      case ETK_PROPERTY_FLOAT:
      {
         float value;
         value = va_arg(*arg, double);
         etk_property_value_float_set(property_value, value);
         break;
      }
      case ETK_PROPERTY_DOUBLE:
      {
         double value;
         value = va_arg(*arg, double);
         etk_property_value_double_set(property_value, value);
         break;
      }
      case ETK_PROPERTY_SHORT:
      {
         short value;
         value = va_arg(*arg, int);
         etk_property_value_short_set(property_value, value);
         break;
      }
      case ETK_PROPERTY_LONG:
      {
         long value;
         value = va_arg(*arg, long);
         etk_property_value_long_set(property_value, value);
         break;
      }
      case ETK_PROPERTY_OBJECT:
      {
         Etk_Object *value;
         value = va_arg(*arg, Etk_Object *);
         etk_property_value_object_set(property_value, value);
         break;
      }
      case ETK_PROPERTY_POINTER:
      {
         void *value;
         value = va_arg(*arg, void *);
         etk_property_value_pointer_set(property_value, value);
         break;
      }
      case ETK_PROPERTY_STRING:
      {
         char *value;
         value = va_arg(*arg, char *);
         etk_property_value_string_set(property_value, value);
         break;
      }
      default:
         break;
   }
}

/**
 * @brief Sets the integer value of a property value
 * @param property_value a property value
 * @param value the value to set
 */
void etk_property_value_int_set(Etk_Property_Value *property_value, int value)
{
   if (!property_value)
      return;

   etk_property_value_clear(property_value);
   property_value->value.int_value = value;
   property_value->type = ETK_PROPERTY_INT;
}

/**
 * @brief Sets the boolean value of a property value
 * @param property_value a property value
 * @param value the value to set
 */
void etk_property_value_bool_set(Etk_Property_Value *property_value, Etk_Bool value)
{
   if (!property_value)
      return;

   etk_property_value_clear(property_value);
   property_value->value.bool_value = value;
   property_value->type = ETK_PROPERTY_BOOL;
}

/**
 * @brief Sets the char value of a property value
 * @param property_value a property value
 * @param value the value to set
 */
void etk_property_value_char_set(Etk_Property_Value *property_value, char value)
{
   if (!property_value)
      return;

   etk_property_value_clear(property_value);
   property_value->value.char_value = value;
   property_value->type = ETK_PROPERTY_CHAR;
}

/**
 * @brief Sets the float value of a property value
 * @param property_value a property value
 * @param value the value to set
 */
void etk_property_value_float_set(Etk_Property_Value *property_value, float value)
{
   if (!property_value)
      return;

   etk_property_value_clear(property_value);
   property_value->value.float_value = value;
   property_value->type = ETK_PROPERTY_FLOAT;
}

/**
 * @brief Sets the double value of a property value
 * @param property_value a property value
 * @param value the value to set
 */
void etk_property_value_double_set(Etk_Property_Value *property_value, double value)
{
   if (!property_value)
      return;

   etk_property_value_clear(property_value);
   property_value->value.double_value = value;
   property_value->type = ETK_PROPERTY_DOUBLE;
}

/**
 * @brief Sets the short value of a property value
 * @param property_value a property value
 * @param value the value to set
 */
void etk_property_value_short_set(Etk_Property_Value *property_value, short value)
{
   if (!property_value)
      return;

   etk_property_value_clear(property_value);
   property_value->value.short_value = value;
   property_value->type = ETK_PROPERTY_SHORT;
}

/**
 * @brief Sets the long value of a property value
 * @param property_value a property value
 * @param value the value to set
 */
void etk_property_value_long_set(Etk_Property_Value *property_value, long value)
{
   if (!property_value)
      return;

   etk_property_value_clear(property_value);
   property_value->value.long_value = value;
   property_value->type = ETK_PROPERTY_LONG;
}

/**
 * @brief Sets the object value of a property value
 * @param property_value a property value
 * @param value the value to set
 */
void etk_property_value_object_set(Etk_Property_Value *property_value, Etk_Object *value)
{
   if (!property_value)
      return;

   etk_property_value_clear(property_value);
   property_value->value.object_value = value;
   property_value->type = ETK_PROPERTY_OBJECT;
}

/**
 * @brief Sets the pointer value of a property value
 * @param property_value a property value
 * @param value the value to set
 */
void etk_property_value_pointer_set(Etk_Property_Value *property_value, void *value)
{
   if (!property_value)
      return;

   etk_property_value_clear(property_value);
   property_value->value.pointer_value = value;
   property_value->type = ETK_PROPERTY_POINTER;
}

/**
 * @brief Sets the string value of a property value
 * @param property_value a property value
 * @param value the value to set
 */
void etk_property_value_string_set(Etk_Property_Value *property_value, const char *value)
{
   if (!property_value)
      return;

   etk_property_value_clear(property_value);
   if (value)
      property_value->value.string_value = strdup(value);
   else
      property_value->value.string_value = NULL;
   property_value->type = ETK_PROPERTY_STRING;
}

/**
 * @brief Gets the value of the property value
 * @param value a value
 * @param type the type of the value
 * @param value_location the location where to store the value
 */
void etk_property_value_get(Etk_Property_Value *value, Etk_Property_Type type, void *value_location)
{
   if (!value || !value_location)
      return;

   switch (type)
   {
      case ETK_PROPERTY_INT:
         *((int *)value_location) = etk_property_value_int_get(value);
         break;
      case ETK_PROPERTY_BOOL:
         *((Etk_Bool *)value_location) = etk_property_value_bool_get(value);
         break;
      case ETK_PROPERTY_CHAR:
         *((char *)value_location) = etk_property_value_char_get(value);
         break;
      case ETK_PROPERTY_FLOAT:
         *((float *)value_location) = etk_property_value_float_get(value);
         break;
      case ETK_PROPERTY_DOUBLE:
         *((double *)value_location) = etk_property_value_double_get(value);
         break;
      case ETK_PROPERTY_SHORT:
         *((short *)value_location) = etk_property_value_short_get(value);
         break;
      case ETK_PROPERTY_LONG:
         *((long *)value_location) = etk_property_value_long_get(value);
         break;
      case ETK_PROPERTY_OBJECT:
         *((Etk_Object **)value_location) = etk_property_value_object_get(value);
         break;
      case ETK_PROPERTY_POINTER:
         *((void **)value_location) = etk_property_value_pointer_get(value);
         break;
      case ETK_PROPERTY_STRING:
         *((const char **)value_location) = etk_property_value_string_get(value);
         break;
      default:
         break;
   }
}

/**
 * @brief Gets the integer value of the property value
 * @param value a value
 * @return Returns the integer value of the property value
 */
int etk_property_value_int_get(Etk_Property_Value *value)
{
   if (!value || value->type != ETK_PROPERTY_INT)
      return 0;
   return value->value.int_value;
}

/**
 * @brief Gets the boolean value of the property value
 * @param value a value
 * @return Returns the boolean value of the property value
 */
Etk_Bool etk_property_value_bool_get(Etk_Property_Value *value)
{
   if (!value || value->type != ETK_PROPERTY_BOOL)
      return ETK_FALSE;
   return value->value.bool_value;
}

/**
 * @brief Gets the char value of the property value
 * @param value a value
 * @return Returns the char value of the property value
 */
char etk_property_value_char_get(Etk_Property_Value *value)
{
   if (!value || value->type != ETK_PROPERTY_CHAR)
      return 0;
   return value->value.char_value;
}

/**
 * @brief Gets the float value of the property value
 * @param value a value
 * @return Returns the float value of the property value
 */
float etk_property_value_float_get(Etk_Property_Value *value)
{
   if (!value || value->type != ETK_PROPERTY_FLOAT)
      return 0.0;
   return value->value.float_value;
}

/**
 * @brief Gets the double value of the property value
 * @param value a value
 * @return Returns the double value of the property value
 */
double etk_property_value_double_get(Etk_Property_Value *value)
{
   if (!value || value->type != ETK_PROPERTY_DOUBLE)
      return 0.0;
   return value->value.double_value;
}

/**
 * @brief Gets the short value of the property value
 * @param value a value
 * @return Returns the short value of the property value
 */
short etk_property_value_short_get(Etk_Property_Value *value)
{
   if (!value || value->type != ETK_PROPERTY_SHORT)
      return 0;
   return value->value.short_value;
}

/**
 * @brief Gets the long value of the property value
 * @param value a value
 * @return Returns the long value of the property value
 */
long etk_property_value_long_get(Etk_Property_Value *value)
{
   if (!value || value->type != ETK_PROPERTY_LONG)
      return 0;
   return value->value.long_value;
}

/**
 * @brief Gets the object value of the property value
 * @param value a value
 * @return Returns the object value of the property value
 */
Etk_Object *etk_property_value_object_get(Etk_Property_Value *value)
{
   if (!value || value->type != ETK_PROPERTY_OBJECT)
      return NULL;
   return value->value.object_value;
}

/**
 * @brief Gets the pointer value of the property value
 * @param value a value
 * @return Returns the pointer value of the property value
 */
void *etk_property_value_pointer_get(Etk_Property_Value *value)
{
   if (!value || value->type != ETK_PROPERTY_POINTER)
      return NULL;
   return value->value.pointer_value;
}

/**
 * @brief Gets the string value of the property value
 * @param value a value
 * @return Returns the string value of the property value
 */
const char *etk_property_value_string_get(Etk_Property_Value *value)
{
   if (!value || value->type != ETK_PROPERTY_STRING)
      return NULL;
   return value->value.string_value;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Property
 *
 * Most of the following functions are used internally by Etk, you usually do not have to directly call any of the
 * etk_property_*() functions. @n
 * If you want to create a property for a new object type, use etk_type_property_add(). @n
 * The only case where you have to use the etk_property_value_*_set/get() functions is in the property_set/get()
 * methods when you are implementing a new widget, to set or to get the value of a property.  @n
 *
 * See Etk_Object for more information about how to use the property system.
 */
