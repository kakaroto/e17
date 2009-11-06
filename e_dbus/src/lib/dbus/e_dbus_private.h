#ifndef E_DBUS_PRIVATE_H
#define E_DBUS_PRIVATE_H

#include <Ecore.h>

#include "E_DBus.h"

#ifndef E_DBUS_COLOR_DEFAULT
#define E_DBUS_COLOR_DEFAULT EINA_COLOR_CYAN
#endif
EAPI extern int _e_dbus_log_dom;
#define DBG(...)   EINA_LOG_DOM_DBG(_e_dbus_log_dom, __VA_ARGS__)
#define INFO(...)    EINA_LOG_DOM_INFO(_e_dbus_log_dom, __VA_ARGS__)
#define WARN(...) EINA_LOG_DOM_WARN(_e_dbus_log_dom, __VA_ARGS__)
#define ERR(...)   EINA_LOG_DOM_ERR(_e_dbus_log_dom, __VA_ARGS__)


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
