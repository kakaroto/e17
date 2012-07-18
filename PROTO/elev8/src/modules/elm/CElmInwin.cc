#include "CElmInwin.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmInwin, content);
GENERATE_METHOD_CALLBACKS(CElmInwin, activate);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmInwin,
                  PROPERTY(content),
                  METHOD(activate));

CElmInwin::CElmInwin(Local<Object> _jsObject, CElmObject *p)
   : CElmLayout(_jsObject,
                elm_win_inwin_add(elm_object_top_widget_get(p->GetEvasObject())))
{
}

void CElmInwin::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Inwin"), GetTemplate()->GetFunction());
}

CElmInwin::~CElmInwin()
{
   cached.content.Dispose();
}

Handle<Value> CElmInwin::activate(const Arguments&)
{
   elm_win_inwin_activate(GetEvasObject());
   return Undefined();
}

Handle<Value> CElmInwin::content_get() const
{
   return cached.content;
}

void CElmInwin::content_set(Handle<Value> object)
{
   cached.content.Dispose();
   cached.content = Persistent<Value>::New(Realise(object, jsObject));
   elm_win_inwin_content_set(eo, GetEvasObjectFromJavascript(cached.content));
}

}
