#include "CElmImage.h"

CElmImage::CElmImage(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_image_add(parent->top_widget_get());
   construct(eo, obj);
}

void CElmImage::resize_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        Evas_Object *parent = elm_object_top_widget_get(eo);
        if (!parent)
          ELM_ERR( "resize object has no parent!");
        else
          {
             is_resize = val->BooleanValue();
             if (is_resize)
               elm_win_resize_object_add(parent, eo);
             else
               elm_win_resize_object_del(parent, eo);
          }
     }
   else
     ELM_ERR( "Resize value not boolean!");
}


void CElmImage::file_set(Handle<Value> val)
{
   if (val->IsString())
     {
        String::Utf8Value str(val);
        if (0 > access(*str, R_OK))
          ELM_WRN( "warning: can't read image file %s", *str);
        elm_image_file_set(eo, *str, NULL);
     }
   ELM_ERR("Value is not string.\n");
}

Handle<Value> CElmImage::file_get(void) const
{
   const char *f = NULL, *key = NULL;
   elm_image_file_get(eo, &f, &key);
   if (f)
     return String::New(f);
   else
     return Null();
}

Handle<Value> CElmImage::smooth_get() const
{
   return Boolean::New(elm_image_smooth_get(eo));
}

void CElmImage::smooth_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_image_smooth_set(eo, val->BooleanValue());
}

Handle<Value> CElmImage::no_scale_get() const
{
   return Boolean::New(elm_image_no_scale_get(eo));
}

void CElmImage::no_scale_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_image_no_scale_set(eo, val->BooleanValue());
}

Handle<Value> CElmImage::fill_outside_get() const
{
   return Boolean::New(elm_image_fill_outside_get(eo));
}

void CElmImage::fill_outside_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_image_fill_outside_set(eo, val->BooleanValue());
}

Handle<Value> CElmImage::editable_get() const
{
   return Boolean::New(elm_image_editable_get(eo));
}

void CElmImage::CElmImage::editable_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_image_editable_set(eo, val->BooleanValue());
}

Handle<Value> CElmImage::aspect_fixed_get() const
{
   return Boolean::New(elm_image_aspect_fixed_get(eo));
}

void CElmImage::aspect_fixed_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_image_aspect_fixed_set(eo, val->BooleanValue());
}

Handle<Value> CElmImage::prescale_get() const
{
   int prescale=elm_image_prescale_get(eo);
   return Integer::New(prescale);
}

void CElmImage::prescale_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        elm_image_prescale_set(eo, val->IntegerValue());
     }
}

Handle<Value> CElmImage::orient_get() const
{
   int orient=elm_image_orient_get(eo);
   return Integer::New(orient);
}

void CElmImage::orient_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        elm_image_orient_set(eo, (Elm_Image_Orient)val->IntegerValue());
     }
}

PROPERTIES_OF(CElmImage) = {
  PROP_HANDLER(CElmImage, file),
  PROP_HANDLER(CElmImage, smooth),
  PROP_HANDLER(CElmImage, no_scale),
  PROP_HANDLER(CElmImage, fill_outside),
  PROP_HANDLER(CElmImage, prescale),
  PROP_HANDLER(CElmImage, orient),
  PROP_HANDLER(CElmImage, editable),
  PROP_HANDLER(CElmImage, aspect_fixed),
  { NULL }
};
