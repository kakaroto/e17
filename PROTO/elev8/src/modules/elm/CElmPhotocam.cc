#include "CElmPhotocam.h"

CElmPhotocam::CElmPhotocam(CEvasObject *parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_photocam_add(parent->top_widget_get());
   construct(eo, obj);
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
   return Number::New(elm_photocam_zoom_mode_get(eo));
}

void CElmPhotocam::zoom_mode_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_photocam_zoom_mode_set(eo, (Elm_Photocam_Zoom_Mode)value->NumberValue());
}

void CElmPhotocam::bounce_set(Handle<Value> val)
{
   bool x_bounce, y_bounce;

   if (get_xy_from_object(val, x_bounce, y_bounce))
     elm_scroller_bounce_set(eo, x_bounce, y_bounce);
}

Handle<Value> CElmPhotocam::bounce_get() const
{
   HandleScope scope;

   Eina_Bool x, y;
   elm_scroller_bounce_get(eo, &x, &y);

   Local<Object> obj = Object::New();
   obj->Set(String::New("x"), Boolean::New(x));
   obj->Set(String::New("y"), Boolean::New(y));

   return scope.Close(obj);
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
