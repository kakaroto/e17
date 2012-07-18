#include "elm.h"
#include "CElmCheck.h"
#include "CElmIcon.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmCheck, on_change);
GENERATE_PROPERTY_CALLBACKS(CElmCheck, state);
GENERATE_PROPERTY_CALLBACKS(CElmCheck, icon);
GENERATE_PROPERTY_CALLBACKS(CElmCheck, onlabel);
GENERATE_PROPERTY_CALLBACKS(CElmCheck, offlabel);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmCheck,
                  PROPERTY(on_change),
                  PROPERTY(state),
                  PROPERTY(icon),
                  PROPERTY(onlabel),
                  PROPERTY(offlabel));

CElmCheck::CElmCheck(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_check_add(_parent->GetEvasObject()))
{
}

void CElmCheck::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Check"),
               GetTemplate()->GetFunction());
}

CElmCheck::~CElmCheck()
{
   cached.icon.Dispose();
   on_change_set(Undefined());
}

void CElmCheck::OnChange(void *)
{
   HandleScope scope;
   Local<Function> callback(Function::Cast(*cb.change));
   Handle<Value> args[1] = { jsObject };

   callback->Call(jsObject, 1, args);
}

void CElmCheck::OnChangeWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmCheck*>(data)->OnChange(event_info);
}

Handle<Value> CElmCheck::on_change_get(void) const
{
   return cb.change;
}

void CElmCheck::on_change_set(Handle<Value> val)
{
   if (!cb.change.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "changed", &OnChangeWrapper);
        cb.change.Dispose();
        cb.change.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.change = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "changed", &OnChangeWrapper, this);
}

Handle<Value> CElmCheck::state_get() const
{
   return Boolean::New(elm_check_state_get(eo));
}

void CElmCheck::state_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_check_state_set(eo, value->BooleanValue());
}

Handle<Value> CElmCheck::icon_get() const
{
   return cached.icon;
}

void CElmCheck::icon_set(Handle<Value> value)
{
   cached.icon.Dispose();

   cached.icon = Persistent<Value>::New(Realise(value, jsObject));
   elm_object_content_set(eo,
                          GetEvasObjectFromJavascript(cached.icon));
}

void CElmCheck::onlabel_set(Handle<Value> val)
{
   if (val->IsString())
     elm_object_part_text_set(eo, "on", *String::Utf8Value(val));
}

Handle<Value> CElmCheck::onlabel_get(void) const
{
   const char *onlabel = elm_object_part_text_get(eo, "on");
   return onlabel ? String::New(onlabel) : Null();
}

void CElmCheck::offlabel_set(Handle<Value> val)
{
   if (val->IsString())
     elm_object_part_text_set(eo, "off", *String::Utf8Value(val));
}

Handle<Value> CElmCheck::offlabel_get(void) const
{
   const char *offlabel = elm_object_part_text_get(eo, "off");
   return offlabel ? String::New(offlabel) : Null();
}

}
