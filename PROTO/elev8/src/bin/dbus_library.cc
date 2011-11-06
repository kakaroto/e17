#include "dbus_library.h"

using namespace v8;

Handle<ObjectTemplate> dbusObj;


Handle<Value> dbus_msg_introspect(const Arguments &args)
{
   HandleScope scope;

   fprintf(stderr,"Calling Introspect API\n");
   Local<Object> self = args.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void* ptr = wrap->Value();
   DBus *dbus = (DBus *)ptr;

   if (args[0]->IsString() && args[1]->IsString() && 
              args[2]->IsString() && args[3]->IsString() )
     {
        String::Utf8Value service(args[0]->ToString());
        String::Utf8Value path(args[1]->ToString());
        String::Utf8Value interface(args[2]->ToString());
        String::Utf8Value method(args[3]->ToString());
        fprintf(stderr, "%s-%s-%s-%s\n", *service, *path, *interface, *method);

        DBusError error;
        DBusMessage *message;
        DBusMessage *reply;
        int reply_timeout;
        int value;

        dbus_error_init (&error);

        /* Construct the message */
        message = dbus_message_new_method_call(
                                           *service,*path,*interface,*method);

        /* Call ListServices method */
        reply_timeout = -1;   /*don't timeout*/

        /* TODO : use e_dbus_introspect() */

        dbus_message_unref (message);

        /* Print the results */
        fprintf (stderr, "Retrieved Value is %u\n", value);
     }
}

void introspect_result(Local<String> property,
                                Local<Value> value,
                                const AccessorInfo& info)
{
   Local<Object> self = info.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void *ptr = wrap->Value();
   DBus *dbus = (DBus *)ptr;
   
   String::Utf8Value prop_name(property);
   dbus->introspectResult.Dispose();
   dbus->introspectResult = Persistent<Value>::New(value);
}

Handle<Value> createDBusInstance(const Arguments& args)
{
   HandleScope scope;

   if (args[0]->IsString())
     {
        String::Utf8Value method(args[0]->ToString());
        if (!(strstr(*method, "System") || strstr(*method, "Session") 
                    ||  strstr(*method, "Starter")))
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

   /* After this step, dbus is integrated to main loop */
   if (strstr(*method, "System"))
      dbus->conn =  e_dbus_bus_get(DBUS_BUS_SYSTEM);
   if (strstr(*method, "Session"))
      dbus->conn =  e_dbus_bus_get(DBUS_BUS_SESSION);
   if (strstr(*method, "Starter"))
      dbus->conn =  e_dbus_bus_get(DBUS_BUS_STARTER);

   if (dbus->conn==NULL)
     {
        fprintf(stderr, "Cannot create DBus\n");
        delete dbus;
        return Undefined();
     }

   Local<FunctionTemplate> introspect = FunctionTemplate::New();
   introspect->SetCallHandler(dbus_msg_introspect);
   dbusObj->Set(String::New("introspect"), introspect);

   dbusObj->SetAccessor(String::New("onIntrospectResult"),NULL,
                                    introspect_result,
                                    Null()
                             );
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
