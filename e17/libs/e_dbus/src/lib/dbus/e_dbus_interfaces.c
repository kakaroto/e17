#include "E_DBus.h"
#include "e_dbus_private.h"

/**
 * This file contains wrappers around the standard interfaces that
 * objects on the bus should implement.
 */

/**
 * Ping the dbus peer
 *
 * @param conn the dbus connection
 * @param destination the bus name that the object is on
 * @param path the object path
 * @param cb_return a callback for a successful return
 * @param data data to pass to the callbacks
 */
EAPI void
e_dbus_peer_ping(E_DBus_Connection*conn, const char *destination, const char *path, E_DBus_Method_Return_Cb cb_return, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call(destination, path, "org.freedesktop.DBus.Peer", "Ping");
  e_dbus_message_send(conn, msg, cb_return, -1, data);
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
EAPI void
e_dbus_peer_get_machine_id(E_DBus_Connection*conn, const char *destination, const char *path, E_DBus_Method_Return_Cb cb_return, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call(destination, path, "org.freedesktop.DBus.Peer", "GetMachineId");
  e_dbus_message_send(conn, msg, cb_return, -1, data);
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
EAPI void
e_dbus_properties_get(E_DBus_Connection*conn, const char *destination, const char *path, const char *interface, const char *property, E_DBus_Method_Return_Cb cb_return, void *data)
{
  DBusMessage *msg;

  msg = dbus_message_new_method_call(destination, path, "org.freedesktop.DBus.Properties", "Get");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID);
  e_dbus_message_send(conn, msg, cb_return, -1, data);
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
EAPI void
e_dbus_properties_set(E_DBus_Connection*conn, const char *destination, const char *path, const char *interface, const char *property, int value_type, void *value, E_DBus_Method_Return_Cb cb_return, void *data)
{
  DBusMessage *msg;
  DBusMessageIter iter, sub;
  DBusError err;

  if (!dbus_type_is_basic(value_type)) 
  {
    if (cb_return)
    {
      dbus_error_init(&err);
      dbus_set_error(&err, "org.enlightenment.DBus.InvalidType", "Only basic types may be set using e_dbus_properties_set()");
      cb_return(data, NULL, &err);

    }
    return;
  }

  msg = dbus_message_new_method_call(destination, path, "org.freedesktop.DBus.Properties", "Set");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID);

  dbus_message_iter_init_append(msg, &iter);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, dbus_message_type_to_string(value_type), &sub);
  dbus_message_iter_append_basic(&sub, value_type, &value);
  dbus_message_iter_close_container(&iter, &sub);

  e_dbus_message_send(conn, msg, cb_return, -1, data);
}
