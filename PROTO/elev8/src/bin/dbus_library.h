#ifndef _DBUS_LIBRARY_H
#define _DBUS_LIBRARY_H

#include <v8.h>
#include <dbus/dbus.h>
#include <E_DBus.h>
#include "dbus_introspect.h"
#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>

class DBus {

   public:
      E_DBus_Connection *conn;
      v8::Persistent<v8::Object> obj;
      v8::Persistent<v8::Value> introspectResult;

};

#endif


