#include "elm.h"
#include "CElmConform.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmConform, content);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmConform,
                  PROPERTY(content));

CElmConform::CElmConform(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_conformant_add(_parent->GetEvasObject()))
{
}

void CElmConform::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Conform"), GetTemplate()->GetFunction());
}

void CElmConform::content_set(Handle<Value> val)
{
   cached.content.Dispose();
   cached.content = Persistent<Value>::New(Realise(val, jsObject));
   elm_object_content_set(eo, GetEvasObjectFromJavascript(cached.content));
}

Handle<Value> CElmConform::content_get() const
{
   return cached.content;
}

CElmConform::~CElmConform()
{
   cached.content.Dispose();
}

}
