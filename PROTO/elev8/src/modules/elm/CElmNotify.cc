#include "elm.h"
#include "CElmNotify.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmNotify, content);
GENERATE_PROPERTY_CALLBACKS(CElmNotify, orient);
GENERATE_PROPERTY_CALLBACKS(CElmNotify, timeout);
GENERATE_PROPERTY_CALLBACKS(CElmNotify, allow_events);
GENERATE_PROPERTY_CALLBACKS(CElmNotify, parent);


GENERATE_TEMPLATE(CElmNotify,
                  PROPERTY(content),
                  PROPERTY(orient),
                  PROPERTY(timeout),
                  PROPERTY(allow_events),
                  PROPERTY(parent));

CElmNotify::CElmNotify(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject,
		elm_notify_add(elm_object_top_widget_get(parent->GetEvasObject())))
{
}

CElmNotify::~CElmNotify()
{
   cached.content.Dispose();
   notify_parent.Dispose();
}

void CElmNotify::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Notify"), GetTemplate()->GetFunction());
}

Handle<Value> CElmNotify::content_get() const
{
   return cached.content;
}

void CElmNotify::content_set(Handle<Value> val)
{
   cached.content.Dispose();
   cached.content = Persistent<Value>::New(Realise(val, jsObject));
   elm_object_content_set(eo, GetEvasObjectFromJavascript(cached.content));
}

Handle<Value> CElmNotify::orient_get() const
{
   return Number::New(elm_notify_orient_get(eo));
}

void CElmNotify::orient_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_notify_orient_set(eo, (Elm_Notify_Orient)val->Int32Value());
}

Handle<Value> CElmNotify::timeout_get() const
{
   return Number::New(elm_notify_timeout_get(eo));
}

void CElmNotify::timeout_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_notify_timeout_set(eo, val->Int32Value());
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

Handle<Value> CElmNotify::parent_get() const
{
   return notify_parent;
}

void CElmNotify::parent_set(Handle<Value> val)
{
   if (val->IsObject())
     {
       elm_notify_parent_set(eo, GetEvasObjectFromJavascript(val));

       notify_parent.Dispose();
       notify_parent = Persistent<Value>::New(val);
     }
}

}
