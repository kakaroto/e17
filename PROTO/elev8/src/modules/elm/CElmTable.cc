#include "CElmTable.h"

CElmTable::CElmTable(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_table_add(parent->top_widget_get());
   construct(eo, obj);
   get_object()->Set(String::New("unpack"), FunctionTemplate::New(unpack)->GetFunction());
   get_object()->Set(String::New("pack"), FunctionTemplate::New(pack)->GetFunction());
   get_object()->Set(String::New("clear"), FunctionTemplate::New(clear)->GetFunction());
   items_set(obj->Get(String::New("subobjects")));
}

Handle<Value> CElmTable::pack(const Arguments& args)
{
   CEvasObject *self = eo_from_info(args.This());
   CElmTable *table = static_cast<CElmTable *>(self);
   if (args[0]->IsObject())
     {
        return table->new_item_set(args[0]);
     }
   return Undefined();
}

Handle<Value> CElmTable::unpack(const Arguments&)
{
   return Undefined();
}

Handle<Value> CElmTable::clear(const Arguments& args)
{
   CEvasObject *self = eo_from_info(args.This());
   CElmTable *table = static_cast<CElmTable *>(self);
   elm_table_clear(table->get(), true);
   table->table_items.clear();
   return Undefined();
}

void CElmTable::items_set(Handle<Value> val)
{
   if (!val->IsObject())
     {
        ELM_ERR( "not an object!");
        return;
     }

   Local<Object> in = val->ToObject();
   Local<Array> props = in->GetPropertyNames();

   /* iterate through elements and instantiate them */
   for (unsigned int i = 0; i < props->Length(); i++)
     {
        Local<Value> x = props->Get(Integer::New(i));
        String::Utf8Value val(x);

        Local<Value> item = in->Get(x->ToString());

        new_item_set(item);
     }
}

Handle<Value> CElmTable::new_item_set(Handle<Value> item)
{
   CEvasObject *child = NULL;
   if (!item->IsObject())
     {
        // FIXME: permit adding strings here?
        ELM_ERR( "list item is not an object");
        return Undefined();
     }

   Local<Value> subobj = item->ToObject()->Get(String::New("subobject"));

   if ( subobj->IsObject())
     {
        child = make_or_get(this, subobj);
        if(!child)
          return Undefined();
     }
   else
     {
        return Undefined();
     }

   Local<Value> xpos = item->ToObject()->Get(String::New("x"));
   Local<Value> ypos = item->ToObject()->Get(String::New("y"));
   Local<Value> width = item->ToObject()->Get(String::New("w"));
   Local<Value> height = item->ToObject()->Get(String::New("h"));

   if (!xpos->IsNumber() || !ypos->IsNumber() || !width->IsNumber() ||
       !height->IsNumber())
     {
        ELM_ERR("Coordinates not set or not a number? x=%d, y=%d, w=%d or h=%d",
                xpos->IsNumber(), ypos->IsNumber(), width->IsNumber(),
                height->IsNumber());
        return Undefined();
     }

   int x,y,w,h;

   x = xpos->IntegerValue();
   y = ypos->IntegerValue();
   w = width->IntegerValue();
   h = height->IntegerValue();

   elm_table_pack(this->get(), child->get(), x, y, w, h);
   ELM_INF("Packing new table item at %d %d %d %d", x,y,w,h);
   table_items.push_back(child);
   return child->get_object();
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
   HandleScope handle_scope;
   if (!val->IsObject())
     return;
   Local<Object> obj = val->ToObject();
   Local<Value> x = obj->Get(String::New("x"));
   Local<Value> y = obj->Get(String::New("y"));
   if (!x->IsNumber() || !y->IsNumber())
     return;
   elm_table_padding_set(eo, x->NumberValue(), y->NumberValue());
}

Handle<Value> CElmTable::padding_get() const
{
   Evas_Coord x, y;
   elm_table_padding_get(eo, &x, &y);
   Local<Object> obj = Object::New();
   obj->Set(String::New("x"), Boolean::New(x));
   obj->Set(String::New("y"), Boolean::New(y));
   return obj;
}

PROPERTIES_OF(CElmTable) = {
     PROP_HANDLER(CElmTable, homogeneous),
     PROP_HANDLER(CElmTable, padding),
     { NULL }
};


