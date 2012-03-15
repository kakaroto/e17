#include "CElmFlip.h"

CElmFlip::CElmFlip(CEvasObject* parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_flip_add(parent->get());
   construct(eo, obj);

   get_object()->Set(String::NewSymbol("flip"), FunctionTemplate::New(do_flip)->GetFunction());
}

Handle<Value> CElmFlip::do_flip(const Arguments& args)
{
   CEvasObject *self = eo_from_info(args.This());
   CElmFlip *flipper = static_cast <CElmFlip *>(self);
   flipper->flip(ELM_FLIP_ROTATE_Y_CENTER_AXIS);
   return Undefined();
}

void CElmFlip::flip(Elm_Flip_Mode mode)
{
   elm_flip_go(eo, mode);
}

Handle<Value> CElmFlip::front_get() const
{
   Evas_Object *front = elm_object_part_content_get(eo, "front");
   if (!front)
     return Undefined();
   CEvasObject *front_obj = static_cast<CEvasObject*>(evas_object_data_get(front, "cppobj"));
   if (front_obj)
     return front_obj->get_object();
   return Undefined();
}

void CElmFlip::front_set(Handle<Value> object)
{
   CEvasObject *front = make_or_get(this, object);
   if (front)
     elm_object_part_content_set(eo, "front", front->get());
}

Handle<Value> CElmFlip::back_get() const
{
   Evas_Object *back = elm_object_part_content_get(eo, "back");
   if (!back)
     return Undefined();
   CEvasObject *back_obj = static_cast<CEvasObject*>(evas_object_data_get(back, "cppobj"));
   if (back_obj)
     return back_obj->get_object();
   return Undefined();
}

void CElmFlip::back_set(Handle<Value> object)
{
   CEvasObject *back = make_or_get(this, object);
   if (back)
     elm_object_part_content_set(eo, "back", back->get());
}

PROPERTIES_OF(CElmFlip) = {
   PROP_HANDLER(CElmFlip, front),
   PROP_HANDLER(CElmFlip, back),
   { NULL },
};
