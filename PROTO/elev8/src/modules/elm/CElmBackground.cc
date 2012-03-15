#include "CElmBackground.h"

CElmBackground::CElmBackground(CEvasObject * parent, Local <Object> obj)
    : CEvasObject()
    , prop_handler(property_list_base)
{
    eo = elm_bg_add(parent->get());
    construct(eo, obj);
}

void CElmBackground::image_set(Handle <Value> val)
{
    if (val->IsString()) {
        String::Utf8Value str(val);
        elm_bg_file_set(eo, *str, NULL);
    }
}

Handle <Value> CElmBackground::image_get(void) const
{
    const char *file = NULL, *group = NULL;
    elm_bg_file_get(eo, &file, &group);
    if (file)
        return String::New(file);
    return Null();
}

Handle <Value> CElmBackground::red_get() const
{
    int r, g, b;
    elm_bg_color_get(eo, &r, &g, &b);
    return Number::New(r);
}

void CElmBackground::red_set(Handle <Value> val)
{
    if (val->IsNumber()) {
        int r, g, b;
        elm_bg_color_get(eo, &r, &g, &b);
        r = val->ToNumber()->Value();
        elm_bg_color_set(eo, r, g, b);
    }
}

Handle <Value> CElmBackground::green_get() const
{
    int r, g, b;
    elm_bg_color_get(eo, &r, &g, &b);
    return Number::New(g);
}

void CElmBackground::green_set(Handle <Value> val)
{
    if (val->IsNumber()) {
        int r, g, b;
        elm_bg_color_get(eo, &r, &g, &b);
        g = val->ToNumber()->Value();
        elm_bg_color_set(eo, r, g, b);
    }
}

Handle <Value> CElmBackground::blue_get() const
{
    int r, g, b;
    elm_bg_color_get(eo, &r, &g, &b);
    return Number::New(b);
}

void CElmBackground::blue_set(Handle <Value> val)
{
    if (val->IsNumber()) {
        int r, g, b;
        elm_bg_color_get(eo, &r, &g, &b);
        b = val->ToNumber()->Value();
        elm_bg_color_set(eo, r, g, b);
    }
}

PROPERTIES_OF(CElmBackground) = {
    PROP_HANDLER(CElmBackground, red),
    PROP_HANDLER(CElmBackground, green),
    PROP_HANDLER(CElmBackground, blue),
    { NULL }
};
