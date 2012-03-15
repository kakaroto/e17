#include "CElmSegment.h"

CElmSegment::CElmSegment(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_segment_control_add(parent->get());
   construct(eo, obj);

   items_set(obj->Get(String::New("items")));
}

void CElmSegment::items_set(Handle<Value> val)
{
   HandleScope scope;

   if (!val->IsObject())
     {
        ELM_ERR("not an object!");
        return;
     }

   Local<Object> in = val->ToObject();
   Local<Array> props = in->GetPropertyNames();

   /* iterate through elements and instantiate them */
   for (unsigned int i = 0; i < props->Length(); i++)
     {
        Local<Value> item = in->Get(props->Get(Integer::New(i))->ToString());
        if (!item->IsObject())
          {
             // FIXME: permit adding strings here?
             ELM_ERR("list item is not an object");
             continue;
          }

        Local<Value> label = item->ToObject()->Get(String::New("label"));
        elm_segment_control_item_add(eo, NULL, *String::Utf8Value(label));
     }
}

Handle<Value> CElmSegment::items_get() const
{
   return Undefined();
}

PROPERTIES_OF(CElmSegment) = {
   PROP_HANDLER(CElmSegment, items),
   { NULL }
};
