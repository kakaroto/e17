#include "E_Nm.h"
#include "e_nm_private.h"

EAPI E_NM_Context *
e_nm_new(void)
{
  E_NM_Context *ctx;

  ctx = calloc(1, sizeof(E_NM_Context));
  if (!ctx) return NULL;

  ctx->conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
  if (!ctx->conn)
    goto error;

  return ctx;

error:
  free(ctx);
  return NULL;

}

EAPI void
e_nm_free(E_NM_Context *ctx)
{
  e_dbus_connection_close(ctx->conn);
  free(ctx);
}

static void
cb_signal_manager_state_changed(void *data, DBusMessage *msg)
{
  E_NM_Context *ctx;
  dbus_uint32_t state;
  DBusError err;
  if (!msg || !data) return;

  ctx = data;
  dbus_error_init(&err);
  dbus_message_get_args(msg, &err, DBUS_TYPE_UINT32, &state, DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&err))
  {
    printf("Error: %s - %s\n", err.name, err.message);
    return;
  }

  if (ctx->cb_manager_state_changed)
    ctx->cb_manager_state_changed(ctx->data_manager_state_changed, (int)state);
}

static void
cb_signal_manager_device_added(void *data, DBusMessage *msg)
{
  E_NM_Context *ctx;
  const char *device;
  DBusError err;
  if (!msg || !data) return;

  ctx = data;
  dbus_error_init(&err);
  dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &device, DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&err))
  {
    printf("Error: %s - %s\n", err.name, err.message);
    return;
  }

  if (ctx->cb_manager_device_added)
    ctx->cb_manager_device_added(ctx->data_manager_device_added, device);
}

static void
cb_signal_manager_device_removed(void *data, DBusMessage *msg)
{
  E_NM_Context *ctx;
  const char *device;
  DBusError err;
  if (!msg || !data) return;

  ctx = data;
  dbus_error_init(&err);
  dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &device, DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&err))
  {
    printf("Error: %s - %s\n", err.name, err.message);
    return;
  }

  if (ctx->cb_manager_device_removed)
    ctx->cb_manager_device_removed(ctx->data_manager_device_removed, device);
}


EAPI void
e_nm_callback_manager_state_changed_set(E_NM_Context *ctx, E_NM_Cb_Manager_State_Changed cb_func, void *user_data)
{
  ctx->cb_manager_state_changed = cb_func;
  ctx->data_manager_state_changed = user_data;

  e_dbus_signal_handler_add(ctx->conn, E_NM_SERVICE, E_NM_PATH_NETWORK_MANAGER, E_NM_INTERFACE_NETWORK_MANAGER, "StateChanged", cb_signal_manager_state_changed, ctx);
}

EAPI void
e_nm_callback_manager_device_added_set(E_NM_Context *ctx, E_NM_Cb_Manager_Device_Added cb_func, void *user_data)
{
  ctx->cb_manager_device_added = cb_func;
  ctx->data_manager_device_added = user_data;

  e_dbus_signal_handler_add(ctx->conn, E_NM_SERVICE, E_NM_PATH_NETWORK_MANAGER, E_NM_INTERFACE_NETWORK_MANAGER, "DeviceAdded", cb_signal_manager_device_added, ctx);
}

EAPI void
e_nm_callback_manager_device_removed_set(E_NM_Context *ctx, E_NM_Cb_Manager_Device_Removed cb_func, void *user_data)
{
  ctx->cb_manager_device_removed = cb_func;
  ctx->data_manager_device_removed = user_data;

  e_dbus_signal_handler_add(ctx->conn, E_NM_SERVICE, E_NM_PATH_NETWORK_MANAGER, E_NM_INTERFACE_NETWORK_MANAGER, "DeviceRemoved", cb_signal_manager_device_removed, ctx);
}
