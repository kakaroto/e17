#include "CElmDaySelector.h"

CElmDaySelector::CElmDaySelector(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_dayselector_add(parent->top_widget_get());
   construct(eo, obj);
}

Handle<Value> CElmDaySelector::day_selected_get(int day) const
{
   return Boolean::New(elm_dayselector_day_selected_get(eo, (Elm_Dayselector_Day)day));
}

void CElmDaySelector::day_selected_set(int day, Handle<Value> val)
{
   if (val->IsBoolean())
     elm_dayselector_day_selected_set(eo, (Elm_Dayselector_Day)day, val->BooleanValue());
}

Handle<Value> CElmDaySelector::monday_get() const
{
   return day_selected_get(ELM_DAYSELECTOR_MON);
}

void CElmDaySelector::monday_set(Handle<Value> val)
{
   day_selected_set(ELM_DAYSELECTOR_MON, val);
}

Handle<Value> CElmDaySelector::tuesday_get() const
{
   return day_selected_get(ELM_DAYSELECTOR_TUE);
}

void CElmDaySelector::tuesday_set(Handle<Value> val)
{
   day_selected_set(ELM_DAYSELECTOR_TUE, val);
}

Handle<Value> CElmDaySelector::wednesday_get() const
{
   return day_selected_get(ELM_DAYSELECTOR_WED);
}

void CElmDaySelector::wednesday_set(Handle<Value> val)
{
   day_selected_set(ELM_DAYSELECTOR_WED, val);
}

Handle<Value> CElmDaySelector::thursday_get() const
{
   return day_selected_get(ELM_DAYSELECTOR_THU);
}

void CElmDaySelector::thursday_set(Handle<Value> val)
{
   day_selected_set(ELM_DAYSELECTOR_THU, val);
}

Handle<Value> CElmDaySelector::friday_get() const
{
   return day_selected_get(ELM_DAYSELECTOR_FRI);
}

void CElmDaySelector::friday_set(Handle<Value> val)
{
   day_selected_set(ELM_DAYSELECTOR_FRI, val);
}

Handle<Value> CElmDaySelector::saturday_get() const
{
   return day_selected_get(ELM_DAYSELECTOR_SAT);
}

void CElmDaySelector::saturday_set(Handle<Value> val)
{
   day_selected_set(ELM_DAYSELECTOR_SAT, val);
}

Handle<Value> CElmDaySelector::sunday_get() const
{
   return day_selected_get(ELM_DAYSELECTOR_SUN);
}

void CElmDaySelector::sunday_set(Handle<Value> val)
{
   day_selected_set(ELM_DAYSELECTOR_SUN, val);
}

Handle<Value> CElmDaySelector::week_start_get() const
{
   return Number::New(elm_dayselector_week_start_get(eo));
}

void CElmDaySelector::week_start_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_dayselector_week_start_set(eo, (Elm_Dayselector_Day)val->ToNumber()->Value());
}

Handle<Value> CElmDaySelector::weekend_start_get() const
{
   return Number::New(elm_dayselector_weekend_start_get(eo));
}

void CElmDaySelector::weekend_start_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_dayselector_weekend_start_set(eo, (Elm_Dayselector_Day)val->ToNumber()->Value());
}

Handle<Value> CElmDaySelector::weekend_length_get() const
{
   return Number::New(elm_dayselector_weekend_length_get(eo));
}

void CElmDaySelector::weekend_length_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_dayselector_weekend_length_set(eo, val->ToNumber()->Value());
}

void CElmDaySelector::on_changed(void *)
{
   HandleScope handle_scope;
   Handle<Object> obj = get_object();
   Handle<Value> val = on_changed_val;
   assert(val->IsFunction());
   Handle<Function> fn(Function::Cast(*val));
   Handle<Value> args[1] = { obj };
   fn->Call(obj, 1, args);
}

void CElmDaySelector::eo_on_changed(void *data, Evas_Object *, void *event_info)
{
   CElmDaySelector *changed = static_cast<CElmDaySelector*>(data);
   changed->on_changed(event_info);
}

void CElmDaySelector::on_changed_set(Handle<Value> val)
{
   on_changed_val.Dispose();
   on_changed_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_smart_callback_add(eo, "dayselector,changed", &eo_on_changed, this);
   else
     evas_object_smart_callback_del(eo, "dayselector,changed", &eo_on_changed);
}

Handle<Value> CElmDaySelector::on_changed_get(void) const
{
   return on_changed_val;
}

void CElmDaySelector::on_lang_changed(void *)
{
   HandleScope handle_scope;
   Handle<Object> obj = get_object();
   Handle<Value> val = on_lang_changed_val;
   assert(val->IsFunction());
   Handle<Function> fn(Function::Cast(*val));
   Handle<Value> args[1] = { obj };
   fn->Call(obj, 1, args);
}

void CElmDaySelector::eo_on_lang_changed(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmDaySelector*>(data)->on_lang_changed(event_info);
}

void CElmDaySelector::on_lang_changed_set(Handle<Value> val)
{
   on_lang_changed_val.Dispose();

   on_lang_changed_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_smart_callback_add(eo, "language,changed", &eo_on_lang_changed, this);
   else
     evas_object_smart_callback_del(eo, "language,changed", &eo_on_lang_changed);
}

Handle<Value> CElmDaySelector::on_lang_changed_get(void) const
{
   return on_lang_changed_val;
}

PROPERTIES_OF(CElmDaySelector) = {
   PROP_HANDLER(CElmDaySelector, monday),
   PROP_HANDLER(CElmDaySelector, tuesday),
   PROP_HANDLER(CElmDaySelector, wednesday),
   PROP_HANDLER(CElmDaySelector, thursday),
   PROP_HANDLER(CElmDaySelector, friday),
   PROP_HANDLER(CElmDaySelector, saturday),
   PROP_HANDLER(CElmDaySelector, sunday),
   PROP_HANDLER(CElmDaySelector, weekend_start),
   PROP_HANDLER(CElmDaySelector, week_start),
   PROP_HANDLER(CElmDaySelector, weekend_length),
   PROP_HANDLER(CElmDaySelector, on_changed),
   PROP_HANDLER(CElmDaySelector, on_lang_changed),
   { NULL },
};
