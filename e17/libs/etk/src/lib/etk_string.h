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

/** @file etk_string.h */
#ifndef _ETK_STRING_H_
#define _ETK_STRING_H_

#include <stdarg.h>

#include "etk_object.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: Etk_Object is a little too heavy for a string...
   maybe we could use Etk_Base instead? */

/**
 * @defgroup Etk_String Etk_String
 * @brief An object that offers convenient methods to manipulate a string
 * @{
 */

/** Gets the type of a string */
#define ETK_STRING_TYPE            (etk_string_type_get())
/** Casts the object to an Etk_String */
#define ETK_STRING(obj)            (ETK_OBJECT_CAST((obj), ETK_STRING_TYPE, Etk_String))
/** Checks if the object is an Etk_String */
#define ETK_IS_STRING(obj)         (ETK_OBJECT_CHECK_TYPE((obj), ETK_STRING_TYPE))

/**
 * @brief @widget Etk_String is an object that offers convenient methods to manipulate a string
 * @structinfo
 */
struct Etk_String
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;

   char *string;
   int length;
   int allocated_length;
};

Etk_Type   *etk_string_type_get(void);

Etk_String *etk_string_new(const char *value);
Etk_String *etk_string_new_sized(const char *value, int size);
Etk_String *etk_string_new_printf(const char *format, ...);
Etk_String *etk_string_new_vprintf(const char *format, va_list args);
Etk_String *etk_string_copy(const Etk_String *string);

const char *etk_string_get(Etk_String *string);
int         etk_string_length_get(Etk_String *string);
Etk_String *etk_string_truncate(Etk_String *string, int length);
Etk_String *etk_string_delete(Etk_String *string, int pos, int size);
Etk_String *etk_string_clear(Etk_String *string);

Etk_String *etk_string_set(Etk_String *string, const char *value);
Etk_String *etk_string_set_sized(Etk_String *string, const char *value, int length);
Etk_String *etk_string_set_printf(Etk_String *string, const char *format, ...);
Etk_String *etk_string_set_vprintf(Etk_String *string, const char *format, va_list args);

Etk_String *etk_string_prepend(Etk_String *string, const char *text);
Etk_String *etk_string_prepend_sized(Etk_String *string, const char *text, int length);
Etk_String *etk_string_prepend_char(Etk_String *string, char c);
Etk_String *etk_string_prepend_printf(Etk_String *string, const char *format, ...);
Etk_String *etk_string_prepend_vprintf(Etk_String *string, const char *format, va_list args);

Etk_String *etk_string_append(Etk_String *string, const char *text);
Etk_String *etk_string_append_sized(Etk_String *string, const char *text, int length);
Etk_String *etk_string_append_char(Etk_String *string, char c);
Etk_String *etk_string_append_printf(Etk_String *string, const char *format, ...);
Etk_String *etk_string_append_vprintf(Etk_String *string, const char *format, va_list args);

Etk_String *etk_string_insert(Etk_String *string, int pos, const char *text);
Etk_String *etk_string_insert_sized(Etk_String *string, int pos, const char *text, int length);
Etk_String *etk_string_insert_char(Etk_String *string, int pos, char c);
Etk_String *etk_string_insert_printf(Etk_String *string, int pos, const char *format, ...);
Etk_String *etk_string_insert_vprintf(Etk_String *string, int pos, const char *format, va_list args);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
