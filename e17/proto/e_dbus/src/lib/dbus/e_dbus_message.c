#include <stdlib.h>
#include "E_DBus.h"

typedef struct E_DBus_Pending_Call_Data E_DBus_Pending_Call_Data;
struct E_DBus_Pending_Call_Data
{
  int                     serial;

  E_DBus_Method_Return_Cb cb_return;
  E_DBus_Error_Cb         cb_error;

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

  msg = dbus_pending_call_steal_reply(pending);
  if (!msg)
  {
    if (data->cb_error)
      data->cb_error(data->data, "E.DBus.NoReply", "There was no reply to this method call.");
    return;
  }

  dbus_error_init(&err);
  if (dbus_set_error_from_message(&err, msg))
  {
    if (data->cb_error)
      data->cb_error(data->data, err.name, err.message);
  }
  else
  {
    if (data->cb_return)
      data->cb_return(data->data, msg);
  }

  dbus_message_unref(msg);
  dbus_pending_call_unref(pending);
}


/**
 * @brief Send a DBus message with callbacks
 * @param conn The DBus connection
 * @param msg  The message to send
 * @param cb_return A callback function for returns (only used if @a msg is a method-call)
 * @param cb_error  A callback function for errors
 * @param timeout   A timeout in milliseconds, after which a synthetic error will be generated
 * @return a DBusPendingCall that can be used to cancel the current call
 */
DBusPendingCall *
e_dbus_message_send(DBusConnection *conn, DBusMessage *msg, E_DBus_Method_Return_Cb cb_return, E_DBus_Error_Cb cb_error, int timeout, void *data)
{
  DBusPendingCall *pending;

  if (!dbus_connection_send_with_reply(conn, msg, &pending, timeout))
    return NULL;

  if (cb_return || cb_error)
  {
    E_DBus_Pending_Call_Data *pdata;

    pdata = calloc(1, sizeof(E_DBus_Pending_Call_Data));
    pdata->cb_return = cb_return;
    pdata->cb_error = cb_error;
    pdata->data = data;

    dbus_pending_call_set_notify(pending, cb_pending, pdata, free);
  }

  return pending;
}
