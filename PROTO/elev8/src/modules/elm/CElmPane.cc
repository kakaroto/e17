#include "CElmPane.h"

CElmPane::CElmPane(CEvasObject *parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_panes_add(parent->top_widget_get());
   construct(eo, obj);

   CEvasObject *left = make_or_get(this, obj->Get(String::New("left")));
   if (left)
     elm_object_part_content_set(eo, "elm.swallow.left", left->get());

   CEvasObject *right = make_or_get(this, obj->Get(String::New("right")));
   if (right)
     elm_object_part_content_set(eo, "elm.swallow.right", right->get());
}

Handle<Value> CElmPane::horizontal_get() const
{
   return Number::New(elm_panes_horizontal_get(eo));
}

void CElmPane::horizontal_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_panes_horizontal_set(eo, val->BooleanValue());
}

PROPERTIES_OF(CElmPane) = {
   PROP_HANDLER(CElmPane, horizontal),
   { NULL }
};

