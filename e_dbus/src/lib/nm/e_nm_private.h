#ifndef E_NM_PRIVATE_H
#define E_NM_PRIVATE_H

#define E_NM_PATH "/org/freedesktop/NetworkManager"
#define _E_NM_SERVICE "org.freedesktop.NetworkManager"
#define _E_NM_INTERFACE "org.freedesktop.NetworkManager"
#define _E_NM_INTERFACE_ACCESSPOINT "org.freedesktop.NetworkManager.AccessPoint"
#define _E_NM_INTERFACE_DEVICE "org.freedesktop.NetworkManager.Device"
#define _E_NM_INTERFACE_DEVICE_WIRELESS "org.freedesktop.NetworkManager.Device.Wireless"
#define _E_NM_INTERFACE_DEVICE_WIRED "org.freedesktop.NetworkManager.Device.Wired"
#define _E_NM_INTERFACE_IP4CONFIG "org.freedesktop.NetworkManager.IP4Config"
#define _E_NM_INTERFACE_CONNECTION_ACTIVE "org.freedesktop.NetworkManager.Connection.Active"
#define _E_NMS_PATH "/org/freedesktop/NetworkManagerSettings"
#define E_NMS_SERVICE_SYSTEM "org.freedesktop.NetworkManagerSystemSettings"
#define E_NMS_SERVICE_USER "org.freedesktop.NetworkManagerUserSettings"
#define _E_NMS_INTERFACE "org.freedesktop.NetworkManagerSettings"
#define _E_NMS_INTERFACE_CONNECTION "org.freedesktop.NetworkManagerSettings.Connection"

#define e_nm_call_new(member) dbus_message_new_method_call(_E_NM_SERVICE, E_NM_PATH, _E_NM_INTERFACE, member)
#define e_nms_call_new(service, member) dbus_message_new_method_call(service, _E_NMS_PATH, _E_NMS_INTERFACE, member)
#define e_nms_connection_call_new(service, conn, member) dbus_message_new_method_call(service, conn, _E_NMS_INTERFACE_CONNECTION, member)

#define e_nm_properties_get(con, prop, cb, data) e_dbus_properties_get(con, _E_NM_SERVICE, E_NM_PATH, _E_NM_INTERFACE, prop, (E_DBus_Method_Return_Cb) cb, data)
#define e_nm_properties_set(con, prop, type, value, cb, data) e_dbus_properties_set(con, _E_NM_SERVICE, E_NM_PATH, _E_NM_INTERFACE, prop, type, value, (E_DBus_Method_Return_Cb) cb, data)
#define e_nm_access_point_properties_get(con, dev, prop, cb, data) e_dbus_properties_get(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_ACCESSPOINT, prop, (E_DBus_Method_Return_Cb) cb, data)
#define e_nm_device_properties_get(con, dev, prop, cb, data) e_dbus_properties_get(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_DEVICE, prop, (E_DBus_Method_Return_Cb) cb, data)
#define e_nm_device_wired_properties_get(con, dev, prop, cb, data) e_dbus_properties_get(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_DEVICE_WIRED, prop, (E_DBus_Method_Return_Cb) cb, data)
#define e_nm_device_wireless_properties_get(con, dev, prop, cb, data) e_dbus_properties_get(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_DEVICE_WIRELESS, prop, (E_DBus_Method_Return_Cb) cb, data)
#define e_nm_ip4_config_properties_get(con, dev, prop, cb, data) e_dbus_properties_get(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_IP4CONFIG, prop, (E_DBus_Method_Return_Cb) cb, data)
#define e_nm_active_connection_properties_get(con, dev, prop, cb, data) e_dbus_properties_get(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_CONNECTION_ACTIVE, prop, (E_DBus_Method_Return_Cb) cb, data)

#define e_nm_signal_handler_add(con, sig, cb, data) e_dbus_signal_handler_add(con, _E_NM_SERVICE, E_NM_PATH, _E_NM_INTERFACE, sig, cb, data)
#define e_nm_access_point_signal_handler_add(con, dev, sig, cb, data) e_dbus_signal_handler_add(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_ACCESSPOINT, sig, cb, data)
#define e_nm_device_signal_handler_add(con, dev, sig, cb, data) e_dbus_signal_handler_add(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_DEVICE, sig, cb, data)
#define e_nm_device_wired_signal_handler_add(con, dev, sig, cb, data) e_dbus_signal_handler_add(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_DEVICE_WIRED, sig, cb, data)
#define e_nm_device_wireless_signal_handler_add(con, dev, sig, cb, data) e_dbus_signal_handler_add(con, _E_NM_SERVICE, dev, _E_NM_INTERFACE_DEVICE_WIRELESS, sig, cb, data)

#define e_nms_signal_handler_add(con, service, sig, cb, data) e_dbus_signal_handler_add(con, service, _E_NMS_PATH, _E_NMS_INTERFACE, sig, cb, data)

typedef struct E_NM_Internal E_NM_Internal;
struct E_NM_Internal
{
  E_NM nm;

  E_DBus_Connection *conn;

  int  (*state_changed)(E_NM *nm, E_NM_State state);
  int  (*properties_changed)(E_NM *nm);
  int  (*device_added)(E_NM *nm, const char *device);
  int  (*device_removed)(E_NM *nm, const char *device);
  Ecore_List *handlers;

  void *data;
};

typedef struct E_NM_Device_Internal E_NM_Device_Internal;
struct E_NM_Device_Internal
{
  E_NM_Device dev;

  E_NM_Internal *nmi;

  int  (*state_changed)(E_NM_Device *device, E_NM_State new_state, E_NM_State old_state, E_NM_Device_State_Reason reason);
  int  (*properties_changed)(E_NM_Device *device);
  /* TODO: Save some bytes by having internal wired and wireless object */
  int  (*access_point_added)(E_NM_Device *device, const char *access_point);
  int  (*access_point_removed)(E_NM_Device *device, const char *access_point);
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

typedef struct E_NMS_Internal E_NMS_Internal;
struct E_NMS_Internal
{
  E_NM_Internal *nmi;

  int  (*new_connection)(E_NMS *nms, const char *service_name, const char *connection);
  Ecore_List *handlers;

  void *data;
};

typedef struct E_NMS_Connection_Internal E_NMS_Connection_Internal;
struct E_NMS_Connection_Internal
{
  E_NMS_Connection conn;
  E_NM_Internal *nmi;

  void *data;
};

typedef struct E_NM_Active_Connection_Internal E_NM_Active_Connection_Internal;
struct E_NM_Active_Connection_Internal
{
  E_NM_Active_Connection conn;

  E_NM_Internal *nmi;
  char          *path;
};

typedef int (*Object_Cb)(void *data, void *reply);
#define OBJECT_CB(function) ((Object_Cb)function)

typedef struct Property Property;
typedef struct Property_Data Property_Data;
struct Property
{
  const char *name;
  const char *sig;
  void (*func)(Property_Data *data, DBusMessageIter *iter);
  size_t offset;
};
struct Property_Data
{
  E_NM_Internal   *nmi;
  char            *object;
  Object_Cb        cb_func;
  void            *reply;
  void            *data;

  Property        *property;
};

typedef struct Reply_Data Reply_Data;
struct Reply_Data
{
  void  *object;
  int  (*cb_func)(void *data, void *reply);
  void  *data;
  void  *reply;
};

void  property(void *data, DBusMessage *msg, DBusError *err);
void  parse_properties(void *data, Property *properties, DBusMessage *msg);

void *cb_nm_object_path_list(DBusMessage *msg, DBusError *err);
void  free_nm_object_path_list(void *data);
void *cb_nm_settings(DBusMessage *msg, DBusError *err);
void  free_nm_settings(void *data);

int   check_arg_type(DBusMessageIter *iter, char type);

void  property_data_free(Property_Data *data);

const char *ip4_address2str(unsigned int address);
#endif
