#include "etk_string.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "etk_utils.h"

/**
 * @addtogroup Etk_String Etk_String
 * @{
 */
 
#define ETK_STRING_BLOCK_SIZE 128
#define ETK_STRING_SIZE_TO_ALLOC(length)        (((length) + (ETK_STRING_BLOCK_SIZE - 1)) / ETK_STRING_BLOCK_SIZE) * ETK_STRING_BLOCK_SIZE

enum _Etk_String_Property_Id
{
   ETK_STRING_STRING_PROPERTY
};
   
static void _etk_string_constructor(Etk_String *string);
static void _etk_string_destructor(Etk_String *string);
static char *_etk_string_vprintf(const char *format, va_list args);

/**
 * @brief Gets the type of an Etk_String
 * @return Returns the type on an Etk_String
 */
Etk_Type *etk_string_type_get()
{
   static Etk_Type *string_type = NULL;

   if (!string_type)
   {
      string_type = etk_type_new("Etk_String", ETK_OBJECT_TYPE, sizeof(Etk_String), ETK_CONSTRUCTOR(_etk_string_constructor), ETK_DESTRUCTOR(_etk_string_destructor));
      etk_type_property_add(string_type, "string", ETK_STRING_STRING_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_string(NULL));
   }

   return string_type;
}

/**
 * @brief Creates a new string
 * @param value the default value of the string. Can be NULL
 * @return Returns the new string
 */
Etk_String *etk_string_new(const char *value)
{
   return etk_string_set(ETK_STRING(etk_object_new(ETK_STRING_TYPE, NULL)), value);
}

/**
 * @brief Creates a new string, with a specific size.
 * @param value the default value of the string. Can be NULL
 * @param size If @a size is lower than the length of @a value, the string will be truncated. @n
 * Otherwise, extra memory will be allocated (useful if you planned to insert text often and want to avoid too many reallocations)
 * @return Returns the new string
 */
Etk_String *etk_string_new_sized(const char *value, int size)
{
   return etk_string_set_sized(ETK_STRING(etk_object_new(ETK_STRING_TYPE, NULL)), value, size);
}

/**
 * @brief Creates a new string, and sets its default value from the format and the arguments
 * @param format the format to set to the string. It uses the same arguments than printf()
 * @param ... the arguments corresponding to the format
 * @return Returns the new string
 */
Etk_String *etk_string_new_printf(const char *format, ...)
{
   Etk_String *new_string;
   va_list args;
   
   va_start(args, format);
   new_string = etk_string_set_vprintf(ETK_STRING(etk_object_new(ETK_STRING_TYPE, NULL)), format, args);
   va_end(args);
   
   return new_string;
}

/**
 * @brief Creates a new string, and sets its default value from the format and the arguments
 * @param format the format to set to the string. It uses the same arguments than printf()
 * @param args the arguments corresponding to the format
 * @return Returns the new string
 */
Etk_String *etk_string_new_vprintf(const char *format, va_list args)
{
   return etk_string_set_vprintf(ETK_STRING(etk_object_new(ETK_STRING_TYPE, NULL)), format, args);
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
 * @param Gets the string as an array of char
 * @param string a string
 * @param Returns the string as an array of char (a pointer on the first character)
 */
const char *etk_string_get(Etk_String *string)
{
   if (!string)
      return NULL;
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
 * @param length the new length of the string. If @a length is greater than the current length of the string, the function does nothing
 * @return Returns the truncated string
 */
Etk_String *etk_string_truncate(Etk_String *string, int length)
{
   if (!string)
      return NULL;
   
   if (length < string->length)
   {
      string->string[length] = 0;
      string->length = length;
      etk_object_notify(ETK_OBJECT(string), "string");
   }
   return string;
}

/**
 * @brief Sets the value of a string
 * @param string a string
 * @param value the value to assign to the string
 * @return Returns the string
 */
Etk_String *etk_string_set(Etk_String *string, const char *value)
{
   if (!string)
      return NULL;
   
   if (!value)
      etk_string_set_sized(string, NULL, 0);
   else
      etk_string_set_sized(string, value, strlen(value));
   
   return string;
}

/**
 * @brief Sets the value of a string, with a specific size.
 * @param string a string
 * @param value the value to assign to the string
 * @param size If @a size is lower than the length of @a value, the string will be truncated. @n
 * Otherwise, extra memory will be allocated (useful if you planned to insert text often and want to avoid too many reallocations)
 * @return Returns the string
 */
Etk_String *etk_string_set_sized(Etk_String *string, const char *value, int size)
{
   if (!string)
      return NULL;
   
   if (!value || *value == 0 || size <= 0)
   {
      *string->string = 0;
      string->length = 0;
   }
   else
   {
      int length;
      
      if (size > string->allocated_length)
      {
         free(string->string);
         string->string = malloc(ETK_STRING_SIZE_TO_ALLOC(size) + 1);
         string->allocated_length = ETK_STRING_SIZE_TO_ALLOC(size);
      }
      
      length = strlen(value);
      string->length = ETK_MIN(length, size);
      strncpy(string->string, value, string->length);
      string->string[string->length] = 0;
   }
   
   etk_object_notify(ETK_OBJECT(string), "string");
   return string;
}

/**
 * @brief Sets the value of the string from the format and the arguments
 * @param string a string
 * @param format the format to set to the string. It uses the same arguments than printf()
 * @param ... the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_set_printf(Etk_String *string, const char *format, ...)
{
   va_list args;
   
   if (!string)
      return NULL;
   
   va_start(args, format);
   etk_string_set_vprintf(string, format, args);
   va_end(args);
   
   return string;
}

/**
 * @brief Sets the value of the string from the format and the arguments
 * @param string a string
 * @param format the format to set to the string. It uses the same arguments than printf()
 * @param args the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_set_vprintf(Etk_String *string, const char *format, va_list args)
{
   va_list args2;
   char *text;
   
   if (!string)
      return NULL;
   
   va_copy(args2, args);
   text = _etk_string_vprintf(format, args);
   etk_string_set(string, text);
   free(text);
   va_end(args2);
   
   return string;
}

/**
 * @brief Prepends a text to a string
 * @param string a string
 * @param text the text to prepend to the string
 * @return Returns the string
 */
Etk_String *etk_string_prepend(Etk_String *string, const char *text)
{
   return etk_string_insert(string, 0, text);
}

/**
 * @brief Prepends a text with a specific length  to a string
 * @param string a string
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
 * @param string a string
 * @param c the character to prepend to the string
 * @return Returns the string
 */
Etk_String *etk_string_prepend_char(Etk_String *string, char c)
{
   return etk_string_insert_char(string, 0, c);
}

/**
 * @brief Prepends a text to a string from the format and the arguments
 * @param string a string
 * @param format the format to prepend to the string. It uses the same arguments than printf()
 * @param ... the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_prepend_printf(Etk_String *string, const char *format, ...)
{
   va_list args;
   
   if (!string)
      return NULL;
   
   va_start(args, format);
   etk_string_prepend_vprintf(string, format, args);
   va_end(args);
   
   return string;
}

/**
 * @brief Prepends a text to a string from the format and the arguments
 * @param string a string
 * @param format the format to prepend to the string. It uses the same arguments than printf()
 * @param args the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_prepend_vprintf(Etk_String *string, const char *format, va_list args)
{
   va_list args2;
   
   if (!string)
      return NULL;
   
   va_copy(args2, args);
   etk_string_insert_vprintf(string, 0, format, args2);
   va_end(args2);
   
   return string;
}

/**
 * @brief Appends a text to a string
 * @param string a string
 * @param text the text to append to the string
 * @return Returns the string
 */
Etk_String *etk_string_append(Etk_String *string, const char *text)
{
   return etk_string_insert(string, string->length, text);
}

/**
 * @brief Appends a text with a specific length  to a string
 * @param string a string
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
 * @param string a string
 * @param c the character to append to the string
 * @return Returns the string
 */
Etk_String *etk_string_append_char(Etk_String *string, char c)
{
   return etk_string_insert_char(string, string->length, c);
}

/**
 * @brief Appends a text to a string from the format and the arguments
 * @param string a string
 * @param format the format to append to the string. It uses the same arguments than printf()
 * @param ... the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_append_printf(Etk_String *string, const char *format, ...)
{
   va_list args;
   
   if (!string)
      return NULL;
   
   va_start(args, format);
   etk_string_append_vprintf(string, format, args);
   va_end(args);
   
   return string;
}

/**
 * @brief Appends a text to a string from the format and the arguments
 * @param string a string
 * @param format the format to append to the string. It uses the same arguments than printf()
 * @param args the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_append_vprintf(Etk_String *string, const char *format, va_list args)
{
   va_list args2;
   
   if (!string)
      return NULL;
   
   va_copy(args2, args);
   etk_string_insert_vprintf(string, string->length, format, args2);
   va_end(args2);
   
   return string;
}

/**
 * @brief Inserts a text into a string
 * @param string a string
 * @param pos the position where to insert the text
 * @param text the text to insert into the string
 * @return Returns the string
 */
Etk_String *etk_string_insert(Etk_String *string, int pos, const char *text)
{
   if (!string)
      return NULL;
   if (!text || *text == 0)
      return string;
   return etk_string_insert_sized(string, pos, text, strlen(text));
}

/**
 * @brief Inserts a text with a specific length into a string
 * @param string a string
 * @param pos the position where to insert the text
 * @param text the text to insert into the string
 * @param length the length of the text to insert
 * @return Returns the string
 */
Etk_String *etk_string_insert_sized(Etk_String *string, int pos, const char *text, int length)
{
   if (!string)
      return NULL;
   if (!text || *text == 0 || length <= 0)
      return string;
   
   pos = ETK_CLAMP(pos, 0, string->length);
   length = ETK_MIN(length, strlen(text));
   if (string->length + length > string->allocated_length)
   {
      string->string = realloc(string->string, ETK_STRING_SIZE_TO_ALLOC(string->length + length) + 1);
      string->allocated_length = ETK_STRING_SIZE_TO_ALLOC(string->length + length);
   }
   
   memmove(&string->string[pos + length], &string->string[pos], string->length - pos);
   strncpy(&string->string[pos], text, length);
   string->length += length;
   string->string[string->length] = 0;
   
   etk_object_notify(ETK_OBJECT(string), "string");
   
   return string;
}

/**
 * @brief Inserys a character into a string
 * @param string a string
 * @param pos the positon where to insert the char
 * @param c the character to insert into the string
 * @return Returns the string
 */
Etk_String *etk_string_insert_char(Etk_String *string, int pos, char c)
{
   int i;
   
   if (!string)
      return NULL;
   if (c == 0)
      return etk_string_truncate(string, pos);
   
   pos = ETK_CLAMP(pos, 0, string->length);
   if (string->length + 1 > string->allocated_length)
   {
      string->string = realloc(string->string, ETK_STRING_SIZE_TO_ALLOC(string->length + 1) + 1);
      string->allocated_length = ETK_STRING_SIZE_TO_ALLOC(string->length + 1);
   }
   for (i = string->length - 1; i >= pos; i--)
      string->string[i + 1] = string->string[i];
   
   string->string[pos] = c;
   string->length++;
   string->string[string->length] = 0;
   
   etk_object_notify(ETK_OBJECT(string), "string");
   
   return string;
}

/**
 * @brief Inserts a text into a string from the format and the arguments
 * @param string a string
 * @param pos the positon where to insert the char
 * @param format the format to insert into the string. It uses the same arguments than printf()
 * @param ... the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_insert_printf(Etk_String *string, int pos, const char *format, ...)
{
   va_list args;
   
   if (!string)
      return NULL;
   
   va_start(args, format);
   etk_string_insert_vprintf(string, pos, format, args);
   va_end(args);
   
   return string;
}

/**
 * @brief Inserts a text into a string from the format and the arguments
 * @param string a string
 * @param pos the positon where to insert the char
 * @param format the format to insert into the string. It uses the same arguments than printf()
 * @param args the arguments corresponding to the format
 * @return Returns the string
 */
Etk_String *etk_string_insert_vprintf(Etk_String *string, int pos, const char *format, va_list args)
{
   va_list args2;
   char *text_to_append;
   
   if (!string)
      return NULL;
   
   va_copy(args2, args);
   text_to_append = _etk_string_vprintf(format, args2);
   etk_string_insert(string, pos, text_to_append);
   free(text_to_append);
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
   
   string->string = strdup("");
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

/**************************
 *
 * Private functions
 *
 **************************/

/* Creates a new string (char *) from the format and the args, and returns it. The returned string will have to be freed */
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
   va_copy(args2, args);
   text = malloc(length + 1);
   vsprintf(text, format, args2);
   va_end(args2);
   
   return text;
}

/** @} */
