#ifndef E_HAL_H
#define E_HAL_H
#include <E_DBus.h>
#include <Ecore_Data.h>
#include <eina_stringshare.h>

#ifdef EAPI
#undef EAPI
#endif
#ifdef _MSC_VER
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#define E_HAL_SENDER "org.freedesktop.Hal"
#define E_HAL_MANAGER_PATH "/org/freedesktop/Hal/Manager"
#define E_HAL_MANAGER_INTERFACE "org.freedesktop.Hal.Manager"
#define E_HAL_DEVICE_INTERFACE "org.freedesktop.Hal.Device"
#define E_HAL_DEVICE_VOLUME_INTERFACE "org.freedesktop.Hal.Device.Volume"

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
    dbus_bool_t b;
    double d;
    dbus_uint64_t u64;
    Ecore_List *strlist;
  } val;
};

struct E_Hal_Properties
{
  Ecore_Hash *properties;
};

typedef struct E_Hal_Properties E_Hal_Device_Get_All_Properties_Return;
typedef struct E_Hal_Property E_Hal_Device_Get_Property_Return;
typedef struct E_Hal_Bool_Return E_Hal_Device_Query_Capability_Return;
typedef struct E_Hal_String_List_Return E_Hal_String_List_Return;
typedef struct E_Hal_String_List_Return E_Hal_Manager_Get_All_Devices_Return;
typedef struct E_Hal_Bool_Return E_Hal_Manager_Device_Exists_Return;
typedef struct E_Hal_String_List_Return E_Hal_Manager_Find_Device_String_Match_Return;
typedef struct E_Hal_String_List_Return E_Hal_Manager_Find_Device_By_Capability_Return;


typedef struct E_Hal_UDI_Return E_Hal_Manager_Device_Added;
typedef struct E_Hal_UDI_Return E_Hal_Manager_Device_Removed;
typedef struct E_Hal_Capability E_Hal_Manager_New_Capability;

#ifdef __cplusplus
extern "C" {
#endif

/* org.freedesktop.Hal.Device */
   EAPI int e_hal_device_get_property(E_DBus_Connection *conn, const char *udi, const char *property, E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_hal_device_get_all_properties(E_DBus_Connection *conn, const char *udi, E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_hal_device_query_capability(E_DBus_Connection *conn, const char *udi, const char *capability, E_DBus_Callback_Func cb_func, void *data);

/* org.freedesktop.Hal.Manager */
   EAPI int e_hal_manager_get_all_devices(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_hal_manager_device_exists(E_DBus_Connection *conn, const char *udi, E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_hal_manager_find_device_string_match(E_DBus_Connection *conn, const char *key, const char *value, E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_hal_manager_find_device_by_capability(E_DBus_Connection *conn, const char *capability, E_DBus_Callback_Func cb_func, void *data);

/* utility functions */
   EAPI void           e_hal_property_free(E_Hal_Property *prop);
   EAPI char          *e_hal_property_string_get(E_Hal_Properties *properties, const char *key, int *err);
   EAPI char           e_hal_property_bool_get(E_Hal_Properties *properties, const char *key, int *err);
   EAPI int            e_hal_property_int_get(E_Hal_Properties *properties, const char *key, int *err);
   EAPI dbus_uint64_t  e_hal_property_uint64_get(E_Hal_Properties *properties, const char *key, int *err);
   EAPI double         e_hal_property_double_get(E_Hal_Properties *properties, const char *key, int *err);
   EAPI Ecore_List    *e_hal_property_strlist_get(E_Hal_Properties *properties, const char *key, int *err);

/* (un)mount */
   EAPI int e_hal_device_volume_mount(E_DBus_Connection *conn, const char *udi, const char *mount_point, const char *fstype, Ecore_List *options, E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_hal_device_volume_unmount(E_DBus_Connection *conn, const char *udi, Ecore_List *options, E_DBus_Callback_Func cb_func, void *data);

#ifdef __cplusplus
}
#endif

#endif
