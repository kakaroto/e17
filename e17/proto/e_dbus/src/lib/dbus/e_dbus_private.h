#ifndef E_DBUS_PRIVATE_H
#define E_DBUS_PRIVATE_H

#include "dbus/dbus.h"
#include "Ecore_Data.h"

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

#endif
