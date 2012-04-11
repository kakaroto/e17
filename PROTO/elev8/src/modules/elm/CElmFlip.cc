#include "CElmFlip.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmFlip, front);
GENERATE_PROPERTY_CALLBACKS(CElmFlip, back);
GENERATE_METHOD_CALLBACKS(CElmFlip, flip);

GENERATE_TEMPLATE(CElmFlip,
                  PROPERTY(front),
                  PROPERTY(back),
                  METHOD(flip));

CElmFlip::CElmFlip(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_flip_add(parent->GetEvasObject()))
{
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
                               GetEvasObjectFromJavascript<CElmObject>(cached.front));
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
                               GetEvasObjectFromJavascript<CElmObject>(cached.back));
}

}
