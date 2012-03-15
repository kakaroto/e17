#include "CElmInwin.h"

CElmInwin::CElmInwin(CEvasObject *parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_win_inwin_add(parent->top_widget_get());
   construct(eo, obj);

   get_object()->Set(String::NewSymbol("activate"), FunctionTemplate::New(activate)->GetFunction());
}

Handle<Value> CElmInwin::activate(const Arguments& args)
{
   CElmInwin *inwin = static_cast<CElmInwin *>(eo_from_info(args.This()));

   elm_win_inwin_activate(inwin->get());
   return Undefined();
}

Handle<Value> CElmInwin::content_get() const
{
   Evas_Object *content = elm_win_inwin_content_get(eo);
   if (!content)
     return Undefined();
   CEvasObject *content_obj = static_cast<CEvasObject*>(evas_object_data_get(content, "cppobj"));
   if (content_obj)
     return content_obj->get_object();
   return Undefined();
}

void CElmInwin::content_set(Handle<Value> object)
{
   CEvasObject *content = make_or_get(this, object);
   if (content)
     elm_win_inwin_content_set(eo, content->get());
}

PROPERTIES_OF(CElmInwin) = {
   PROP_HANDLER(CElmInwin, content),
   { NULL },
};
