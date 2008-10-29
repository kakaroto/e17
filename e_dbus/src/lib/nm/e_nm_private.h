#ifndef E_NM_PRIVATE_H
#define E_NM_PRIVATE_H

#define E_NM_SERVICE "org.freedesktop.NetworkManager"
#define E_NM_PATH_NETWORK_MANAGER "/org/freedesktop/NetworkManager"
#define E_NM_INTERFACE_NETWORK_MANAGER "org.freedesktop.NetworkManager"
#define E_NM_INTERFACE_ACCESSPOINT "org.freedesktop.NetworkManager.AccessPoint"
#define E_NM_INTERFACE_DEVICE "org.freedesktop.NetworkManager.Device"
#define E_NM_INTERFACE_DEVICE_WIRELESS "org.freedesktop.NetworkManager.Device.Wireless"
#define E_NM_INTERFACE_DEVICE_WIRED "org.freedesktop.NetworkManager.Device.Wired"
#define E_NM_INTERFACE_IP4CONFIG "org.freedesktop.NetworkManager.IP4Config"

#define e_nm_manager_call_new(member) dbus_message_new_method_call(E_NM_SERVICE, E_NM_PATH_NETWORK_MANAGER, E_NM_INTERFACE_NETWORK_MANAGER, member)

#define e_nm_device_call_new(path, member) dbus_message_new_method_call(E_NM_SERVICE, path, E_NM_INTERFACE_DEVICE, member)

#define e_nm_network_call_new(member) dbus_message_new_method_call(E_NM_SERVICE, E_NM_PATH_NETWORK_MANAGER, E_NM_INTERFACE_NETWORK_MANAGER, member)

#define e_nm_device_properties_get(con, dev, prop, cb, data) e_dbus_properties_get(con, E_NM_SERVICE, dev, E_NM_INTERFACE_DEVICE, prop, (E_DBus_Method_Return_Cb) cb, data)

#define e_nm_device_properties_get_all(con, dev, cb, data) e_dbus_properties_get_all(con, E_NM_SERVICE, dev, E_NM_INTERFACE_DEVICE, (E_DBus_Method_Return_Cb) cb, data)

#define E_NM_CHECK_SIGNATURE(msg, err, sig)                  \
  if (dbus_error_is_set((err)))                              \
  {                                                          \
    printf("Error: %s - %s\n", (err)->name, (err)->message); \
    return NULL;                                             \
  }                                                          \
                                                             \
  if (!dbus_message_has_signature((msg), (sig)))             \
  {                                                          \
    dbus_set_error((err), DBUS_ERROR_INVALID_SIGNATURE, ""); \
    return NULL;                                             \
  }

struct E_NM_Context
{
  E_DBus_Connection *conn;

  E_NM_Cb_Manager_State_Changed cb_manager_state_changed;
  void *data_manager_state_changed;

  E_NM_Cb_Manager_Device_Added cb_manager_device_added;
  void *data_manager_device_added;

  E_NM_Cb_Manager_Device_Removed cb_manager_device_removed;
  void *data_manager_device_removed;
};

typedef struct E_NM_Data E_NM_Data;
struct E_NM_Data
{
  E_NM_Context         *ctx;
  E_DBus_Callback_Func  cb_func;
  void                 *data;
  void                 *reply;
};

int e_nm_get_from_nm(E_NM_Context *ctx, E_DBus_Callback_Func cb_func, void *data,
                     const char *method, int rettype);
int e_nm_get_from_device(E_NM_Context *ctx, const char *device,
                         E_DBus_Callback_Func cb_func, void *data,
                         const char *method, int rettype);

void *cb_nm_generic(DBusMessage *msg, DBusError *err);
void  free_nm_generic(void *data);
void *cb_nm_int32(DBusMessage *msg, DBusError *err);
void *cb_nm_uint32(DBusMessage *msg, DBusError *err);
void *cb_nm_object_path(DBusMessage *msg, DBusError *err);
void *cb_nm_boolean(DBusMessage *msg, DBusError *err);
void *cb_nm_object_path_list(DBusMessage *msg, DBusError *err);
void  free_nm_object_path_list(void *data);

int   nm_check_arg_type(DBusMessageIter *iter, char type);
#endif
