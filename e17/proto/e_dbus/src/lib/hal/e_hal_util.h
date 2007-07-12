#ifndef E_HAL_UTIL_H
#define E_HAL_UTIL_H

#include "E_Hal.h"

void e_hal_property_free(E_Hal_Property *prop);
char *e_hal_property_string_get(E_Hal_Properties *properties, const char *key, int *err);
char e_hal_property_bool_get(E_Hal_Properties *properties, const char *key, int *err);
int e_hal_property_int_get(E_Hal_Properties *properties, const char *key, int *err);
dbus_uint64_t e_hal_property_uint64_get(E_Hal_Properties *properties, const char *key, int *err);
double e_hal_property_double_get(E_Hal_Properties *properties, const char *key, int *err);
Ecore_List *e_hal_property_strlist_get(E_Hal_Properties *properties, const char *key, int *err);

#endif
