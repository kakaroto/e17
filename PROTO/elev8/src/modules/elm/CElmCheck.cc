#include "CElmCheck.h"

CElmCheck::CElmCheck(CEvasObject *parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_check_add(parent->get());
   //differs only in style from toggle.
   construct(eo, obj);
}

void CElmCheck::eo_on_changed(void *data, Evas_Object *, void *)
{
   static_cast<CElmCheck*>(data)->on_changed(0);
}

void CElmCheck::on_changed(void *)
{
   HandleScope handle_scope;

   Handle<Object> obj = get_object();
   Handle<Value> val = on_changed_val;
   // FIXME: pass event_info to the callback
   // FIXME: turn the pieces below into a do_callback method
   assert(val->IsFunction());
   Handle<Function> fn(Function::Cast(*val));
   Handle<Value> args[1] = { obj };
   fn->Call(obj, 1, args);
}

void CElmCheck::on_changed_set(Handle<Value> val)
{
   on_changed_val.Dispose();
   on_changed_val = Persistent<Value>::New(val);

   if (val->IsFunction())
     evas_object_smart_callback_add(eo, "changed", &eo_on_changed, this);
   else
     evas_object_smart_callback_del(eo, "changed", &eo_on_changed);
}

Handle<Value> CElmCheck::on_changed_get(void) const
{
   return on_changed_val;
}

Handle<Value> CElmCheck::icon_get() const
{
   return the_icon;
}

void CElmCheck::icon_set(Handle<Value> value)
{
   the_icon.Dispose();
   CEvasObject *icon = make_or_get(this, value);
   elm_object_content_set(eo, icon->get());
   the_icon = Persistent<Value>::New(icon->get_object());
}

void CElmCheck::state_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_check_state_set(eo, value->BooleanValue());
}

Handle<Value> CElmCheck::state_get() const
{
   return Boolean::New(elm_check_state_get(eo));
}

CElmCheck::~CElmCheck()
{
   the_icon.Dispose();
   on_changed_val.Dispose();
}

void CElmCheck::onlabel_set(Handle<Value> val)
{
   if (val->IsString())
     elm_object_part_text_set(eo, "on", *String::Utf8Value(val));
}

Handle<Value> CElmCheck::onlabel_get(void) const
{
   const char *onlabel = elm_object_part_text_get(eo, "on");
   return onlabel ? String::New(onlabel) : Null();
}

void CElmCheck::offlabel_set(Handle<Value> val)
{
   if (val->IsString())
     elm_object_part_text_set(eo, "off", *String::Utf8Value(val));
}

Handle<Value> CElmCheck::offlabel_get(void) const
{
   const char *offlabel = elm_object_part_text_get(eo, "off");
   return offlabel ? String::New(offlabel) : Null();
}

PROPERTIES_OF(CElmCheck) = {
    PROP_HANDLER(CElmCheck, state),
    PROP_HANDLER(CElmCheck, icon),
    PROP_HANDLER(CElmCheck, on_changed),
    PROP_HANDLER(CElmCheck, offlabel),
    PROP_HANDLER(CElmCheck, onlabel),
    { NULL }
};

