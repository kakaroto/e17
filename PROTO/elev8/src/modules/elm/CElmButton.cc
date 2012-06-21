#include "elm.h"
#include "CElmButton.h"
#include "CElmIcon.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmButton, icon);
GENERATE_PROPERTY_CALLBACKS(CElmButton, autorepeat);

GENERATE_TEMPLATE(CElmButton,
                  PROPERTY(icon),
                  PROPERTY(autorepeat));

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

Handle<Value> CElmButton::autorepeat_get() const
{
   return Boolean::New(elm_button_autorepeat_get(eo));
}

void CElmButton::autorepeat_set(Handle<Value> value)
{
   if (value->IsBoolean())
      elm_button_autorepeat_set(eo, value->BooleanValue());
}

}
