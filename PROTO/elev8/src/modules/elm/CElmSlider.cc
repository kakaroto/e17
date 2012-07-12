#include "elm.h"
#include "CElmSlider.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmSlider, on_change);
GENERATE_PROPERTY_CALLBACKS(CElmSlider, units);
GENERATE_PROPERTY_CALLBACKS(CElmSlider, indicator);
GENERATE_PROPERTY_CALLBACKS(CElmSlider, span);
GENERATE_PROPERTY_CALLBACKS(CElmSlider, icon);
GENERATE_PROPERTY_CALLBACKS(CElmSlider, end);
GENERATE_PROPERTY_CALLBACKS(CElmSlider, value);
GENERATE_PROPERTY_CALLBACKS(CElmSlider, min);
GENERATE_PROPERTY_CALLBACKS(CElmSlider, max);
GENERATE_PROPERTY_CALLBACKS(CElmSlider, inverted);
GENERATE_PROPERTY_CALLBACKS(CElmSlider, horizontal);
GENERATE_PROPERTY_CALLBACKS(CElmSlider, indicator_show);

GENERATE_TEMPLATE(CElmSlider,
                  PROPERTY(on_change),
                  PROPERTY(units),
                  PROPERTY(indicator),
                  PROPERTY(span),
                  PROPERTY(icon),
                  PROPERTY(end),
                  PROPERTY(value),
                  PROPERTY(min),
                  PROPERTY(max),
                  PROPERTY(inverted),
                  PROPERTY(horizontal),
                  PROPERTY(indicator_show));

CElmSlider::CElmSlider(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_slider_add(parent->GetEvasObject()))
{
}

void CElmSlider::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Slider"),
               GetTemplate()->GetFunction());
}

CElmSlider::~CElmSlider()
{
   cached.icon.Dispose();
   cached.end.Dispose();
   on_change_set(Undefined());
}

void CElmSlider::units_set(Handle<Value> value)
{
   if (value->IsString())
     elm_slider_unit_format_set(eo, *String::Utf8Value(value));
}

Handle<Value> CElmSlider::units_get() const
{
   return String::New(elm_slider_unit_format_get(eo));
}

void CElmSlider::indicator_set(Handle<Value> value)
{
   if (value->IsString())
     elm_slider_indicator_format_set(eo, *String::Utf8Value(value));
}

Handle<Value> CElmSlider::indicator_get() const
{
   return String::New(elm_slider_indicator_format_get(eo));
}

Handle<Value> CElmSlider::span_get() const
{
   return Integer::New(elm_slider_span_size_get(eo));
}

void CElmSlider::span_set(Handle<Value> value)
{
   if (value->IsInt32())
     elm_slider_span_size_set(eo, value->Int32Value());
}

Handle<Value> CElmSlider::icon_get() const
{
   return cached.icon;
}

void CElmSlider::icon_set(Handle<Value> value)
{
   cached.icon.Dispose();

   cached.icon = Persistent<Value>::New(Realise(value, jsObject));
   elm_object_content_set(eo,
                          GetEvasObjectFromJavascript(cached.icon));
}

Handle<Value> CElmSlider::end_get() const
{
   return cached.end;
}

void CElmSlider::end_set(Handle<Value> value)
{
   cached.end.Dispose();

   if (value->IsUndefined())
     {
        elm_object_part_content_unset(eo, "elm.swallow.end");
        cached.end.Clear();
        return;
     }

   cached.end = Persistent<Value>::New(Realise(value, jsObject));
   elm_object_part_content_set(eo, "elm.swallow.end",
                               GetEvasObjectFromJavascript(cached.end));
}

Handle<Value> CElmSlider::value_get() const
{
   return Number::New(elm_slider_value_get(eo));
}

void CElmSlider::value_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_slider_value_set(eo, value->NumberValue());
}

Handle<Value> CElmSlider::min_get() const
{
   double min;

   elm_slider_min_max_get(eo, &min, NULL);
   return Number::New(min);
}

void CElmSlider::min_set(Handle<Value> value)
{
   if (value->IsNumber())
     {
        double max;
        elm_slider_min_max_get(eo, NULL, &max);
        elm_slider_min_max_set(eo, value->NumberValue(), max);
     }
}

Handle<Value> CElmSlider::max_get() const
{
   double max;

   elm_slider_min_max_get(eo, NULL, &max);
   return Number::New(max);
}

void CElmSlider::max_set(Handle<Value> value)
{
   if (value->IsNumber())
     {
        double min;
        elm_slider_min_max_get(eo, &min, NULL);
        elm_slider_min_max_set(eo, min, value->NumberValue());
     }
}

Handle<Value> CElmSlider::inverted_get() const
{
   return Boolean::New(elm_slider_inverted_get(eo));
}

void CElmSlider::inverted_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_slider_inverted_set(eo, value->BooleanValue());
}

Handle<Value> CElmSlider::horizontal_get() const
{
   return Boolean::New(elm_slider_horizontal_get(eo));
}

void CElmSlider::horizontal_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_slider_horizontal_set(eo, value->BooleanValue());
}

Handle<Value> CElmSlider::indicator_show_get() const
{
   return Boolean::New(elm_slider_indicator_show_get(eo));
}

void CElmSlider::indicator_show_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_slider_indicator_show_set(eo, value->BooleanValue());
}

void CElmSlider::OnChange(void *)
{
   HandleScope scope;
   Local<Function> callback(Function::Cast(*cb.change));
   Handle<Value> args[1] = { jsObject };

   callback->Call(jsObject, 1, args);
}

void CElmSlider::OnChangeWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmSlider*>(data)->OnChange(event_info);
}

Handle<Value> CElmSlider::on_change_get(void) const
{
   return cb.change;
}

void CElmSlider::on_change_set(Handle<Value> val)
{
   if (!cb.change.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "changed", &OnChangeWrapper);
        cb.change.Dispose();
        cb.change.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.change = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "changed", &OnChangeWrapper, this);
}

}
