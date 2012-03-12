#include "CElmIcon.h"

CElmIcon::CElmIcon(CEvasObject * parent, Local <Object> obj)
    : CEvasObject()
    , prop_handler(property_list_base)
{
    eo = elm_icon_add(parent->top_widget_get());
    construct(eo, obj);
}

void CElmIcon::resizable_up_set(Handle <Value> val)
{
    Eina_Bool up, down;
    if (val->IsBoolean()) {
        elm_icon_resizable_get(eo, &up, &down);
        elm_icon_resizable_set(eo, val->BooleanValue(), down);
    }
}

Handle <Value> CElmIcon::resizable_up_get() const
{
    Eina_Bool up, down;
    elm_icon_resizable_get(eo, &up, &down);
    return Boolean::New(up);
}

void CElmIcon::resizable_down_set(Handle <Value> val)
{
    Eina_Bool up, down;
    if (val->IsBoolean()) {
        elm_icon_resizable_get(eo, &up, &down);
        elm_icon_resizable_set(eo, up, val->BooleanValue());
    }
}

Handle <Value> CElmIcon::resizable_down_get() const
{
    Eina_Bool up, down;
    elm_icon_resizable_get(eo, &up, &down);
    return Boolean::New(down);
}

Handle <Value> CElmIcon::prescale_get() const
{
    int prescale = elm_icon_prescale_get(eo);
    return Integer::New(prescale);
}

void CElmIcon::prescale_set(Handle <Value> val)
{
    if (val->IsNumber())
        elm_icon_prescale_set(eo, val->IntegerValue());
}

void CElmIcon::image_set(Handle <Value> val)
{
    if (val->IsString()) {
        String::Utf8Value str(val);
        if (0 > access(*str, R_OK))
            ELM_ERR("warning: can't read icon file %s", *str);

        elm_icon_file_set(eo, *str, NULL);
    }
}

Handle <Value> CElmIcon::image_get(void) const
{
    const char *file = NULL, *group = NULL;
    elm_icon_file_get(eo, &file, &group);
    if (file)
        return String::New(file);
    else
        return Null();
}

template<> CEvasObject::CPropHandler<CElmIcon>::property_list CEvasObject::CPropHandler<CElmIcon>::list[] =
{
     PROP_HANDLER(CElmIcon, resizable_up),
     PROP_HANDLER(CElmIcon, resizable_down),
     PROP_HANDLER(CElmIcon, prescale),
     { NULL, NULL, NULL },
};
