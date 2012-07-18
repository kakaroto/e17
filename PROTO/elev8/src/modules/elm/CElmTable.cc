#include "CElmTable.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmTable, homogeneous);
GENERATE_PROPERTY_CALLBACKS(CElmTable, padding);
GENERATE_METHOD_CALLBACKS(CElmTable, pack);
GENERATE_METHOD_CALLBACKS(CElmTable, unpack);
GENERATE_METHOD_CALLBACKS(CElmTable, clear);

GENERATE_TEMPLATE(CElmTable,
                  PROPERTY(homogeneous),
                  PROPERTY(padding),
                  METHOD(pack),
                  METHOD(unpack),
                  METHOD(clear));

CElmTable::CElmTable(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_table_add(parent->GetEvasObject()))
{
}

void CElmTable::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Table"), GetTemplate()->GetFunction());
}

void CElmTable::DidRealiseElement(Local<Value> val)
{
   pack(val->ToObject());
}

void CElmTable::pack(Handle<Object> obj)
{
   Local<Value> x, y, colspan, rowspan;
   Handle<Value> element = obj->Get(String::NewSymbol("element"));

   if (element->IsUndefined())
     return;

   element = Realise(obj->Get(String::NewSymbol("element")), jsObject);
   obj->Set(String::NewSymbol("element"), element);

   x = obj->Get(String::NewSymbol("col"));
   y = obj->Get(String::NewSymbol("row"));
   colspan = obj->Get(String::NewSymbol("colspan"));
   rowspan = obj->Get(String::NewSymbol("rowspan"));

   if (colspan.IsEmpty() || !colspan->IsNumber())
     colspan = Integer::New(1);

   if (rowspan.IsEmpty() || !rowspan->IsNumber())
     rowspan = Integer::New(1);

   if (!x->IsNumber() || !y->IsNumber())
     {
        ELM_ERR("Coordinates not set or not a number? elem=%s x=%d, y=%d, w=%d or h=%d",
                *String::Utf8Value(obj), x->IsNumber(), y->IsNumber(),
                colspan->IsNumber(), rowspan->IsNumber());
        return;
     }

   elm_table_pack(GetEvasObject(), GetEvasObjectFromJavascript(element),
                  x->IntegerValue(), y->IntegerValue(),
                  colspan->IntegerValue(), rowspan->IntegerValue());
}

Handle<Value> CElmTable::pack(const Arguments &args)
{
   if (!args[0]->IsObject())
     return Undefined();

   Local<Object> desc = args[0]->ToObject();

   if (!desc->Has(String::NewSymbol("element")))
     {
        ELM_ERR("Need an elm element to be packed");
        return Undefined();
     }

   Local<Object> obj = desc->Clone();
   obj->Set(String::NewSymbol("element"),
            Realise(desc->Get(String::NewSymbol("element")), jsObject));
   pack(obj);

   return Undefined();
}

Handle<Value> CElmTable::Pack(Handle<Value> obj)
{
   pack(obj->ToObject());
   return obj;
}

Handle<Value> CElmTable::Unpack(Handle<Value> item)
{
   HandleScope scope;
   Handle<Value> element = item->ToObject()->Get(String::NewSymbol("element"));

   if (element->IsUndefined())
     return Undefined();

   CElmObject *obj = GetObjectFromJavascript(element);

   int col, row, colspan, rowspan;

   elm_table_pack_get(obj->GetEvasObject(), &col, &row, &colspan, &rowspan);
   Handle<Object> result = Object::New();
   result->Set(String::NewSymbol("col"), Integer::New(col));
   result->Set(String::NewSymbol("row"), Integer::New(row));
   result->Set(String::NewSymbol("colspan"), Integer::New(colspan));
   result->Set(String::NewSymbol("rowspan"), Integer::New(rowspan));
   result->Set(String::NewSymbol("element"), item);
   elm_table_unpack(eo, obj->GetEvasObject());

   return scope.Close(result);
}

Handle<Value> CElmTable::unpack(const Arguments&)
{
   ELM_DBG("unpack");
   return Undefined();
}

Handle<Value> CElmTable::clear(const Arguments&)
{
   ELM_DBG("clear");
   return Undefined();
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
