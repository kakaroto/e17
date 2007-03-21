#ifndef E_NM_PRIVATE_H
#define E_NM_PRIVATE_H

#define E_NM_SERVICE "org.freedesktop.NetworkManager"
#define E_NM_INTERFACE_NETWORK_MANAGER "org.freedesktop.NetworkManager"
#define E_NM_PATH_NETWORK_MANAGER "/org/freedesktop/NetworkManager"
#define E_NM_INTERFACE_DEVICE "org.freedesktop.NetworkManager.Device"
#define E_NM_INTERFACE_DEVICE_WIRED "org.freedesktop.NetworkManager.Device.Wired"
#define E_NM_INTERFACE_DEVICE_WIRELESS "org.freedesktop.NetworkManager.Device.Wireless"

typedef struct E_NM_Callback E_NM_Callback;
struct E_NM_Callback
{
  E_NM_Callback_Func func;
  void *user_data;
};

struct E_NM_Context
{
  DBusConnection *conn;

  E_NM_Cb_Manager_State_Change cb_manager_state_change;
  void *data_manager_state_change;

  E_NM_Cb_Manager_Device_Added cb_manager_device_added;
  void *data_manager_device_added;

  E_NM_Cb_Manager_Device_Removed cb_manager_device_removed;
  void *data_manager_device_removed;
};



E_NM_Callback * e_nm_callback_new(E_NM_Callback_Func cb_func, void *user_data);
void e_nm_callback_free(E_NM_Callback *callback);

void cb_nm_generic(void *data, DBusMessage *msg, DBusError *err);
void cb_nm_string_list(void *data, DBusMessage *msg, DBusError *err);
#endif
