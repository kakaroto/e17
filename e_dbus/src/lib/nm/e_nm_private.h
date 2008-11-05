#ifndef E_NM_PRIVATE_H
#define E_NM_PRIVATE_H

#define _E_NM_SERVICE "org.freedesktop.NetworkManager"
#define _E_NM_PATH "/org/freedesktop/NetworkManager"
#define _E_NM_INTERFACE "org.freedesktop.NetworkManager"
#define _E_NM_INTERFACE_ACCESSPOINT "org.freedesktop.NetworkManager.AccessPoint"
#define _E_NM_INTERFACE_DEVICE "org.freedesktop.NetworkManager.Device"
#define _E_NM_INTERFACE_DEVICE_WIRELESS "org.freedesktop.NetworkManager.Device.Wireless"
#define _E_NM_INTERFACE_DEVICE_WIRED "org.freedesktop.NetworkManager.Device.Wired"
#define _E_NM_INTERFACE_IP4CONFIG "org.freedesktop.NetworkManager.IP4Config"

#define e_nm_call_new(member) dbus_message_new_method_call(_E_NM_SERVICE, _E_NM_PATH, _E_NM_INTERFACE, member)
#define e_nm_properties_get(con, prop, cb, data) e_dbus_properties_get(con, _E_NM_SERVICE, _E_NM_PATH, _E_NM_INTERFACE, prop, (E_DBus_Method_Return_Cb) cb, data)
#define e_nm_access_point_properties_get(con, dev, prop, cb, data) e_dbus_properties_get(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_ACCESSPOINT, prop, (E_DBus_Method_Return_Cb) cb, data)
#define e_nm_device_properties_get(con, dev, prop, cb, data) e_dbus_properties_get(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_DEVICE, prop, (E_DBus_Method_Return_Cb) cb, data)
#define e_nm_ip4_config_properties_get(con, dev, prop, cb, data) e_dbus_properties_get(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_IP4CONFIG, prop, (E_DBus_Method_Return_Cb) cb, data)

#define e_nm_signal_handler_add(con, sig, cb, data) e_dbus_signal_handler_add(con, _E_NM_SERVICE, _E_NM_PATH, _E_NM_INTERFACE, sig, cb, data)
#define e_nm_device_signal_handler_add(con, dev, sig, cb, data) e_dbus_signal_handler_add(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE, sig, cb, data)

typedef struct Property Property;
struct Property
{
  const char *name;
  void (*func)(void *data, DBusMessage *msg, DBusError *err);
  size_t offset;
};

typedef struct E_NM_Internal E_NM_Internal;
struct E_NM_Internal
{
  E_NM nm;

  E_DBus_Connection *conn;

  int  (*state_changed)(E_NM *nm, unsigned int state);
  int  (*properties_changed)(E_NM *nm);
  int  (*device_added)(E_NM *nm, const char *device);
  int  (*device_removed)(E_NM *nm, const char *device);

  Ecore_List *handlers;
#if 0
  Ecore_List *devices;
#endif

  void *data;
};

typedef struct E_NM_Device_Internal E_NM_Device_Internal;
struct E_NM_Device_Internal
{
  E_NM_Device dev;

  E_NM_Internal *nmi;

  int  (*state_changed)(E_NM_Device *device, unsigned int state);
  int  (*properties_changed)(E_NM_Device *device);

  Ecore_List *handlers;

  void *data;
};

typedef struct E_NM_Access_Point_Internal E_NM_Access_Point_Internal;
struct E_NM_Access_Point_Internal
{
  E_NM_Access_Point ap;

  E_NM_Internal *nmi;

  int  (*properties_changed)(E_NM_Access_Point *device);

  Ecore_List *handlers;

  void *data;
};

typedef struct E_NM_IP4_Config_Internal E_NM_IP4_Config_Internal;
struct E_NM_IP4_Config_Internal
{
  E_NM_IP4_Config cfg;

  E_NM_Internal *nmi;
};

typedef struct E_NM_Data E_NM_Data;
struct E_NM_Data
{
  E_NM_Internal        *nmi;
  int                 (*cb_func)(void *data, void *reply);
  char                 *object;
  void                 *data;
  void                 *reply;

  Property             *property;
};

void property_string(void *data, DBusMessage *msg, DBusError *err);
void property_object_path(void *data, DBusMessage *msg, DBusError *err);
void property_uint32(void *data, DBusMessage *msg, DBusError *err);
void property_bool(void *data, DBusMessage *msg, DBusError *err);
void property_uint32_list_list(void *data, DBusMessage *msg, DBusError *err);
void property_uint32_list(void *data, DBusMessage *msg, DBusError *err);
void property_string_list(void *data, DBusMessage *msg, DBusError *err);

#if 0
void *cb_nm_generic(DBusMessage *msg, DBusError *err);
void  free_nm_generic(void *data);
void *cb_nm_int32(DBusMessage *msg, DBusError *err);
void *cb_nm_uint32(DBusMessage *msg, DBusError *err);
void *cb_nm_object_path(DBusMessage *msg, DBusError *err);
void *cb_nm_boolean(DBusMessage *msg, DBusError *err);
#endif
void *cb_nm_object_path_list(DBusMessage *msg, DBusError *err);
void  free_nm_object_path_list(void *data);

int   nm_check_arg_type(DBusMessageIter *iter, char type);

void  e_nm_data_free(E_NM_Data *data);
#endif
