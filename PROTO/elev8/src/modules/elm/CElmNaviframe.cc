#include "elm.h"
#include "CElmNaviframe.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmNaviframe, title_visible);
GENERATE_PROPERTY_CALLBACKS(CElmNaviframe, event_enabled);
GENERATE_PROPERTY_CALLBACKS(CElmNaviframe, prev_btn_auto_pushed);
GENERATE_PROPERTY_CALLBACKS(CElmNaviframe, content_preserve_on_pop);
GENERATE_RO_PROPERTY_CALLBACKS(CElmNaviframe, items);
GENERATE_RO_PROPERTY_CALLBACKS(CElmNaviframe, top_item);
GENERATE_RO_PROPERTY_CALLBACKS(CElmNaviframe, bottom_item);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, pop);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, promote);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, item_promote);
GENERATE_METHOD_CALLBACKS(CElmNaviframe, pop_to);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmNaviframe,
                  PROPERTY(title_visible),
                  PROPERTY(event_enabled),
                  PROPERTY(prev_btn_auto_pushed),
                  PROPERTY(content_preserve_on_pop),
                  PROPERTY_RO(items),
                  PROPERTY_RO(top_item),
                  PROPERTY_RO(bottom_item),
                  METHOD(pop),
                  METHOD(promote),
                  METHOD(item_promote),
                  METHOD(pop_to));


struct elm::CElmNaviframe::Item::Strings elm::CElmNaviframe::Item::str;

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
}

void CElmNaviframe::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Naviframe"), GetTemplate()->GetFunction());
}

Handle<Value> CElmNaviframe::pop(const Arguments&)
{
   Local<Object> elements = GetJSObject()->Get(String::NewSymbol("elements"))->ToObject();
   Local<Array> props = elements->GetOwnPropertyNames();
   elements->Delete(props->Get(props->Length() - 1)->ToString());
   return Undefined();
}

Handle<Value> CElmNaviframe::Pack(Handle<Value> value, Handle<Value> replace)
{
   HandleScope scope;
   Elm_Object_Item *object_item = NULL;
   Local<Object> obj = value->ToObject();
   Local<Value> before = obj->Get(String::NewSymbol("before"));

   if (before->IsUndefined() && !replace->IsUndefined())
     before = replace->ToObject()->Get(String::NewSymbol("before"));
   else if (before->IsString() || before->IsNumber())
     before = GetJSObject()->Get(String::NewSymbol("elements"))->ToObject()->Get(before);

   bool has_style = obj->Get(String::NewSymbol("style"))->IsString();
   String::Utf8Value style(obj->Get(String::NewSymbol("style"))->ToString());

   if (before->IsUndefined())
     {
        object_item = elm_naviframe_item_push(eo, NULL, NULL, NULL, NULL,
                                              has_style ? *style : NULL);
     }
   else
     {
        Item *item = Item::Unwrap(before);
        object_item = elm_naviframe_item_insert_before(eo, item->object_item,
                                                       NULL, NULL, NULL, NULL,
                                                       has_style ? *style : NULL);
     }

   title_visible_eval();
   return scope.Close((new Item(object_item, obj->ToObject(), GetJSObject()))->ToObject());
}

Handle<Value> CElmNaviframe::Unpack(Handle<Value> value)
{
   elm_naviframe_item_pop(eo);
   return value;
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
