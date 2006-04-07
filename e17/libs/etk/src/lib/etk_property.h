/** @file etk_property.h */
#ifndef _ETK_PROPERTY_H_
#define _ETK_PROPERTY_H_

#include <stdarg.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Property Etk_Property
 * @{
 */

/**
 * @enum Etk_Property_Type
 * @brief The type of a property
 */
typedef enum _Etk_Property_Type
{
   /* Used when the property is not initialized to a value */
   ETK_PROPERTY_NONE,
   ETK_PROPERTY_INT,
   ETK_PROPERTY_BOOL,
   ETK_PROPERTY_CHAR,
   ETK_PROPERTY_FLOAT,
   ETK_PROPERTY_DOUBLE,
   ETK_PROPERTY_SHORT,
   ETK_PROPERTY_LONG,
   ETK_PROPERTY_POINTER,
   ETK_PROPERTY_STRING,
   ETK_PROPERTY_OTHER,
} Etk_Property_Type;

/** 
 * @enum Etk_Property_Flags
 * @brief Describes if the property is readable, writable or both. @n
 * You can also specify if the property needs to be initialized to its default value at the constuction of a new object (ETK_PROPERTY_CONSTRUCT)
 */
typedef enum _Etk_Property_Flags
{
   ETK_PROPERTY_NO_ACCESS = 1 << 0,
   ETK_PROPERTY_READABLE = 1 << 1,
   ETK_PROPERTY_WRITABLE = 1 << 2,
   ETK_PROPERTY_READABLE_WRITABLE = ETK_PROPERTY_READABLE | ETK_PROPERTY_WRITABLE,
   ETK_PROPERTY_CONSTRUCT = 1 << 3
} Etk_Property_Flags;

/**
 * @struct Etk_Property
 * @brief An Etk_Property is a name associated to a type. The property have a default value, and can be readable, writable or both
 */
struct _Etk_Property
{
   /* private: */
   char *name;
   int id;
   Etk_Property_Type type;
   Etk_Property_Flags flags;
   Etk_Property_Value *default_value;
};

/**
 * @struct Etk_Property_Value
 * @brief An Etk_Property_Value is an Etk_Property_Type associated to an Etk_Property_Value_Value @n
 * It's mainly used to set a value to an object property
 */
struct _Etk_Property_Value
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
      void *pointer_value;
      char *string_value;
   } value;
   Etk_Property_Type type;
};

Etk_Property *etk_property_new(const char *name, int property_id, Etk_Property_Type type, Etk_Property_Flags flags, Etk_Property_Value *default_value);
void etk_property_delete(Etk_Property *property);
Etk_Bool etk_property_default_value_set(Etk_Property *property, Etk_Property_Value *default_value);
Etk_Property_Type etk_property_type_get(Etk_Property *property);

Etk_Property_Value *etk_property_value_new();
void etk_property_value_delete(Etk_Property_Value *value);
Etk_Property_Type etk_property_value_type_get(Etk_Property_Value *value);

Etk_Property_Value *etk_property_value_create         (Etk_Property_Type type, ...);
Etk_Property_Value *etk_property_value_create_valist  (Etk_Property_Type type, va_list *arg);
Etk_Property_Value *etk_property_value_int            (int value);
Etk_Property_Value *etk_property_value_bool           (Etk_Bool value);
Etk_Property_Value *etk_property_value_char           (char value);
Etk_Property_Value *etk_property_value_float          (float value);
Etk_Property_Value *etk_property_value_double         (double value);
Etk_Property_Value *etk_property_value_short          (short value);
Etk_Property_Value *etk_property_value_long           (long value);
Etk_Property_Value *etk_property_value_pointer        (void *value);
Etk_Property_Value *etk_property_value_string         (const char *value);

void etk_property_value_set         (Etk_Property_Value *property_value, Etk_Property_Type type, ...);
void etk_property_value_set_valist  (Etk_Property_Value *property_value, Etk_Property_Type type, va_list *arg);
void etk_property_value_int_set     (Etk_Property_Value *property_value, int value);
void etk_property_value_bool_set    (Etk_Property_Value *property_value, Etk_Bool value);
void etk_property_value_char_set    (Etk_Property_Value *property_value, char value);
void etk_property_value_float_set   (Etk_Property_Value *property_value, float value);
void etk_property_value_double_set  (Etk_Property_Value *property_value, double value);
void etk_property_value_short_set   (Etk_Property_Value *property_value, short value);
void etk_property_value_long_set    (Etk_Property_Value *property_value, long value);
void etk_property_value_pointer_set (Etk_Property_Value *property_value, void *value);
void etk_property_value_string_set  (Etk_Property_Value *property_value, const char *value);

void        etk_property_value_get(Etk_Property_Value *value, Etk_Property_Type type, void *value_location);
int         etk_property_value_int_get(Etk_Property_Value *value);
Etk_Bool    etk_property_value_bool_get(Etk_Property_Value *value);
char        etk_property_value_char_get(Etk_Property_Value *value);
float       etk_property_value_float_get(Etk_Property_Value *value);
double      etk_property_value_double_get(Etk_Property_Value *value);
short       etk_property_value_short_get(Etk_Property_Value *value);
long        etk_property_value_long_get(Etk_Property_Value *value);
void *      etk_property_value_pointer_get(Etk_Property_Value *value);
const char *etk_property_value_string_get(Etk_Property_Value *value);

/** @} */

#endif
