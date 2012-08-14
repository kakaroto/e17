#include <v8.h>
#include <stdlib.h>
#include "environment.h"

using namespace v8;

namespace environment {

static Handle<Value> EnvironGet(Local<String> name, const AccessorInfo &)
{
   char *env_var = getenv(*String::Utf8Value(name));

   if (env_var)
     return String::New(env_var);

   return Undefined();
}

static Handle<Value> EnvironSet(Local<String> name, Local<Value> value, const AccessorInfo &)
{
   if (value->IsUndefined())
     {
        unsetenv(*String::Utf8Value(name));
        return Undefined();
     }

   if (setenv(*String::Utf8Value(name), *String::Utf8Value(value), 1) < 0)
     return Undefined();

   return value;
}

void RegisterModule(Handle<ObjectTemplate> global)
{
   Handle<ObjectTemplate> environ = ObjectTemplate::New();
   environ->SetNamedPropertyHandler(EnvironGet, EnvironSet);

   global->Set("environment", environ);
}

}
