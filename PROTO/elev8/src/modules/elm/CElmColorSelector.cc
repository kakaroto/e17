#include "CElmColorSelector.h"

CElmColorSelector::CElmColorSelector(CEvasObject *parent, Local<Object> obj) 
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_colorselector_add(parent->top_widget_get());
   construct(eo, obj);
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

void CElmColorSelector::on_changed(void *)
{
   Handle<Object> obj = get_object();
   Handle<Value> val = on_changed_val;
   assert(val->IsFunction());
   Handle<Function> fn(Function::Cast(*val));
   Handle<Value> args[1] = { obj };
   fn->Call(obj, 1, args);
}

void CElmColorSelector::eo_on_changed(void *data, Evas_Object *, void *event_info)
{
   CElmColorSelector *changed = static_cast<CElmColorSelector*>(data);
   changed->on_changed(event_info);
}

void CElmColorSelector::on_changed_set(Handle<Value> val)
{
   on_changed_val.Dispose();
   on_changed_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_smart_callback_add(eo, "changed", &eo_on_changed, this);
   else
     evas_object_smart_callback_del(eo, "changed", &eo_on_changed);
}

Handle<Value> CElmColorSelector::on_changed_get(void) const
{
   return on_changed_val;
}

PROPERTIES_OF(CElmColorSelector) = {
     PROP_HANDLER(CElmColorSelector, red),
     PROP_HANDLER(CElmColorSelector, green),
     PROP_HANDLER(CElmColorSelector, blue),
     PROP_HANDLER(CElmColorSelector, alpha),
     PROP_HANDLER(CElmColorSelector, on_changed),
     { NULL }
};
