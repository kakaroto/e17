#include "CElmNaviframe.h"

CElmNaviframe::CElmNaviframe(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_naviframe_add(parent->top_widget_get());
   construct(eo, obj);
   get_object()->Set(String::New("pop"), FunctionTemplate::New(pop)->GetFunction());
   get_object()->Set(String::New("push"), FunctionTemplate::New(push)->GetFunction());
}

Handle<Value> CElmNaviframe::pop(const Arguments& args)
{
   CEvasObject *self = eo_from_info(args.This());
   CElmNaviframe *naviFrame = static_cast<CElmNaviframe *>(self);

   if (elm_naviframe_top_item_get(naviFrame->get()))
     elm_naviframe_item_pop(naviFrame->get());

   return Undefined();
}

Handle<Value> CElmNaviframe::push(const Arguments& args)
{
   CEvasObject *self = eo_from_info(args.This());
   CElmNaviframe *naviFrame = static_cast<CElmNaviframe *>(self);
   CEvasObject *prev_btn = NULL, *next_btn = NULL, *content;
   bool has_prev_btn = false, has_next_btn = false;

   if (!args[0]->IsObject())
     return ThrowException(Exception::Error(String::New("Parameter 1 should be an object description or an elm.widget")));

   if (!args[1]->IsString())
     return ThrowException(Exception::Error(String::New("Parameter 2 should be a string")));

   if (args.Length() >= 3) {
        if (!args[2]->IsObject())
          return ThrowException(Exception::Error(String::New("Parameter 3 should either be undefined or an object description")));
        has_prev_btn = true;
   }

   if (args.Length() >= 4) {
        if (!args[3]->IsObject())
          return ThrowException(Exception::Error(String::New("Parameter 4 should either be undefined or an object description")));
        has_next_btn = true;
   }

   content = make_or_get(naviFrame, args[0]);
   if (!content)
     return ThrowException(Exception::Error(String::New("Could not create content from description")));

   if (has_prev_btn)
     {
        prev_btn = make_or_get(naviFrame, args[2]->ToObject());
        if (!prev_btn)
          return ThrowException(Exception::Error(String::New("Could not create back button from description")));
     }
   if (has_next_btn)
     {
        next_btn = make_or_get(naviFrame, args[3]->ToObject());
        if (!next_btn)
          return ThrowException(Exception::Error(String::New("Could not create next button from description")));
     }

   String::Utf8Value titleParam(args[1]->ToString());
   elm_naviframe_item_push(naviFrame->get(),
                           *titleParam,
                           has_prev_btn ? prev_btn->get() : 0,
                           has_next_btn ? next_btn->get() : 0,
                           content->get(),
                           0);
   return Undefined();
}

PROPERTIES_OF(CElmNaviframe) = NO_PROPERTIES;
