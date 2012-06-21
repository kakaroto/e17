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

class Timer {
public:
   Timer(double interval_, Handle<Value> callback_, bool repeat_, const Local<Object>& thisObj_) {
      obj = Persistent<Object>::New(Timer::GetTemplate()->GetFunction()->NewInstance());
      obj->SetPointerInInternalField(0, this);
      obj.MakeWeak(this, Timer::Delete);

      timer = ecore_timer_add(interval_, Timer::OnInterval, this);
      thisObj = Persistent<Object>::New(thisObj_);
      callback = Persistent<Value>::New(callback_);
      repeat = repeat_;
   }

   Handle<Object> ToObject() {
      return obj;
   }

   static Persistent<FunctionTemplate> GetTemplate() {
      if (Timer::tmpl.IsEmpty())
        {
           Timer::tmpl = Persistent<FunctionTemplate>::New(FunctionTemplate::New());
           Timer::tmpl->InstanceTemplate()->SetInternalFieldCount(1);
           Timer::tmpl->SetClassName(String::NewSymbol("Timer"));
        }

      return Timer::tmpl;
   }

   ~Timer() {
      HandleScope scope;

      ecore_timer_del(timer);
      thisObj.Dispose();
      callback.Dispose();

      obj->Set(String::NewSymbol("disposed_already"), Boolean::New(true));
   }

private:
   Persistent<Object> obj;
   Ecore_Timer *timer;
   Persistent<Object> thisObj;
   Persistent<Value> callback;
   Eina_Bool repeat;

   static Persistent<FunctionTemplate> tmpl;

   static void Delete(Persistent<Value> obj, void *data) {
      HandleScope scope;

      if (!obj->ToObject()->Has(String::NewSymbol("disposed_already")))
        delete static_cast<Timer *>(data);

      obj.Clear();
      obj.Dispose();
   }

   static Eina_Bool OnInterval(void *data)
   {
      HandleScope scope;
      Timer *t = static_cast<Timer *>(data);
      Handle<Function> func(Function::Cast(*t->callback));
      Eina_Bool repeat = t->repeat;
      func->Call(t->thisObj, 0, NULL);

      return repeat;
   }
};

Persistent<FunctionTemplate> Timer::tmpl;

static void clearInterval(Handle<Value> val)
{
   HandleScope scope;
   delete static_cast<Timer *>(val->ToObject()->GetPointerFromInternalField(0));
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
   if (Timer::GetTemplate()->HasInstance(args[0]))
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
