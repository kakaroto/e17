#include "dbus_library.h"

using namespace v8;

Handle<ObjectTemplate> dbusObj;

Handle<Value> createDBusInstance(const Arguments& args)
{
   HandleScope scope;

   if (args[0]->IsString())
     {
        String::Utf8Value method(args[0]->ToString());
        if (!(strstr(*method, "System") || (strstr(*method, "Session"))))
          {
             fprintf(stderr,"Undefined Session type\n");
             return Undefined();
          }
     }
   else
     {
        fprintf(stderr,"Please specify System or Session as parameter\n");
        return Undefined();
     }

   fprintf(stderr,"Creating DBUS Instance\n");
   DBus *dbus = new DBus();
   String::Utf8Value method(args[0]->ToString());

   if (strstr(*method, "System"))
      dbus->conn =  e_dbus_bus_get(DBUS_BUS_SYSTEM);
   if (strstr(*method, "Session"))
      dbus->conn =  e_dbus_bus_get(DBUS_BUS_SESSION);

   dbus->obj.Dispose();
   dbus->obj = Persistent<Object>::New(dbusObj->NewInstance());
   dbus->obj->SetInternalField(0, External::New(dbus));

   fprintf(stderr, "DBus Interface initialized\n");
   return dbus->obj; 
}

int dbus_v8_setup(Handle<ObjectTemplate> global)
{
   if (!e_dbus_init())
     {
        fprintf(stderr, "Cannot Init to E_DBus\n");
        return -1;
     }

   fprintf(stderr, "Creating DBus Instance\n");

   /* Add support for DBus Service Introspection */
   dbusObj = ObjectTemplate::New();
   dbusObj->SetInternalFieldCount(1);
   global->Set(String::New("dbus"), FunctionTemplate::New(createDBusInstance));
   return 0;
}
