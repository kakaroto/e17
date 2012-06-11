#include "elm.h"
#include "CElmToolbar.h"

namespace elm {

using namespace v8;

GENERATE_METHOD_CALLBACKS(CElmToolbar, append);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, always_select);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, no_select);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, icon_size);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, item_priority);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, item_separator);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, homogeneous);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, align);

GENERATE_TEMPLATE(CElmToolbar,
                  METHOD(append),
                  PROPERTY(always_select),
                  PROPERTY(no_select),
                  PROPERTY(icon_size),
                  PROPERTY(item_priority),
                  PROPERTY(item_separator),
                  PROPERTY(homogeneous),
                  PROPERTY(align));

CElmToolbar::CElmToolbar(Local <Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_toolbar_add(parent->GetEvasObject()))
{
}

void CElmToolbar::DidRealiseElement(Local<Value> val)
{
   Local<Object> obj = val->ToObject();
   append(obj->Get(String::NewSymbol("icon")), obj->Get(String::NewSymbol("label")),
          obj->Get(String::NewSymbol("data")), obj->Get(String::NewSymbol("on_select")));
}

void CElmToolbar::append(Local<Value> icon, Local<Value> label,
                         Local<Value> data, Local<Value> callback)
{
   elm_toolbar_item_append(eo,
                           icon->IsUndefined() ? NULL : *String::Utf8Value(icon),
                           label->IsUndefined() ? NULL : *String::Utf8Value(label),
                           OnSelect, new Item(jsObject, data, callback));
}

Handle<Value> CElmToolbar::append(const Arguments& args)
{
   append(args[0], args[1], args[2], args[3]);
   return Undefined();
}

void CElmToolbar::OnSelect(void *data, Evas_Object *, void *)
{
   Item *item = static_cast<Item *>(data);

   if (!item->callback->IsFunction())
     return;

   HandleScope scope;
   Local<Function> callback(Function::Cast(*item->callback));

   if (item->data->IsUndefined())
     {
        callback->Call(item->self, 0, NULL);
     }
   else
     {
        Handle<Value> args[1] = { item->data };
        callback->Call(item->self, 1, args);
     }

}

void CElmToolbar::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Toolbar"), GetTemplate()->GetFunction());
}

void CElmToolbar::always_select_set(Handle<Value> value)
{
   if (value->IsBoolean())
      elm_toolbar_always_select_mode_set(eo, value->BooleanValue());
}

Handle<Value> CElmToolbar::always_select_get() const
{
   return Boolean::New(elm_toolbar_always_select_mode_get(eo));
}

void CElmToolbar::no_select_set(Handle<Value> value)
{
   if (value->IsBoolean())
      elm_toolbar_no_select_mode_set(eo, value->BooleanValue());
}

Handle<Value> CElmToolbar::no_select_get() const
{
   return Boolean::New(elm_toolbar_no_select_mode_get(eo));
}

void CElmToolbar::icon_size_set(Handle<Value> value)
{
   if (value->IsInt32())
      elm_toolbar_icon_size_set(eo, value->Int32Value());
}

Handle<Value> CElmToolbar::icon_size_get() const
{
   return int::New(elm_toolbar_icon_size_get(eo));
}

void CElmToolbar::item_priority_set(Handle<Value> value)
{
   if (value->IsInt32())
      elm_toolbar_item_priority_set(eo, value->Int32Value());
}

Handle<Value> CElmToolbar::item_priority_get() const
{
   return int::New(elm_toolbar_item_priority_get(eo));
}

void CElmToolbar::item_separator_set(Handle<Value> value)
{
   if (value->IsBoolean())
      elm_toolbar_item_separator_set(it, value->BooleanValue());
}

Handle<Value> CElmToolbar::item_separator_get() const
{
   return Boolean::New(elm_toolbar_item_separator_get(it));
}

void CElmToolbar::homogeneous_set(Handle<Value> value)
{
   if (value->IsBoolean())
      elm_toolbar_homogeneous_set(eo, value->BooleanValue());
}

Handle<Value> CElmToolbar::homogeneous_get() const
{
   return Boolean::New(elm_toolbar_homogeneous_get(eo));
}

void CElmToolbar::align_set(Handle<Value> value)
{
   if (value->IsNumber())
      elm_toolbar_align_set(eo, value->NumberValue());
}

Handle<Value> CElmToolbar::align_get() const
{
   return double::New(elm_toolbar_align_get(eo));
}

}
