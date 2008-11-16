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

EAPI E_NMS_Connection *
e_nms_connection_get(E_NMS *nms, const char *service_name, const char *connection)
{
  E_NMS_Internal            *nmsi;
  E_NMS_Connection_Internal *conn;

  nmsi = (E_NMS_Internal *)nms;
  conn = calloc(1, sizeof(E_NMS_Connection_Internal));
  conn->nmi = nmsi->nmi;
  conn->conn.path = strdup(connection);
  conn->conn.service_name = strdup(service_name);
  return &conn->conn;
}

EAPI void
e_nms_connection_free(E_NMS_Connection *conn)
{
  if (!conn) return;

  if (conn->service_name) free(conn->service_name);
  if (conn->path) free(conn->path);
  free(conn);
}

EAPI void
e_nms_connection_dump(E_NMS_Connection *conn)
{
  if (!conn) return;

  printf("E_NMS_Connection:\n");
  printf("service_name: %s\n", conn->service_name);
  printf("path        : %s\n", conn->path);
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

  msg = e_nms_connection_call_new(conn->conn.service_name, conn->conn.path, "GetSettings");

  ret = e_dbus_method_call_send(conn->nmi->conn, msg, cb_nm_settings, cb_nms_settings, free_nm_settings, -1, d) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

