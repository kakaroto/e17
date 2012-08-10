#include "elm.h"
#include "CElmBox.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmBox, horizontal);
GENERATE_PROPERTY_CALLBACKS(CElmBox, homogeneous);

GENERATE_TEMPLATE_FULL(CElmObject, CElmBox,
                       PROPERTY(horizontal),
                       PROPERTY(homogeneous));

CElmBox::CElmBox(Local <Object> _jsObject, CElmObject *parent)
    : CElmObject(_jsObject, elm_box_add(parent->GetEvasObject()))
{
}

void CElmBox::horizontal_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_box_horizontal_set(eo, val->BooleanValue());
}

Handle<Value> CElmBox::horizontal_get() const
{
   return Boolean::New(elm_box_horizontal_get(eo));
}

void CElmBox::homogeneous_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_box_homogeneous_set(eo, val->BooleanValue());
}

Handle<Value> CElmBox::homogeneous_get() const
{
   return Boolean::New(elm_box_homogeneous_get(eo));
}

Handle<Value> CElmBox::Pack(Handle<Value> obj, Handle<Value>)
{
   Local<Value> before = obj->ToObject()->Get(String::NewSymbol("before"));

   if (before->IsUndefined() && !replace->IsUndefined())
     before = replace->ToObject()->Get(String::NewSymbol("before"));
   else if (before->IsString() || before->IsNumber())
     before = GetJSObject()->Get(String::NewSymbol("elements"))->ToObject()->Get(before);

   obj = Realise(obj, GetJSObject());

   if (before->IsUndefined())
      elm_box_pack_end(eo, GetEvasObjectFromJavascript(obj));
   else
      elm_box_pack_before(eo, GetEvasObjectFromJavascript(obj),
                          GetEvasObjectFromJavascript(before));

   return obj;
}

Handle<Value> CElmBox::Unpack(Handle<Value> obj)
{
   delete GetObjectFromJavascript(obj);
   return obj;
}

void CElmBox::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Box"), GetTemplate()->GetFunction());
}

}
