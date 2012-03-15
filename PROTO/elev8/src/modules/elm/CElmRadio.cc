#include "CElmRadio.h"

CElmRadio::CElmRadio(CEvasObject * parent, Local <Object> obj)
    : CEvasObject()
    , prop_handler(property_list_base)
{
    eo = elm_radio_add(parent->get());
    construct(eo, obj);
}

CElmRadio::~CElmRadio()
{
    the_icon.Dispose();
    the_group.Dispose();
}

Handle <Value> CElmRadio::icon_get() const
{
    return the_icon;
}

void CElmRadio::icon_set(Handle <Value> value)
{
    the_icon.Dispose();
    CEvasObject *icon = make_or_get(this, value);
    elm_object_content_set(eo, icon->get());
    the_icon = Persistent <Value>::New(icon->get_object());
}

Handle <Value> CElmRadio::group_get() const
{
    return the_group;
}

void CElmRadio::group_set(Handle <Value> value)
{
    the_group = Persistent <Value>::New(value);

    CEvasObject *parent = get_parent();
    if (!parent)
        return;

    CEvasObject *group = parent->get_child(value);
    if (group) {
        if (dynamic_cast <CElmRadio *>(group))
            elm_radio_group_add(eo, group->get());
        else
            ELM_ERR("%p not a radio button!", group);
    } else
        ELM_ERR("child %s not found!",
            *String::Utf8Value(value->ToString()));
}

Handle <Value> CElmRadio::value_get() const
{
    return Integer::New(elm_radio_state_value_get(eo));
}

void CElmRadio::value_set(Handle <Value> value)
{
    if (value->IsNumber())
        elm_radio_state_value_set(eo, value->Int32Value());
}

PROPERTIES_OF(CElmRadio) = {
    PROP_HANDLER(CElmRadio, icon),
    PROP_HANDLER(CElmRadio, group),
    PROP_HANDLER(CElmRadio, value),
    { NULL }
};
