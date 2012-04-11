#include "elm.h"
#include "CElmImage.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmImage, file);
GENERATE_PROPERTY_CALLBACKS(CElmImage, smooth);
GENERATE_PROPERTY_CALLBACKS(CElmImage, no_scale);
GENERATE_PROPERTY_CALLBACKS(CElmImage, fill_outside);
GENERATE_PROPERTY_CALLBACKS(CElmImage, editable);
GENERATE_PROPERTY_CALLBACKS(CElmImage, aspect_fixed);
GENERATE_PROPERTY_CALLBACKS(CElmImage, prescale);
GENERATE_PROPERTY_CALLBACKS(CElmImage, orient);

GENERATE_TEMPLATE(CElmImage,
                  PROPERTY(file),
                  PROPERTY(smooth),
                  PROPERTY(no_scale),
                  PROPERTY(fill_outside),
                  PROPERTY(editable),
                  PROPERTY(aspect_fixed),
                  PROPERTY(prescale),
                  PROPERTY(orient));

CElmImage::CElmImage(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_image_add(parent->GetEvasObject()))
{
}

void CElmImage::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Image"), GetTemplate()->GetFunction());
}

void CElmImage::file_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   String::Utf8Value str(val);
   if (!elm_image_file_set(eo, *str, NULL))
     ELM_WRN( "warning: can't set image file %s", *str);
}

Handle<Value> CElmImage::file_get(void) const
{
   const char *f = NULL;

   elm_image_file_get(eo, &f, NULL);
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
   int prescale = elm_image_prescale_get(eo);
   return Integer::New(prescale);
}

void CElmImage::prescale_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_image_prescale_set(eo, val->IntegerValue());
}

Handle<Value> CElmImage::orient_get() const
{
   int orient = elm_image_orient_get(eo);
   return Integer::New(orient);
}

void CElmImage::orient_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_image_orient_set(eo, (Elm_Image_Orient)val->IntegerValue());
}

}
