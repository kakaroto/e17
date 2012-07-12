#include "elm.h"
#include "CElmNaviframe.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmNaviframe, title_visible);
GENERATE_PROPERTY_CALLBACKS(CElmNaviframe, event_enabled);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, pop);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, push);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, promote);

GENERATE_TEMPLATE(CElmNaviframe,
                  PROPERTY(title_visible),
                  PROPERTY(event_enabled),
                  METHOD(pop),
                  METHOD(push),
                  METHOD(promote));

CElmNaviframe::CElmNaviframe(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_naviframe_add(parent->GetEvasObject()))
   , title_visible(true)
   , stack(Persistent<Array>::New(Array::New()))
{
}

CElmNaviframe::~CElmNaviframe()
{
   for (uint32_t items = stack->Length(); items; items--)
     stack->Delete(items);
}

void CElmNaviframe::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Naviframe"), GetTemplate()->GetFunction());
}

Handle<Value> CElmNaviframe::pop(const Arguments&)
{
   if (!stack->Length())
     return Undefined();

   stack->Delete(stack->Length());
   elm_naviframe_item_pop(eo);
   title_visible_eval();

   return Undefined();
}

Handle<Value> CElmNaviframe::push(const Arguments& args)
{
   Local<Value> prev_btn, next_btn, content;

   if (!args[0]->IsObject())
     return ThrowException(Exception::Error(String::New("Parameter 1 should be an object description or an elm.widget")));

   if (!args[1]->IsString())
     return ThrowException(Exception::Error(String::New("Parameter 2 should be a string")));

   if (args.Length() >= 3 && !args[2]->IsUndefined())
     {
        if (!args[2]->IsObject())
          return ThrowException(Exception::Error(String::New("Parameter 3 should either be undefined or an object description")));

        prev_btn = Realise(args[2]->ToObject(), GetJSObject());
     }

   if (args.Length() >= 4 && !args[3]->IsUndefined())
     {
        if (!args[3]->IsObject())
          return ThrowException(Exception::Error(String::New("Parameter 4 should either be undefined or an object description")));

        next_btn = Realise(args[3]->ToObject(), GetJSObject());
     }

   content = Realise(args[0]->ToObject(), GetJSObject());

   Local<Object> stacked = Object::New();
   if (!prev_btn.IsEmpty())
     stacked->Set(String::NewSymbol("prev_btn"), prev_btn);
   if (!next_btn.IsEmpty())
     stacked->Set(String::NewSymbol("next_btn"), next_btn);
   stacked->Set(String::NewSymbol("content"), content);
   stack->Set(stack->Length() + 1, stacked);

   String::Utf8Value titleParam(args[1]->ToString());
   elm_naviframe_item_push(eo,
                           *titleParam,
                           prev_btn.IsEmpty() ? NULL : GetEvasObjectFromJavascript(prev_btn),
                           next_btn.IsEmpty() ? NULL : GetEvasObjectFromJavascript(next_btn),
                           GetEvasObjectFromJavascript(content),
                           0);
   title_visible_eval();
   return stacked;
}

Handle<Value> CElmNaviframe::promote(const Arguments& args)
{
   elm_naviframe_item_simple_promote(eo, GetEvasObjectFromJavascript(args[0]));
   title_visible_eval();
   return Undefined();
}

void CElmNaviframe::title_visible_eval()
{
   if (Elm_Object_Item *top_item = elm_naviframe_top_item_get(eo))
      elm_naviframe_item_title_visible_set(top_item, title_visible);
}

void CElmNaviframe::title_visible_set(Handle<Value> val)
{
   title_visible = val->BooleanValue();
   title_visible_eval();
}

Handle<Value> CElmNaviframe::title_visible_get() const
{
   return Boolean::New(title_visible);
}

void CElmNaviframe::event_enabled_set(Handle<Value> val)
{
   if(val->IsBoolean())
     elm_naviframe_event_enabled_set(eo, val->BooleanValue());
}

Handle<Value> CElmNaviframe::event_enabled_get() const
{
   return Boolean::New(elm_naviframe_event_enabled_get(eo));
}

}
