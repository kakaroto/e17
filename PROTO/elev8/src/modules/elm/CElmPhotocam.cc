#include "CElmPhotocam.h"

CElmPhotocam::CElmPhotocam(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_photocam_add(parent->top_widget_get());
   construct(eo, obj);
}
void CElmPhotocam::file_set(Handle<Value> val)
{
   if (val->IsString())
     {
        String::Utf8Value str(val);
        if (0 > access(*str, R_OK))
          ELM_ERR( "warning: can't read image file %s", *str);
        elm_photocam_file_set(eo, *str);
        ELM_INF( "Photcam image file %s", *str);
     }
}

Handle<Value> CElmPhotocam::file_get(void) const
{
   const char *f = NULL;
   f = elm_photocam_file_get (eo);
   if (f)
     return String::New(f);
   else
     return Null();
}

Handle<Value> CElmPhotocam::zoom_get() const
{
   double zoom;
   zoom = elm_photocam_zoom_get(eo);
   return Number::New(zoom);
}

void CElmPhotocam::zoom_set(Handle<Value> value)
{
   if (value->IsNumber())
     {
        double zoom;
        zoom = value->NumberValue();
        elm_photocam_zoom_set (eo, zoom);
     }
}

Handle<Value> CElmPhotocam::zoom_mode_get() const
{
   Elm_Photocam_Zoom_Mode zoom_mode;
   zoom_mode = elm_photocam_zoom_mode_get(eo);
   return Number::New(zoom_mode);
}

void CElmPhotocam::zoom_mode_set(Handle<Value> value)
{
   if (value->IsNumber())
     {
        Elm_Photocam_Zoom_Mode zoom_mode;
        zoom_mode = (Elm_Photocam_Zoom_Mode)value->NumberValue();
        elm_photocam_zoom_mode_set(eo, zoom_mode);
     }
}

void CElmPhotocam::bounce_set(Handle<Value> val)
{
   bool x_bounce = false, y_bounce = false;
   if (get_xy_from_object(val, x_bounce, y_bounce))
     {
        elm_scroller_bounce_set(eo, x_bounce, y_bounce);
     }
}

Handle<Value> CElmPhotocam::bounce_get() const
{
   Eina_Bool x, y;
   elm_scroller_bounce_get(eo, &x, &y);
   Local<Object> obj = Object::New();
   obj->Set(String::New("x"), Boolean::New(x));
   obj->Set(String::New("y"), Boolean::New(y));
   return obj;
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

PROPERTIES_OF(CElmPhotocam) = {
  PROP_HANDLER(CElmPhotocam, file),
  PROP_HANDLER(CElmPhotocam, zoom),
  PROP_HANDLER(CElmPhotocam, zoom_mode),
  PROP_HANDLER(CElmPhotocam, paused),
  PROP_HANDLER(CElmPhotocam, bounce),
  { NULL }
};
