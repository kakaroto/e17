#include "elm.h"
#include "CElmNaviframe.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmNaviframe, title_visible);
GENERATE_PROPERTY_CALLBACKS(CElmNaviframe, event_enabled);
GENERATE_PROPERTY_CALLBACKS(CElmNaviframe, prev_btn_auto_pushed);
GENERATE_PROPERTY_CALLBACKS(CElmNaviframe, item_style);
GENERATE_PROPERTY_CALLBACKS(CElmNaviframe, content_preserve_on_pop);
GENERATE_RO_PROPERTY_CALLBACKS(CElmNaviframe, items);
GENERATE_RO_PROPERTY_CALLBACKS(CElmNaviframe, top_item);
GENERATE_RO_PROPERTY_CALLBACKS(CElmNaviframe, bottom_item);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, pop);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, push);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, promote);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, item_promote);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, pop_to);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmNaviframe,
                  PROPERTY(title_visible),
                  PROPERTY(event_enabled),
                  PROPERTY(prev_btn_auto_pushed),
                  PROPERTY(item_style),
                  PROPERTY(content_preserve_on_pop),
                  PROPERTY_RO(items),
                  PROPERTY_RO(top_item),
                  PROPERTY_RO(bottom_item),
                  METHOD(pop),
                  METHOD(push),
                  METHOD(promote),
                  METHOD(item_promote),
                  METHOD(pop_to));

CElmNaviframe::CElmNaviframe(Local<Object> _jsObject, CElmObject *p)
   : CElmLayout(_jsObject, elm_naviframe_add(p->GetEvasObject()))
   , title_visible(true)
   , stack(Persistent<Array>::New(Array::New()))
{
}

CElmNaviframe::~CElmNaviframe()
{
   for (uint32_t items = stack->Length(); items; items--)
     stack->Delete(items);

   item_style.Dispose();
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
   Handle<Value> prev_btn, next_btn, content;

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

Handle<Value> CElmNaviframe::item_promote(const Arguments& args)
{
   Elm_Object_Item *it = static_cast<Elm_Object_Item *>(External::Unwrap(args[0]->ToObject()));
   elm_naviframe_item_promote(it);
   return Undefined();
}

Handle<Value> CElmNaviframe::pop_to(const Arguments& args)
{
   Elm_Object_Item *it = static_cast<Elm_Object_Item *>(External::Unwrap(args[0]->ToObject()));
   elm_naviframe_item_pop_to(it);
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

void CElmNaviframe::prev_btn_auto_pushed_set(Handle<Value> val)
{
   if(val->IsBoolean())
     elm_naviframe_prev_btn_auto_pushed_set(eo, val->BooleanValue());
}

Handle<Value> CElmNaviframe::prev_btn_auto_pushed_get() const
{
   return Boolean::New(elm_naviframe_prev_btn_auto_pushed_get(eo));
}

void CElmNaviframe::item_style_set(Handle<Value> val)
{
   if(!val->IsArray())
     return;

   Local<Object> style = val->ToObject();
   Elm_Object_Item *it = static_cast<Elm_Object_Item *>(External::Unwrap(style->Get(0)->ToObject()));
   elm_naviframe_item_style_set(it, *String::Utf8Value(style->Get(1)->ToString()));

   item_style.Dispose();
   item_style = Persistent<Value>::New(val);
}

Handle<Value> CElmNaviframe::item_style_get() const
{
   return item_style->ToObject()->Get(1)->ToString();
}

void CElmNaviframe::content_preserve_on_pop_set(Handle<Value> val)
{
   if(val->IsBoolean())
     elm_naviframe_content_preserve_on_pop_set(eo, val->BooleanValue());
}

Handle<Value> CElmNaviframe::content_preserve_on_pop_get() const
{
   return Boolean::New(elm_naviframe_content_preserve_on_pop_get(eo));
}

Handle<Value> CElmNaviframe::items_get() const
{
   Eina_List *l = elm_naviframe_items_get(eo);
   Handle<Array> arr = Array::New(eina_list_count(l));

   void *d;
   int i = 0;
   EINA_LIST_FREE(l, d)
     {
        arr->Set(i, External::Wrap(d)); ++i;
     }

   return arr;
}

Handle<Value> CElmNaviframe::top_item_get() const
{
   return External::Wrap(elm_naviframe_top_item_get(eo));
}

Handle<Value> CElmNaviframe::bottom_item_get() const
{
   return External::Wrap(elm_naviframe_bottom_item_get(eo));
}

}
