#include "elm.h"
#include "CElmColorSelector.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmColorSelector, on_change);
GENERATE_PROPERTY_CALLBACKS(CElmColorSelector, red);
GENERATE_PROPERTY_CALLBACKS(CElmColorSelector, green);
GENERATE_PROPERTY_CALLBACKS(CElmColorSelector, blue);
GENERATE_PROPERTY_CALLBACKS(CElmColorSelector, alpha);
GENERATE_PROPERTY_CALLBACKS(CElmColorSelector, palette_name);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmColorSelector,
                  PROPERTY(on_change),
                  PROPERTY(red),
                  PROPERTY(green),
                  PROPERTY(blue),
                  PROPERTY(alpha),
                  PROPERTY(palette_name));

CElmColorSelector::CElmColorSelector(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_colorselector_add(_parent->GetEvasObject()))
{
}

void CElmColorSelector::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("ColorSelector"),
               GetTemplate()->GetFunction());
}

CElmColorSelector::~CElmColorSelector()
{
   on_change_set(Undefined());
}

void CElmColorSelector::OnChange(void *)
{
   HandleScope scope;
   Local<Function> callback(Function::Cast(*cb.change));
   Handle<Value> args[1] = { jsObject };

   callback->Call(jsObject, 1, args);
}

void CElmColorSelector::OnChangeWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmColorSelector*>(data)->OnChange(event_info);
}

Handle<Value> CElmColorSelector::on_change_get(void) const
{
   return cb.change;
}

void CElmColorSelector::on_change_set(Handle<Value> val)
{
   if (!cb.change.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "changed", &OnChangeWrapper);
        cb.change.Dispose();
        cb.change.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.change = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "changed", &OnChangeWrapper, this);
}

Handle<Value> CElmColorSelector::red_get() const
{
   int r;
   elm_colorselector_color_get(eo, &r, NULL, NULL, NULL);
   return Number::New(r);
}

void CElmColorSelector::red_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   int g, b, a;
   elm_colorselector_color_get(eo, NULL, &g, &b, &a);
   elm_colorselector_color_set(eo, val->ToNumber()->Value(), g, b, a);
}

Handle<Value> CElmColorSelector::green_get() const
{
   int g;
   elm_colorselector_color_get(eo, NULL, &g, NULL, NULL);
   return Number::New(g);
}

void CElmColorSelector::green_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   int r, b, a;
   elm_colorselector_color_get(eo, &r, NULL, &b, &a);
   elm_colorselector_color_set(eo, r, val->ToNumber()->Value(), b, a);
}

Handle<Value> CElmColorSelector::blue_get() const
{
   int b;
   elm_colorselector_color_get(eo, NULL, NULL, &b, NULL);
   return Number::New(b);
}

void CElmColorSelector::blue_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   int r, g, a;
   elm_colorselector_color_get(eo, &r, &g, NULL, &a);
   elm_colorselector_color_set(eo, r, g, val->ToNumber()->Value(), a);
}

Handle<Value> CElmColorSelector::alpha_get() const
{
   int a;
   elm_colorselector_color_get(eo, NULL, NULL, NULL, &a);
   return Number::New(a);
}

void CElmColorSelector::alpha_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   int r, g, b;
   elm_colorselector_color_get(eo, &r, &g, &b, NULL);
   elm_colorselector_color_set(eo, r, g, b, val->ToNumber()->Value());
}

Handle<Value> CElmColorSelector::palette_name_get() const
{
   return String::New(elm_colorselector_palette_name_get(eo));
}

void CElmColorSelector::palette_name_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   elm_colorselector_palette_name_set(eo, *String::Utf8Value(val));
}

}
