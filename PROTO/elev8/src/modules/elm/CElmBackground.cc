#include "CElmBackground.h"

CElmBackground::CElmBackground(CEvasObject *parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_bg_add(parent->get());
   construct(eo, obj);
}

void CElmBackground::image_set(Handle<Value> val)
{
   if (val->IsString())
      elm_bg_file_set(eo, *String::Utf8Value(val), NULL);
}

Handle<Value> CElmBackground::image_get(void) const
{
   const char *file = NULL;
   elm_bg_file_get(eo, &file, NULL);

   if (file)
     return String::New(file);

   return Null();
}

Handle<Value> CElmBackground::red_get() const
{
   int r;
   elm_bg_color_get(eo, &r, NULL, NULL);
   return Number::New(r);
}

void CElmBackground::red_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   int g, b;
   elm_bg_color_get(eo, NULL, &g, &b);
   elm_bg_color_set(eo, val->ToNumber()->Value(), g, b);
}

Handle<Value> CElmBackground::green_get() const
{
   int g;
   elm_bg_color_get(eo, NULL, &g, NULL);
   return Number::New(g);
}

void CElmBackground::green_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   int r, b;
   elm_bg_color_get(eo, &r, NULL, &b);
   elm_bg_color_set(eo, r, val->ToNumber()->Value(), b);
}

Handle<Value> CElmBackground::blue_get() const
{
   int b;
   elm_bg_color_get(eo, NULL, NULL, &b);
   return Number::New(b);
}

void CElmBackground::blue_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   int r, g;
   elm_bg_color_get(eo, &r, &g, NULL);
   elm_bg_color_set(eo, r, g, val->ToNumber()->Value());
}

PROPERTIES_OF(CElmBackground) = {
    PROP_HANDLER(CElmBackground, red),
    PROP_HANDLER(CElmBackground, green),
    PROP_HANDLER(CElmBackground, blue),
    { NULL }
};
