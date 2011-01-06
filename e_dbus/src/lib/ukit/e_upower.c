#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <E_Ukit.h>
#include "e_ukit_private.h"

#define E_UKIT_BUS E_UPOWER_BUS
#define E_UKIT_PATH E_UPOWER_PATH
#define E_UKIT_INTERFACE E_UPOWER_INTERFACE

const char *e_upower_iface="org.freedesktop.UPower.Device";

#if 0
static void cb_device_get_property(void *data, DBusMessage *msg, DBusError *err);
static void cb_device_get_all_properties(void *data, DBusMessage *msg, DBusError *err);
static void cb_device_query_capability(void *data, DBusMessage *msg, DBusError *err);
#endif

/* Properties.Get */
EAPI DBusPendingCall *
e_upower_get_property(E_DBus_Connection *conn, const char *udi, const char *property, E_DBus_Callback_Func cb_func, void *data)
{
   DBusMessage *msg;
   DBusPendingCall *ret;

   EINA_SAFETY_ON_NULL_RETURN_VAL(udi, NULL);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!udi[0], NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(conn, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(property, NULL);

   msg = e_ukit_property_call_new(udi, "Get");
   dbus_message_append_args(msg, DBUS_TYPE_STRING, &e_upower_iface, DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID);
   ret = e_dbus_method_call_send(conn, msg, unmarshal_property, cb_func, free_property, -1, data);
   dbus_message_unref(msg);
   return ret;
}

/* Properties.GetAll */
EAPI DBusPendingCall *
e_upower_get_all_properties(E_DBus_Connection *conn, const char *udi, E_DBus_Callback_Func cb_func, void *data)
{
   DBusMessage *msg;
   DBusPendingCall *ret;

   EINA_SAFETY_ON_NULL_RETURN_VAL(udi, NULL);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!udi[0], NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(conn, NULL);

   msg = e_ukit_property_call_new(udi, "GetAll");
   dbus_message_append_args(msg, DBUS_TYPE_STRING, &e_upower_iface, DBUS_TYPE_INVALID);
   ret = e_dbus_method_call_send(conn, msg, unmarshal_device_get_all_properties, cb_func, free_device_get_all_properties, -1, data);
   dbus_message_unref(msg);
   return ret;
}


EAPI DBusPendingCall *
e_upower_hibernate(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data)
{
   DBusMessage *msg;
   DBusPendingCall *ret;

   EINA_SAFETY_ON_NULL_RETURN_VAL(conn, NULL);

   msg = e_ukit_call_new(E_UKIT_PATH, "Hibernate");

   ret = e_dbus_method_call_send(conn, msg, NULL, cb_func, NULL, -1, data);
   dbus_message_unref(msg);
   return ret;
}

EAPI DBusPendingCall *
e_upower_suspend(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data)
{
   DBusMessage *msg;
   DBusPendingCall *ret;

   EINA_SAFETY_ON_NULL_RETURN_VAL(conn, NULL);

   msg = e_ukit_call_new(E_UKIT_PATH, "Suspend");

   ret = e_dbus_method_call_send(conn, msg, NULL, cb_func, NULL, -1, data);
   dbus_message_unref(msg);
   return ret;
}

EAPI DBusPendingCall *
e_upower_hibernate_allowed(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data)
{
   DBusMessage *msg;
   DBusPendingCall *ret;

   EINA_SAFETY_ON_NULL_RETURN_VAL(conn, NULL);

   msg = e_ukit_call_new(E_UKIT_PATH, "HibernateAllowed");

   ret = e_dbus_method_call_send(conn, msg, unmarshal_property, cb_func, free_property, -1, data);
   dbus_message_unref(msg);
   return ret;
}

EAPI DBusPendingCall *
e_upower_suspend_allowed(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data)
{
   DBusMessage *msg;
   DBusPendingCall *ret;

   EINA_SAFETY_ON_NULL_RETURN_VAL(conn, NULL);
   
   msg = e_ukit_call_new(E_UKIT_PATH, "SuspendAllowed");

   ret = e_dbus_method_call_send(conn, msg, unmarshal_property, cb_func, free_property, -1, data);
   dbus_message_unref(msg);
   return ret;
}

/* EnumerateDevices */
EAPI DBusPendingCall *
e_upower_get_all_devices(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data)
{
   DBusMessage *msg;
   DBusPendingCall *ret;

   EINA_SAFETY_ON_NULL_RETURN_VAL(conn, NULL);

   msg = e_ukit_call_new(E_UKIT_PATH, "EnumerateDevices");
   ret = e_dbus_method_call_send(conn, msg, unmarshal_string_list, cb_func, free_string_list, -1, data);
   dbus_message_unref(msg);
   return ret;
}
