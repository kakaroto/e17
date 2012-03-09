#include "CElmSlider.h"

CElmSlider::CElmSlider(CEvasObject * parent, Local <Object> obj)
    : CEvasObject()
    , prop_handler(property_list_base)
{
    eo = elm_slider_add(parent->get());
    construct(eo, obj);
}

CElmSlider::~CElmSlider()
{
    the_icon.Dispose();
    the_end_object.Dispose();
    on_changed_val.Dispose();
}

void CElmSlider::units_set(Handle <Value> value)
{
    if (value->IsString()) {
        String::Utf8Value str(value);
        elm_slider_unit_format_set(eo, *str);
    }
}

Handle <Value> CElmSlider::units_get() const
{
    return String::New(elm_slider_unit_format_get(eo));
}

void CElmSlider::indicator_set(Handle <Value> value)
{
    if (value->IsString()) {
        String::Utf8Value str(value);
        elm_slider_indicator_format_set(eo, *str);
    }
}

Handle <Value> CElmSlider::indicator_get() const
{
    return String::New(elm_slider_indicator_format_get(eo));
}

Handle <Value> CElmSlider::span_get() const
{
    return Integer::New(elm_slider_span_size_get(eo));
}

void CElmSlider::span_set(Handle <Value> value)
{
    if (value->IsInt32()) {
        int span = value->Int32Value();
        elm_slider_span_size_set(eo, span);
    }
}

Handle <Value> CElmSlider::icon_get() const
{
    return the_icon;
}

void CElmSlider::icon_set(Handle <Value> value)
{
    the_icon.Dispose();
    CEvasObject *icon = make_or_get(this, value);
    elm_object_content_set(eo, icon->get());
    the_icon = Persistent <Value>::New(icon->get_object());
}

Handle <Value> CElmSlider::end_get() const
{
    return the_end_object;
}

void CElmSlider::end_set(Handle <Value> value)
{
    the_end_object.Dispose();
    CEvasObject *end_obj = make_or_get(this, value);
    if (end_obj) {
        elm_object_part_content_set(eo, "elm.swallow.end",
                        end_obj->get());
        the_end_object =
            Persistent <Value>::New(end_obj->get_object());
    } else
        elm_object_part_content_unset(eo, "elm.swallow.end");
}

Handle <Value> CElmSlider::value_get() const
{
    return Number::New(elm_slider_value_get(eo));
}

void CElmSlider::value_set(Handle <Value> value)
{
    if (value->IsNumber())
        elm_slider_value_set(eo, value->NumberValue());
}

Handle <Value> CElmSlider::min_get() const
{
    double min, max;
    elm_slider_min_max_get(eo, &min, &max);
    return Number::New(min);
}

void CElmSlider::min_set(Handle <Value> value)
{
    if (value->IsNumber()) {
        double min, max;
        elm_slider_min_max_get(eo, &min, &max);
        min = value->NumberValue();
        elm_slider_min_max_set(eo, min, max);
    }
}

Handle <Value> CElmSlider::max_get() const
{
    double min, max;
    elm_slider_min_max_get(eo, &min, &max);
    return Number::New(max);
}

void CElmSlider::max_set(Handle <Value> value)
{
    if (value->IsNumber()) {
        double min, max;
        elm_slider_min_max_get(eo, &min, &max);
        max = value->NumberValue();
        elm_slider_min_max_set(eo, min, max);
    }
}

Handle <Value> CElmSlider::inverted_get() const
{
    return Boolean::New(elm_slider_inverted_get(eo));
}

void CElmSlider::inverted_set(Handle <Value> value)
{
    if (value->IsBoolean())
        elm_slider_inverted_set(eo, value->BooleanValue());
}

Handle <Value> CElmSlider::horizontal_get() const
{
    return Boolean::New(elm_slider_horizontal_get(eo));
}

void CElmSlider::horizontal_set(Handle <Value> value)
{
    if (value->IsBoolean())
        elm_slider_horizontal_set(eo, value->BooleanValue());
}

void CElmSlider::eo_on_changed(void *data, Evas_Object *, void *event_info)
{
    CElmSlider *changed = static_cast <CElmSlider *>(data);

    changed->on_changed(event_info);
}

void CElmSlider::on_changed(void *)
{
    Handle <Object> obj = get_object();
    HandleScope handle_scope;
    Handle <Value> val = on_changed_val;
    // FIXME: pass event_info to the callback
    // FIXME: turn the pieces below into a do_callback method
    assert(val->IsFunction());
    Handle <Function> fn(Function::Cast(*val));
    Handle <Value> args[1] = {
    obj};
    fn->Call(obj, 1, args);
}

void CElmSlider::on_changed_set(Handle <Value> val)
{
    on_changed_val.Dispose();
    on_changed_val = Persistent <Value>::New(val);
    if (val->IsFunction())
        evas_object_smart_callback_add(eo, "changed", &eo_on_changed,
                           this);
    else
        evas_object_smart_callback_del(eo, "changed", &eo_on_changed);
}

Handle <Value> CElmSlider::on_changed_get(void) const
{
    return on_changed_val;
}

template<> CEvasObject::CPropHandler<CElmSlider>::property_list CEvasObject::CPropHandler<CElmSlider>::list[] =
{
  PROP_HANDLER(CElmSlider, units),
  PROP_HANDLER(CElmSlider, indicator),
  PROP_HANDLER(CElmSlider, span),
  PROP_HANDLER(CElmSlider, icon),
  PROP_HANDLER(CElmSlider, value),
  PROP_HANDLER(CElmSlider, min),
  PROP_HANDLER(CElmSlider, max),
  PROP_HANDLER(CElmSlider, inverted),
  PROP_HANDLER(CElmSlider, end),
  PROP_HANDLER(CElmSlider, horizontal),
  PROP_HANDLER(CElmSlider, on_changed),
  { NULL, NULL, NULL },
};

