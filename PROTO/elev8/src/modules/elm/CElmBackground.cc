#include "elm.h"
#include "CElmBackground.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmBackground, image);
GENERATE_PROPERTY_CALLBACKS(CElmBackground, red);
GENERATE_PROPERTY_CALLBACKS(CElmBackground, green);
GENERATE_PROPERTY_CALLBACKS(CElmBackground, blue);
GENERATE_PROPERTY_CALLBACKS(CElmBackground, load_size);
GENERATE_PROPERTY_CALLBACKS(CElmBackground, option);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmBackground,
                  PROPERTY(image),
                  PROPERTY(red),
                  PROPERTY(green),
                  PROPERTY(blue),
                  PROPERTY(load_size),
                  PROPERTY(option));

CElmBackground::CElmBackground(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_bg_add(_parent->GetEvasObject()))
{
}

CElmBackground::~CElmBackground()
{
   load_size.Dispose();
}

void CElmBackground::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Background"),
               GetTemplate()->GetFunction());
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

Handle<Value> CElmBackground::load_size_get() const
{
  return load_size;
}

void CElmBackground::load_size_set(Handle<Value> value)
{
   if (!value->IsArray())
     return;

   Local<Object> size = value->ToObject();
   elm_bg_load_size_set(eo,
        size->Get(0)->ToNumber()->Value(),
        size->Get(1)->ToNumber()->Value());

   load_size.Dispose();
   load_size = Persistent<Value>::New(value);
}

Handle<Value> CElmBackground::option_get() const
{
   switch (elm_bg_option_get(eo)) {
     case ELM_BG_OPTION_CENTER:
       return String::NewSymbol("center");
     case ELM_BG_OPTION_SCALE:
       return String::NewSymbol("scale");
     case ELM_BG_OPTION_STRETCH:
       return String::NewSymbol("stretch");
     case ELM_BG_OPTION_TILE:
       return String::NewSymbol("tile");
     case ELM_BG_OPTION_LAST:
       return String::NewSymbol("last");
     default:
       return String::NewSymbol("unknown");
    }
}

void CElmBackground::option_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   String::Utf8Value str(val);
   if (!strcmp(*str, "center"))
     elm_bg_option_set(eo, ELM_BG_OPTION_CENTER);
   else if (!strcmp(*str, "scale"))
     elm_bg_option_set(eo, ELM_BG_OPTION_SCALE);
   else if (!strcmp(*str, "stretch"))
     elm_bg_option_set(eo, ELM_BG_OPTION_STRETCH);
   else if (!strcmp(*str, "tile"))
     elm_bg_option_set(eo, ELM_BG_OPTION_TILE);
   else if (!strcmp(*str, "last"))
     elm_bg_option_set(eo, ELM_BG_OPTION_LAST);
}

}
