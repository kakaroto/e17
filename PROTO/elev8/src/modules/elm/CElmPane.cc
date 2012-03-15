#include "CElmPane.h"

CElmPane::CElmPane(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_panes_add(parent->top_widget_get());
   construct(eo, obj);
   CEvasObject *left, *right;
   left = make_or_get(this, obj->Get(String::New("content_left")));
   if (left)
     {
        elm_object_part_content_set(eo, "elm.swallow.left", left->get());
     }

   right = make_or_get(this, obj->Get(String::New("content_right")));
   if (right)
     {
        elm_object_part_content_set(eo, "elm.swallow.right", right->get());
     }
}

CElmPane::~CElmPane()
{
}

Handle<Value> CElmPane::horizontal_get() const
{
   return Number::New(elm_panes_horizontal_get(eo));
}

void CElmPane::horizontal_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        elm_panes_horizontal_set(eo, val->BooleanValue());
     }
}

void CElmPane::on_press_set(Handle<Value> val)
{
   on_clicked_set(val);
}

Handle<Value> CElmPane::on_press_get(void) const
{
   return on_clicked_val;
}


PROPERTIES_OF(CElmPane) = {
     PROP_HANDLER(CElmPane, horizontal),
     PROP_HANDLER(CElmPane, on_press),
     { NULL }
};

