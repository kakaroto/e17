#include "CElmIcon.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmIcon, resizable_up);
GENERATE_PROPERTY_CALLBACKS(CElmIcon, resizable_down);
GENERATE_PROPERTY_CALLBACKS(CElmIcon, prescale);
GENERATE_PROPERTY_CALLBACKS(CElmIcon, image);
GENERATE_PROPERTY_CALLBACKS(CElmIcon, lookup_order);
GENERATE_PROPERTY_CALLBACKS(CElmIcon, thumb);

GENERATE_TEMPLATE(CElmIcon,
   PROPERTY(resizable_up),
   PROPERTY(resizable_down),
   PROPERTY(prescale),
   PROPERTY(image),
   PROPERTY(lookup_order),
   PROPERTY(thumb));

CElmIcon::CElmIcon(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_icon_add(parent->GetEvasObject()))
{
}

CElmIcon::~CElmIcon()
{
  thumb.Dispose();
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
        elm_image_resizable_get(eo, NULL, &down);
        elm_image_resizable_set(eo, val->BooleanValue(), down);
     }
}

Handle<Value> CElmIcon::resizable_up_get() const
{
   Eina_Bool up;
   elm_image_resizable_get(eo, &up, NULL);
   return Boolean::New(up);
}

void CElmIcon::resizable_down_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        Eina_Bool up;
        elm_image_resizable_get(eo, &up, NULL);
        elm_image_resizable_set(eo, up, val->BooleanValue());
     }
}

Handle<Value> CElmIcon::resizable_down_get() const
{
   Eina_Bool down;
   elm_image_resizable_get(eo, NULL, &down);
   return Boolean::New(down);
}

Handle<Value> CElmIcon::prescale_get() const
{
   return Integer::New(elm_image_prescale_get(eo));
}

void CElmIcon::prescale_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_image_prescale_set(eo, val->IntegerValue());
}

void CElmIcon::image_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   String::Utf8Value str(val);

   if (access(*str, R_OK) == 0)
     {
        elm_image_file_set(eo, *str, NULL);
        return;
     }

   if (elm_icon_standard_set(eo, *str))
     return;

   ELM_ERR("warning: can't read standard or file icon %s", *str);
}

Handle<Value> CElmIcon::image_get(void) const
{
   const char *file = NULL, *group = NULL;

   elm_image_file_get(eo, &file, &group);
   if (file)
     return String::New(file);

   return Null();
}

Handle<Value> CElmIcon::lookup_order_get() const
{
  switch (elm_icon_order_lookup_get(eo))
    {
      case ELM_ICON_LOOKUP_FDO_THEME:
        return String::NewSymbol("fdo,theme");
      case ELM_ICON_LOOKUP_THEME_FDO:
        return String::NewSymbol("theme,fdo");
      case ELM_ICON_LOOKUP_FDO:
        return String::NewSymbol("fdo");
      case ELM_ICON_LOOKUP_THEME:
        return String::NewSymbol("theme");
      default:
        return String::NewSymbol("unknown");
   }
}

void CElmIcon::lookup_order_set(Handle<Value> val)
{
  if (!val->IsString())
    return;

  String::Utf8Value lookup_order(val->ToString());

  if (!strcmp(*lookup_order, "fdo,theme"))
    elm_icon_order_lookup_set(eo, ELM_ICON_LOOKUP_FDO_THEME);
  else if (!strcmp(*lookup_order, "theme,fdo"))
    elm_icon_order_lookup_set(eo, ELM_ICON_LOOKUP_THEME_FDO);
  else if (!strcmp(*lookup_order, "fdo"))
    elm_icon_order_lookup_set(eo, ELM_ICON_LOOKUP_FDO);
  else if (!strcmp(*lookup_order, "theme"))
    elm_icon_order_lookup_set(eo, ELM_ICON_LOOKUP_THEME);
}

Handle<Value> CElmIcon::thumb_get() const
{
   return thumb;
}

void CElmIcon::thumb_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Value> file = val->ToObject()->Get(String::NewSymbol("file"));
   Local<Value> group = val->ToObject()->Get(String::NewSymbol("group"));

   if (!file->IsString() || !group->IsString())
     return;

   elm_icon_thumb_set(eo, *String::Utf8Value(file), *String::Utf8Value(group));

   thumb.Dispose();
   thumb = Persistent<Value>::New(val);
}

}
