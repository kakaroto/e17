#include <stdlib.h>
#include "E_DBus.h"
#include "e_dbus_private.h"

typedef struct E_DBus_Pending_Call_Data E_DBus_Pending_Call_Data;
struct E_DBus_Pending_Call_Data
{
  E_DBus_Method_Return_Cb cb_return;
  void                   *data;
};

static void
cb_pending(DBusPendingCall *pending, void *user_data)
{
  DBusMessage *msg;
  DBusError err;
  E_DBus_Pending_Call_Data *data = user_data;

  if (!dbus_pending_call_get_completed(pending))
  {
    printf("NOT COMPLETED\n");
    return;
  }

  dbus_error_init(&err);
  msg = dbus_pending_call_steal_reply(pending);
  if (!msg)
  {
    if (data->cb_return)
    {
      dbus_set_error(&err, "org.enlightenment.DBus.NoReply", "There was no reply to this method call.");
      data->cb_return(data->data, NULL, &err);
      dbus_error_free(&err);
    }
    return;
  }

  if (dbus_set_error_from_message(&err, msg))
  {
    if (data->cb_return)
      data->cb_return(data->data, NULL, &err);
    dbus_error_free(&err);
  }
  else
  {
    if (data->cb_return)
      data->cb_return(data->data, msg, &err);
  }

  dbus_message_unref(msg);
  dbus_pending_call_unref(pending);
}


/**
 * @brief Send a DBus message with callbacks
 * @param conn The DBus connection
 * @param msg  The message to send
 * @param cb_return A callback function for returns (only used if @a msg is a method-call)
 * @param timeout   A timeout in milliseconds, after which a synthetic error will be generated
 * @return a DBusPendingCall that can be used to cancel the current call
 */
EAPI DBusPendingCall *
e_dbus_message_send(E_DBus_Connection *conn, DBusMessage *msg, E_DBus_Method_Return_Cb cb_return, int timeout, void *data)
{
  DBusPendingCall *pending;

  if (!dbus_connection_send_with_reply(conn->conn, msg, &pending, timeout))
    return NULL;

  if (cb_return && pending)
  {
    E_DBus_Pending_Call_Data *pdata;

    pdata = malloc(sizeof(E_DBus_Pending_Call_Data));
    pdata->cb_return = cb_return;
    pdata->data = data;

    if (!dbus_pending_call_set_notify(pending, cb_pending, pdata, free))
      free(pdata);
  }

  return pending;
}

static void
cb_method_call(void *data, DBusMessage *msg, DBusError *err)
{
  E_DBus_Callback *cb = data;
  void *method_return = NULL;
  DBusError new_err;
  if (!cb) return;

  dbus_error_init(&new_err);
  if (!dbus_error_is_set(err))
    method_return = e_dbus_callback_unmarshal(cb, msg, &new_err);
  else
    dbus_move_error(err, &new_err);

  e_dbus_callback_call(cb, method_return, &new_err);
  e_dbus_callback_return_free(cb, method_return);

  if (dbus_error_is_set(&new_err))
    dbus_error_free(&new_err);

  e_dbus_callback_free(cb);
}

EAPI DBusPendingCall *
e_dbus_method_call_send(E_DBus_Connection *conn, DBusMessage *msg, E_DBus_Unmarshal_Func unmarshal_func, E_DBus_Callback_Func cb_func, E_DBus_Free_Func free_func, int timeout, void *data)
{
  E_DBus_Callback *cb;
  cb = e_dbus_callback_new(cb_func, unmarshal_func, free_func, data);
  return e_dbus_message_send(conn, msg, cb_method_call, timeout, cb);
}
