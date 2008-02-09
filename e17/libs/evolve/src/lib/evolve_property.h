#ifndef _EVOLVE_PROPERTY_H
#define _EVOLVE_PROPERTY_H

typedef struct _Evolve_Property Evolve_Property;
typedef struct _Evolve_Property_Value Evolve_Property_Value;

typedef enum _Evolve_Property_Type
{
	EVOLVE_PROPERTY_UNKNOWN,
	EVOLVE_PROPERTY_INT,
	EVOLVE_PROPERTY_BOOL,
	EVOLVE_PROPERTY_CHAR,
	EVOLVE_PROPERTY_FLOAT,
	EVOLVE_PROPERTY_DOUBLE,
	EVOLVE_PROPERTY_SHORT,
	EVOLVE_PROPERTY_LONG,
	EVOLVE_PROPERTY_OBJECT,
	EVOLVE_PROPERTY_POINTER,
	EVOLVE_PROPERTY_STRING
} Evolve_Property_Type;

struct _Evolve_Property
{
   char *name;
   int id;
   Evolve_Property_Type type;
   Evolve_Property_Value *default_value;
};

struct _Evolve_Property_Value
{
	 int int_value;
   int bool_value;
   char char_value;
   float float_value;
   double double_value;
   short short_value;
   long long_value;
   char *string_value;
	 int type;
};

Evolve_Property_Type evolve_property_type_get(Evolve_Property *property);
Evolve_Property_Value *evolve_property_value_new();
Evolve_Property_Value *evolve_property_value_string(const char *value);
Evolve_Property_Value *evolve_property_value_double(double value);
Evolve_Property_Value *evolve_property_value_int(int value);

const char *evolve_property_value_string_get(Evolve_Property_Value *value);
int evolve_property_value_bool_get(Evolve_Property_Value *value);
long int evolve_property_value_long_get(Evolve_Property_Value *value);
char evolve_property_value_char_get(Evolve_Property_Value *value);
float evolve_property_value_float_get(Evolve_Property_Value *value);
short int evolve_property_value_short_get(Evolve_Property_Value *value);
int evolve_property_value_int_get(Evolve_Property_Value *value);
double evolve_property_value_double_get(Evolve_Property_Value *value);
void evolve_property_value_delete(Evolve_Property_Value *value);

#endif
