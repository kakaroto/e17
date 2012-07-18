#include "CElmIcon.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmIcon, file);
GENERATE_PROPERTY_CALLBACKS(CElmIcon, lookup_order);
GENERATE_PROPERTY_CALLBACKS(CElmIcon, thumb);

GENERATE_TEMPLATE_FULL(CElmImage, CElmIcon,
   PROPERTY(file),
   PROPERTY(lookup_order),
   PROPERTY(thumb));

CElmIcon::CElmIcon(Local<Object> _jsObject, CElmObject *parent)
   : CElmImage(_jsObject, elm_icon_add(parent->GetEvasObject()))
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

void CElmIcon::file_set(Handle<Value> val)
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

Handle<Value> CElmIcon::file_get(void) const
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
