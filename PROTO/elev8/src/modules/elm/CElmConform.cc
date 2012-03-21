#include "CElmConform.h"

CElmConform::CElmConform(CEvasObject *parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_conformant_add(parent->top_widget_get());
   construct(eo, obj);
}

void CElmConform::content_set(Handle<Value> val)
{
   CEvasObject *content = make_or_get(this, val);
   if (content)
     elm_object_content_set(eo, content->get());
}

Handle<Value> CElmConform::content_get() const
{
   Evas_Object *content = elm_object_content_get(eo);
   if (!content)
     return Undefined();
   CEvasObject *content_obj = static_cast<CEvasObject*>(evas_object_data_get(content, "cppobj"));
   if (content_obj)
     return content_obj->get_object();
   return Undefined();
}

CElmConform::~CElmConform()
{
}

PROPERTIES_OF(CElmConform) = {
   PROP_HANDLER(CElmConform, content),
   { NULL }
};
