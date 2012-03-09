#include "CElmProgressBar.h"

Handle<Value> CElmProgressBar::do_pulse(const Arguments& args)
{
   CEvasObject *self = eo_from_info(args.This());
   CElmProgressBar *progress = static_cast<CElmProgressBar *>(self);
   if (args[0]->IsBoolean())
     progress->pulse(args[0]->BooleanValue());
   return Undefined();
}

CElmProgressBar::CElmProgressBar(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_progressbar_add(parent->get());
   construct(eo, obj);
   get_object()->Set(String::New("pulse"), FunctionTemplate::New(do_pulse)->GetFunction());
}

CElmProgressBar::~CElmProgressBar()
{
   the_icon.Dispose();
}

void CElmProgressBar::pulse(bool on)
{
   elm_progressbar_pulse(eo, on);
}

Handle<Value> CElmProgressBar::icon_get() const
{
   return the_icon;
}

void CElmProgressBar::icon_set(Handle<Value> value)
{
   the_icon.Dispose();
   CEvasObject *icon = make_or_get(this, value);
   elm_object_content_set(eo, icon->get());
   the_icon = Persistent<Value>::New(icon->get_object());
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
   return String::New(elm_progressbar_unit_format_get(eo));
}

void CElmProgressBar::units_set(Handle<Value> value)
{
   if (value->IsString())
     {
        String::Utf8Value str(value);
        elm_progressbar_unit_format_set(eo, *str);
     }
}

Handle<Value> CElmProgressBar::span_get() const
{
   return Integer::New(elm_progressbar_span_size_get(eo));
}

void CElmProgressBar::span_set(Handle<Value> value)
{
   if (value->IsInt32())
     {
        int span = value->Int32Value();
        elm_progressbar_span_size_set(eo, span);
     }
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

template<> CEvasObject::CPropHandler<CElmProgressBar>::property_list
CEvasObject::CPropHandler<CElmProgressBar>::list[] = {
     PROP_HANDLER(CElmProgressBar, icon),
     PROP_HANDLER(CElmProgressBar, inverted),
     PROP_HANDLER(CElmProgressBar, horizontal),
     PROP_HANDLER(CElmProgressBar, units),
     PROP_HANDLER(CElmProgressBar, span),
     PROP_HANDLER(CElmProgressBar, pulser),
     PROP_HANDLER(CElmProgressBar, value),
     { NULL, NULL, NULL },
};

