#include "E_DBus.h"
#include "e_dbus_private.h"

EAPI void
e_dbus_request_name(E_DBus_Connection *conn, const char *name, unsigned int flags, E_DBus_Method_Return_Cb cb_return, void *data)
{
  DBusMessage *msg;
  dbus_uint32_t u_flags;

  u_flags = flags;

  msg = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "RequestName");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_UINT32, &u_flags, DBUS_TYPE_INVALID);
  e_dbus_message_send(conn, msg, cb_return, -1, data);
}

EAPI void
e_dbus_release_name(E_DBus_Connection *conn, const char *name, E_DBus_Method_Return_Cb cb_return, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ReleaseName");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
  e_dbus_message_send(conn, msg, cb_return, -1, data);
}


EAPI void
e_dbus_get_name_owner(E_DBus_Connection *conn, const char *name, E_DBus_Method_Return_Cb cb_return, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "GetNameOwner");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
  e_dbus_message_send(conn, msg, cb_return, -1, data);
}

EAPI void
e_dbus_list_names(E_DBus_Connection *conn, E_DBus_Method_Return_Cb cb_return, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames");
  e_dbus_message_send(conn, msg, cb_return, -1, data);
}


EAPI void
e_dbus_list_activatable_names(E_DBus_Connection *conn, E_DBus_Method_Return_Cb cb_return, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListActivatableNames");
  e_dbus_message_send(conn, msg, cb_return, -1, data);
}

EAPI void
e_dbus_name_has_owner(E_DBus_Connection *conn, const char *name, E_DBus_Method_Return_Cb cb_return, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "NameHasOwner");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
  e_dbus_message_send(conn, msg, cb_return, -1, data);
}


EAPI void
e_dbus_start_service_by_name(E_DBus_Connection *conn, const char *name, E_DBus_Method_Return_Cb cb_return, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "StartServiceByName");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
  e_dbus_message_send(conn, msg, cb_return, -1, data);
}
