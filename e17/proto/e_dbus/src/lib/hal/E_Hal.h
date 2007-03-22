#ifndef E_HAL_H
#define E_HAL_H
#include <E_DBus.h>
#include <Ecore_Data.h>

#define E_HAL_SENDER "org.freedesktop.Hal"
#define E_HAL_MANAGER_PATH "/org/freedesktop/Hal/Manager"
#define E_HAL_MANAGER_INTERFACE "org.freedesktop.Hal.Manager"
#define E_HAL_DEVICE_INTERFACE "org.freedesktop.Hal.Device"
#define E_HAL_DEVICE_VOLUME_INTERFACE "org.freedesktop.Hal.Device.Volume"

/**
 * A callback function for a Hal call
 * @param user_data the data passed in to the method call
 * @param event_data a struct containing the return data.
 *
 * For method calls, the return structs use the following naming convention:
 *   E_Hal_<Interface>_<Method_Call_Name>_Return
 */
typedef void (*E_Hal_Callback_Func) (void *user_data, void *method_return, DBusError *error);


/* message return types */

typedef struct E_Hal_Property E_Hal_Property;
typedef struct E_Hal_Properties E_Hal_Properties;

struct E_Hal_String_List_Return
{
  Ecore_List *strings; /* list of const char * */
};

struct E_Hal_Bool_Return
{
  char boolean;
};

struct E_Hal_UDI_Return
{
  const char *udi;
};

struct E_Hal_Capability
{
  const char *udi;
  const char *capability;
};

typedef enum
{
  E_HAL_PROPERTY_TYPE_STRING,
  E_HAL_PROPERTY_TYPE_INT,
  E_HAL_PROPERTY_TYPE_UINT64,
  E_HAL_PROPERTY_TYPE_BOOL,
  E_HAL_PROPERTY_TYPE_DOUBLE,
  E_HAL_PROPERTY_TYPE_STRLIST
} E_Hal_Property_Type;

struct E_Hal_Property
{
  E_Hal_Property_Type type;
  union 
  {
    const char *s;
    int i;
    char b;
    double d;
    dbus_uint64_t u64;
    Ecore_List *strlist;
  } val;
};

struct E_Hal_Properties
{
  Ecore_Hash *properties;
};

typedef struct E_Hal_Callback E_Hal_Callback;
struct E_Hal_Callback
{
  E_Hal_Callback_Func func;
  void *user_data;
};



typedef struct E_Hal_Properties E_Hal_Device_Get_All_Properties_Return;
typedef struct E_Hal_Property E_Hal_Device_Get_Property_Return;
typedef struct E_Hal_Bool_Return E_Hal_Device_Query_Capability_Return;
typedef struct E_Hal_String_List_Return E_Hal_Manager_Get_All_Devices_Return;
typedef struct E_Hal_Bool_Return E_Hal_Manager_Device_Exists_Return;
typedef struct E_Hal_String_List_Return E_Hal_Manager_Find_Device_String_Match_Return;
typedef struct E_Hal_String_List_Return E_Hal_Manager_Find_Device_By_Capability_Return;


typedef struct E_Hal_UDI_Return E_Hal_Manager_Device_Added;
typedef struct E_Hal_UDI_Return E_Hal_Manager_Device_Removed;
typedef struct E_Hal_Capability E_Hal_Manager_New_Capability;

/* org.freedesktop.Hal.Device */
int e_hal_device_get_property(E_DBus_Connection *conn, const char *udi, const char *property, E_Hal_Callback_Func cb_func, void *data);
int e_hal_device_get_all_properties(E_DBus_Connection *conn, const char *udi, E_Hal_Callback_Func cb_func, void *data);
int e_hal_device_query_capability(E_DBus_Connection *conn, const char *udi, const char *capability, E_Hal_Callback_Func cb_func, void *data);

/* org.freedesktop.Hal.Manager */
int e_hal_manager_get_all_devices(E_DBus_Connection *conn, E_Hal_Callback_Func cb_func, void *data);
int e_hal_manager_device_exists(E_DBus_Connection *conn, const char *udi, E_Hal_Callback_Func cb_func, void *data);
int e_hal_manager_find_device_string_match(E_DBus_Connection *conn, const char *key, const char *value, E_Hal_Callback_Func cb_func, void *data);
int e_hal_manager_find_device_by_capability(E_DBus_Connection *conn, const char *capability, E_Hal_Callback_Func cb_func, void *data);

E_Hal_Callback *e_hal_callback_new(E_Hal_Callback_Func cb_func, void *user_data);
void e_hal_callback_free(E_Hal_Callback *callback);

/* utility functions */
void e_hal_property_free(E_Hal_Property *prop);
char *e_hal_property_string_get(E_Hal_Properties *properties, const char *key, int *err);
char e_hal_property_bool_get(E_Hal_Properties *properties, const char *key, int *err);
int e_hal_property_int_get(E_Hal_Properties *properties, const char *key, int *err);
dbus_uint64_t e_hal_property_uint64_get(E_Hal_Properties *properties, const char *key, int *err);
double e_hal_property_double_get(E_Hal_Properties *properties, const char *key, int *err);
Ecore_List *e_hal_property_strlist_get(E_Hal_Properties *properties, const char *key, int *err);


void cb_error_generic(void *data, const char *name, const char *msg);

#endif
