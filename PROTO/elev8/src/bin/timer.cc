#include <v8.h>
#include <Ecore.h>
#include "timer.h"

using namespace v8;

namespace timer {

static int log_domain;

#define DBG(...) EINA_LOG_DOM_DBG(log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(log_domain, __VA_ARGS__)
#define CRT(...) EINA_LOG_DOM_CRITICAL(log_domain, __VA_ARGS__)

static Persistent<FunctionTemplate> tmpl;
typedef struct { Persistent<Object> obj; } Timer;

static Eina_Bool OnInterval(void *data)
{
   HandleScope scope;
   Handle<Object> obj = static_cast<Timer *>(data)->obj;
   Handle<Function> func(Function::Cast(*obj->GetHiddenValue(String::NewSymbol("callback"))));
   func->Call(obj->GetHiddenValue(String::NewSymbol("this"))->ToObject(), 0, NULL);
   return obj->GetHiddenValue(String::NewSymbol("repeat"))->BooleanValue();
}

static void clearInterval(Handle<Value> val)
{
   Handle<Object> obj = val->ToObject();
   Ecore_Timer *et = static_cast<Ecore_Timer *>(obj->GetPointerFromInternalField(0));
   if (et == NULL) return;
   ecore_timer_del(et);
   obj->SetPointerInInternalField(0, NULL);
}

static void Delete(Persistent<Value> object, void *data)
{
   Timer *timer = static_cast<Timer *>(data);
   clearInterval(timer->obj);
   object.Dispose();
   delete timer;
}

static Handle<Value> New(const Arguments& args, bool repeat)
{
   HandleScope scope;

   if (!args[0]->IsFunction())
     return Undefined();

   if (tmpl.IsEmpty())
     {
        tmpl = Persistent<FunctionTemplate>::New(FunctionTemplate::New());
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);
        tmpl->SetClassName(String::NewSymbol("Timer"));
     }

   double interval = args[1]->IsNumber() ? args[1]->Uint32Value() / 1000. : 0;

   Timer *timer = new Timer;
   timer->obj = Persistent<Object>::New(tmpl->GetFunction()->NewInstance());
   Ecore_Timer *et = ecore_timer_add(interval, OnInterval, timer);

   timer->obj->SetPointerInInternalField(0, et);
   timer->obj->SetHiddenValue(String::NewSymbol("this"), args.This());
   timer->obj->SetHiddenValue(String::NewSymbol("callback"), args[0]);
   timer->obj->SetHiddenValue(String::NewSymbol("repeat"), Boolean::New(repeat));

   timer->obj.MakeWeak(timer, Delete);

   return scope.Close(timer->obj);
}

static Handle<Value> clearInterval(const Arguments& args)
{
   if (tmpl->HasInstance(args[0]))
      clearInterval(args[0]);
   return Undefined();
}

static Handle<Value> setInterval(const Arguments& args)
{
   return New(args, true);
}

static Handle<Value> setTimeout(const Arguments& args)
{
   return New(args, false);
}

void RegisterModule(Handle<ObjectTemplate> global)
{
   log_domain = eina_log_domain_register("elev8-timer", EINA_COLOR_ORANGE);
   if (!log_domain)
     {
        ERR( "could not register elev8-timer log domain.");
        log_domain = EINA_LOG_DOMAIN_GLOBAL;
     }

   INF("elev8-timer Logging initialized. %d", log_domain);

   ecore_init();
   global->Set("setTimeout", FunctionTemplate::New(setTimeout));
   global->Set("setInterval", FunctionTemplate::New(setInterval));
   global->Set("clearTimeout", FunctionTemplate::New(clearInterval));
   global->Set("clearInterval", FunctionTemplate::New(clearInterval));
}

}
