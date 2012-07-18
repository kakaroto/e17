#include "elm.h"
#include "CElmClock.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmClock, show_am_pm);
GENERATE_PROPERTY_CALLBACKS(CElmClock, show_seconds);
GENERATE_PROPERTY_CALLBACKS(CElmClock, hour);
GENERATE_PROPERTY_CALLBACKS(CElmClock, minute);
GENERATE_PROPERTY_CALLBACKS(CElmClock, second);
GENERATE_PROPERTY_CALLBACKS(CElmClock, edit);
GENERATE_PROPERTY_CALLBACKS(CElmClock, first_interval);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmClock,
                  PROPERTY(show_am_pm),
                  PROPERTY(show_seconds),
                  PROPERTY(hour),
                  PROPERTY(minute),
                  PROPERTY(second),
                  PROPERTY(edit),
                  PROPERTY(first_interval));

CElmClock::CElmClock(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_clock_add(_parent->GetEvasObject()))
{
}

void CElmClock::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Clock"),
               GetTemplate()->GetFunction());
}

Handle<Value> CElmClock::show_am_pm_get() const
{
   Eina_Bool show_am_pm = elm_clock_show_am_pm_get(eo);
   return Boolean::New(show_am_pm);
}

void CElmClock::show_am_pm_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_clock_show_am_pm_set(eo, val->ToNumber()->Value());
}

Handle<Value> CElmClock::show_seconds_get() const
{
   return Boolean::New(elm_clock_show_seconds_get(eo));
}

void CElmClock::show_seconds_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_clock_show_seconds_set(eo, val->ToNumber()->Value());
}

Handle<Value> CElmClock::hour_get() const
{
   int hour;

   elm_clock_time_get(eo, &hour, NULL, NULL);
   return Number::New(hour);
}

Handle<Value> CElmClock::minute_get() const
{
   int minute;

   elm_clock_time_get(eo, NULL, &minute, NULL);
   return Number::New(minute);
}

Handle<Value> CElmClock::second_get() const
{
   int second;

   elm_clock_time_get(eo, NULL, NULL, &second);
   return Number::New(second);
}

void CElmClock::hour_set(Handle<Value> val)
{
   if (!val->IsNumber())
     {
        ELM_ERR("%s: value is not a Number!", __FUNCTION__);
        return;
     }

   int minute, second;
   elm_clock_time_get(eo, NULL, &minute, &second);
   elm_clock_time_set(eo, val->ToNumber()->Value(), minute, second);
}

void CElmClock::minute_set(Handle<Value> val)
{
   if (!val->IsNumber())
     {
        ELM_ERR("%s: value is not a Number!", __FUNCTION__);
        return;
     }

   int hour, second;
   elm_clock_time_get(eo, &hour, NULL, &second);
   elm_clock_time_set(eo, hour, val->ToNumber()->Value(), second);
}

void CElmClock::second_set(Handle<Value> val)
{
   if (!val->IsNumber())
     {
        ELM_ERR( "%s: value is not a Number!", __FUNCTION__);
        return;
     }

   int hour, minute;
   elm_clock_time_get(eo, &hour, &minute, NULL);
   elm_clock_time_set(eo, hour , minute, val->ToNumber()->Value());
}

Handle<Value> CElmClock::edit_get() const
{
   return Boolean::New(elm_clock_edit_get(eo));
}

void CElmClock::edit_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_clock_edit_set(eo, val->ToBoolean()->Value());
}

Handle<Value> CElmClock::first_interval_get() const
{
   return Number::New(elm_clock_first_interval_get(eo));
}

void CElmClock::first_interval_set(Handle<Value> value)
{
   if (value->IsNumber())
      elm_clock_first_interval_set(eo, value->NumberValue());
}

}
