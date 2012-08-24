#include "CElmProgressBar.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmProgressBar, icon);
GENERATE_PROPERTY_CALLBACKS(CElmProgressBar, inverted);
GENERATE_PROPERTY_CALLBACKS(CElmProgressBar, horizontal);
GENERATE_PROPERTY_CALLBACKS(CElmProgressBar, units);
GENERATE_PROPERTY_CALLBACKS(CElmProgressBar, span);
GENERATE_PROPERTY_CALLBACKS(CElmProgressBar, pulser);
GENERATE_PROPERTY_CALLBACKS(CElmProgressBar, value);
GENERATE_METHOD_CALLBACKS(CElmProgressBar, pulse);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmProgressBar,
                  PROPERTY(icon),
                  PROPERTY(inverted),
                  PROPERTY(horizontal),
                  PROPERTY(units),
                  PROPERTY(span),
                  PROPERTY(pulser),
                  PROPERTY(value),
                  METHOD(pulse));

Handle<Value> CElmProgressBar::pulse(const Arguments& args)
{
   if (args[0]->IsBoolean())
     elm_progressbar_pulse(eo, args[0]->BooleanValue());
   return Undefined();
}

CElmProgressBar::CElmProgressBar(Local<Object> _jsObject, CElmObject *p)
   : CElmLayout(_jsObject, elm_progressbar_add(p->GetEvasObject()))
{
}

void CElmProgressBar::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("ProgressBar"), GetTemplate()->GetFunction());
}

CElmProgressBar::~CElmProgressBar()
{
   cached.icon.Dispose();
}

Handle<Value> CElmProgressBar::icon_get() const
{
   return cached.icon;
}

void CElmProgressBar::icon_set(Handle<Value> value)
{
   cached.icon.Dispose();
   cached.icon = Persistent<Value>::New(Realise(value, jsObject));
   elm_object_content_set(eo, GetEvasObjectFromJavascript(cached.icon));
}

Handle<Value> CElmProgressBar::inverted_get() const
{
   return Boolean::New(elm_progressbar_inverted_get(eo));
}

void CElmProgressBar::inverted_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_progressbar_inverted_set(eo, value->BooleanValue());
}

Handle<Value> CElmProgressBar::horizontal_get() const
{
   return Boolean::New(elm_progressbar_horizontal_get(eo));
}

void CElmProgressBar::horizontal_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_progressbar_horizontal_set(eo, value->BooleanValue());
}

Handle<Value> CElmProgressBar::units_get() const
{
   const char* units = elm_progressbar_unit_format_get(eo);

   return units ? String::New(units) :  Undefined();
}

void CElmProgressBar::units_set(Handle<Value> value)
{
   if (value->IsString())
     elm_progressbar_unit_format_set(eo, *String::Utf8Value(value));
}

Handle<Value> CElmProgressBar::span_get() const
{
   return Integer::New(elm_progressbar_span_size_get(eo));
}

void CElmProgressBar::span_set(Handle<Value> value)
{
   if (value->IsInt32())
     elm_progressbar_span_size_set(eo, value->Int32Value());
}

Handle<Value> CElmProgressBar::pulser_get() const
{
   return Boolean::New(elm_progressbar_pulse_get(eo));
}

void CElmProgressBar::pulser_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_progressbar_pulse_set(eo, value->BooleanValue());
}

Handle<Value> CElmProgressBar::value_get() const
{
   return Number::New(elm_progressbar_value_get(eo));
}

void CElmProgressBar::value_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_progressbar_value_set(eo, value->NumberValue());
}

}
