/** @file etk_string.h */
#ifndef _ETK_STRING_H_
#define _ETK_STRING_H_

#include "etk_object.h"
#include <stdarg.h>
#include "etk_types.h"

/**
 * @defgroup Etk_String Etk_String
 * @{
 */

/** @brief Gets the type of a string */
#define ETK_STRING_TYPE            (etk_string_type_get())
/** @brief Casts the object to an Etk_String */
#define ETK_STRING(obj)            (ETK_OBJECT_CAST((obj), ETK_STRING_TYPE, Etk_String))
/** @brief Checks if the object is an Etk_Text_Buffer */
#define ETK_IS_STRING(obj)         (ETK_OBJECT_CHECK_TYPE((obj), ETK_STRING_TYPE))

/**
 * @struct Etk_String
 * @brief An Etk_String is an easy way to manipulate a string
 */
struct _Etk_String
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;
   
   char *string;
   int length;
   int allocated_length;
};

Etk_Type *etk_string_type_get();

Etk_String *etk_string_new(const char *value);
Etk_String *etk_string_new_sized(const char *value, int size);
Etk_String *etk_string_new_printf(const char *format, ...);
Etk_String *etk_string_new_vprintf(const char *format, va_list args);
Etk_String *etk_string_copy(const Etk_String *string);

const char *etk_string_get(Etk_String *string);
int etk_string_length_get(Etk_String *string);
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

#endif
