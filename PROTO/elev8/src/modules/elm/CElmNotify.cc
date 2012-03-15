#include "CElmNotify.h"

CElmNotify::CElmNotify(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
{
   eo = elm_notify_add(parent->top_widget_get());
   construct(eo, obj);
}

Handle<Value> CElmNotify::content_get() const
{
   return Undefined();
}

void CElmNotify::content_set(Handle<Value> val)
{
   if (val->IsObject())
     {
        content = make_or_get(this, val);
        if (content)
          {
             elm_object_content_set(eo, content->get());
          }
     }
}

Handle<Value> CElmNotify::orient_get() const
{
   return Number::New(elm_notify_orient_get(eo));
}

void CElmNotify::orient_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        double orient = val->ToInt32()->Value();
        elm_notify_orient_set(eo, (Elm_Notify_Orient)orient);
        ELM_INF("Value of orient = %g", orient);
     }
}

Handle<Value> CElmNotify::timeout_get() const
{
   return Number::New(elm_notify_timeout_get(eo));
}

void CElmNotify::timeout_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        double timeout = val->ToInt32()->Value();
        elm_notify_timeout_set(eo, timeout);
     }
}

Handle<Value> CElmNotify::allow_events_get() const
{
   return Boolean::New(elm_notify_allow_events_get(eo));
}

void CElmNotify::allow_events_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_notify_allow_events_set(eo, val->BooleanValue());
}

PROPERTIES_OF(CElmNotify) = {
     PROP_HANDLER(CElmNotify, content),
     PROP_HANDLER(CElmNotify, orient),
     PROP_HANDLER(CElmNotify, timeout),
     PROP_HANDLER(CElmNotify, allow_events),
     { NULL }
};
