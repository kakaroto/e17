#ifndef E_HAL_UTIL_H
#define E_HAL_UTIL_H

#include "E_Hal.h"
/**
 * A callback function for a Hal call
 * @param user_data the data passed in to the method call
 * @param event_data a struct containing the return data.
 *
 * For method calls, the return structs use the following naming convention:
 *   E_Hal_<Interface>_<Method_Call_Name>_Return
 */
typedef void (*E_Hal_Callback_Func) (void *user_data, void *method_return);

typedef struct E_Hal_Callback E_Hal_Callback;
struct E_Hal_Callback
{
  E_Hal_Callback_Func func;
  void *user_data;
};

E_Hal_Callback *e_hal_callback_new(E_Hal_Callback_Func cb_func, void *user_data);
void e_hal_callback_free(E_Hal_Callback *callback);


void e_hal_property_free(E_Hal_Property *prop);
char *e_hal_property_string_get(E_Hal_Properties *properties, const char *key, int *err);
char e_hal_property_bool_get(E_Hal_Properties *properties, const char *key, int *err);
int e_hal_property_int_get(E_Hal_Properties *properties, const char *key, int *err);
dbus_uint64_t e_hal_property_uint64_get(E_Hal_Properties *properties, const char *key, int *err);
double e_hal_property_double_get(E_Hal_Properties *properties, const char *key, int *err);
Ecore_List *e_hal_property_strlist_get(E_Hal_Properties *properties, const char *key, int *err);


void cb_error_generic(void *data, const char *name, const char *msg);

#endif
