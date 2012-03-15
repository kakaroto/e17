#include "CElmLayout.h"

CElmLayout::CElmLayout(CEvasObject *parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   the_contents = Persistent <Object>::New(Object::New());
   eo = elm_layout_add(parent->top_widget_get());
   construct(eo, obj);
}

Handle<Value> CElmLayout::contents_get() const
{
   HandleScope scope;

   return scope.Close(the_contents);
}

void CElmLayout::contents_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Handle<Object> contents = val->ToObject();
   Handle<Array> properties = contents->GetPropertyNames();
   for (unsigned int i = 0; i <properties->Length(); i++)
     {
        Handle<Value> element = properties->Get(Integer::New(i));
        CEvasObject *child = make_or_get(this, contents->Get(element->ToString()));
        if (child)
          {
             elm_object_part_content_set(eo, *String::Utf8Value(element), child->get());
             the_contents->Set(element, child->get_object());
          }
     }
}

Handle<Value> CElmLayout::file_get() const
{
   // FIXME: implement
   return Undefined();
}

void CElmLayout::file_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Object> obj = val->ToObject();
   Local<Value> fileParam = obj->Get(String::New("name"));
   Local<Value> groupParam = obj->Get(String::New("group"));

   elm_layout_file_set(eo,
                       *String::Utf8Value(fileParam),
                       *String::Utf8Value(groupParam));
}

Handle <Value> CElmLayout::theme_get() const
{
   // FIXME: implement
   return Undefined();
}

void CElmLayout::theme_set(Handle <Value> val)
{
   if (!val->IsObject())
     return;

   Local<Object> obj = val->ToObject();
   Local<Value> classParam = obj->Get(String::New("class"));
   Local<Value> groupParam = obj->Get(String::New("group"));
   Local<Value> styleParam = obj->Get(String::New("style"));

   elm_layout_theme_set(eo,
                        *String::Utf8Value(classParam),
                        *String::Utf8Value(groupParam),
                        *String::Utf8Value(styleParam));
}

PROPERTIES_OF(CElmLayout) = {
   PROP_HANDLER(CElmLayout, file),
   PROP_HANDLER(CElmLayout, theme),
   PROP_HANDLER(CElmLayout, contents),
   { NULL }
};
