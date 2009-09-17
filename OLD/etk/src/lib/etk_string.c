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

/** @file etk_string.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_string.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "etk_utils.h"

/**
 * @addtogroup Etk_String Etk_String
 * @{
 */

#define BLOCK_SIZE 128
#define SIZE_TO_ALLOC(length) \
      (((length) + (BLOCK_SIZE - 1)) / BLOCK_SIZE) * BLOCK_SIZE

enum _Etk_String_Property_Id
{
   ETK_STRING_STRING_PROPERTY
};

static void  _etk_string_constructor(Etk_String *string);
static void  _etk_string_destructor(Etk_String *string);
static void  _etk_string_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void  _etk_string_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static char *_etk_string_vprintf(const char *format, va_list args);
static int   _etk_string_strlen_max(const char *string, int max_len);

static char *_empty_string = "";

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_String
 * @return Returns the type of an Etk_String
 */
Etk_Type *etk_string_type_get(void)
{
   static Etk_Type *string_type = NULL;

   if (!string_type)
   {
      string_type = etk_type_new("Etk_String", ETK_OBJECT_TYPE, sizeof(Etk_String),
            ETK_CONSTRUCTOR(_etk_string_constructor), ETK_DESTRUCTOR(_etk_string_destructor), NULL);
      etk_type_property_add(string_type, "string", ETK_STRING_STRING_PROPERTY,
            ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      
      string_type->property_set = _etk_string_property_set;
      string_type->property_get = _etk_string_property_get;
   }

   return string_type;
}

/**
 * @brief Creates a new string
 * @param value the default value of the string. It can be NULL
 * @return Returns the new string
 */
Etk_String *etk_string_new(const char *value)
{
   Etk_String *string;
   
   string = ETK_STRING(etk_object_new(ETK_STRING_TYPE, NULL));
   return etk_string_set(string, value);
}

/**
 * @brief Creates a new string, with a specific size.
 * @param value the default value of the string. It can be NULL
 * @param size If @a size is lower than the length of @a value, the value will be truncated.
 * Otherwise, if @a size is greater than the length of @a value, extra memory will be allocated.
 * It may be useful if you plan to often insert text and want to avoid too many reallocations.
 * @return Returns the new string
 */
Etk_String *etk_string_new_sized(const char *value, int size)
{
   Etk_String *string;
   
   string = ETK_STRING(etk_object_new(ETK_STRING_TYPE, NULL));
   return etk_string_set_sized(string, value, size);
}

/**
 * @brief Creates a new string, and sets its default value from the given format and arguments
 * @param format the format to set to the string. It uses the same arguments as printf()
 * @param ... the arguments corresponding to the format
 * @return Returns the new string
 */
Etk_String *etk_string_new_printf(const char *format, ...)
{
   Etk_String *string;
   va_list args;

   va_start(args, format);
   string = ETK_STRING(etk_object_new(ETK_STRING_TYPE, NULL));
   string = etk_string_set_vprintf(string, format, args);
   va_end(args);

   return string;
}

/**
 * @brief Creates a new string, and sets its default value from the given format and arguments
 * @param format the format to set to the string. It uses the same arguments as printf()
 * @param args the arguments corresponding to the format
 * @return Returns the new string
 */
Etk_String *etk_string_new_vprintf(const char *format, va_list args)
{
   Etk_String *string;
   va_list args2;

   va_copy(args2, args);
   string = ETK_STRING(etk_object_new(ETK_STRING_TYPE, NULL));
   string = etk_string_set_vprintf(string, format, args2);
   va_end(args2);

   return string;
}

/**
 * @brief Creates a new string and copies the text from @a string
 * @param string the string to copy
 * @return Returns the new copied string
 */
Etk_String *etk_string_copy(const Etk_String *string)
{
   if (!string)
      return etk_string_new(NULL);
   else
      return etk_string_new_sized(string->string, string->allocated_length);
}

/**
 * @brief Gets the string as an array of chars
 * @param string a string
 * @param Returns the string as an array of chars (i.e a pointer on the first character)
 */
const char *etk_string_get(Etk_String *string)
{
   if (!string || !string->string)
      return _empty_string;
   return string->string;
}

/**
 * @brief Gets the length of the string
 * @param string a string
 * @return Returns the length of the string
 */
int etk_string_length_get(Etk_String *string)
{
   if (!string)
      return 0;
   return string->length;
}

/**
 * @brief Truncates the string
 * @param string a string
 * @param length the new length of the string. If @a length is greater than
 * the current length of the string, the function does nothing
 * @return Returns the truncated string
 */
Etk_String *etk_string_truncate(Etk_String *string, int length)
{
   if (!string || length < 0)
      return NULL;

   if (length < string->length)
   {
      string->string[length] = '\0';
      string->length = length;
      etk_object_notify(ETK_OBJECT(string), "string");
   }
   
   return string;
}

/**
 * @brief Removes from the string a segment of @a size chars from the position @a pos
 * @param string a string
 * @param pos the position where to start the deletion (starting from 0)
 * @param size the number of chars to remove
 * @return Returns the string
 */
Etk_String *etk_string_delete(Etk_String *string, int pos, int size)
{
   if (!string)
      return NULL;
   if (pos < 0 || pos >= string->length || size <= 0)
      return string;

   if (pos + size >= string->length)
      return etk_string_truncate(string, pos);
   else
   {
      memmove(&string->string[pos], &string->string[pos + size], string->length - pos - size);
      return etk_string_truncate(string, string->length - size);
   }
}

/**
 * @brief Clears the string: all the allocated memory will be freed,
 * and the length of the string will be set to 0
 * @param string the string to clear
 * @return Returns the cleared string
 */
Etk_String *etk_string_clear(Etk_String *string)
{
   if (!string)
      return NULL;

   free(string->string);
   string->string = NULL;
   string->length = 0;
   string->allocated_length = 0;

   return string;
}

/**
 * @brief Sets the value of a string
 * @param string a string. If @a string is NULL, a new string is created
 * @param value the value to assign to the string
 * @return Returns the string
 */
Etk_String *etk_string_set(Etk_String *string, const char *value)
{
   return etk_string_set_sized(string, value, value ? strlen(value) : 0);
}

/**
 * @brief Sets the value of a string, with a specific size.
 * @param string a string. If @a string is NULL, a new string is created
 * @param value the value to assign to the string
 * @param size If @a size is lower than the length of @a value, the value will be truncated.
 * Otherwise, if @a size is greater than the length of @a value, extra memory will be allocated.
 * It may be useful if you plan to often insert text and want to avoid too many reallocations.
 * @return Returns the string
 */
Etk_String *etk_string_set_sized(Etk_String *string, const char *value, int size)
{
   if (!string)
      return etk_string_new_sized(value, size);

   if (!value || value[0] == '\0' || size <= 0)
      return etk_string_clear(string);
   else
   {
      if (size > string->allocated_length)
      {
         free(string->string);
         string->string = malloc(SIZE_TO_ALLOC(size) + 1);
         string->allocated_length = SIZE_TO_ALLOC(size);
      }

      string->length = _etk_string_strlen_max(value, size);
      strncpy(string->string, value, string->length);
      string->string[string->length] = '\0';
   }

   etk_object_notify(ETK_OBJECT(string), "string");
   return string;
}

/**
 * @brief Sets the value of the string from the given format and arguments
 * @param string a string. If @a string is NULL, a new string is created
 * @param format the format to set to the string. It uses the same arguments as printf()
 * @param ... the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_set_printf(Etk_String *string, const char *format, ...)
{
   va_list args;

   va_start(args, format);
   string = etk_string_set_vprintf(string, format, args);
   va_end(args);

   return string;
}

/**
 * @brief Sets the value of the string from the format and the arguments
 * @param string a string. If @a string is NULL, a new string is created
 * @param format the format to set to the string. It uses the same arguments than printf()
 * @param args the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_set_vprintf(Etk_String *string, const char *format, va_list args)
{
   va_list args2;
   char *text;

   va_copy(args2, args);
   text = _etk_string_vprintf(format, args2);
   string = etk_string_set(string, text);
   free(text);
   va_end(args2);

   return string;
}

/**
 * @brief Prepends a text to a string
 * @param string a string. If @a string is NULL, a new string is created
 * @param text the text to prepend to the string
 * @return Returns the string
 */
Etk_String *etk_string_prepend(Etk_String *string, const char *text)
{
   return etk_string_insert(string, 0, text);
}

/**
 * @brief Prepends a text with a specific length to a string
 * @param string a string. If @a string is NULL, a new string is created
 * @param text the text to prepend to the string
 * @param length the length of the text to prepend
 * @return Returns the string
 */
Etk_String *etk_string_prepend_sized(Etk_String *string, const char *text, int length)
{
   return etk_string_insert_sized(string, 0, text, length);
}

/**
 * @brief Prepends a character to a string
 * @param string a string. If @a string is NULL, a new string is created
 * @param c the character to prepend to the string
 * @return Returns the string
 */
Etk_String *etk_string_prepend_char(Etk_String *string, char c)
{
   return etk_string_insert_char(string, 0, c);
}

/**
 * @brief Prepends a text to the string, from the given format and arguments
 * @param string a string. If @a string is NULL, a new string is created
 * @param format the format of the text to prepend to the string.
 * It uses the same arguments than printf()
 * @param ... the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_prepend_printf(Etk_String *string, const char *format, ...)
{
   va_list args;

   va_start(args, format);
   string = etk_string_prepend_vprintf(string, format, args);
   va_end(args);

   return string;
}

/**
 * @brief Prepends a text to the string, from the given format and arguments
 * @param string a string. If @a string is NULL, a new string is created
 * @param format the format of the text to prepend to the string.
 * It uses the same arguments than printf()
 * @param args the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_prepend_vprintf(Etk_String *string, const char *format, va_list args)
{
   va_list args2;

   va_copy(args2, args);
   string = etk_string_insert_vprintf(string, 0, format, args2);
   va_end(args2);

   return string;
}

/**
 * @brief Appends a text to a string
 * @param string a string. If @a string is NULL, a new string is created
 * @param text the text to append to the string
 * @return Returns the string
 */
Etk_String *etk_string_append(Etk_String *string, const char *text)
{
   return etk_string_insert(string, string->length, text);
}

/**
 * @brief Appends a text with a specific length to a string
 * @param string a string. If @a string is NULL, a new string is created
 * @param text the text to append to the string
 * @param length the length of the text to append
 * @return Returns the string
 */
Etk_String *etk_string_append_sized(Etk_String *string, const char *text, int length)
{
   return etk_string_insert_sized(string, string->length, text, length);
}

/**
 * @brief Appends a character to a string
 * @param string a string. If @a string is NULL, a new string is created
 * @param c the character to append to the string
 * @return Returns the string
 */
Etk_String *etk_string_append_char(Etk_String *string, char c)
{
   return etk_string_insert_char(string, string->length, c);
}

/**
 * @brief Appends a text to the string, from the given format and arguments
 * @param string a string. If @a string is NULL, a new string is created
 * @param format the format of the text to append to the string.
 * It uses the same arguments than printf()
 * @param ... the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_append_printf(Etk_String *string, const char *format, ...)
{
   va_list args;

   va_start(args, format);
   string = etk_string_append_vprintf(string, format, args);
   va_end(args);

   return string;
}

/**
 * @brief Appends a text to the string, from the given format and arguments
 * @param string a string. If @a string is NULL, a new string is created
 * @param format the format of the text to append to the string.
 * It uses the same arguments than printf()
 * @param args the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_append_vprintf(Etk_String *string, const char *format, va_list args)
{
   va_list args2;

   va_copy(args2, args);
   string = etk_string_insert_vprintf(string, string->length, format, args2);
   va_end(args2);

   return string;
}

/**
 * @brief Inserts a text into a string, at a given position
 * @param string a string. If @a string is NULL, a new string is created
 * @param pos the position where to insert the text (starting from 0)
 * @param text the text to insert into the string
 * @return Returns the string
 */
Etk_String *etk_string_insert(Etk_String *string, int pos, const char *text)
{
   return etk_string_insert_sized(string, pos, text, text ? strlen(text) : 0);
}

/**
 * @brief Inserts a text with a specific length into a string, at a given position
 * @param string a string. If @a string is NULL, a new string is created
 * @param pos the position where to insert the text (starting from 0)
 * @param text the text to insert into the string
 * @param length the maximum length of the text to insert
 * @return Returns the string
 */
Etk_String *etk_string_insert_sized(Etk_String *string, int pos, const char *text, int length)
{
   if (!string)
      return etk_string_new_sized(text, length);
   if (!text || text[0] == '\0' || length <= 0)
      return string;

   pos = ETK_CLAMP(pos, 0, string->length);
   length = _etk_string_strlen_max(text, length);
   if (string->length + length > string->allocated_length)
   {
      string->string = realloc(string->string, SIZE_TO_ALLOC(string->length + length) + 1);
      string->allocated_length = SIZE_TO_ALLOC(string->length + length);
   }

   memmove(&string->string[pos + length], &string->string[pos], string->length - pos);
   strncpy(&string->string[pos], text, length);
   string->length += length;
   string->string[string->length] = '\0';

   etk_object_notify(ETK_OBJECT(string), "string");
   return string;
}

/**
 * @brief Inserts a character into a string, at a given position
 * @param string a string. If @a string is NULL, a new string is created
 * @param pos the position where to insert the char
 * @param c the character to insert into the string
 * @return Returns the string
 */
Etk_String *etk_string_insert_char(Etk_String *string, int pos, char c)
{
   int i;

   if (!string)
      return etk_string_insert_char(etk_string_new(NULL), pos, c);
   if (c == '\0')
      return etk_string_truncate(string, pos);

   pos = ETK_CLAMP(pos, 0, string->length);
   if (string->length + 1 > string->allocated_length)
   {
      string->string = realloc(string->string, SIZE_TO_ALLOC(string->length + 1) + 1);
      string->allocated_length = SIZE_TO_ALLOC(string->length + 1);
   }
   for (i = string->length - 1; i >= pos; i--)
      string->string[i + 1] = string->string[i];

   string->string[pos] = c;
   string->length++;
   string->string[string->length] = '\0';

   etk_object_notify(ETK_OBJECT(string), "string");
   return string;
}

/**
 * @brief Inserts a text into a string, from the given format and arguments, at a given position
 * @param string a string. If @a string is NULL, a new string is created
 * @param pos the position where to insert the text
 * @param format the format to insert into the string. It uses the same arguments than printf()
 * @param ... the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_insert_printf(Etk_String *string, int pos, const char *format, ...)
{
   va_list args;

   va_start(args, format);
   string = etk_string_insert_vprintf(string, pos, format, args);
   va_end(args);

   return string;
}

/**
 * @brief Inserts a text into a string, from the given format and arguments, at a given position
 * @param string a string. If @a string is NULL, a new string is created
 * @param pos the position where to insert the text
 * @param format the format to insert into the string. It uses the same arguments than printf()
 * @param args the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_insert_vprintf(Etk_String *string, int pos, const char *format, va_list args)
{
   va_list args2;
   char *text;

   va_copy(args2, args);
   text = _etk_string_vprintf(format, args2);
   string = etk_string_insert(string, pos, text);
   free(text);
   va_end(args2);

   return string;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the string */
static void _etk_string_constructor(Etk_String *string)
{
   if (!string)
      return;

   string->string = NULL;
   string->length = 0;
   string->allocated_length = 0;
}

/* Frees the string */
static void _etk_string_destructor(Etk_String *string)
{
   if (!string)
      return;
   free(string->string);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_string_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_String *string;

   if (!(string = ETK_STRING(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_STRING_STRING_PROPERTY:
         etk_string_set(string, etk_property_value_string_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_string_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_String *string;

   if (!(string = ETK_STRING(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_STRING_STRING_PROPERTY:
         etk_property_value_string_set(value, string->string);
         break;
      default:
         break;
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Creates a new string (char *) from the given format and args, and returns it.
 * The returned string will have to be freed */
static char *_etk_string_vprintf(const char *format, va_list args)
{
   char c;
   char *text;
   va_list args2;
   int length;

   if (!format)
      return NULL;

   va_copy(args2, args);
   length = vsnprintf(&c, 1, format, args2);
   va_end(args2);
   text = malloc(length + 1);
   va_copy(args2, args);
   vsprintf(text, format, args2);
   va_end(args2);

   return text;
}

/* Calculates the length of the given string.
 * The returned length can not be greater than "max_len" */
static int _etk_string_strlen_max(const char *string, int max_len)
{
   int i;
   
   if (!string)
      return 0;
   
   for (i = 0; i < max_len && string[i] != '\0'; i++);
   return i;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_String
 *
 * TODOC
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_String
 *
 * \par Properties:
 * @prop_name "string": the value of the string
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 */
