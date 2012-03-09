#include "CElmCheck.h"

CElmCheck::CElmCheck(CEvasObject *parent, Local<Object> obj) :
    CEvasObject(),
    prop_handler(property_list_base)
{
    eo = elm_check_add(parent->get());
    construct(eo, obj);
}

CElmCheck::~CElmCheck()
{
    the_icon.Dispose();
}

void CElmCheck::state_set(Handle<Value> value)
{
    if (value->IsBoolean())
        elm_check_state_set(eo, value->BooleanValue());
}

Handle<Value> CElmCheck::state_get() const
{
    return Boolean::New(elm_check_state_get(eo));
}

Handle<Value> CElmCheck::icon_get() const
{
    return the_icon;
}

void CElmCheck::icon_set(Handle<Value> value)
{
    the_icon.Dispose();
    CEvasObject *icon = make_or_get(this, value);
    elm_object_content_set(eo, icon->get());
    the_icon = Persistent<Value>::New(icon->get_object());
}

template<> CEvasObject::CPropHandler<CElmCheck>::property_list
CEvasObject::CPropHandler<CElmCheck>::list[] = {
    PROP_HANDLER(CElmCheck, state),
    PROP_HANDLER(CElmCheck, icon),
    { NULL, NULL, NULL },
};

