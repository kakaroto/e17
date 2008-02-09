#include <string.h>

#include "evolve_private.h"

Evolve_Property_Type evolve_property_type_get(Evolve_Property *property)
{
	if (!property || !property->default_value)
		return EVOLVE_PROPERTY_UNKNOWN;

	return property->default_value->type;
}

Evolve_Property_Value *evolve_property_value_new()
{
	Evolve_Property_Value *value = calloc(1, sizeof(Evolve_Property_Value));
	value->type = EVOLVE_PROPERTY_UNKNOWN;
	return value;
}

Evolve_Property_Value *evolve_property_value_string(const char *value)
{
   Evolve_Property_Value *new_value;
   new_value = evolve_property_value_new();
	 new_value->string_value = value ? strdup(value) : NULL;
	 new_value->type = EVOLVE_PROPERTY_STRING;
   return new_value;
}

Evolve_Property_Value *evolve_property_value_double(double value)
{
   Evolve_Property_Value *new_value;
   new_value = evolve_property_value_new();
	 new_value->double_value = value;
	 new_value->type = EVOLVE_PROPERTY_DOUBLE;
   return new_value;
}

Evolve_Property_Value *evolve_property_value_int(int value)
{
   Evolve_Property_Value *new_value;
   new_value = evolve_property_value_new();
	 new_value->int_value = value;
	 new_value->type = EVOLVE_PROPERTY_INT;
   return new_value;
}

const char *evolve_property_value_string_get(Evolve_Property_Value *value)
{
	if (!value)
		return NULL;
	return value->string_value;
}

int evolve_property_value_bool_get(Evolve_Property_Value *value)
{
	if (!value)
		return ETK_FALSE;
	return value->bool_value;
}

long int evolve_property_value_long_get(Evolve_Property_Value *value)
{
	if (!value)
		return 0;
	return value->long_value;
}

char evolve_property_value_char_get(Evolve_Property_Value *value)
{
	if (!value)
		return 0;
	return value->char_value;
}

float evolve_property_value_float_get(Evolve_Property_Value *value)
{
	if (!value)
		return 0;
	return value->float_value;
}

short int evolve_property_value_short_get(Evolve_Property_Value *value)
{
	if (!value)
		return 0;
	return value->short_value;
}

int evolve_property_value_int_get(Evolve_Property_Value *value)
{
	if (!value)
		return 0;
	return value->int_value;
}

double evolve_property_value_double_get(Evolve_Property_Value *value)
{
	if (!value)
		return 0;
	return value->double_value;
}

void evolve_property_value_delete(Evolve_Property_Value *value)
{
	if (!value)
		return;
	if (value->string_value) free(value->string_value);
	free(value);
}
