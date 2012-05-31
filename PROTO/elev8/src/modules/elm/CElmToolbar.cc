#include "elm.h"
#include "CElmToolbar.h"

namespace elm {

using namespace v8;

GENERATE_METHOD_CALLBACKS(CElmToolbar, append);
GENERATE_TEMPLATE(CElmToolbar, METHOD(append));

CElmToolbar::CElmToolbar(Local <Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_toolbar_add(parent->GetEvasObject()))
{
}

Handle<Value> CElmToolbar::append(const Arguments& args)
{
   Local<Value> icon = args[0];
   Local<Value> label = args[1];

   elm_toolbar_item_append(eo,
                           icon->IsUndefined() ? NULL : *String::Utf8Value(icon),
                           label->IsUndefined() ? NULL : *String::Utf8Value(label),
                           OnSelect, new Item(args[2], args[3]));

   return Undefined();
}

void CElmToolbar::OnSelect(void *data, Evas_Object *, void *)
{
   Item *item = static_cast<Item *>(data);

   if (!item->callback->IsFunction())
     return;

   HandleScope scope;
   Local<Function> callback(Function::Cast(*item->callback));
   Handle<Value> args[1] = { item->data };
   callback->Call(item->data->ToObject(), 1, args);
}

void CElmToolbar::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Toolbar"), GetTemplate()->GetFunction());
}

}
