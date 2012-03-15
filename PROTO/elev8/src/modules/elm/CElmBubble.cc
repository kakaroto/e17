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
   if (!val->IsObject()) {
        ELM_ERR("%s: value is not an object!", __FUNCTION__);
        return;
   }

   Local<Object> obj = val->ToObject();
   Local<Value> it = obj->Get(String::New("item"));
   Local<Value> lbl = obj->Get(String::New("label"));
   elm_object_part_text_set(eo, *String::Utf8Value(it), *String::Utf8Value(lbl));
}

Handle<Value> CElmBubble::corner_get() const
{
   return Number::New(elm_bubble_pos_get(eo));
}

void CElmBubble::corner_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_bubble_pos_set(eo, (Elm_Bubble_Pos)val->ToNumber()->Value());
}

template<> CEvasObject::CPropHandler<CElmBubble>::property_list CEvasObject::CPropHandler<CElmBubble>::list[] =
  {
     PROP_HANDLER(CElmBubble, text_part),
     PROP_HANDLER(CElmBubble, corner),
     { NULL, NULL, NULL },
  };
