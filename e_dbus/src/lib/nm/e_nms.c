#include "E_Nm.h"
#include "e_nm_private.h"

static int
cb_nms_connection(void *data, E_NMS_Connection *conn)
{
  Reply_Data  *d;
  Ecore_List  *list;

  d = data;
  list = d->object;
  if (conn)
    ecore_list_append(list, conn);
  ecore_list_first_remove(list);

  ecore_list_first_goto(list);
  if (ecore_list_empty_is(list))
  {
    d->cb_func(d->data, NULL);
    ecore_list_destroy(list);
    free(d);
  }
  else if (ecore_list_current(list) != (void *)-1)
  {
    d->cb_func(d->data, list);
    free(d);
  }
  return 1;
}

static void
cb_nms_connections(void *data, void *reply, DBusError *err)
{
  Reply_Data  *d;
  E_NMS *nms;
  Ecore_List *connections;
  Ecore_List *list;
  const char *conn;

  d = data;
  nms = d->object;
  if (dbus_error_is_set(err))
  {
    d->cb_func(d->data, NULL);
    free(d);
    return;
  }
  connections = reply;
  ecore_list_first_goto(connections);
  list = ecore_list_new();
  ecore_list_free_cb_set(list, ECORE_FREE_CB(e_nms_connection_free));
  d->object = list;
  while ((conn = ecore_list_next(connections)))
  {
    ecore_list_append(list, (void *)-1);
    e_nms_connection_get(nms, conn, cb_nms_connection, d);
  }
}

EAPI int
e_nms_get(E_NM *nm, int (*cb_func)(void *data, E_NMS *nms), void *data)
{
  E_NMS_Internal *nmsi;

  nmsi = calloc(1, sizeof(E_NMS_Internal));
  nmsi->nmi = (E_NM_Internal *)nm;
  (*cb_func)(data, (E_NMS *)nmsi);
  return 1;
}

EAPI void
e_nms_free(E_NMS *nms)
{
  if (!nms) return;
  free(nms);
}

EAPI void
e_nms_dump(E_NMS *nms)
{
  if (!nms) return;
  printf("E_NMS:\n");
  printf("\n");
}

EAPI int
e_nms_list_connections(E_NMS *nms, int (*cb_func)(void *data, Ecore_List *list), void *data)
{
  DBusMessage *msg;
  Reply_Data   *d;
  E_NMS_Internal *nmsi;
  int ret;

  nmsi = (E_NMS_Internal *)nms;
  d = calloc(1, sizeof(Reply_Data));
  d->cb_func = OBJECT_CB(cb_func);
  d->data = data;
  d->object = nmsi;

  msg = e_nms_call_new("ListConnections");

  ret = e_dbus_method_call_send(nmsi->nmi->conn, msg, cb_nm_object_path_list, cb_nms_connections, free_nm_object_path_list, -1, d) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

