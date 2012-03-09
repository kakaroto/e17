#include "CElmClock.h"

CElmClock::CElmClock(CEvasObject *parent, Local<Object> obj) :
    CEvasObject(),
    prop_handler(property_list_base)
{
    eo = elm_clock_add(parent->top_widget_get());
    construct(eo, obj);
}

CElmClock::~CElmClock()
{
}

Handle<Value> CElmClock::show_am_pm_get() const
{
    Eina_Bool show_am_pm = elm_clock_show_am_pm_get(eo);
    return Boolean::New(show_am_pm);
}

void CElmClock::show_am_pm_set(Handle<Value> val)
{
    if (val->IsNumber())
    {
        int value = val->ToNumber()->Value();
        elm_clock_show_am_pm_set(eo, value);
    }
}

Handle<Value> CElmClock::show_seconds_get() const
{
    Eina_Bool show_seconds = elm_clock_show_seconds_get(eo);
    return Boolean::New(show_seconds);
}

void CElmClock::show_seconds_set(Handle<Value> val)
{
    if (val->IsNumber())
    {
        int value = val->ToNumber()->Value();
        elm_clock_show_seconds_set(eo, value);
    }
}

Handle<Value> CElmClock::hour_get() const
{
    int hour = 0;
    elm_clock_time_get(eo, &hour, NULL, NULL);
    return Number::New(hour);
}

Handle<Value> CElmClock::minute_get() const
{
    int minute = 0;
    elm_clock_time_get(eo, NULL, &minute, NULL);
    return Number::New(minute);
}

Handle<Value> CElmClock::second_get() const
{
    int second = 0;
    elm_clock_time_get(eo, NULL, NULL, &second);
    return Number::New(second);
}

void CElmClock::hour_set(Handle<Value> val)
{
    if (!val->IsNumber())
    {
        ELM_ERR( "%s: value is not a Number!", __FUNCTION__);
        return;
    }
    int hour = 0;
    int minute = 0;
    int second = 0;
    // use either this or the class getters (involves conversion from Value to int)
    elm_clock_time_get(eo, &hour, &minute, &second);

    hour = val->ToNumber()->Value();
    elm_clock_time_set(eo, hour , minute, second);
}

void CElmClock::minute_set(Handle<Value> val)
{
    if (!val->IsNumber())
    {
        ELM_ERR( "%s: value is not a Number!", __FUNCTION__);
        return;
    }
    int hour = 0;
    int minute = 0;
    int second = 0;
    elm_clock_time_get(eo, &hour, &minute, &second);
    minute = val->ToNumber()->Value();
    elm_clock_time_set(eo, hour , minute, second);
}

void CElmClock::second_set(Handle<Value> val)
{
    if (!val->IsNumber())
    {
        ELM_ERR( "%s: value is not a Number!", __FUNCTION__);
        return;
    }
    int hour = 0;
    int minute = 0;
    int second = 0;
    elm_clock_time_get(eo, &hour, &minute, &second);
    second = val->ToNumber()->Value();
    elm_clock_time_set(eo, hour , minute, second);
}

Handle<Value> CElmClock::edit_get() const
{
    Eina_Bool editable = elm_clock_edit_get(eo);
    return Boolean::New(editable);
}

void CElmClock::edit_set(Handle<Value> val)
{
    if (val->IsBoolean())
    {
        Eina_Bool value = val->ToBoolean()->Value();
        elm_clock_edit_set(eo, value);
    }
}

template<> CEvasObject::CPropHandler<CElmClock>::property_list
CEvasObject::CPropHandler<CElmClock>::list[] = {
    PROP_HANDLER(CElmClock, edit),
    PROP_HANDLER(CElmClock, hour),
    PROP_HANDLER(CElmClock, minute),
    PROP_HANDLER(CElmClock, second),
    PROP_HANDLER(CElmClock, show_seconds),
    PROP_HANDLER(CElmClock, show_am_pm),
    { NULL, NULL, NULL },
};

