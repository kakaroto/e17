#include "CElmTable.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmTable, homogeneous);
GENERATE_PROPERTY_CALLBACKS(CElmTable, padding);

GENERATE_TEMPLATE_FULL(CElmObject, CElmTable,
                  PROPERTY(homogeneous),
                  PROPERTY(padding));

CElmTable::CElmTable(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_table_add(parent->GetEvasObject()))
{
}

void CElmTable::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Table"), GetTemplate()->GetFunction());
}

Handle<Value> CElmTable::Pack(Handle<Value> _value, Handle<Value> _replace)
{
   HandleScope scope;

   struct {
        Local<String> col;
        Local<String> row;
        Local<String> colspan;
        Local<String> rowspan;
        Local<String> element;
   } str = {
        String::NewSymbol("col"),
        String::NewSymbol("row"),
        String::NewSymbol("colspan"),
        String::NewSymbol("rowspan"),
        String::NewSymbol("element"),
   };

   Local<Object> value = _value->ToObject()->Clone();
   Local<Object> replace = _replace->IsUndefined() ? Object::New() : _replace->ToObject();
   Local<Value> element = value->Get(str.element);

   if (element->IsUndefined())
     return scope.Close(value);

   Local<Value> col = value->Get(str.col);
   if (col->IsUndefined())
     col = replace->Get(str.col);

   Local<Value> row = value->Get(str.row);
   if (row->IsUndefined())
     row = replace->Get(str.row);

   Local<Value> colspan = value->Get(str.colspan);
   if (colspan->IsUndefined())
     colspan = replace->Get(str.colspan);

   Local<Value> rowspan = value->Get(str.rowspan);
   if (rowspan->IsUndefined())
     rowspan = replace->Get(str.rowspan);

   if (colspan.IsEmpty() || !colspan->IsNumber())
     colspan = Integer::New(1);

   if (rowspan.IsEmpty() || !rowspan->IsNumber())
     rowspan = Integer::New(1);

   if (!col->IsNumber() || !row->IsNumber())
     {
        ELM_ERR("Coordinates not set or not a number? elem=%s x=%d, y=%d, w=%d or h=%d",
                *String::Utf8Value(element), col->IsNumber(), row->IsNumber(),
                colspan->IsNumber(), rowspan->IsNumber());
        return scope.Close(value);
     }

   Handle<Value> realised = Realise(element, GetJSObject());
   elm_table_pack(GetEvasObject(),
                  GetEvasObjectFromJavascript(realised),
                  col->IntegerValue(), row->IntegerValue(),
                  colspan->IntegerValue(), rowspan->IntegerValue());

   value->Set(str.element, realised);
   return scope.Close(value);
}

Handle<Value> CElmTable::Unpack(Handle<Value> item)
{
   HandleScope scope;
   Handle<Value> element = item->ToObject()->Get(String::New("element"));
   delete GetObjectFromJavascript(element);
   return scope.Close(item);
}

void CElmTable::homogeneous_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_table_homogeneous_set(eo, val->BooleanValue());
}

Handle<Value> CElmTable::homogeneous_get() const
{
   return Boolean::New(elm_table_homogeneous_get(eo));
}

void CElmTable::padding_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Object> obj = val->ToObject();
   Local<Value> x = obj->Get(String::NewSymbol("x"));
   Local<Value> y = obj->Get(String::NewSymbol("y"));

   if (!x->IsNumber() || !y->IsNumber())
     return;

   elm_table_padding_set(eo, x->NumberValue(), y->NumberValue());
}

Handle<Value> CElmTable::padding_get() const
{
   Evas_Coord x, y;

   elm_table_padding_get(eo, &x, &y);

   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("x"), Boolean::New(x));
   obj->Set(String::NewSymbol("y"), Boolean::New(y));

   return obj;
}

}
