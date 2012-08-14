#include "elm.h"
#include "CElmNotify.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmNotify, content);
GENERATE_PROPERTY_CALLBACKS(CElmNotify, orient);
GENERATE_PROPERTY_CALLBACKS(CElmNotify, timeout);
GENERATE_PROPERTY_CALLBACKS(CElmNotify, allow_events);
GENERATE_PROPERTY_CALLBACKS(CElmNotify, parent);


GENERATE_TEMPLATE_FULL(CElmLayout, CElmNotify,
                  PROPERTY(content),
                  PROPERTY(orient),
                  PROPERTY(timeout),
                  PROPERTY(allow_events),
                  PROPERTY(parent));

CElmNotify::CElmNotify(Local<Object> _jsObject, CElmObject *p)
   : CElmLayout(_jsObject,
		elm_notify_add(elm_object_top_widget_get(p->GetEvasObject())))
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
   switch (elm_notify_orient_get(eo)) {
     case ELM_NOTIFY_ORIENT_TOP: return String::New("top");
     case ELM_NOTIFY_ORIENT_CENTER: return String::New("center");
     case ELM_NOTIFY_ORIENT_BOTTOM: return String::New("bottom");
     case ELM_NOTIFY_ORIENT_LEFT: return String::New("left");
     case ELM_NOTIFY_ORIENT_RIGHT: return String::New("right");
     case ELM_NOTIFY_ORIENT_TOP_LEFT: return String::New("top-left");
     case ELM_NOTIFY_ORIENT_TOP_RIGHT: return String::New("top-right");
     case ELM_NOTIFY_ORIENT_BOTTOM_LEFT: return String::New("bottom-left");
     case ELM_NOTIFY_ORIENT_BOTTOM_RIGHT: return String::New("bottom-right");
     default: return String::New("unknown");
   }
}

void CElmNotify::orient_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        elm_notify_orient_set(eo, (Elm_Notify_Orient)val->Int32Value());
        return;
     }

   if (!val->IsString())
     return;

   String::Utf8Value orient(val->ToString());
   if (!strcmp(*orient, "top"))
     elm_notify_orient_set(eo, ELM_NOTIFY_ORIENT_TOP);
   else if (!strcmp(*orient, "center"))
     elm_notify_orient_set(eo, ELM_NOTIFY_ORIENT_CENTER);
   else if (!strcmp(*orient, "bottom"))
     elm_notify_orient_set(eo, ELM_NOTIFY_ORIENT_BOTTOM);
   else if (!strcmp(*orient, "left"))
     elm_notify_orient_set(eo, ELM_NOTIFY_ORIENT_LEFT);
   else if (!strcmp(*orient, "right"))
     elm_notify_orient_set(eo, ELM_NOTIFY_ORIENT_RIGHT);
   else if (!strcmp(*orient, "top-left"))
     elm_notify_orient_set(eo, ELM_NOTIFY_ORIENT_TOP_LEFT);
   else if (!strcmp(*orient, "top-right"))
     elm_notify_orient_set(eo, ELM_NOTIFY_ORIENT_TOP_RIGHT);
   else if (!strcmp(*orient, "bottom-left"))
     elm_notify_orient_set(eo, ELM_NOTIFY_ORIENT_BOTTOM_LEFT);
   else if (!strcmp(*orient, "bottom-right"))
     elm_notify_orient_set(eo, ELM_NOTIFY_ORIENT_BOTTOM_RIGHT);
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
