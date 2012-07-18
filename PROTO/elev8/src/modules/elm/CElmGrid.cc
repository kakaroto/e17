#include "CElmGrid.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmGrid, size);

GENERATE_TEMPLATE_FULL(CElmObject, CElmGrid,
                  PROPERTY(size));

CElmGrid::CElmGrid(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_grid_add(parent->GetEvasObject()))
{
}

void CElmGrid::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Grid"), GetTemplate()->GetFunction());
}

Handle<Value> CElmGrid::Pack(Handle<Value> value, Handle<Value> _replace)
{
   HandleScope scope;

   struct {
        Local<String> x;
        Local<String> y;
        Local<String> w;
        Local<String> h;
        Local<String> element;
   } str = {
        String::NewSymbol("x"),
        String::NewSymbol("y"),
        String::NewSymbol("width"),
        String::NewSymbol("height"),
        String::NewSymbol("element"),
   };

   if (!value->IsObject())
     return Undefined();

   Handle<Object> obj = value->ToObject()->Clone();
   Handle<Value> element = obj->Get(str.element);
   Handle<Object> replace = _replace->IsObject() ? _replace->ToObject() : Object::New();

   if (element->IsUndefined())
     return Undefined();

   Local<Value> x = obj->Get(str.x);
   if (x->IsUndefined())
     x = replace->Get(str.x);

   Local<Value> y = obj->Get(str.y);
   if (y->IsUndefined())
     y = replace->Get(str.y);

   Local<Value> w = obj->Get(str.w);
   if (w->IsUndefined())
     w = replace->Get(str.w);

   Local<Value> h = obj->Get(str.h);
   if (h->IsUndefined())
     h = replace->Get(str.h);

   element = Realise(element, jsObject);
   obj->Set(String::NewSymbol("element"), element);

   elm_grid_pack(GetEvasObject(),
                 GetEvasObjectFromJavascript(element),
                 x->IntegerValue(),
                 y->IntegerValue(),
                 w->IntegerValue(),
                 h->IntegerValue());

   return scope.Close(obj);
}

Handle<Value> CElmGrid::Unpack(Handle<Value> item)
{
   HandleScope scope;
   Handle<Value> element = item->ToObject()->Get(String::New("element"));
   delete GetObjectFromJavascript(element);
   return scope.Close(item);
}

void CElmGrid::size_set(Handle<Value> val)
{
   Local<Object> obj = val->ToObject();
   Local<Value> x = obj->Get(String::NewSymbol("x"));
   Local<Value> y = obj->Get(String::NewSymbol("y"));

   if (x.IsEmpty() || !x->IsNumber())
     return;

   if (y.IsEmpty() || !y->IsNumber())
     return;

   elm_grid_size_set(eo, x->IntegerValue(), y->IntegerValue());
}

Handle<Value> CElmGrid::size_get() const
{
   HandleScope scope;

   int x, y;
   elm_grid_size_get (eo, &x, &y);

   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("x"), Number::New(x));
   obj->Set(String::NewSymbol("y"), Number::New(y));

   return scope.Close(obj);
}

}
