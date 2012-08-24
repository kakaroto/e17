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

static void clearInterval(Handle<Value> val);

class Timer {
public:
   Eina_Bool repeat;
   Eina_Bool on_interval;

   Timer(double interval_, Handle<Value> callback_, bool repeat_, const Local<Object>& thisObj_) {
      obj = Persistent<Object>::New(Object::New());
      obj->SetHiddenValue(String::NewSymbol("elev8::timer"), External::Wrap(this));

      timer = ecore_timer_add(interval_, Timer::OnInterval, this);
      thisObj = Persistent<Object>::New(thisObj_);
      callback = Persistent<Value>::New(callback_);
      repeat = repeat_;
      on_interval = false;
   }

   Handle<Object> ToObject() {
      return obj;
   }

   ~Timer() {
      obj->DeleteHiddenValue(String::NewSymbol("elev8::timer"));
      ecore_timer_del(timer);
      obj.Dispose();
      thisObj.Dispose();
      callback.Dispose();
   }

private:
   Persistent<Object> obj;
   Ecore_Timer *timer;
   Persistent<Object> thisObj;
   Persistent<Value> callback;

   static Persistent<FunctionTemplate> tmpl;

   static Eina_Bool OnInterval(void *data)
   {
      HandleScope scope;
      Timer *t = static_cast<Timer *>(data);
      Handle<Function> func(Function::Cast(*t->callback));
      t->on_interval = true;
      func->Call(t->thisObj, 0, NULL);
      t->on_interval = false;

      if (t->repeat)
        return ECORE_CALLBACK_RENEW;

      clearInterval(t->ToObject());
      return ECORE_CALLBACK_CANCEL;
   }
};

static void clearInterval(Handle<Value> val)
{
   HandleScope scope;

   if (!val->IsObject())
     return;

   Local<Value> timer = val->ToObject()->GetHiddenValue(String::NewSymbol("elev8::timer"));

   if (timer.IsEmpty())
     return;

   Timer *t = static_cast<Timer *>(External::Unwrap(timer));
   t->repeat = false;

   if (!t->on_interval)
     delete t;
}

static Handle<Value> New(const Arguments& args, bool repeat)
{
   HandleScope scope;

   if (!args[0]->IsFunction())
     return Undefined();

   double interval = args[1]->IsNumber() ? args[1]->NumberValue() / 1000. : 0;
   return ((new Timer(interval, args[0], repeat, args.This()))->ToObject());
}

static Handle<Value> clearInterval(const Arguments& args)
{
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
