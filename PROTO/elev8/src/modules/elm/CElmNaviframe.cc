#include "elm.h"
#include "CElmNaviframe.h"

namespace elm {

using namespace v8;

GENERATE_METHOD_CALLBACKS(CElmNaviframe, pop);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, push);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, promote);

GENERATE_TEMPLATE(CElmNaviframe,
                  METHOD(pop),
                  METHOD(push),
                  METHOD(promote));

CElmNaviframe::CElmNaviframe(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_naviframe_add(parent->GetEvasObject()))
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

   return Undefined();
}

Handle<Value> CElmNaviframe::push(const Arguments& args)
{
   Local<Value> prev_btn, next_btn, content;

   if (!args[0]->IsObject())
     return ThrowException(Exception::Error(String::New("Parameter 1 should be an object description or an elm.widget")));

   if (!args[1]->IsString())
     return ThrowException(Exception::Error(String::New("Parameter 2 should be a string")));

   if (args.Length() >= 3)
     {
        if (!args[2]->IsObject())
          return ThrowException(Exception::Error(String::New("Parameter 3 should either be undefined or an object description")));

        prev_btn = Realise(args[2]->ToObject(), GetJSObject());
     }

   if (args.Length() >= 4)
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
   return stacked;
}

Handle<Value> CElmNaviframe::promote(const Arguments& args)
{
   elm_naviframe_item_simple_promote(eo, GetEvasObjectFromJavascript(args[0]));
   return Undefined();
}

}
