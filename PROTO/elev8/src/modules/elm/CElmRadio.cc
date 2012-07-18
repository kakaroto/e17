#include "CElmRadio.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmRadio, icon);
GENERATE_PROPERTY_CALLBACKS(CElmRadio, group);
GENERATE_PROPERTY_CALLBACKS(CElmRadio, value);
GENERATE_PROPERTY_CALLBACKS(CElmRadio, group_value);
GENERATE_RO_PROPERTY_CALLBACKS(CElmRadio, group_selected);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmRadio,
   PROPERTY(icon),
   PROPERTY(group),
   PROPERTY(value),
   PROPERTY(group_value),
   PROPERTY_RO(group_selected));

CElmRadio::CElmRadio(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_radio_add(_parent->GetEvasObject()))
   , parent(_parent)
{
}

void CElmRadio::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Radio"), GetTemplate()->GetFunction());
}

CElmRadio::~CElmRadio()
{
   cached.icon.Dispose();
   cached.group.Dispose();
}

Handle<Value> CElmRadio::icon_get() const
{
   return cached.icon;
}

void CElmRadio::icon_set(Handle<Value> value)
{
   cached.icon.Dispose();
   cached.icon = Persistent<Value>::New(Realise(value, jsObject));
   elm_object_content_set(eo, GetEvasObjectFromJavascript(cached.icon));
}

Handle<Value> CElmRadio::group_get() const
{
   return cached.group;
}

void CElmRadio::group_set(Handle<Value> value)
{
   if (!value->IsString())
     return;

   Handle<Object> js_parent = parent->GetJSObject();
   Local<Value> groups = js_parent->GetHiddenValue(String::NewSymbol("elm::radio::groups"));

   if (groups.IsEmpty())
     {
        groups = Object::New();
        js_parent->SetHiddenValue(String::NewSymbol("elm::radio::groups"), groups);
     }

   cached.group.Dispose();
   cached.group = Persistent<Value>::New(value);
   Local<Value> sibling = groups->ToObject()->Get(value);

   if (sibling == Undefined())
     groups->ToObject()->Set(value, jsObject, ReadOnly);
   else
     elm_radio_group_add(eo, GetEvasObjectFromJavascript(sibling));
}

Handle<Value> CElmRadio::value_get() const
{
   return Integer::New(elm_radio_state_value_get(eo));
}

void CElmRadio::value_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_radio_state_value_set(eo, value->Int32Value());
}

Handle<Value> CElmRadio::group_value_get() const
{
   return Integer::New(elm_radio_value_get(eo));
}

void CElmRadio::group_value_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_radio_value_set(eo, value->Int32Value());
}

Handle<Value> CElmRadio::group_selected_get() const
{
   Evas_Object *selected_item = elm_radio_selected_object_get(eo);

   CElmObject *selected = (CElmObject*)evas_object_data_get(selected_item, "this");

   return selected->GetJSObject();
}

}
