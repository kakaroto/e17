#include "CElmFlip.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmFlip, front);
GENERATE_PROPERTY_CALLBACKS(CElmFlip, back);
GENERATE_PROPERTY_CALLBACKS(CElmFlip, perspective);
GENERATE_RO_PROPERTY_CALLBACKS(CElmFlip, front_visible);
GENERATE_METHOD_CALLBACKS(CElmFlip, flip);

GENERATE_TEMPLATE_FULL(CElmContainer, CElmFlip,
                  PROPERTY(front),
                  PROPERTY(back),
                  PROPERTY(perspective),
                  PROPERTY_RO(front_visible),
                  METHOD(flip));

CElmFlip::CElmFlip(Local<Object> _jsObject, CElmObject *p)
   : CElmContainer(_jsObject, elm_flip_add(p->GetEvasObject()))
{
}

CElmFlip::~CElmFlip()
{
   cached.front.Dispose();
   cached.back.Dispose();
   perspective.Dispose();
}

void CElmFlip::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Flip"),
               GetTemplate()->GetFunction());
}

Handle<Value> CElmFlip::flip(const Arguments&)
{
   elm_flip_go(eo, ELM_FLIP_ROTATE_Y_CENTER_AXIS);
   return Undefined();
}

Handle<Value> CElmFlip::front_get() const
{
   return cached.front;
}

void CElmFlip::front_set(Handle<Value> object)
{
   cached.front.Dispose();
   cached.front = Persistent<Value>::New(Realise(object, jsObject));
   elm_object_part_content_set(eo, "front",
                               GetEvasObjectFromJavascript(cached.front));
}

Handle<Value> CElmFlip::back_get() const
{
   return cached.back;
}

void CElmFlip::back_set(Handle<Value> object)
{
   cached.back.Dispose();
   cached.back = Persistent<Value>::New(Realise(object, jsObject));
   elm_object_part_content_set(eo, "back",
                               GetEvasObjectFromJavascript(cached.back));
}

Handle<Value> CElmFlip::perspective_get() const
{
   return perspective;
}

void CElmFlip::perspective_set(Handle<Value> value)
{
   if(!value->IsArray())
     return;

   Local<Object> sizes = value->ToObject();
   elm_flip_perspective_set(eo,
        sizes->Get(0)->ToNumber()->Value(),
        sizes->Get(1)->ToNumber()->Value(),
        sizes->Get(2)->ToNumber()->Value());

   perspective.Dispose();
   perspective = Persistent<Value>::New(value);
}

Handle<Value> CElmFlip::front_visible_get() const
{
   return Boolean::New(elm_flip_front_visible_get(eo));
}

}
