#include <v8.h>
#include <Eet.h>
#include <Ecore.h>
#include "storage.h"

using namespace v8;

namespace storage {

int log;

#define STORAGE_DBG(...) EINA_LOG_DOM_DBG(log, __VA_ARGS__)
#define STORAGE_INF(...) EINA_LOG_DOM_INFO(log, __VA_ARGS__)
#define STORAGE_WRN(...) EINA_LOG_DOM_WARN(log, __VA_ARGS__)
#define STORAGE_ERR(...) EINA_LOG_DOM_ERR(log, __VA_ARGS__)
#define STORAGE_CRT(...) EINA_LOG_DOM_CRIT(log, __VA_ARGS__)

static Eet_File *ef = NULL;
static Persistent<String> databaseFile;
static const char* const defaultDatabaseFile = "/tmp/elev8.eet";

static void ensureEetIsOpen()
{
   if (ef) return;

   STORAGE_INF("Opening database: %s\n", *String::Utf8Value(databaseFile));
   ef = eet_open(*String::Utf8Value(databaseFile), EET_FILE_MODE_READ_WRITE);
   if (ef) return;

   if (!strncmp(*String::Utf8Value(databaseFile), defaultDatabaseFile, sizeof(defaultDatabaseFile) - 1))
     {
        STORAGE_CRT("Could not open default database file at %s", defaultDatabaseFile);
        return;
     }

   HandleScope scope;

   STORAGE_ERR("Could not open database at %s, trying default path", *String::Utf8Value(databaseFile));
   databaseFile.Dispose();
   databaseFile = Persistent<String>::New(String::New(defaultDatabaseFile));
   ensureEetIsOpen();
}

static void closeEetOnExit()
{
   if (ef)
     {
        STORAGE_INF("Closing database: %s\n", *String::Utf8Value(databaseFile));
        eet_close(ef);
     }
}

static Handle<Value> clear(const Arguments&)
{
   HandleScope scope;
   char **items;
   int size;

   ensureEetIsOpen();

   items = eet_list(ef, "*", &size);
   for (int i = 0; i < size; i++) eet_delete(ef, items[i]);
   free(items);
   return Undefined();
}

static Handle<Value> remove(const Arguments& args)
{
   HandleScope scope;

   ensureEetIsOpen();

   eet_delete(ef, *String::Utf8Value(args[0]));
   return Undefined();
}

static Handle<Value> setItem(Local<Value> property, Local<Value> value)
{
   HandleScope scope;

   ensureEetIsOpen();

   eet_write(ef, *String::Utf8Value(property), *String::Utf8Value(value),
             value->ToString()->Utf8Length(), 1);
   return Undefined();
}

static Handle<Value> setItem(const Arguments& args)
{
   return setItem(args[0], args[1]);
}

static Handle<Value> getItem(Local<Value> property)
{
   HandleScope scope;
   Handle<String> result;
   char *ret;
   int size;

   ensureEetIsOpen();

   ret = (char *)eet_read(ef, *String::Utf8Value(property), &size);
   if (!ret) return Null();

   result = String::New(ret, size);
   free(ret);

   return scope.Close(result);

}

static Handle<Value> getItem(const Arguments& args)
{
   return getItem(args[0]);
}

static Handle<Value> key(const Arguments& args)
{
   HandleScope scope;
   int index = args[0]->Uint32Value();

   ensureEetIsOpen();

   if (index > eet_num_entries(ef)) return Null();

   char **items;
   int size;
   items = eet_list(ef, "*", &size);
   Handle<String> result = String::New(items[index]);
   free(items);
   return scope.Close(result);
}

static Handle<Value> length(Local<String>, const AccessorInfo&)
{
   HandleScope scope;
   return scope.Close(Integer::New(eet_num_entries(ef)));
}

static Handle<Value> databaseGet(Local<String>, const AccessorInfo&)
{
   HandleScope scope;
   return scope.Close(databaseFile);
}

static void databaseSet(Local<String>, Local<Value> value, const AccessorInfo&)
{
   if (!value->IsString()) return;

   HandleScope scope;

   databaseFile.Dispose();
   eet_close(ef);
   ef = NULL;

   databaseFile = Persistent<String>::New(value->ToString());
}

void RegisterModule(Handle<ObjectTemplate> global)
{
   log = eina_log_domain_register("elev8-store", EINA_COLOR_ORANGE);
   if (!log)
     {
        STORAGE_ERR("could not register elev8-store log domain.");
        log = EINA_LOG_DOMAIN_GLOBAL;
     }

   STORAGE_INF("elev8-store Logging initialized. %d", log);

   eet_init();
   ecore_init();

   atexit(closeEetOnExit);

   databaseFile = Persistent<String>::New(String::New(defaultDatabaseFile));

   Handle<FunctionTemplate> tmpl = FunctionTemplate::New();
   tmpl->SetClassName(String::New("Storage"));

   Handle<ObjectTemplate> inst_t = tmpl->InstanceTemplate();

   inst_t->Set("setItem", FunctionTemplate::New(setItem));
   inst_t->Set("getItem", FunctionTemplate::New(getItem));
   inst_t->Set("clear", FunctionTemplate::New(clear));
   inst_t->Set("removeItem", FunctionTemplate::New(remove));
   inst_t->Set("key", FunctionTemplate::New(key));
   inst_t->SetAccessor(String::NewSymbol("length"), length, NULL);
   inst_t->SetAccessor(String::NewSymbol("database"), databaseGet, databaseSet);

   global->Set("LocalStorage", tmpl);
}

}
