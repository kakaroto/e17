#include "elm.h"
#include "CElmSpinner.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmSpinner, label_format);
GENERATE_PROPERTY_CALLBACKS(CElmSpinner, step);
GENERATE_PROPERTY_CALLBACKS(CElmSpinner, min);
GENERATE_PROPERTY_CALLBACKS(CElmSpinner, max);
GENERATE_PROPERTY_CALLBACKS(CElmSpinner, editable);
GENERATE_PROPERTY_CALLBACKS(CElmSpinner, disabled);
GENERATE_PROPERTY_CALLBACKS(CElmSpinner, special_value);
GENERATE_PROPERTY_CALLBACKS(CElmSpinner, wrap);
GENERATE_PROPERTY_CALLBACKS(CElmSpinner, round);

GENERATE_TEMPLATE(CElmSpinner,
                  PROPERTY(label_format),
                  PROPERTY(step),
                  PROPERTY(min),
                  PROPERTY(max),
                  PROPERTY(editable),
                  PROPERTY(disabled),
                  PROPERTY(special_value),
                  PROPERTY(wrap),
                  PROPERTY(round));

CElmSpinner::CElmSpinner(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_spinner_add(parent->GetEvasObject()))
{
}

void CElmSpinner::Initialize(Handle<Object> target)
{
      target->Set(String::NewSymbol("Spinner"),
                                 GetTemplate()->GetFunction());
}

Handle<Value> CElmSpinner::label_format_get() const
{
   return String::New(elm_spinner_label_format_get(eo));
}

void CElmSpinner::label_format_set(Handle<Value> val)
{
   if (val->IsString())
     elm_spinner_label_format_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmSpinner::step_get() const
{
   return Number::New(elm_spinner_step_get(eo));
}

void CElmSpinner::step_set(Handle <Value> val)
{
   if (val->IsNumber())
     elm_spinner_step_set(eo, val->ToNumber()->Value());
}

Handle <Value> CElmSpinner::min_get() const
{
   double min;
   elm_spinner_min_max_get(eo, &min, NULL);
   return Number::New(min);
}

void CElmSpinner::min_set(Handle<Value> value)
{
   if (!value->IsNumber())
     return;

   double max;
   elm_spinner_min_max_get(eo, NULL, &max);
   elm_spinner_min_max_set(eo, value->NumberValue(), max);
}

Handle<Value> CElmSpinner::max_get() const
{
   double max;

   elm_spinner_min_max_get(eo, NULL, &max);
   return Number::New(max);
}

void CElmSpinner::max_set(Handle<Value> value)
{
   if (!value->IsNumber())
     return;

   double min;
   elm_spinner_min_max_get(eo, &min, NULL);
   elm_spinner_min_max_set(eo, min, value->NumberValue());
}

Handle<Value> CElmSpinner::editable_get() const
{
   return Number::New(elm_spinner_editable_get(eo));
}

void CElmSpinner::editable_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_spinner_editable_set(eo, val->BooleanValue());
}

Handle<Value> CElmSpinner::disabled_get() const
{
   return Boolean::New(elm_spinner_editable_get(eo));
}

void CElmSpinner::disabled_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_object_disabled_set(eo, val->BooleanValue());
}

Handle<Value> CElmSpinner::special_value_get() const
{
    //No getter available
    return Undefined();
}

void CElmSpinner::special_value_set(Handle<Value> val)
{
   if (val->IsObject())
     return;

   Local<Value> value = val->ToObject()->Get(String::NewSymbol("value"));
   Local<Value> label = val->ToObject()->Get(String::NewSymbol("label"));
   elm_spinner_special_value_add(eo, value->ToInt32()->Value(),
                                 *String::Utf8Value(label));
}

Handle<Value> CElmSpinner::wrap_get() const
{
   return Boolean::New(elm_spinner_wrap_get(eo));
}

void CElmSpinner::wrap_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_spinner_wrap_set(eo, val->BooleanValue());
}

Handle<Value> CElmSpinner::round_get() const
{
   return Integer::New(elm_spinner_round_get(eo));
}

void CElmSpinner::round_set(Handle <Value> val)
{
   if (val->IsInt32())
     elm_spinner_round_set(eo, val->ToInt32()->Value());
}

}
