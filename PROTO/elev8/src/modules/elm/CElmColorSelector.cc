#include "CElmColorSelector.h"

CElmColorSelector::CElmColorSelector(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_colorselector_add(parent->top_widget_get());
   construct(eo, obj);
}

Handle<Value> CElmColorSelector::red_get() const
{
   int r, g, b, a;
   elm_colorselector_color_get(eo, &r, &g, &b, &a);
   return Number::New(r);
}

void CElmColorSelector::red_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        int r, g, b, a;
        elm_colorselector_color_get(eo, &r, &g, &b, &a);
        r = val->ToNumber()->Value();
        elm_colorselector_color_set(eo, r, g, b, a);
     }
}

Handle<Value> CElmColorSelector::green_get() const
{
   int r, g, b, a;
   elm_colorselector_color_get(eo, &r, &g, &b, &a);
   return Number::New(g);
}

void CElmColorSelector::green_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        int r, g, b, a;
        elm_colorselector_color_get(eo, &r, &g, &b, &a);
        g = val->ToNumber()->Value();
        elm_colorselector_color_set(eo, r, g, b, a);
     }
}
Handle<Value> CElmColorSelector::blue_get() const
{
   int r, g, b, a;
   elm_colorselector_color_get(eo, &r, &g, &b, &a);
   return Number::New(b);
}

void CElmColorSelector::blue_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        int r, g, b, a;
        elm_colorselector_color_get(eo, &r, &g, &b, &a);
        b = val->ToNumber()->Value();
        elm_colorselector_color_set(eo, r, g, b, a);
     }
}
Handle<Value> CElmColorSelector::alpha_get() const
{
   int r, g, b, a;
   elm_colorselector_color_get(eo, &r, &g, &b, &a);
   return Number::New(a);
}

void CElmColorSelector::alpha_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        int r, g, b, a;
        elm_colorselector_color_get(eo, &r, &g, &b, &a);
        a = val->ToNumber()->Value();
        elm_colorselector_color_set(eo, r, g, b, a);
     }
}
void CElmColorSelector::on_changed(void *)
{
   Handle<Object> obj = get_object();
   HandleScope handle_scope;
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

template<> CEvasObject::CPropHandler<CElmColorSelector>::property_list
CEvasObject::CPropHandler<CElmColorSelector>::list[] = {
     PROP_HANDLER(CElmColorSelector, red),
     PROP_HANDLER(CElmColorSelector, green),
     PROP_HANDLER(CElmColorSelector, blue),
     PROP_HANDLER(CElmColorSelector, alpha),
     PROP_HANDLER(CElmColorSelector, on_changed),
     { NULL, NULL, NULL },
};
