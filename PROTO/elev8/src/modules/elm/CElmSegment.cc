#include "CElmSegment.h"

CElmSegment::CElmSegment(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_segment_control_add(parent->get());
   construct(eo, obj);
   //items_set(obj->Get(String::New("items")));
}

Handle<Object> CElmSegment::items_set(Handle<Value> val)
{
   /* add an list of children */
   Local<Object> out = Object::New();

   if (!val->IsObject())
     {
        ELM_ERR( "not an object!");
        return out;
     }

   Local<Object> in = val->ToObject();
   Local<Array> props = in->GetPropertyNames();

   /* iterate through elements and instantiate them */
   for (unsigned int i = 0; i < props->Length(); i++)
     {

        Local<Value> x = props->Get(Integer::New(i));
        String::Utf8Value val(x);

        Local<Value> item = in->Get(x->ToString());
        if (!item->IsObject())
          {
             // FIXME: permit adding strings here?
             ELM_ERR( "list item is not an object");
             continue;
          }
        Local<Value> label = item->ToObject()->Get(String::New("label"));

        String::Utf8Value str(label);
        elm_segment_control_item_add(eo, NULL, *str);
     }

   return out;
}

PROPERTIES_OF(CElmSegment) = NO_PROPERTIES;
