#ifndef E_DBUS_PRIVATE_H
#define E_DBUS_PRIVATE_H

#include "dbus/dbus.h"
#include "Ecore_Data.h"

#define DEBUG_LVL 0
#define DEBUG(lvl, ...) if (lvl <= DEBUG_LVL) printf(__VA_ARGS__)

struct E_DBus_Connection
{
  DBusBusType shared_type;
  DBusConnection *conn;
  char *conn_name;

  Ecore_List *fd_handlers;
  Ecore_List *timeouts;

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

int  e_dbus_signal_init(void);
void e_dbus_signal_shutdown(void);
int  e_dbus_object_init(void);
void e_dbus_object_shutdown(void);

extern int e_dbus_idler_active;
void e_dbus_signal_handlers_clean(E_DBus_Connection *conn);

const char *e_dbus_basic_type_as_string(int type);

  
#endif
