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
GENERATE_PROPERTY_CALLBACKS(CElmImage, resizable_up);
GENERATE_PROPERTY_CALLBACKS(CElmImage, resizable_down);
GENERATE_PROPERTY_CALLBACKS(CElmImage, animated);
GENERATE_PROPERTY_CALLBACKS(CElmImage, animated_play);
GENERATE_PROPERTY_CALLBACKS(CElmImage, preload_disabled);
GENERATE_RO_PROPERTY_CALLBACKS(CElmImage, object_size);
GENERATE_RO_PROPERTY_CALLBACKS(CElmImage, animated_available);


GENERATE_TEMPLATE(CElmImage,
                  PROPERTY(file),
                  PROPERTY(smooth),
                  PROPERTY(no_scale),
                  PROPERTY(fill_outside),
                  PROPERTY(editable),
                  PROPERTY(aspect_fixed),
                  PROPERTY(prescale),
                  PROPERTY(orient),
                  PROPERTY(resizable_up),
                  PROPERTY(resizable_down),
                  PROPERTY(animated),
                  PROPERTY(animated_play),
                  PROPERTY(preload_disabled),
                  PROPERTY_RO(object_size),
                  PROPERTY_RO(animated_available));

CElmImage::CElmImage(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_image_add(parent->GetEvasObject()))
{
}

CElmImage::~CElmImage()
{
  preload_disabled.Dispose();
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

Handle<Value> CElmImage::object_size_get() const
{
   HandleScope scope;

   int w, h;
   elm_image_object_size_get(eo, &w, &h);

   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("w"), Number::New(w));
   obj->Set(String::NewSymbol("h"), Number::New(h));

   return scope.Close(obj);
}

void CElmImage::resizable_up_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        Eina_Bool down;
        elm_image_resizable_get(eo, NULL, &down);
        elm_image_resizable_set(eo, val->BooleanValue(), down);
     }
}

Handle<Value> CElmImage::resizable_up_get() const
{
   Eina_Bool up;
   elm_image_resizable_get(eo, &up, NULL);
   return Boolean::New(up);
}

void CElmImage::resizable_down_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        Eina_Bool up;
        elm_image_resizable_get(eo, &up, NULL);
        elm_image_resizable_set(eo, up, val->BooleanValue());
     }
}

Handle<Value> CElmImage::resizable_down_get() const
{
   Eina_Bool down;
   elm_image_resizable_get(eo, NULL, &down);
   return Boolean::New(down);
}

Handle<Value> CElmImage::animated_available_get() const
{
   return Boolean::New(elm_image_animated_available_get(eo)); 
}

Handle<Value> CElmImage::animated_get() const
{
   return Boolean::New(elm_image_animated_get(eo));
}

void CElmImage::animated_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_image_animated_set(eo, val->BooleanValue());
}

Handle<Value> CElmImage::animated_play_get() const
{
   return Boolean::New(elm_image_animated_play_get(eo));
}

void CElmImage::animated_play_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_image_animated_play_set(eo, val->BooleanValue());
}

Handle<Value> CElmImage::preload_disabled_get() const
{
  return preload_disabled;
}

void CElmImage::preload_disabled_set(Handle<Value> val)
{
  if (!val->IsBoolean())
    return;

  elm_image_preload_disabled_set(eo, val->BooleanValue());

  preload_disabled.Dispose();
  preload_disabled = Persistent<Value>::New(val);
}

}
