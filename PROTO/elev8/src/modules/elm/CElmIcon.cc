#include "CElmIcon.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmIcon, resizable_up);
GENERATE_PROPERTY_CALLBACKS(CElmIcon, resizable_down);
GENERATE_PROPERTY_CALLBACKS(CElmIcon, prescale);
GENERATE_PROPERTY_CALLBACKS(CElmIcon, image);

GENERATE_TEMPLATE(CElmIcon,
   PROPERTY(resizable_up),
   PROPERTY(resizable_down),
   PROPERTY(prescale),
   PROPERTY(image));

CElmIcon::CElmIcon(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_icon_add(parent->GetEvasObject()))
{
}

void CElmIcon::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Icon"),
               GetTemplate()->GetFunction());
}

void CElmIcon::resizable_up_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        Eina_Bool down;
        elm_icon_resizable_get(eo, NULL, &down);
        elm_icon_resizable_set(eo, val->BooleanValue(), down);
     }
}

Handle<Value> CElmIcon::resizable_up_get() const
{
   Eina_Bool up;
   elm_icon_resizable_get(eo, &up, NULL);
   return Boolean::New(up);
}

void CElmIcon::resizable_down_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        Eina_Bool up;
        elm_icon_resizable_get(eo, &up, NULL);
        elm_icon_resizable_set(eo, up, val->BooleanValue());
     }
}

Handle<Value> CElmIcon::resizable_down_get() const
{
   Eina_Bool down;
   elm_icon_resizable_get(eo, NULL, &down);
   return Boolean::New(down);
}

Handle<Value> CElmIcon::prescale_get() const
{
   return Integer::New(elm_icon_prescale_get(eo));
}

void CElmIcon::prescale_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_icon_prescale_set(eo, val->IntegerValue());
}

void CElmIcon::image_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   String::Utf8Value str(val);

   if (access(*str, R_OK) == 0)
     {
        elm_icon_file_set(eo, *str, NULL);
        return;
     }

   if (elm_icon_standard_set(eo, *str))
     return;

   ELM_ERR("warning: can't read standard or file icon %s", *str);
}

Handle<Value> CElmIcon::image_get(void) const
{
   const char *file = NULL, *group = NULL;

   elm_icon_file_get(eo, &file, &group);
   if (file)
     return String::New(file);

   return Null();
}

}
