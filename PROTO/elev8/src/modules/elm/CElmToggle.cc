#include "CElmToggle.h"

CElmToggle::CElmToggle(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_check_add(parent->top_widget_get());
   elm_object_style_set(eo, "toggle");
   construct(eo, obj);
}

void CElmToggle::eo_on_changed(void *data, Evas_Object *, void *event_info)
{
   CElmToggle *changed = static_cast<CElmToggle*>(data);

   changed->on_changed(event_info);
}

void CElmToggle::on_changed(void *)
{
   Handle<Object> obj = get_object();
   HandleScope handle_scope;
   Handle<Value> val = on_changed_val;
   // FIXME: pass event_info to the callback
   // FIXME: turn the pieces below into a do_callback method
   assert(val->IsFunction());
   Handle<Function> fn(Function::Cast(*val));
   Handle<Value> args[1] = { obj };
   fn->Call(obj, 1, args);
}

void CElmToggle::on_changed_set(Handle<Value> val)
{
   on_changed_val.Dispose();
   on_changed_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_smart_callback_add(eo, "changed", &eo_on_changed, this);
   else
     evas_object_smart_callback_del(eo, "changed", &eo_on_changed);
}

Handle<Value> CElmToggle::on_changed_get(void) const
{
   return on_changed_val;
}


void CElmToggle::onlabel_set(Handle<Value> val)
{
   if (val->IsString())
     {
        String::Utf8Value str(val);
        elm_object_part_text_set(eo, "on", *str);
     }
}

Handle<Value> CElmToggle::onlabel_get(void) const
{
   const char *onlabel = NULL;
   onlabel = elm_object_part_text_get(eo, "on");
   if (onlabel)
     return String::New(onlabel);
   else
     return Null();
}

void CElmToggle::offlabel_set(Handle<Value> val)
{
   if (val->IsString())
     {
        String::Utf8Value str(val);
        elm_object_part_text_set(eo, "off", *str);
     }
}

Handle<Value> CElmToggle::offlabel_get(void) const
{
   const char *offlabel = NULL;
   offlabel = elm_object_part_text_get(eo, "off");
   if (offlabel)
     return String::New(offlabel);
   else
     return Null();
}

Handle<Value> CElmToggle::icon_get() const
{
   return the_icon;
}

void CElmToggle::icon_set(Handle<Value> value)
{
   the_icon.Dispose();
   CEvasObject *icon = make_or_get(this, value);
   elm_object_content_set(eo, icon->get());
   the_icon = Persistent<Value>::New(icon->get_object());
}

void CElmToggle::state_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_check_state_set(eo, val->BooleanValue());
}

Handle<Value> CElmToggle::state_get() const
{
   return Boolean::New(elm_check_state_get(eo));
}

PROPERTIES_OF(CElmToggle) = {
     PROP_HANDLER(CElmToggle, offlabel),
     PROP_HANDLER(CElmToggle, onlabel),
     PROP_HANDLER(CElmToggle, icon),
     PROP_HANDLER(CElmToggle, state),
     PROP_HANDLER(CElmToggle, on_changed),
     { NULL }
};
