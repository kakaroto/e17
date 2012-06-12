#include "elm.h"
#include "CElmToolbar.h"

namespace elm {

using namespace v8;

GENERATE_METHOD_CALLBACKS(CElmToolbar, append);

GENERATE_TEMPLATE(CElmToolbar,
                  METHOD(append));

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

}
