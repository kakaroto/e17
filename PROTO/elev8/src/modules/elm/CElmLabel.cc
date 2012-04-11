#include "CElmLabel.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmLabel, wrap);

GENERATE_TEMPLATE(CElmLabel,
                  PROPERTY(wrap));

CElmLabel::CElmLabel(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_label_add(parent->GetEvasObject()))
{
}

void CElmLabel::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Label"), GetTemplate()->GetFunction());
}

void CElmLabel::wrap_set(Handle<Value> wrap)
{
   if (wrap->IsNumber())
     elm_label_line_wrap_set(eo, static_cast<Elm_Wrap_Type>(wrap->Int32Value()));
}

Handle<Value> CElmLabel::wrap_get() const
{
   HandleScope scope;

   return scope.Close(Integer::New(elm_label_line_wrap_get(eo)));
}

}
