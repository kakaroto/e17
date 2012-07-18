#include "CElmScroller.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmScroller, bounce);
GENERATE_PROPERTY_CALLBACKS(CElmScroller, policy);
GENERATE_PROPERTY_CALLBACKS(CElmScroller, content);
GENERATE_PROPERTY_CALLBACKS(CElmScroller, widget_base_theme);
GENERATE_PROPERTY_CALLBACKS(CElmScroller, propagate_events);
GENERATE_PROPERTY_CALLBACKS(CElmScroller, horizontal_gravity);
GENERATE_PROPERTY_CALLBACKS(CElmScroller, vertical_gravity);
GENERATE_PROPERTY_CALLBACKS(CElmScroller, relative_page_size);
GENERATE_PROPERTY_CALLBACKS(CElmScroller, page_size);
GENERATE_PROPERTY_CALLBACKS(CElmScroller, limit_minimum_size);
GENERATE_RO_PROPERTY_CALLBACKS(CElmScroller, region);
GENERATE_RO_PROPERTY_CALLBACKS(CElmScroller, current_page);
GENERATE_RO_PROPERTY_CALLBACKS(CElmScroller, last_page);
GENERATE_RO_PROPERTY_CALLBACKS(CElmScroller, size_child);
GENERATE_METHOD_CALLBACKS(CElmScroller, region_show);
GENERATE_METHOD_CALLBACKS(CElmScroller, region_bring_in);
GENERATE_METHOD_CALLBACKS(CElmScroller, page_show);
GENERATE_METHOD_CALLBACKS(CElmScroller, page_bring_in);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmScroller,
                  PROPERTY(bounce),
                  PROPERTY(policy),
                  PROPERTY(content),
                  PROPERTY(widget_base_theme),
                  PROPERTY(propagate_events),
                  PROPERTY(horizontal_gravity),
                  PROPERTY(vertical_gravity),
                  PROPERTY(relative_page_size),
                  PROPERTY(page_size),
                  PROPERTY(limit_minimum_size),
                  PROPERTY_RO(region),
                  PROPERTY_RO(current_page),
                  PROPERTY_RO(last_page),
                  PROPERTY_RO(size_child),
                  METHOD(region_show),
                  METHOD(region_bring_in),
                  METHOD(page_show),
                  METHOD(page_bring_in));

CElmScroller::CElmScroller(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_scroller_add(_parent->GetEvasObject()))
{
}

CElmScroller::CElmScroller(Local<Object> _jsObject, Evas_Object *child)
   : CElmLayout(_jsObject, child)
{
}

void CElmScroller::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Scroller"), GetTemplate()->GetFunction());
}

CElmScroller::~CElmScroller()
{
   cached.content.Dispose();
   widget_base_theme.Dispose();
   relative_page_size.Dispose();
   page_size.Dispose();
   limit_minimum_size.Dispose();
}

void CElmScroller::bounce_set(Handle<Value> val)
{
   Handle<Value> x = val->ToObject()->Get(String::NewSymbol("x"));
   Handle<Value> y = val->ToObject()->Get(String::NewSymbol("y"));

   if ((!x->IsBoolean()) || (!y->IsBoolean()))
     return;

   elm_scroller_bounce_set(eo, x->BooleanValue(), y->BooleanValue());
}

Handle<Value> CElmScroller::bounce_get() const
{
   Eina_Bool x, y;

   elm_scroller_bounce_get(eo, &x, &y);

   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("x"), Boolean::New(x));
   obj->Set(String::NewSymbol("y"), Boolean::New(y));

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
       return String::NewSymbol("auto");
   case ELM_SCROLLER_POLICY_ON:
       return String::NewSymbol("on");
   case ELM_SCROLLER_POLICY_OFF:
       return String::NewSymbol("off");
   case ELM_SCROLLER_POLICY_LAST:
       return String::NewSymbol("last");
   default:
       return String::NewSymbol("unknown");
   }
}

void CElmScroller::policy_set(Handle<Value> val)
{
   Local<Value> x = val->ToObject()->Get(String::NewSymbol("x"));
   Local<Value> y = val->ToObject()->Get(String::NewSymbol("y"));

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
   obj->Set(String::NewSymbol("x"), string_from_policy(x_policy));
   obj->Set(String::NewSymbol("y"), string_from_policy(y_policy));

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

Handle<Value> CElmScroller::widget_base_theme_get() const
{
   return widget_base_theme;
}

const char *elm_widget_style_get(const Evas_Object *obj);

void CElmScroller::widget_base_theme_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Value> widget = val->ToObject()->Get(String::NewSymbol("widget"));
   Local<Value> base = val->ToObject()->Get(String::NewSymbol("base"));

   elm_layout_theme_set(eo, *String::Utf8Value(widget),
             *String::Utf8Value(base), elm_widget_style_get(eo));

   widget_base_theme.Dispose();
   widget_base_theme = Persistent<Value>::New(val);
}

Handle<Value> CElmScroller::region_show(const Arguments &args)
{
   for (int i = 0; i < 4; i++)
     if (!args[i]->IsNumber())
       return Undefined();

   elm_scroller_region_show(eo, args[0]->ToInt32()->Value(), args[1]->ToInt32()->Value(),
       args[2]->ToInt32()->Value(), args[3]->ToInt32()->Value());

   return Undefined();
}

Handle<Value> CElmScroller::region_bring_in(const Arguments &args)
{
   for (int i = 0; i < 4; i++)
     if (!args[i]->IsNumber())
       return Undefined();

   elm_scroller_region_bring_in(eo, args[0]->ToInt32()->Value(), args[1]->ToInt32()->Value(),
       args[2]->ToInt32()->Value(), args[3]->ToInt32()->Value());

   return Undefined();
}

Handle<Value> CElmScroller::region_get() const
{
   HandleScope scope;

   int x, y, w, h;
   elm_scroller_region_get(eo, &x, &y, &w, &h);

   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("x"), Number::New(x));
   obj->Set(String::NewSymbol("y"), Number::New(y));
   obj->Set(String::NewSymbol("width"), Number::New(w));
   obj->Set(String::NewSymbol("height"), Number::New(h));

   return scope.Close(obj);
}

Handle<Value> CElmScroller::page_show(const Arguments &args)
{
   for (int i = 0; i < 2; i++)
     if (!args[i]->IsNumber())
       return Undefined();

   elm_scroller_page_show(eo, args[0]->ToInt32()->Value(), args[1]->ToInt32()->Value());

   return Undefined();
}

Handle<Value> CElmScroller::page_bring_in(const Arguments &args)
{
   for (int i = 0; i < 2; i++)
     if (!args[i]->IsNumber())
       return Undefined();

   elm_scroller_page_show(eo, args[0]->ToInt32()->Value(), args[1]->ToInt32()->Value());

   return Undefined();
}

void CElmScroller::propagate_events_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_scroller_propagate_events_set(eo, val->BooleanValue());
}

Handle<Value> CElmScroller::propagate_events_get() const
{
   return Boolean::New(elm_scroller_propagate_events_get(eo));
}

void CElmScroller::vertical_gravity_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   double horizontal_gravity;
   elm_scroller_gravity_get(eo, &horizontal_gravity, NULL);
   elm_scroller_gravity_set(eo, horizontal_gravity, val->ToNumber()->Value());
}

Handle<Value> CElmScroller::vertical_gravity_get() const
{
   double vertical_gravity;
   elm_scroller_gravity_get(eo, NULL, &vertical_gravity);
   return Number::New(vertical_gravity);
}

void CElmScroller::horizontal_gravity_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   double vertical_gravity;
   elm_scroller_gravity_get(eo, NULL, &vertical_gravity);
   elm_scroller_gravity_set(eo, val->ToNumber()->Value(), vertical_gravity);
}

Handle<Value> CElmScroller::horizontal_gravity_get() const
{
   double horizontal_gravity;
   elm_scroller_gravity_get(eo, &horizontal_gravity, NULL);
   return Number::New(horizontal_gravity);
}

Handle<Value> CElmScroller::current_page_get() const
{
   HandleScope scope;

   int h_pagenumber, v_pagenumber;

   elm_scroller_current_page_get(eo, &h_pagenumber, &v_pagenumber);

   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("horizontal"), Number::New(h_pagenumber));
   obj->Set(String::NewSymbol("vertical"), Number::New(v_pagenumber));

   return scope.Close(obj);
}

Handle<Value> CElmScroller::last_page_get() const
{
   HandleScope scope;

   int h_pagenumber, v_pagenumber;

   elm_scroller_last_page_get(eo, &h_pagenumber, &v_pagenumber);

   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("horizontal"), Number::New(h_pagenumber));
   obj->Set(String::NewSymbol("vertical"), Number::New(v_pagenumber));

   return scope.Close(obj);
}

Handle<Value> CElmScroller::size_child_get() const
{
   HandleScope scope;

   int w, h;

   elm_scroller_child_size_get(eo, &w, &h);

   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("width"), Number::New(w));
   obj->Set(String::NewSymbol("height"), Number::New(h));

   return scope.Close(obj);
}

Handle<Value> CElmScroller::relative_page_size_get() const
{
   return relative_page_size;
}

void CElmScroller::relative_page_size_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Value> horizontal = val->ToObject()->Get(String::NewSymbol("horizontal"));
   Local<Value> vertical = val->ToObject()->Get(String::NewSymbol("vertical"));

   if (!horizontal->IsNumber() || !vertical->IsNumber())
     return;

   elm_scroller_page_relative_set(eo, horizontal->NumberValue(), vertical->NumberValue());

   relative_page_size.Dispose();
   relative_page_size = Persistent<Value>::New(val);
}

Handle<Value> CElmScroller::page_size_get() const
{
   return page_size;
}

void CElmScroller::page_size_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Value> horizontal = val->ToObject()->Get(String::NewSymbol("horizontal"));
   Local<Value> vertical = val->ToObject()->Get(String::NewSymbol("vertical"));

   if (!horizontal->IsNumber() || !vertical->IsNumber())
     return;

   elm_scroller_page_size_set(eo, horizontal->IntegerValue(), vertical->IntegerValue());

   page_size.Dispose();
   page_size = Persistent<Value>::New(val);
}

Handle<Value> CElmScroller::limit_minimum_size_get() const
{
   return limit_minimum_size;
}

void CElmScroller::limit_minimum_size_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Value> horizontal = val->ToObject()->Get(String::NewSymbol("horizontal"));
   Local<Value> vertical = val->ToObject()->Get(String::NewSymbol("vertical"));

   if (!horizontal->IsBoolean() || !vertical->IsBoolean())
     return;

   elm_scroller_content_min_limit(eo, horizontal->BooleanValue(), vertical->BooleanValue());

   limit_minimum_size.Dispose();
   limit_minimum_size = Persistent<Value>::New(val);
}
}
