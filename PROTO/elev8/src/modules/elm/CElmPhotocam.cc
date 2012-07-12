#include "elm.h"
#include "CElmPhotocam.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmPhotocam, file);
GENERATE_PROPERTY_CALLBACKS(CElmPhotocam, zoom);
GENERATE_PROPERTY_CALLBACKS(CElmPhotocam, zoom_mode);
GENERATE_PROPERTY_CALLBACKS(CElmPhotocam, horizontal_bounce);
GENERATE_PROPERTY_CALLBACKS(CElmPhotocam, vertical_bounce);
GENERATE_PROPERTY_CALLBACKS(CElmPhotocam, paused);
GENERATE_RO_PROPERTY_CALLBACKS(CElmPhotocam, image_region);
GENERATE_METHOD_CALLBACKS(CElmPhotocam, image_region_show);
GENERATE_METHOD_CALLBACKS(CElmPhotocam, image_region_bring_in);

GENERATE_TEMPLATE(CElmPhotocam,
                  PROPERTY(file),
                  PROPERTY(zoom),
                  PROPERTY(zoom_mode),
                  PROPERTY(horizontal_bounce),
                  PROPERTY(vertical_bounce),
                  PROPERTY(paused),
                  PROPERTY_RO(image_region),
                  METHOD(image_region_show),
                  METHOD(image_region_bring_in));

CElmPhotocam::CElmPhotocam(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_photocam_add(elm_object_top_widget_get(parent->GetEvasObject())))
{
}

void CElmPhotocam::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Photocam"), GetTemplate()->GetFunction());
}

void CElmPhotocam::file_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   String::Utf8Value str(val);
   if (!access(*str, R_OK))
     {
        elm_photocam_file_set(eo, *str);
        return;
     }

   ELM_ERR("warning: can't read image file %s", *str);
}

Handle<Value> CElmPhotocam::file_get(void) const
{
   const char *file = elm_photocam_file_get(eo);
   return file ? String::New(file) : Null();
}

Handle<Value> CElmPhotocam::zoom_get() const
{
   return Number::New(elm_photocam_zoom_get(eo));
}

void CElmPhotocam::zoom_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_photocam_zoom_set(eo, value->NumberValue());
}

Handle<Value> CElmPhotocam::zoom_mode_get() const
{
   switch (elm_photocam_zoom_mode_get(eo)) {
   case ELM_PHOTOCAM_ZOOM_MODE_MANUAL:
      return String::NewSymbol("manual");
   case ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT:
      return String::NewSymbol("auto-fit");
   case ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL:
      return String::NewSymbol("auto-fill");
   default:
      return String::NewSymbol("auto-fit-in");
   }
}

void CElmPhotocam::zoom_mode_set(Handle<Value> value)
{
   Elm_Photocam_Zoom_Mode new_mode;

   if (value->IsNumber())
     new_mode = (Elm_Photocam_Zoom_Mode)value->NumberValue();
   else if (value->IsString())
     {
        String::Utf8Value mode(value->ToString());
        if (!strcmp(*mode, "manual"))
          new_mode = ELM_PHOTOCAM_ZOOM_MODE_MANUAL;
        else if (!strcmp(*mode, "auto-fit"))
          new_mode = ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT;
        else if (!strcmp(*mode, "auto-fill"))
          new_mode = ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL;
        else
          new_mode = ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT_IN;
      }
    else
      new_mode = ELM_PHOTOCAM_ZOOM_MODE_MANUAL;

   elm_photocam_zoom_mode_set(eo, new_mode);
}

void CElmPhotocam::vertical_bounce_set(Handle<Value> val)
{
   if (!val->IsBoolean())
     return;

   Eina_Bool horizontal_bounce;
   elm_photocam_bounce_get(eo, &horizontal_bounce, NULL);
   elm_photocam_bounce_set(eo, horizontal_bounce, val->ToBoolean()->Value());
}

Handle<Value> CElmPhotocam::vertical_bounce_get() const
{
   Eina_Bool vertical_bounce;
   elm_photocam_bounce_get(eo, NULL, &vertical_bounce);
   return Boolean::New(vertical_bounce);
}

void CElmPhotocam::horizontal_bounce_set(Handle<Value> val)
{
   if (!val->IsBoolean())
     return;

   Eina_Bool vertical_bounce;
   elm_photocam_bounce_get(eo, NULL, &vertical_bounce);
   elm_photocam_bounce_set(eo, val->ToBoolean()->Value(), vertical_bounce);
}

Handle<Value> CElmPhotocam::horizontal_bounce_get() const
{
   Eina_Bool horizontal_bounce;
   elm_photocam_bounce_get(eo, &horizontal_bounce, NULL);
   return Boolean::New(horizontal_bounce);
}

void CElmPhotocam::paused_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_photocam_paused_set(eo, val->BooleanValue());
}

Handle<Value> CElmPhotocam::paused_get() const
{
   return Boolean::New(elm_photocam_paused_get(eo));
}


Handle<Value> CElmPhotocam::image_region_get() const
{
   HandleScope scope;

   int x, y, w, h;
   elm_photocam_image_region_get(eo, &x, &y, &w, &h);

   Local<Object> obj = Object::New();
   obj->Set(String::New("x"), Number::New(x));
   obj->Set(String::New("y"), Number::New(y));
   obj->Set(String::New("w"), Number::New(w));
   obj->Set(String::New("h"), Number::New(h));

   return scope.Close(obj);
}


Handle<Value> CElmPhotocam::image_region_show(const Arguments &args)
{
   for (int i = 0; i < 4; i++)
     if (!args[i]->IsNumber())
       return Undefined();

   elm_photocam_image_region_show(eo, args[0]->ToInt32()->Value(), args[1]->ToInt32()->Value(),
       args[2]->ToInt32()->Value(), args[3]->ToInt32()->Value());

   return Undefined();

}

Handle<Value> CElmPhotocam::image_region_bring_in(const Arguments &args)
{
   for (int i = 0; i < 4; i++)
     if (!args[i]->IsNumber())
       return Undefined();

   elm_photocam_image_region_bring_in(eo, args[0]->ToInt32()->Value(), args[1]->ToInt32()->Value(),
       args[2]->ToInt32()->Value(), args[3]->ToInt32()->Value());

   return Undefined();
}

}
