#include "CElmClock.h"

CElmClock::CElmClock(CEvasObject *parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_clock_add(parent->top_widget_get());
   construct(eo, obj);
}

Handle<Value> CElmClock::show_am_pm_get() const
{
   HandleScope scope;
   Eina_Bool show_am_pm = elm_clock_show_am_pm_get(eo);

   return scope.Close(Boolean::New(show_am_pm));
}

void CElmClock::show_am_pm_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_clock_show_am_pm_set(eo, val->ToNumber()->Value());
}

Handle<Value> CElmClock::show_seconds_get() const
{
   HandleScope scope;

   return scope.Close(Boolean::New(elm_clock_show_seconds_get(eo)));
}

void CElmClock::show_seconds_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_clock_show_seconds_set(eo, val->ToNumber()->Value());
}

Handle<Value> CElmClock::hour_get() const
{
   HandleScope scope;
   int hour;

   elm_clock_time_get(eo, &hour, NULL, NULL);
   return scope.Close(Number::New(hour));
}

Handle<Value> CElmClock::minute_get() const
{
   HandleScope scope;
   int minute;

   elm_clock_time_get(eo, NULL, &minute, NULL);
   return scope.Close(Number::New(minute));
}

Handle<Value> CElmClock::second_get() const
{
   HandleScope scope;
   int second;

   elm_clock_time_get(eo, NULL, NULL, &second);
   return scope.Close(Number::New(second));
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
   HandleScope scope;
   return scope.Close(Boolean::New(elm_clock_edit_get(eo)));
}

void CElmClock::edit_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_clock_edit_set(eo, val->ToBoolean()->Value());
}

PROPERTIES_OF(CElmClock) = {
   PROP_HANDLER(CElmClock, edit),
   PROP_HANDLER(CElmClock, hour),
   PROP_HANDLER(CElmClock, minute),
   PROP_HANDLER(CElmClock, second),
   PROP_HANDLER(CElmClock, show_seconds),
   PROP_HANDLER(CElmClock, show_am_pm),
   { NULL }
};
