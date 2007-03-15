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
typedef struct E_DBus_Signal_Handler E_DBus_Signal_Handler;

typedef DBusMessage *(* E_DBus_Object_Method_Cb)(E_DBus_Object *obj, DBusMessage *message);
typedef void (*E_DBus_Method_Return_Cb) (void *data, DBusMessage *msg);
typedef void (*E_DBus_Error_Cb) (void *data, const char *error_name, const char *error_msg);
typedef void (*E_DBus_Signal_Cb) (void *data, DBusMessage *msg);

int e_dbus_init(void);
void e_dbus_shutdown(void);

/* setting up the connection */

DBusConnection *e_dbus_bus_get(DBusBusType type);
void e_dbus_connection_close(DBusConnection *conn);

int e_dbus_connection_setup(DBusConnection *conn);

/* bus methods */
void e_dbus_get_name_owner(DBusConnection *conn, const char *name, E_DBus_Method_Return_Cb cb_return, E_DBus_Error_Cb cb_error, void *data);

/* receiving method calls */
E_DBus_Object *e_dbus_object_add(DBusConnection *conn, const char *object_path, void *data);
void e_dbus_object_free(E_DBus_Object *obj);
int e_dbus_object_method_add(E_DBus_Object *obj, const char *interface, const char *member, const char *signature, const char *reply_signature, E_DBus_Object_Method_Cb func);


/* sending method calls */


DBusPendingCall *e_dbus_message_send(DBusConnection *conn, DBusMessage *msg, E_DBus_Method_Return_Cb cb_return, E_DBus_Error_Cb cb_error, int timeout, void *data);


/* signal receiving */

int  e_dbus_signal_init(void);
void e_dbus_signal_shutdown(void);
E_DBus_Signal_Handler *e_dbus_signal_handler_add(DBusConnection *conn, const char *sender, const char *path, const char *interface, const char *member, E_DBus_Signal_Cb cb_signal, void *data);

#define DEBUG_LVL 0
#define DEBUG(lvl, ...) if (lvl <= DEBUG_LVL) printf(__VA_ARGS__)

#endif
