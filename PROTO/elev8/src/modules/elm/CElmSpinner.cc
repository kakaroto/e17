#include "CElmSpinner.h"

CElmSpinner::CElmSpinner(CEvasObject * parent, Local <Object> obj)
    : CEvasObject()
    , prop_handler(property_list_base)
{
    eo = elm_spinner_add(parent->top_widget_get());
    construct(eo, obj);
}

Handle <Value> CElmSpinner::label_format_get() const
{
    return String::New(elm_spinner_label_format_get(eo));
}

void CElmSpinner::label_format_set(Handle <Value> val)
{
    if (val->IsString())
        elm_spinner_label_format_set(eo, *String::Utf8Value(val));
}

Handle <Value> CElmSpinner::step_get() const
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
    double min, max;
    elm_spinner_min_max_get(eo, &min, &max);
    return Number::New(min);
}

void CElmSpinner::min_set(Handle <Value> value)
{
    if (value->IsNumber()) {
        double max;
        elm_spinner_min_max_get(eo, 0, &max);
        elm_spinner_min_max_set(eo, value->NumberValue(), max);
    }
}

Handle <Value> CElmSpinner::max_get() const
{
    double max;
    elm_spinner_min_max_get(eo, 0, &max);
    return Number::New(max);
}

void CElmSpinner::max_set(Handle <Value> value)
{
    if (value->IsNumber()) {
        double min;
        elm_spinner_min_max_get(eo, &min, 0);
        elm_spinner_min_max_set(eo, min, value->NumberValue());
    }
}

Handle <Value> CElmSpinner::style_get() const
{
    return String::New(elm_object_style_get(eo));
}

void CElmSpinner::style_set(Handle <Value> val)
{
    if (val->IsString())
        elm_object_style_set(eo, *String::Utf8Value(val));
}

Handle <Value> CElmSpinner::editable_get() const
{
    return Number::New(elm_spinner_editable_get(eo));
}

void CElmSpinner::editable_set(Handle <Value> val)
{
    if (val->IsBoolean())
        elm_spinner_editable_set(eo, val->BooleanValue());
}

Handle <Value> CElmSpinner::disabled_get() const
{
    return Boolean::New(elm_spinner_editable_get(eo));
}

void CElmSpinner::disabled_set(Handle <Value> val)
{
    if (val->IsBoolean())
        elm_object_disabled_set(eo, val->BooleanValue());
}

Handle <Value> CElmSpinner::special_value_get() const
{
    //No getter available
    return Undefined();
}

void CElmSpinner::special_value_set(Handle <Value> val)
{
    if (val->IsObject()) {
        Local <Value> value = val->ToObject()->Get(String::New("value"));
        Local <Value> label = val->ToObject()->Get(String::New("label"));
        elm_spinner_special_value_add(eo, value->ToInt32()->Value(), *String::Utf8Value(label));
    }
}

template<> CEvasObject::CPropHandler<CElmSpinner>::property_list CEvasObject::CPropHandler<CElmSpinner>::list[] =
{
    PROP_HANDLER(CElmSpinner, label_format),
    PROP_HANDLER(CElmSpinner, step),
    PROP_HANDLER(CElmSpinner, min),
    PROP_HANDLER(CElmSpinner, max),
    PROP_HANDLER(CElmSpinner, style),
    PROP_HANDLER(CElmSpinner, disabled),
    PROP_HANDLER(CElmSpinner, editable),
    PROP_HANDLER(CElmSpinner, special_value),
    { NULL, NULL, NULL },
};
