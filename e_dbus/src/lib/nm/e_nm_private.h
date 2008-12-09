#ifndef E_NM_PRIVATE_H
#define E_NM_PRIVATE_H

#define E_NM_PATH "/org/freedesktop/NetworkManager"
#define E_NM_SERVICE "org.freedesktop.NetworkManager"
#define E_NM_INTERFACE "org.freedesktop.NetworkManager"
#define E_NM_INTERFACE_ACCESSPOINT "org.freedesktop.NetworkManager.AccessPoint"
#define E_NM_INTERFACE_DEVICE "org.freedesktop.NetworkManager.Device"
#define E_NM_INTERFACE_DEVICE_WIRELESS "org.freedesktop.NetworkManager.Device.Wireless"
#define E_NM_INTERFACE_DEVICE_WIRED "org.freedesktop.NetworkManager.Device.Wired"
#define E_NM_INTERFACE_IP4CONFIG "org.freedesktop.NetworkManager.IP4Config"
#define E_NM_INTERFACE_CONNECTION_ACTIVE "org.freedesktop.NetworkManager.Connection.Active"
#define E_NMS_PATH "/org/freedesktop/NetworkManagerSettings"
#define E_NMS_SERVICE_SYSTEM "org.freedesktop.NetworkManagerSystemSettings"
#define E_NMS_SERVICE_USER "org.freedesktop.NetworkManagerUserSettings"
#define E_NMS_INTERFACE "org.freedesktop.NetworkManagerSettings"
#define E_NMS_INTERFACE_SYSTEM "org.freedesktop.NetworkManagerSettings.System"
#define E_NMS_INTERFACE_CONNECTION "org.freedesktop.NetworkManagerSettings.Connection"
#define E_NMS_INTERFACE_CONNECTION_SECRETS "org.freedesktop.NetworkManagerSettings.Connection.Secrets"

#define e_nm_call_new(member) dbus_message_new_method_call(E_NM_SERVICE, E_NM_PATH, E_NM_INTERFACE, member)
#define e_nm_device_wireless_call_new(device, member) dbus_message_new_method_call(E_NM_SERVICE, device, E_NM_INTERFACE_DEVICE_WIRELESS, member)
#define e_nms_call_new(service, member) dbus_message_new_method_call(service, E_NMS_PATH, E_NMS_INTERFACE, member)
#define e_nms_system_call_new(service, member) dbus_message_new_method_call(service, E_NMS_PATH, E_NMS_INTERFACE_SYSTEM, member)
#define e_nms_connection_call_new(service, conn, member) dbus_message_new_method_call(service, conn, E_NMS_INTERFACE_CONNECTION, member)
#define e_nms_connection_secrets_call_new(service, conn, member) dbus_message_new_method_call(service, conn, E_NMS_INTERFACE_CONNECTION_SECRETS, member)

#define e_nm_properties_set(con, prop, type, value, cb, data) e_dbus_properties_set(con, E_NM_SERVICE, E_NM_PATH, E_NM_INTERFACE, prop, type, value, (E_DBus_Method_Return_Cb) cb, data)

#define e_nm_signal_handler_add(con, sig, cb, data) e_dbus_signal_handler_add(con, E_NM_SERVICE, E_NM_PATH, E_NM_INTERFACE, sig, cb, data)
#define e_nm_access_point_signal_handler_add(con, dev, sig, cb, data) e_dbus_signal_handler_add(con, E_NM_SERVICE, dev, E_NM_INTERFACE_ACCESSPOINT, sig, cb, data)
#define e_nm_device_signal_handler_add(con, dev, sig, cb, data) e_dbus_signal_handler_add(con, E_NM_SERVICE, dev, E_NM_INTERFACE_DEVICE, sig, cb, data)
#define e_nm_device_wired_signal_handler_add(con, dev, sig, cb, data) e_dbus_signal_handler_add(con, E_NM_SERVICE, dev, E_NM_INTERFACE_DEVICE_WIRED, sig, cb, data)
#define e_nm_device_wireless_signal_handler_add(con, dev, sig, cb, data) e_dbus_signal_handler_add(con, E_NM_SERVICE, dev, E_NM_INTERFACE_DEVICE_WIRELESS, sig, cb, data)

#define e_nms_signal_handler_add(con, service, sig, cb, data) e_dbus_signal_handler_add(con, service, E_NMS_PATH, E_NMS_INTERFACE, sig, cb, data)
#define e_nms_system_signal_handler_add(con, service, sig, cb, data) e_dbus_signal_handler_add(con, service, E_NMS_PATH, E_NMS_INTERFACE_SYSTEM, sig, cb, data)
#define e_nms_connection_signal_handler_add(con, service, dev, sig, cb, data) e_dbus_signal_handler_add(con, service, dev, E_NMS_INTERFACE_CONNECTION, sig, cb, data)

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
  E_NMS nms;

  E_NM_Internal *nmi;

  int  (*new_connection)(E_NMS *nms, const char *service_name, const char *connection);
  int  (*properties_changed)(E_NMS *nms);
  Ecore_List *handlers;

  void *data;
};

typedef struct E_NMS_Connection_Internal E_NMS_Connection_Internal;
struct E_NMS_Connection_Internal
{
  E_NMS_Connection conn;
  E_NM_Internal *nmi;

  int  (*updated)(E_NMS_Connection *conn, Ecore_Hash *settings);
/* TODO:  int  (*removed)(E_NMS_Connection *conn); */
  Ecore_List *handlers;

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
  const char      *service;
  char            *object;
  const char      *interface;
  Object_Cb        cb_func;
  void            *reply;
  void            *data;

  const Property  *property;
};

typedef struct Reply_Data Reply_Data;
struct Reply_Data
{
  void  *object;
  int  (*cb_func)(void *data, void *reply);
  void  *data;
  void  *reply;
};

int   property_get(E_DBus_Connection *conn, Property_Data *data);
void  property(void *data, DBusMessage *msg, DBusError *err);
void  parse_properties(void *data, const Property *properties, DBusMessage *msg);

void *cb_nm_object_path_list(DBusMessage *msg, DBusError *err);
void  free_nm_object_path_list(void *data);
Ecore_Hash *parse_settings(DBusMessage *msg);

int   check_arg_type(DBusMessageIter *iter, char type);

void  property_data_free(Property_Data *data);

const char *ip4_address2str(unsigned int address);
#endif
