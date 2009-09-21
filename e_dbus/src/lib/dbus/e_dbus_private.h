#ifndef E_DBUS_PRIVATE_H
#define E_DBUS_PRIVATE_H

#include "dbus/dbus.h"
#include "Ecore_Data.h"
#include "eina_log.h"

#define E_DBUS_COLOR_DEFAULT "\033[36m"
extern int _E_DBUS_LOG_DOM_GLOBAL ;
#define E_DBUS_LOG_DBG(...)   EINA_LOG_DOM_DBG(_E_DBUS_LOG_DOM_GLOBAL , __VA_ARGS__)
#define E_DBUS_LOG_INFO(...)    EINA_LOG_DOM_INFO(_E_DBUS_LOG_DOM_GLOBAL , __VA_ARGS__)
#define E_DBUS_LOG_WARN(...) EINA_LOG_DOM_WARN(_E_DBUS_LOG_DOM_GLOBAL , __VA_ARGS__)
#define E_DBUS_LOG_ERR(...)   EINA_LOG_DOM_ERR(_E_DBUS_LOG_DOM_GLOBAL , __VA_ARGS__)


struct E_DBus_Connection
{
  DBusBusType shared_type;
  DBusConnection *conn;
  char *conn_name;

  Eina_List *fd_handlers;
  Eina_List *timeouts;
  Eina_List *signal_handlers;
  void (*signal_dispatcher)(E_DBus_Connection *conn, DBusMessage *msg);

  Ecore_Idler *idler;

  int refcount;
};

struct E_DBus_Callback
{
  E_DBus_Callback_Func cb_func;
  E_DBus_Unmarshal_Func unmarshal_func;
  E_DBus_Free_Func free_func;
  void *user_data;
};

int  e_dbus_object_init(void);
void e_dbus_object_shutdown(void);

extern int e_dbus_idler_active;
void e_dbus_signal_handlers_clean(E_DBus_Connection *conn);
void e_dbus_signal_handlers_free_all(E_DBus_Connection *conn);


const char *e_dbus_basic_type_as_string(int type);

  
#endif
