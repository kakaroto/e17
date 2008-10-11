#include "E_DBus.h"
#include "e_dbus_private.h"

static inline DBusMessage *
_dbus_message_method_call(const char *method_name)
{
   DBusMessage *msg;

   msg = dbus_message_new_method_call
     ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus",
      method_name);
   if (!msg)
     fprintf(stderr, "ERROR: failed to create message for method call: %s\n",
	     method_name);
   return msg;
}

static inline DBusPendingCall *
_dbus_call__void(E_DBus_Connection *conn, const const char *method_name, E_DBus_Method_Return_Cb cb_return, const void *data)
{
  DBusMessage *msg;
  DBusPendingCall *ret;

  if (!conn)
    {
       fprintf(stderr, "ERROR: no connection for call of %s\n", method_name);
       return NULL;
    }

  msg = _dbus_message_method_call(method_name);
  if (!msg)
    return NULL;
  ret = e_dbus_message_send(conn, msg, cb_return, -1, (void *)data);
  dbus_message_unref(msg);

  if (!ret)
    fprintf(stderr, "ERROR: failed to call %s()\n", method_name);

  return ret;
}

static inline DBusPendingCall *
_dbus_call__str(E_DBus_Connection *conn, const const char *method_name, const char *str, E_DBus_Method_Return_Cb cb_return, const void *data)
{
  DBusMessage *msg;
  DBusPendingCall *ret;

  if (!conn)
    {
       fprintf(stderr, "ERROR: no connection for call of %s\n", method_name);
       return NULL;
    }

  msg = _dbus_message_method_call(method_name);
  if (!msg)
    return NULL;
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID);
  ret = e_dbus_message_send(conn, msg, cb_return, -1, (void *)data);
  dbus_message_unref(msg);

  if (!ret)
    fprintf(stderr, "ERROR: failed to call %s(\"%s\")\n", method_name, str);

  return ret;
}

EAPI DBusPendingCall *
e_dbus_request_name(E_DBus_Connection *conn, const char *name, unsigned int flags, E_DBus_Method_Return_Cb cb_return, const void *data)
{
   DBusPendingCall *ret;
   DBusMessage *msg;
   dbus_uint32_t u_flags;

  if (!conn)
    {
       fputs("ERROR: no connection for call of RequestName\n", stderr);
       return NULL;
    }

   u_flags = flags;

   msg = _dbus_message_method_call("RequestName");
   if (!msg)
     return NULL;
   dbus_message_append_args(msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_UINT32, &u_flags, DBUS_TYPE_INVALID);
   ret = e_dbus_message_send(conn, msg, cb_return, -1, (void *)data);
   dbus_message_unref(msg);
   return ret;
}

EAPI DBusPendingCall *
e_dbus_release_name(E_DBus_Connection *conn, const char *name, E_DBus_Method_Return_Cb cb_return, const void *data)
{
   return _dbus_call__str(conn, "ReleaseName", name, cb_return, data);
}


EAPI DBusPendingCall *
e_dbus_get_name_owner(E_DBus_Connection *conn, const char *name, E_DBus_Method_Return_Cb cb_return, const void *data)
{
   return _dbus_call__str(conn, "GetNameOwner", name, cb_return, data);
}

EAPI DBusPendingCall *
e_dbus_list_names(E_DBus_Connection *conn, E_DBus_Method_Return_Cb cb_return, const void *data)
{
   return _dbus_call__void(conn, "ListNames", cb_return, data);
}


EAPI DBusPendingCall *
e_dbus_list_activatable_names(E_DBus_Connection *conn, E_DBus_Method_Return_Cb cb_return, const void *data)
{
   return _dbus_call__void(conn, "ListActivatableNames", cb_return, data);
}

EAPI DBusPendingCall *
e_dbus_name_has_owner(E_DBus_Connection *conn, const char *name, E_DBus_Method_Return_Cb cb_return, const void *data)
{
   return _dbus_call__str(conn, "NameHasOwner", name, cb_return, data);
}


EAPI DBusPendingCall *
e_dbus_start_service_by_name(E_DBus_Connection *conn, const char *name, E_DBus_Method_Return_Cb cb_return, const void *data)
{
   return _dbus_call__str(conn, "StartServiceByName", name, cb_return, data);
}
