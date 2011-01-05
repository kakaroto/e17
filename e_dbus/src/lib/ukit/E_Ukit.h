#ifndef E_UKIT_H
#define E_UKIT_H

#include <Eina.h>
#include <E_DBus.h>

/**
 * @defgroup EUkit_Group EUkit
 *
 * @{
 */

#define E_UDISKS_BUS "org.freedesktop.UDisks"
#define E_UDISKS_PATH "/org/freedesktop/UDisks"
#define E_UDISKS_INTERFACE "org.freedesktop.UDisks.Device"

#define E_UPOWER_BUS "org.freedesktop.UPower"
#define E_UPOWER_PATH "/org/freedesktop/UPower"
#define E_UPOWER_INTERFACE "org.freedesktop.UPower.Device"

/* message return types */

typedef struct E_Ukit_Property E_Ukit_Property;
typedef struct E_Ukit_Properties E_Ukit_Properties;

struct E_Ukit_String_List_Return
{
   Eina_List *strings; /* list of const char * */
};

struct E_Ukit_Bool_Return
{
   Eina_Bool boolean;
};

struct E_Ukit_UDI_Return
{
   const char *udi;
};

struct E_Ukit_Capability
{
   const char *udi;
   const char *capability;
};

typedef enum
{
   E_UKIT_PROPERTY_TYPE_STRING,
   E_UKIT_PROPERTY_TYPE_INT,
   E_UKIT_PROPERTY_TYPE_UINT32,
   E_UKIT_PROPERTY_TYPE_UINT64,
   E_UKIT_PROPERTY_TYPE_INT64,
   E_UKIT_PROPERTY_TYPE_BOOL,
   E_UKIT_PROPERTY_TYPE_DOUBLE,
   E_UKIT_PROPERTY_TYPE_STRLIST
} E_Ukit_Property_Type;

struct E_Ukit_Property
{
   E_Ukit_Property_Type type;
   union 
     {
        const char *s;
        int i;
        Eina_Bool b;
        double d;
        uint32_t u;
        uint64_t t;
        int64_t x;
        Eina_List *strlist;
     } val;
};

struct E_Ukit_Properties
{
   Eina_Hash *properties;
};

typedef enum
{
   E_UPOWER_BATTERY_UNKNOWN,
   E_UPOWER_BATTERY_LION,
   E_UPOWER_BATTERY_LPOLYMER,
   E_UPOWER_BATTERY_LIRONPHOS,
   E_UPOWER_BATTERY_LEAD,
   E_UPOWER_BATTERY_NICAD,
   E_UPOWER_BATTERY_METALHYDRYDE
} E_UPower_Battery_Type;

typedef enum
{
   E_UPOWER_STATE_UNKNOWN,
   E_UPOWER_STATE_CHARGING,
   E_UPOWER_STATE_DISCHARGING,
   E_UPOWER_STATE_EMPTY,
   E_UPOWER_STATE_FULL,
   E_UPOWER_STATE_PENDINGCHARGE,
   E_UPOWER_STATE_PENDINGDISCHARGE
} E_Upower_State;

typedef enum
{
   E_UPOWER_SOURCE_UNKNOWN,
   E_UPOWER_SOURCE_AC,
   E_UPOWER_SOURCE_BATTERY,
   E_UPOWER_SOURCE_UPS,
   E_UPOWER_SOURCE_MONITOR,
   E_UPOWER_SOURCE_MOUSE,
   E_UPOWER_SOURCE_KEYBOARD,
   E_UPOWER_SOURCE_PDA,
   E_UPOWER_SOURCE_PHONE
} E_Upower_Source;
   

typedef struct E_Ukit_Properties E_Ukit_Get_All_Properties_Return;
typedef struct E_Ukit_Property E_Ukit_Get_Property_Return;
typedef struct E_Ukit_String_List_Return E_Ukit_String_List_Return;
typedef struct E_Ukit_String_List_Return E_Ukit_Get_All_Devices_Return;

typedef struct E_Ukit_UDI_Return E_Ukit_Device_Added;
typedef struct E_Ukit_UDI_Return E_Ukit_Device_Removed;
typedef struct E_Ukit_Capability E_Ukit_New_Capability;

#ifdef __cplusplus
extern "C" {
#endif

   EAPI int e_ukit_init(void);
   EAPI int e_ukit_shutdown(void);

   EAPI DBusPendingCall *e_udisks_get_property(E_DBus_Connection *conn, const char *udi, const char *property, E_DBus_Callback_Func cb_func, void *data);
   EAPI DBusPendingCall *e_udisks_get_all_properties(E_DBus_Connection *conn, const char *udi, E_DBus_Callback_Func cb_func, void *data);
   EAPI DBusPendingCall *e_udisks_get_all_devices(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data);

   EAPI DBusPendingCall *e_upower_get_property(E_DBus_Connection *conn, const char *udi, const char *property, E_DBus_Callback_Func cb_func, void *data);
   EAPI DBusPendingCall *e_upower_get_all_properties(E_DBus_Connection *conn, const char *udi, E_DBus_Callback_Func cb_func, void *data);
   EAPI DBusPendingCall *e_upower_get_all_devices(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data);

   EAPI DBusPendingCall * e_upower_suspend_allowed(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data);
   EAPI DBusPendingCall * e_upower_suspend(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data);
   EAPI DBusPendingCall * e_upower_hibernate(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data);
   EAPI DBusPendingCall * e_upower_hibernate_allowed(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data);

/* utility functions */
   EAPI void                e_ukit_property_free(E_Ukit_Property *prop);
   EAPI const char         *e_ukit_property_string_get(E_Ukit_Properties *properties, const char *key, int *err);
   EAPI Eina_Bool           e_ukit_property_bool_get(E_Ukit_Properties *properties, const char *key, int *err);
   EAPI int                 e_ukit_property_int_get(E_Ukit_Properties *properties, const char *key, int *err);
   EAPI uint32_t            e_ukit_property_uint32_get(E_Ukit_Properties *properties, const char *key, int *err);
   EAPI uint64_t            e_ukit_property_uint64_get(E_Ukit_Properties *properties, const char *key, int *err);
   EAPI int64_t             e_ukit_property_int64_get(E_Ukit_Properties *properties, const char *key, int *err);
   EAPI double              e_ukit_property_double_get(E_Ukit_Properties *properties, const char *key, int *err);
   EAPI const Eina_List    *e_ukit_property_strlist_get(E_Ukit_Properties *properties, const char *key, int *err);

/* (un)mount */
   EAPI DBusPendingCall *e_udisks_volume_mount(E_DBus_Connection *conn, const char *udi, const char *fstype, Eina_List *options);
   EAPI DBusPendingCall *e_udisks_volume_unmount(E_DBus_Connection *conn, const char *udi, Eina_List *options);
   EAPI DBusPendingCall *e_udisks_volume_eject(E_DBus_Connection *conn, const char *udi, Eina_List *options);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
