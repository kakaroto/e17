#include "CElmBubble.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmBubble, text_part);
GENERATE_PROPERTY_CALLBACKS(CElmBubble, corner);
GENERATE_PROPERTY_CALLBACKS(CElmBubble, content);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmBubble,
                  PROPERTY(text_part),
                  PROPERTY(corner),
                  PROPERTY(content));

CElmBubble::CElmBubble(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_bubble_add(_parent->GetEvasObject()))
{
   jsObject->SetHiddenValue(String::NewSymbol("content"), Undefined());
}

void CElmBubble::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Bubble"), GetTemplate()->GetFunction());
}

CElmBubble::~CElmBubble()
{
   cached.content.Dispose();
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
   Local<Value> it = obj->Get(String::NewSymbol("item"));
   Local<Value> text = obj->Get(String::NewSymbol("text"));

   elm_object_part_text_set(eo, *String::Utf8Value(it),
                            *String::Utf8Value(text));
}

Handle<Value> CElmBubble::corner_get() const
{
   const char *corner_to_string[] =
     { "top_left", "top_right", "bottom_left", "bottom_right", NULL };
   return String::New(corner_to_string[elm_bubble_pos_get(eo)]);
}

void CElmBubble::corner_set(Handle<Value> val)
{
   const char *corner_to_string[] =
     { "top_left", "top_right", "bottom_left", "bottom_right", NULL };

   String::Utf8Value new_corner(val);

   for (unsigned int i = 0; corner_to_string[i]; i++)
     if (!strcmp(*new_corner, corner_to_string[i]))
        elm_bubble_pos_set(eo, (Elm_Bubble_Pos)i);
}

Handle<Value> CElmBubble::content_get() const
{
   return cached.content;
}

void CElmBubble::content_set(Handle<Value> val)
{
   cached.content.Dispose();
   cached.content = Persistent<Value>::New(Realise(val, jsObject));
   elm_object_content_set(eo, GetEvasObjectFromJavascript(cached.content));
}

}
