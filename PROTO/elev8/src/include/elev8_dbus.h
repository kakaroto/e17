#ifndef ELEV8_DBUS_LIBRARY_H
#define ELEV8_DBUS_LIBRARY_H

#include <v8.h>
#include <dbus/dbus.h>
#include <E_DBus.h>
#include <Ecore.h>
#include <Eina.h>
#include <ctype.h>
#include <map>
#include <elev8_common.h>

/* memory efficient structures to hold introspection information.
 * maybe Eina_Inlist could be replaced with single array of structures
 * (not to be confused with Eina_Array!!!!)
 */

struct DBus_Method_Argument
{
   EINA_INLIST;
   const char *type;
   const char *name;
   Eina_Bool is_output:1;
};

struct DBus_Method
{
   EINA_INLIST;
   const char *name;
   Eina_Inlist *arguments;
};

struct DBus_Signal_Argument
{
   EINA_INLIST;
   const char *type;
   const char *name;
};

struct DBus_Signal
{
   EINA_INLIST;
   const char *name;
   Eina_Inlist *arguments;
};

struct DBus_Property
{
   EINA_INLIST;
   const char *name;
   const char *type;
   Eina_Bool can_read:1;
   Eina_Bool can_write:1;
};

struct DBus_Interface
{
   EINA_INLIST;
   const char *name;
   Eina_Inlist *methods;
   Eina_Inlist *signals;
   Eina_Inlist *properties;
};

struct DBus_Node
{
   EINA_INLIST;
   const char *name;
   Eina_Inlist *interfaces;
   Eina_Inlist *children;
};

struct DBus_Introspection_Parse_Ctxt
{
   struct DBus_Node *node;
   struct DBus_Interface *interface;
   struct DBus_Method *method;
   struct DBus_Signal *signal;
   // the above fields in this struct only aid in parsing the xml response.
   // after parsing is done, they are set to NULL.

   // master nodes list which stores multiple nodes - based on user request.
   Eina_Inlist *nodes;
   const char *xml_str;
};

class DBus {

   public:
      E_DBus_Connection *conn;
      v8::Persistent<v8::Value> js_introspect_cb;
      v8::Persistent<v8::Object> obj;
      struct DBus_Introspection_Parse_Ctxt *ctxt;
};

struct dbus_cache
{
   v8::Local<v8::String> service;
   DBus *dbus;
};

#endif
