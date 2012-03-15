#include "CElmBubble.h"

CElmBubble::CElmBubble(CEvasObject * parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_bubble_add(parent->top_widget_get());
   construct(eo, obj);

   CEvasObject *content = make_or_get(this, obj->Get(String::New("content")));
   if (content)
     elm_object_content_set(eo, content->get());
}

Handle<Value> CElmBubble::text_part_get() const
{
   return Undefined();
}

void CElmBubble::text_part_set(Handle<Value> val)
{
   HandleScope scope;
   if (!val->IsObject()) {
        ELM_ERR("%s: value is not an object!", __FUNCTION__);
        return;
   }

   Local<Object> obj = val->ToObject();
   Local<Value> it = obj->Get(String::New("item"));
   Local<Value> text = obj->Get(String::New("text"));

   elm_object_part_text_set(eo, *String::Utf8Value(it),
                            *String::Utf8Value(text));
}

Handle<Value> CElmBubble::corner_get() const
{
   return Number::New(elm_bubble_pos_get(eo));
}

void CElmBubble::corner_set(Handle<Value> val)
{
   HandleScope scope;

   if (val->IsNumber())
     elm_bubble_pos_set(eo, (Elm_Bubble_Pos)val->ToNumber()->Value());
}

PROPERTIES_OF(CElmBubble) = {
   PROP_HANDLER(CElmBubble, text_part),
   PROP_HANDLER(CElmBubble, corner),
   { NULL }
};
