#ifndef E_DBUS_H
#define E_DBUS_H

#define DBUS_API_SUBJECT_TO_CHANGE

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <dbus/dbus.h>
#include <Ecore.h>

#ifdef EAPI
#undef EAPI
#endif
#ifdef _MSC_VER
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

   EAPI extern int E_DBUS_EVENT_SIGNAL;

   typedef struct E_DBus_Connection E_DBus_Connection;
   typedef struct E_DBus_Object E_DBus_Object;
   typedef struct E_DBus_Interface E_DBus_Interface;
   typedef struct E_DBus_Signal_Handler E_DBus_Signal_Handler;

   typedef DBusMessage *(* E_DBus_Method_Cb)(E_DBus_Object *obj, DBusMessage *message);
   typedef void (*E_DBus_Method_Return_Cb) (void *data, DBusMessage *msg, DBusError *error);
   typedef void (*E_DBus_Signal_Cb) (void *data, DBusMessage *msg);

   typedef void (*E_DBus_Object_Property_Get_Cb) (E_DBus_Object *obj, const char *property, int *type, void **value);
   typedef int  (*E_DBus_Object_Property_Set_Cb) (E_DBus_Object *obj, const char *property, int type, void *value);

/**
 * A callback function for a DBus call
 * @param user_data the data passed in to the method call
 * @param event_data a struct containing the return data.
 */
   typedef void (*E_DBus_Callback_Func) (void *user_data, void *method_return, DBusError *error);
   typedef void *(*E_DBus_Unmarshal_Func) (DBusMessage *msg, DBusError *err);
   typedef void (*E_DBus_Free_Func) (void *data);

   typedef struct E_DBus_Callback E_DBus_Callback;


   EAPI int e_dbus_init(void);
   EAPI int e_dbus_shutdown(void);

/* setting up the connection */

   EAPI E_DBus_Connection *e_dbus_bus_get(DBusBusType type);

   EAPI void e_dbus_connection_ref(E_DBus_Connection *conn);

   EAPI E_DBus_Connection *e_dbus_connection_setup(DBusConnection *conn);
   EAPI void e_dbus_connection_close(E_DBus_Connection *conn);

/* receiving method calls */
   EAPI E_DBus_Interface *e_dbus_interface_new(const char *interface);
   EAPI void e_dbus_interface_ref(E_DBus_Interface *iface);
   EAPI void e_dbus_interface_unref(E_DBus_Interface *iface);
   EAPI void e_dbus_object_interface_attach(E_DBus_Object *obj, E_DBus_Interface *iface);
   EAPI void e_dbus_object_interface_detach(E_DBus_Object *obj, E_DBus_Interface *iface);
   EAPI int e_dbus_interface_method_add(E_DBus_Interface *iface, const char *member, const char *signature, const char *reply_signature, E_DBus_Method_Cb func);

   EAPI int e_dbus_interface_signal_add(E_DBus_Interface *iface, const char *name, const char *signature);

   EAPI E_DBus_Object *e_dbus_object_add(E_DBus_Connection *conn, const char *object_path, void *data);
   EAPI void e_dbus_object_free(E_DBus_Object *obj);
   EAPI void *e_dbus_object_data_get(E_DBus_Object *obj);

   EAPI void e_dbus_object_property_get_cb_set(E_DBus_Object *obj, E_DBus_Object_Property_Get_Cb func);
   EAPI void e_dbus_object_property_set_cb_set(E_DBus_Object *obj, E_DBus_Object_Property_Set_Cb func);


/* sending method calls */


   EAPI DBusPendingCall *e_dbus_message_send(E_DBus_Connection *conn, DBusMessage *msg, E_DBus_Method_Return_Cb cb_return, int timeout, void *data);

   EAPI DBusPendingCall *e_dbus_method_call_send(E_DBus_Connection *conn, DBusMessage *msg, E_DBus_Unmarshal_Func unmarshal_func, E_DBus_Callback_Func cb_func, E_DBus_Free_Func free_func, int timeout, void *data);


/* signal receiving */

   EAPI E_DBus_Signal_Handler *e_dbus_signal_handler_add(E_DBus_Connection *conn, const char *sender, const char *path, const char *interface, const char *member, E_DBus_Signal_Cb cb_signal, void *data);
   EAPI void e_dbus_signal_handler_del(E_DBus_Connection *conn, E_DBus_Signal_Handler *sh);

/* standard dbus method calls */

   EAPI DBusPendingCall *e_dbus_request_name(E_DBus_Connection *conn, const char *name,
					     unsigned int flags,
					     E_DBus_Method_Return_Cb cb_return,
					     const void *data);
   EAPI DBusPendingCall *e_dbus_release_name(E_DBus_Connection *conn, const char *name,
					     E_DBus_Method_Return_Cb cb_return,
					     const void *data);

   EAPI DBusPendingCall *e_dbus_get_name_owner(E_DBus_Connection *conn, const char *name,
					       E_DBus_Method_Return_Cb cb_return,
					       const void *data);
   EAPI DBusPendingCall *e_dbus_list_names(E_DBus_Connection *conn,
					   E_DBus_Method_Return_Cb cb_return,
					   const void *data);
   EAPI DBusPendingCall *e_dbus_list_activatable_names(E_DBus_Connection *conn,
						       E_DBus_Method_Return_Cb cb_return,
						       const void *data);
   EAPI DBusPendingCall *e_dbus_name_has_owner(E_DBus_Connection *conn, const char *name,
					       E_DBus_Method_Return_Cb cb_return,
					       const void *data);
   EAPI DBusPendingCall *e_dbus_start_service_by_name(E_DBus_Connection *conn, const char *name,
						      E_DBus_Method_Return_Cb cb_return,
						      const void *data);

/* standard methods calls on objects */
   EAPI DBusPendingCall *e_dbus_peer_ping(E_DBus_Connection *conn, const char *destination,
					  const char *path, E_DBus_Method_Return_Cb cb_return,
					  const void *data);
   EAPI DBusPendingCall *e_dbus_peer_get_machine_id(E_DBus_Connection *conn,
						    const char *destination, const char *path,
						    E_DBus_Method_Return_Cb cb_return,
						    const void *data);
   EAPI DBusPendingCall *e_dbus_properties_get_all(E_DBus_Connection *conn, const char *destination,
						   const char *path, const char *interface,
						   E_DBus_Method_Return_Cb cb_return,
						   const void *data);
   EAPI DBusPendingCall *e_dbus_properties_get(E_DBus_Connection *conn, const char *destination,
					       const char *path, const char *interface,
					       const char *property,
					       E_DBus_Method_Return_Cb cb_return,
					       const void *data);
   EAPI DBusPendingCall *e_dbus_properties_set(E_DBus_Connection *conn, const char *destination,
					       const char *path, const char *interface,
					       const char *property, int value_type,
					       const void *value, E_DBus_Method_Return_Cb cb_return,
					       const void *data);


   EAPI E_DBus_Callback *e_dbus_callback_new(E_DBus_Callback_Func cb_func, E_DBus_Unmarshal_Func unmarshal_func, E_DBus_Free_Func free_func, void *user_data);

   EAPI void e_dbus_callback_free(E_DBus_Callback *callback);
   EAPI void e_dbus_callback_call(E_DBus_Callback *cb, void *data, DBusError *error);
   EAPI void *e_dbus_callback_unmarshal(E_DBus_Callback *cb, DBusMessage *msg, DBusError *err);
   EAPI void e_dbus_callback_return_free(E_DBus_Callback *callback, void *data);

#ifdef __cplusplus
}
#endif

#endif
