#include "elm.h"
#include "CElmDaySelector.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmDaySelector, on_change);
GENERATE_PROPERTY_CALLBACKS(CElmDaySelector, on_lang_change);
GENERATE_PROPERTY_CALLBACKS(CElmDaySelector, monday);
GENERATE_PROPERTY_CALLBACKS(CElmDaySelector, tuesday);
GENERATE_PROPERTY_CALLBACKS(CElmDaySelector, wednesday);
GENERATE_PROPERTY_CALLBACKS(CElmDaySelector, thursday);
GENERATE_PROPERTY_CALLBACKS(CElmDaySelector, friday);
GENERATE_PROPERTY_CALLBACKS(CElmDaySelector, saturday);
GENERATE_PROPERTY_CALLBACKS(CElmDaySelector, sunday);
GENERATE_PROPERTY_CALLBACKS(CElmDaySelector, week_start);
GENERATE_PROPERTY_CALLBACKS(CElmDaySelector, weekend_start);
GENERATE_PROPERTY_CALLBACKS(CElmDaySelector, weekend_length);

GENERATE_TEMPLATE(CElmDaySelector,
                  PROPERTY(on_change),
                  PROPERTY(on_lang_change),
                  PROPERTY(monday),
                  PROPERTY(tuesday),
                  PROPERTY(wednesday),
                  PROPERTY(thursday),
                  PROPERTY(friday),
                  PROPERTY(saturday),
                  PROPERTY(sunday),
                  PROPERTY(week_start),
                  PROPERTY(weekend_start),
                  PROPERTY(weekend_length));

CElmDaySelector::CElmDaySelector(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_dayselector_add(parent->GetEvasObject()))
{
}

void CElmDaySelector::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("DaySelector"),
               GetTemplate()->GetFunction());
}

CElmDaySelector::~CElmDaySelector()
{
   on_change_set(Undefined());
   on_lang_change_set(Undefined());
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

void CElmDaySelector::OnChange(void *)
{
   HandleScope scope;
   Local<Function> callback(Function::Cast(*cb.change));
   Handle<Value> args[1] = { jsObject };

   callback->Call(jsObject, 1, args);
}

void CElmDaySelector::OnChangeWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmDaySelector*>(data)->OnChange(event_info);
}

Handle<Value> CElmDaySelector::on_change_get(void) const
{
   return cb.change;
}

void CElmDaySelector::on_change_set(Handle<Value> val)
{
   if (!cb.change.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "dayselector,changed", &OnChangeWrapper);
        cb.change.Dispose();
        cb.change.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.change = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "dayselector,changed", &OnChangeWrapper, this);
}

void CElmDaySelector::OnLangChange(void *)
{
   HandleScope scope;
   Local<Function> callback(Function::Cast(*cb.lang_change));
   Handle<Value> args[1] = { jsObject };

   callback->Call(jsObject, 1, args);
}

void CElmDaySelector::OnLangChangeWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmDaySelector*>(data)->OnLangChange(event_info);
}

Handle<Value> CElmDaySelector::on_lang_change_get(void) const
{
   return cb.lang_change;
}

void CElmDaySelector::on_lang_change_set(Handle<Value> val)
{
   if (!cb.lang_change.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "language,changed", &OnLangChangeWrapper);
        cb.lang_change.Dispose();
        cb.lang_change.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.lang_change = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "language,changed", &OnLangChangeWrapper, this);
}

}
