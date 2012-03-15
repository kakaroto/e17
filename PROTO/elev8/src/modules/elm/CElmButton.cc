#include "CElmButton.h"

CElmButton::CElmButton(CEvasObject *parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_button_add(parent->top_widget_get());
   construct(eo, obj);
}

CElmButton::~CElmButton()
{
   the_icon.Dispose();
}

Handle<Value> CElmButton::icon_get() const
{
   return the_icon;
}

void CElmButton::icon_set(Handle<Value> value)
{
   the_icon.Dispose();

   CEvasObject *icon = make_or_get(this, value);
   elm_object_content_set(eo, icon->get());
   the_icon = Persistent<Value>::New(icon->get_object());
}

PROPERTIES_OF(CElmButton) = {
   PROP_HANDLER(CElmButton, icon),
   { NULL }
};
