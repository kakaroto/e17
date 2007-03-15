#include "E_DBus.h"

void
e_dbus_get_name_owner(DBusConnection *conn, const char *name, E_DBus_Method_Return_Cb cb_return, E_DBus_Error_Cb cb_error, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "GetNameOwner");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
  e_dbus_message_send(conn, msg, cb_return, cb_error, -1, data);
}

void
e_dbus_list_names(DBusConnection *conn, E_DBus_Method_Return_Cb cb_return, E_DBus_Error_Cb cb_error, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames");
  e_dbus_message_send(conn, msg, cb_return, cb_error, -1, data);
}


void
e_dbus_list_activatable_names(DBusConnection *conn, E_DBus_Method_Return_Cb cb_return, E_DBus_Error_Cb cb_error, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListActivatableNames");
  e_dbus_message_send(conn, msg, cb_return, cb_error, -1, data);
}

void
e_dbus_name_has_owner(DBusConnection *conn, const char *name, E_DBus_Method_Return_Cb cb_return, E_DBus_Error_Cb cb_error, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "NameHasOwner");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
  e_dbus_message_send(conn, msg, cb_return, cb_error, -1, data);
}


void
e_dbus_start_service_by_name(DBusConnection *conn, const char *name, E_DBus_Method_Return_Cb cb_return, E_DBus_Error_Cb cb_error, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "StartServiceByName");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
  e_dbus_message_send(conn, msg, cb_return, cb_error, -1, data);
}
