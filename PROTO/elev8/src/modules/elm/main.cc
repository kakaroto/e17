#include <v8.h>

#include <Elementary.h>

using namespace v8;

#include "elm.h"
#include "CElmObject.h"
#include "CElmWindow.h"
#include "CElmBackground.h"

namespace elm {

int log_domain;
extern "C" void RegisterModule(Handle<Object> target);

#ifdef USE_NODE
static const char *log_domain_name = "node-elm";
NODE_MODULE(elm, elm::RegisterModule);
#else
static const char *log_domain_name = "elev8-elm";
#endif

static Persistent<Value> datadir;
static Persistent<Value> tmpdir;
static Persistent<Value> theme;

static Handle<Value>
datadir_getter(Local<String>, const AccessorInfo&)
{
   return datadir;
}

static void
datadir_setter(Local<String>, Local<Value> value, const AccessorInfo&)
{
   datadir.Dispose();
   datadir = Persistent<Value>::New(value);
}

static Handle<Value>
tmpdir_getter(Local<String>, const AccessorInfo&)
{
   return tmpdir;
}

static void
tmpdir_setter(Local<String>, Local<Value> value, const AccessorInfo&)
{
   tmpdir.Dispose();
   tmpdir = Persistent<Value>::New(value);
}

static Handle<Value>
theme_getter(Local<String>, const AccessorInfo&)
{
   return theme;
}

static void
theme_setter(Local<String>, Local<Value> value, const AccessorInfo&)
{
   HandleScope scope;
   theme.Dispose();
   setenv("ELM_THEME", *String::Utf8Value(value), 1);

   theme = Persistent<Value>::New(value);
}

static void Initialize(Handle<Object> target)
{
   HandleScope scope;
   target->SetAccessor(String::NewSymbol("tmpdir"),
                       tmpdir_getter, tmpdir_setter);
   target->SetAccessor(String::NewSymbol("datadir"),
                       datadir_getter, datadir_setter);
   target->SetAccessor(String::NewSymbol("theme"),
                       theme_getter, theme_setter);

   /* setup data directory */
   datadir = Persistent<String>::New(String::New(PACKAGE_DATA_DIR "/" ));
   tmpdir = Persistent<String>::New(String::New(PACKAGE_TMP_DIR "/" ));
}

extern "C"
void RegisterModule(Handle<Object> target)
{
   int argc = 0;
   char *argv[] = {};

   log_domain = eina_log_domain_register(log_domain_name, EINA_COLOR_GREEN);
   if (!log_domain) {
      ELM_ERR("Could not register %s log domain.", log_domain_name);
      log_domain = EINA_LOG_DOMAIN_GLOBAL;
   }
   ELM_INF("%s log domain initialized %d", log_domain_name, log_domain);
   elm_init(argc, argv);

   Initialize(target);
   CElmObject::Initialize(target);
   CElmWindow::Initialize(target);
   CElmBackground::Initialize(target);
}

}
