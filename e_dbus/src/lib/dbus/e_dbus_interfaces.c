#include "E_DBus.h"
#include "e_dbus_private.h"

/**
 * This file contains wrappers around the standard interfaces that
 * objects on the bus should implement.
 */

static inline DBusPendingCall *
_dbus_peer_call(E_DBus_Connection *conn, const char *method_name, const char *destination, const char *path, E_DBus_Method_Return_Cb cb_return, const void *data)
{
  DBusMessage *msg;
  DBusPendingCall *ret;

  msg = dbus_message_new_method_call
    (destination, path, "org.freedesktop.DBus.Peer", method_name);
  if (!msg)
    {
       fprintf(stderr,
	       "ERROR: failed to create message for method call: %s() at "
	       "\"%s\" at \"%s\"\n",
	       method_name, destination, path);
       return NULL;
    }

  ret = e_dbus_message_send(conn, msg, cb_return, -1, (void *)data);
  dbus_message_unref(msg);

  if (!ret)
    fprintf(stderr, "ERROR: could not %s() \"%s\" at \"%s\".\n",
	    method_name, destination, path);

  return ret;
}

/**
 * Ping the dbus peer
 *
 * @param conn the dbus connection
 * @param destination the bus name that the object is on
 * @param path the object path
 * @param cb_return a callback for a successful return
 * @param data data to pass to the callbacks
 */
EAPI DBusPendingCall *
e_dbus_peer_ping(E_DBus_Connection *conn, const char *destination, const char *path, E_DBus_Method_Return_Cb cb_return, const void *data)
{
   return _dbus_peer_call(conn, "Ping", destination, path, cb_return, data);
}

/**
 * Get the UUID of the peer
 *
 * @param conn the dbus connection
 * @param destination the bus name that the object is on
 * @param path the object path
 * @param cb_return a callback for a successful return
 * @param data data to pass to the callbacks
 */
EAPI DBusPendingCall *
e_dbus_peer_get_machine_id(E_DBus_Connection *conn, const char *destination, const char *path, E_DBus_Method_Return_Cb cb_return, const void *data)
{
   return _dbus_peer_call(conn, "GetMachineId", destination, path, cb_return, data);
}

static inline DBusMessage *
_dbus_message_property_method_call(E_DBus_Connection *conn, const char *method_name, const char *destination, const char *path, const char *interface, const char *property)
{
  DBusMessage *msg;

  if (!conn)
    {
       fprintf(stderr, "ERROR: no connection for call of %s\n", method_name);
       return NULL;
    }

  msg = dbus_message_new_method_call
    (destination, path, "org.freedesktop.DBus.Properties", method_name);
  if (!msg)
    {
       fprintf(stderr,
	       "ERROR: failed to create message for method call: %s() at "
	       "\"%s\" at \"%s\"\n",
	       method_name, destination, path);
       return NULL;
    }

  if (property)
  {
    dbus_message_append_args(msg, DBUS_TYPE_STRING, &interface,
			     DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID);
  }
  else
  {
    dbus_message_append_args(msg, DBUS_TYPE_STRING, &interface, DBUS_TYPE_INVALID);
  }

  return msg;
}

EAPI DBusPendingCall *
e_dbus_properties_get_all(E_DBus_Connection *conn, const char *destination, const char *path, const char *interface, E_DBus_Method_Return_Cb cb_return, const void *data)
{
  DBusMessage *msg;
  DBusPendingCall *ret;

  msg = _dbus_message_property_method_call
    (conn, "GetAll", destination, path, interface, NULL);
  if (!msg)
    return NULL;
  ret = e_dbus_message_send(conn, msg, cb_return, -1, (void *)data);
  dbus_message_unref(msg);

  if (!ret)
    fprintf(stderr, "ERROR: failed to call GetAll() at \"%s\" at \"%s\"\n",
	    destination, path);

  return ret;
}

/**
 * Get the value of a property on an object
 *
 * @param conn the dbus connection
 * @param destination the bus name that the object is on
 * @param path the object path
 * @param interface the interface name of the property
 * @param property the name of the property
 * @param cb_return a callback for a successful return
 * @param data data to pass to the callbacks
 */
EAPI DBusPendingCall *
e_dbus_properties_get(E_DBus_Connection *conn, const char *destination, const char *path, const char *interface, const char *property, E_DBus_Method_Return_Cb cb_return, const void *data)
{
  DBusMessage *msg;
  DBusPendingCall *ret;

  msg = _dbus_message_property_method_call
    (conn, "Get", destination, path, interface, property);
  if (!msg)
    return NULL;
  ret = e_dbus_message_send(conn, msg, cb_return, -1, (void *)data);
  dbus_message_unref(msg);

  if (!ret)
    fprintf(stderr, "ERROR: failed to call Get() at \"%s\" at \"%s\"\n",
	    destination, path);

  return ret;
}

/**
 * Set the value of a property on an object
 *
 * @param conn the dbus connection
 * @param destination the bus name that the object is on
 * @param path the object path
 * @param interface the interface name of the property
 * @param property the name of the property
 * @param value_type the type of the property's value
 * @param value a pointer to the value
 * @param cb_return a callback for a successful return
 * @param data data to pass to the callbacks
 */
EAPI DBusPendingCall *
e_dbus_properties_set(E_DBus_Connection *conn, const char *destination, const char *path, const char *interface, const char *property, int value_type, const void *value, E_DBus_Method_Return_Cb cb_return, const void *data)
{
  DBusMessage *msg;
  DBusMessageIter iter, sub;
  DBusError err;
  DBusPendingCall *ret;
  char sig[2];

  if (!dbus_type_is_basic(value_type))
  {
    if (cb_return)
    {
      dbus_error_init(&err);
      dbus_set_error(&err, "org.enlightenment.DBus.InvalidType", "Only basic types may be set using e_dbus_properties_set()");
      cb_return((void *)data, NULL, &err);

    }
    return NULL;
  }

  msg = _dbus_message_property_method_call
    (conn, "Set", destination, path, interface, property);
  if (!msg)
    return NULL;

  dbus_message_iter_init_append(msg, &iter);
  sig[0] = value_type;
  sig[1] = 0;
  dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, sig, &sub);
  dbus_message_iter_append_basic(&sub, value_type, value);
  dbus_message_iter_close_container(&iter, &sub);

  ret = e_dbus_message_send(conn, msg, cb_return, -1, (void *)data);
  dbus_message_unref(msg);

  if (!ret)
    fprintf(stderr, "ERROR: failed to call Set() at \"%s\" at \"%s\"\n",
	    destination, path);

  return ret;
}
