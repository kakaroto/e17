#include "E_Nm.h"
#include "e_nm_private.h"

#include <string.h>

static void
cb_nms_settings(void *data, void *reply, DBusError *err)
{
  Reply_Data  *d;

  d = data;
  if (dbus_error_is_set(err))
  {
    printf("Error: %s - %s\n", err->name, err->message);
    d->cb_func(d->data, NULL);
    free(d);
    return;
  }

  d->cb_func(d->data, reply);
  free(d);
}

EAPI int
e_nms_connection_get(E_NMS *nms, E_NMS_Context context, const char *connection, int (*cb_func)(void *data, E_NMS_Connection *conn), void *data)
{
  E_NMS_Internal            *nmsi;
  E_NMS_Connection_Internal *conn;

  nmsi = (E_NMS_Internal *)nms;
  conn = calloc(1, sizeof(E_NMS_Connection_Internal));
  conn->nmi = nmsi->nmi;
  conn->path = strdup(connection);
  conn->context = context;
  (*cb_func)(data, (E_NMS_Connection *)conn);
  return 1;
}

EAPI void
e_nms_connection_free(E_NMS_Connection *connection)
{
  E_NMS_Connection_Internal *conn;
  if (!connection) return;
  conn = (E_NMS_Connection_Internal *)connection;

  if (conn->path) free(conn->path);
  free(conn);
}

EAPI void
e_nms_connection_dump(E_NMS_Connection *connection)
{
  E_NMS_Connection_Internal *conn;
  if (!connection) return;
  conn = (E_NMS_Connection_Internal *)connection;

  printf("E_NMS_Connection:\n");
  printf("context: ");
  switch (conn->context)
  {
    case E_NMS_CONTEXT_SYSTEM:
      printf("E_NMS_CONTEXT_SYSTEM\n");
      break;
    case E_NMS_CONTEXT_USER:
      printf("E_NMS_CONTEXT_USER\n");
      break;
  }
  printf("path   : %s\n", conn->path);
  printf("\n");
}

EAPI int
e_nms_connection_get_settings(E_NMS_Connection *connection, int (*cb_func)(void *data, Ecore_Hash *settings), void *data)
{
  DBusMessage *msg;
  Reply_Data   *d;
  E_NMS_Connection_Internal *conn;
  int ret;

  conn = (E_NMS_Connection_Internal *)connection;
  d = calloc(1, sizeof(Reply_Data));
  d->object = conn;
  d->cb_func = OBJECT_CB(cb_func);
  d->data = data;

  msg = e_nms_connection_call_new(conn->context, conn->path, "GetSettings");

  ret = e_dbus_method_call_send(conn->nmi->conn, msg, cb_nm_settings, cb_nms_settings, free_nm_settings, -1, d) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

