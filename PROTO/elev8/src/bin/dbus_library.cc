#include "dbus_library.h"
#include <expat.h>
#include "dbus_introspect.h"

using namespace v8;
using namespace dbus_library;

Handle<ObjectTemplate> dbusObj;

void introspect_cb(void *data, DBusMessage *msg, DBusError *error)
{
   char *value;
   int size = 0;
   DBus *dbus = (DBus *)data;
   /* Extract the data from the reply */
   if (!dbus_message_get_args(msg,error,DBUS_TYPE_STRING,&value, DBUS_TYPE_INVALID))
     {
        fprintf (stderr, "Failed to complete call\n");
        return;
     }

   //the string from dbus_message_get_args is always null terminated
   size = strlen(value);

   /* Print the results */
   fprintf (stderr, "Retrieved Value is %s\n", value);

   XML_Parser expat;
   Parser *parser = ParserNew();

   expat = XML_ParserCreate(NULL);
   XML_SetUserData(expat, parser);
   XML_SetElementHandler(expat, 
               expat_StartElementHandler,
               expat_EndElementHandler);

   if (!XML_Parse(expat, value, size, true))
     {
        enum XML_Error e;

        e = XML_GetErrorCode (expat);
        if (e == XML_ERROR_NO_MEMORY)
          fprintf(stderr, "Not enough memory to parse XML document");
        else
          fprintf(stderr, "Error in D-BUS description XML, line %ld, column %ld: %s\n",
                       XML_GetCurrentLineNumber (expat),
                       XML_GetCurrentColumnNumber (expat),
                       XML_ErrorString (e));
     }
   printf("success\n");

   ParserPrint(parser);

   ParserRelease(&parser);

   if (dbus->introspect_result->IsFunction())
     {
        Local<Function> func = Function::Cast(*(dbus->introspect_result));
        if (!func.IsEmpty())
          {
             fprintf (stderr, "Retrieved Value is %s\n", value);
             Persistent<String> path = static_cast<Persistent<String> >(
					                    String::New(value));
             Handle<Value> args[1] = { path };
             func->Call(func, 1, args);
          }
     }
}

Handle<Value> dbus_msg_introspect(const Arguments &args)
{
   HandleScope scope;

   fprintf(stderr,"Calling Introspect API\n");
   Local<Object> self = args.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void* ptr = wrap->Value();
   DBus *dbus = (DBus *)ptr;

   if (args[0]->IsString() && args[1]->IsString())
     {
        String::Utf8Value service(args[0]->ToString());
        String::Utf8Value path(args[1]->ToString());
        fprintf(stderr, "%s-%s\n", *service, *path);

        DBusPendingCall *pc;

        /* TODO : use e_dbus_introspect() */
        pc = e_dbus_introspect(dbus->conn, *service, *path, introspect_cb, ptr);

        if (!pc)
          {
             fprintf(stderr, "Cannot introspect the given DBUS\n");
          }
        fprintf(stderr, "Introspect called\n");
     }
   return Undefined();
}

void introspect_result(Local<String> property,
                                Local<Value> value,
                                const AccessorInfo& info)
{
   Local<Object> self = info.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void* ptr = wrap->Value();
   DBus *dbus = (DBus *)ptr;

   String::Utf8Value prop_name(property);
   dbus->introspect_result.Dispose();
   dbus->introspect_result = Persistent<Value>::New(value);
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

   dbusObj->SetAccessor(String::New("on_introspect_result"),NULL,
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
