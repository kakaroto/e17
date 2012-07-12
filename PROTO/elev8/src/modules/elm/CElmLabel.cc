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
   String::Utf8Value mode_string(wrap->ToString());

   if (!strcmp(*mode_string, "none"))
     elm_label_line_wrap_set(eo, ELM_WRAP_NONE);
   else if (!strcmp(*mode_string, "char"))
     elm_label_line_wrap_set(eo, ELM_WRAP_CHAR);
   else if (!strcmp(*mode_string, "word"))
     elm_label_line_wrap_set(eo, ELM_WRAP_WORD);
   else if (!strcmp(*mode_string, "mixed"))
     elm_label_line_wrap_set(eo, ELM_WRAP_MIXED);
}

Handle<Value> CElmLabel::wrap_get() const
{
   switch (elm_label_line_wrap_get(eo)) {
     case ELM_WRAP_NONE:
       return String::NewSymbol("none");
     case ELM_WRAP_CHAR:
       return String::NewSymbol("char");
     case ELM_WRAP_WORD:
       return String::NewSymbol("word");
     case ELM_WRAP_MIXED:
       return String::NewSymbol("mixed");
     default:
       return String::NewSymbol("unknown");
   }
}

}
