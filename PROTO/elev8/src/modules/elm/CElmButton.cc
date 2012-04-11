#include "elm.h"
#include "CElmButton.h"
#include "CElmIcon.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmButton, icon);

GENERATE_TEMPLATE(CElmButton,
                  PROPERTY(icon));

CElmButton::CElmButton(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_button_add(parent->GetEvasObject()))
{
}

void CElmButton::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Button"),
               GetTemplate()->GetFunction());
}

CElmButton::~CElmButton()
{
   cached.icon.Dispose();
}

Handle<Value> CElmButton::icon_get() const
{
   return cached.icon;
}

void CElmButton::icon_set(Handle<Value> value)
{
   cached.icon.Dispose();

   cached.icon = Persistent<Value>::New(Realise(value, jsObject));
   elm_object_content_set(eo,
                          GetEvasObjectFromJavascript(cached.icon));
}

}
