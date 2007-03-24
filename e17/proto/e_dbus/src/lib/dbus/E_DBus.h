#ifndef E_DBUS_H
#define E_DBUS_H

#define DBUS_API_SUBJECT_TO_CHANGE

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <dbus/dbus.h>
#include <Ecore.h>

extern int E_DBUS_EVENT_SIGNAL;

typedef struct E_DBus_Connection E_DBus_Connection;
typedef struct E_DBus_Object E_DBus_Object;
typedef struct E_DBus_Interface E_DBus_Interface;
typedef struct E_DBus_Signal_Handler E_DBus_Signal_Handler;

typedef DBusMessage *(* E_DBus_Method_Cb)(E_DBus_Object *obj, DBusMessage *message);
typedef void (*E_DBus_Method_Return_Cb) (void *data, DBusMessage *msg, DBusError *error);
typedef void (*E_DBus_Signal_Cb) (void *data, DBusMessage *msg);

typedef void (*E_DBus_Object_Property_Get_Cb) (E_DBus_Object *obj, const char *property, int *type, void **value);
typedef int  (*E_DBus_Object_Property_Set_Cb) (E_DBus_Object *obj, const char *property, int type, void *value);

int e_dbus_init(void);
void e_dbus_shutdown(void);

/* setting up the connection */

E_DBus_Connection *e_dbus_bus_get(DBusBusType type);

void e_dbus_connection_ref(E_DBus_Connection *conn);
void e_dbus_connection_unref(E_DBus_Connection *conn);

E_DBus_Connection *e_dbus_connection_setup(DBusConnection *conn);

/* receiving method calls */
E_DBus_Interface *e_dbus_interface_new(const char *interface);
void e_dbus_object_interface_attach(E_DBus_Object *obj, E_DBus_Interface *iface);
int e_dbus_interface_method_add(E_DBus_Interface *iface, const char *member, const char *signature, const char *reply_signature, E_DBus_Method_Cb func);

E_DBus_Object *e_dbus_object_add(E_DBus_Connection *conn, const char *object_path, void *data);
void e_dbus_object_free(E_DBus_Object *obj);
void *e_dbus_object_data_get(E_DBus_Object *obj);

void e_dbus_object_property_get_cb_set(E_DBus_Object *obj, E_DBus_Object_Property_Get_Cb func);
void e_dbus_object_property_set_cb_set(E_DBus_Object *obj, E_DBus_Object_Property_Set_Cb func);


/* sending method calls */


DBusPendingCall *e_dbus_message_send(E_DBus_Connection *conn, DBusMessage *msg, E_DBus_Method_Return_Cb cb_return, int timeout, void *data);


/* signal receiving */

int  e_dbus_signal_init(void);
void e_dbus_signal_shutdown(void);
E_DBus_Signal_Handler *e_dbus_signal_handler_add(E_DBus_Connection *conn, const char *sender, const char *path, const char *interface, const char *member, E_DBus_Signal_Cb cb_signal, void *data);

/* standard dbus method calls */

void e_dbus_request_name(E_DBus_Connection *conn, const char *name, 
                         unsigned int flags,
                         E_DBus_Method_Return_Cb cb_return,
                         void *data);
void e_dbus_release_name(E_DBus_Connection *conn, const char *name,
                         E_DBus_Method_Return_Cb cb_return,
                         void *data);

void e_dbus_get_name_owner(E_DBus_Connection *conn, const char *name,
                           E_DBus_Method_Return_Cb cb_return,
                           void *data);
void e_dbus_list_names(E_DBus_Connection *conn,
                       E_DBus_Method_Return_Cb cb_return,
                       void *data);
void e_dbus_list_activatable_names(E_DBus_Connection *conn,
                                   E_DBus_Method_Return_Cb cb_return,
                                   void *data);
void e_dbus_name_has_owner(E_DBus_Connection *conn, const char *name,
                           E_DBus_Method_Return_Cb cb_return,
                           void *data);
void e_dbus_start_service_by_name(E_DBus_Connection *conn, const char *name,
                                  E_DBus_Method_Return_Cb cb_return,
                                  void *data);

/* standard methods calls on objects */
void e_dbus_peer_ping(E_DBus_Connection *conn, const char *destination,
                      const char *path, E_DBus_Method_Return_Cb cb_return,
                      void *data);
void e_dbus_peer_get_machine_id(E_DBus_Connection *conn,
                                const char *destination, const char *path,
                                E_DBus_Method_Return_Cb cb_return,
                                void *data);
void e_dbus_properties_get(E_DBus_Connection *conn, const char *destination,
                           const char *path, const char *interface,
                           const char *property,
                           E_DBus_Method_Return_Cb cb_return,
                           void *data);
void e_dbus_properties_set(E_DBus_Connection *conn, const char *destination,
                           const char *path, const char *interface,
                           const char *property, int value_type,
                           void *value, E_DBus_Method_Return_Cb cb_return,
                           void *data);



#define DEBUG_LVL 0
#define DEBUG(lvl, ...) if (lvl <= DEBUG_LVL) printf(__VA_ARGS__)

#endif
