#include "CElmScroller.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmScroller, bounce);
GENERATE_PROPERTY_CALLBACKS(CElmScroller, policy);
GENERATE_PROPERTY_CALLBACKS(CElmScroller, content);

GENERATE_TEMPLATE(CElmScroller,
                  PROPERTY(bounce),
                  PROPERTY(policy),
                  PROPERTY(content));

CElmScroller::CElmScroller(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_scroller_add(parent->GetEvasObject()))
{
}

void CElmScroller::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Scroller"), GetTemplate()->GetFunction());
}

CElmScroller::~CElmScroller()
{
   cached.content.Dispose();
}

void CElmScroller::bounce_set(Handle<Value> val)
{
   Handle<Value> x = val->ToObject()->Get(String::New("x"));
   Handle<Value> y = val->ToObject()->Get(String::New("y"));

   if ((!x->IsBoolean()) || (!y->IsBoolean()))
     return;

   elm_scroller_bounce_set(eo, x->BooleanValue(), y->BooleanValue());
}

Handle<Value> CElmScroller::bounce_get() const
{
   Eina_Bool x, y;

   elm_scroller_bounce_get(eo, &x, &y);

   Local<Object> obj = Object::New();
   obj->Set(String::New("x"), Boolean::New(x));
   obj->Set(String::New("y"), Boolean::New(y));

   return obj;
}

Elm_Scroller_Policy CElmScroller::policy_from_string(Handle<Value> val)
{
   String::Utf8Value str(val);
   Elm_Scroller_Policy policy = ELM_SCROLLER_POLICY_AUTO;

   if (!strcmp(*str, "auto"))
     policy = ELM_SCROLLER_POLICY_AUTO;
   else if (!strcmp(*str, "on"))
     policy = ELM_SCROLLER_POLICY_ON;
   else if (!strcmp(*str, "off"))
     policy = ELM_SCROLLER_POLICY_OFF;
   else if (!strcmp(*str, "last"))
     policy = ELM_SCROLLER_POLICY_LAST;
   else
     ELM_ERR("unknown scroller policy %s", *str);

   return policy;
}

Local<Value> CElmScroller::string_from_policy(Elm_Scroller_Policy policy)
{
   switch (policy) {
   case ELM_SCROLLER_POLICY_AUTO:
       return String::New("auto");
   case ELM_SCROLLER_POLICY_ON:
       return String::New("on");
   case ELM_SCROLLER_POLICY_OFF:
       return String::New("off");
   case ELM_SCROLLER_POLICY_LAST:
       return String::New("last");
   default:
       return String::New("unknown");
   }
}

void CElmScroller::policy_set(Handle<Value> val)
{
   Local<Value> x = val->ToObject()->Get(String::New("x"));
   Local<Value> y = val->ToObject()->Get(String::New("y"));

   if ((!x->IsString()) || (!y->IsString()))
     return;

   Elm_Scroller_Policy x_policy, y_policy;
   x_policy = policy_from_string(x);
   y_policy = policy_from_string(y);
   elm_scroller_policy_set(eo, x_policy, y_policy);
}

Handle<Value> CElmScroller::policy_get() const
{
   Elm_Scroller_Policy x_policy, y_policy;

   elm_scroller_policy_get(eo, &x_policy, &y_policy);

   Local<Object> obj = Object::New();
   obj->Set(String::New("x"), string_from_policy(x_policy));
   obj->Set(String::New("y"), string_from_policy(y_policy));

   return obj;
}

void CElmScroller::content_set(Handle<Value> val)
{
   cached.content.Dispose();
   cached.content = Persistent<Value>::New(Realise(val, jsObject));
   elm_object_content_set(eo, GetEvasObjectFromJavascript(cached.content));
}

Handle<Value> CElmScroller::content_get() const
{
   return cached.content;
}

}
