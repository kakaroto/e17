#include "elm.h"
#include "CElmToolbar.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmToolbar, icon_size);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, icon_order_lookup);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, homogeneous);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, item_align);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, shrink_mode);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, horizontal);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, standard_priority);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, select_mode);
GENERATE_RO_PROPERTY_CALLBACKS(CElmToolbar, items_count);

GENERATE_TEMPLATE(CElmToolbar,
                  PROPERTY(icon_size),
                  PROPERTY(icon_order_lookup),
                  PROPERTY(homogeneous),
                  PROPERTY(item_align),
                  PROPERTY(shrink_mode),
                  PROPERTY(horizontal),
                  PROPERTY(standard_priority),
                  PROPERTY_RO(items_count),
                  PROPERTY(select_mode));

CElmToolbar::CElmToolbar(Local <Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_toolbar_add(parent->GetEvasObject()))
{
}

CElmToolbar::State::State(Local<Object> item, Local<Value> value)
{
   HandleScope scope;

   Local<Object> obj = value->ToObject();
   Local<Value> icon = obj->Get(String::NewSymbol("icon"));
   Local<Value> label = obj->Get(String::NewSymbol("label"));

   state = elm_toolbar_item_state_add(Item::Unwrap(item)->object_item,
                                      icon->IsUndefined() ? NULL : *String::Utf8Value(icon),
                                      label->IsUndefined() ? NULL : *String::Utf8Value(label),
                                      OnSelect, this);

   self = Persistent<Object>::New(obj->Clone());
}

void CElmToolbar::State::OnSelect(void *data, Evas_Object *, void *)
{
   HandleScope scope;
   Handle<Object> self = static_cast<State *>(data)->ToObject();

   Local<Function> callback
      (Function::Cast(*self->Get(String::NewSymbol("on_select"))));

   if (!callback->IsFunction())
     return;

   Handle<Value> args[1] = { self };
   callback->Call(self, 1, args);
}

CElmToolbar::Item::Item(Handle<Value> value, Handle<Object> parent)
{
   HandleScope scope;
   static Persistent<ObjectTemplate> tmpl;

   if (tmpl.IsEmpty())
     {
        Local<FunctionTemplate> klass = FunctionTemplate::New();
        klass->SetClassName(String::NewSymbol("ToolbarItem"));

        tmpl = Persistent<ObjectTemplate>::New(klass->InstanceTemplate());

        tmpl->SetAccessor(String::NewSymbol("icon"), Item::GetIcon,
                          Item::SetIcon);
        tmpl->SetAccessor(String::NewSymbol("label"),
                          Item::GetLabel, Item::SetLabel);
        tmpl->SetAccessor(String::NewSymbol("priority"),
                          Item::GetPriority, Item::SetPriority);
        tmpl->SetAccessor(String::NewSymbol("separator"),
                          Item::GetSeparator, Item::SetSeparator);
        tmpl->SetAccessor(String::NewSymbol("enable"),
                          Item::GetEnable, Item::SetEnable);
        tmpl->SetAccessor(String::NewSymbol("element"),
                          Item::GetElement, Item::SetElement);
        tmpl->SetAccessor(String::NewSymbol("states"),
                          Item::GetStates, Item::SetStates);
        tmpl->SetAccessor(String::NewSymbol("state"),
                          Item::GetState, Item::SetState);
     }

   Local<Value> before = value->ToObject()->Get(String::NewSymbol("before"));

   self = Persistent<Object>::New(tmpl->NewInstance());
   self->SetHiddenValue(String::NewSymbol("item"), External::Wrap(this));
   self->SetHiddenValue(String::NewSymbol("parent"), parent);

   if (before->IsUndefined())
     {
        object_item = elm_toolbar_item_append
           (GetEvasObjectFromJavascript(parent), NULL, NULL, OnSelect, this);
     }
   else
     {
        object_item = elm_toolbar_item_insert_before
           (GetEvasObjectFromJavascript(parent),
            (Elm_Object_Item *)External::Unwrap(before),
            NULL, NULL, OnSelect, this);
     }

   Local<Object> val = value->ToObject();
   Local<Array> props = val->GetOwnPropertyNames();

   for (unsigned int i = 0; i < props->Length(); i++)
     {
        Local<Value> key = props->Get(i);
        self->Set(key, val->Get(key));
     }
}

CElmToolbar::Item::~Item()
{
   self.Dispose();
   elm_object_item_del(object_item);
}

CElmToolbar::Item *CElmToolbar::Item::Unwrap(Handle<Value> value)
{
   Handle<Value> self = value->ToObject()->GetHiddenValue(String::NewSymbol("item"));

   if (self.IsEmpty())
     return NULL;

   return static_cast<CElmToolbar::Item *> (External::Unwrap(self));
}

CElmToolbar::Item *CElmToolbar::Item::Unwrap(const AccessorInfo &info)
{
   return Unwrap(info.This());
}

Handle<Value> CElmToolbar::Item::GetIcon(Local<String> key, const AccessorInfo &info)
{
   return Unwrap(info)->ToObject()->GetHiddenValue(key);
}

void CElmToolbar::Item::SetIcon(Local<String> key, Local<Value> value, const AccessorInfo &info)
{
   if (value->IsString())
     {
        Item *item = Unwrap(info);
        item->ToObject()->SetHiddenValue(key, value);
        elm_toolbar_item_icon_set(item->object_item, *String::Utf8Value(value));
     }
}

Handle<Value> CElmToolbar::Item::GetLabel(Local<String>, const AccessorInfo &info)
{
   return String::New(elm_object_item_part_text_get(Unwrap(info)->object_item, NULL));
}

void CElmToolbar::Item::SetLabel(Local<String>, Local<Value> value, const AccessorInfo &info)
{
   if (value->IsString())
     elm_object_item_part_text_set(Unwrap(info)->object_item, NULL, *String::Utf8Value(value));
}

void CElmToolbar::Item::SetPriority(Local<String>, Local<Value> value, const AccessorInfo &info)
{
   if (value->IsNumber())
     elm_toolbar_item_priority_set(Unwrap(info)->object_item, value->Int32Value());
}

Handle<Value> CElmToolbar::Item::GetPriority(Local<String>, const AccessorInfo &info)
{
   return Number::New(elm_toolbar_item_priority_get(Unwrap(info)->object_item));
}

void CElmToolbar::Item::SetSeparator(Local<String>, Local<Value> value, const AccessorInfo &info)
{
   elm_toolbar_item_separator_set(Unwrap(info)->object_item, value->BooleanValue());
}

Handle<Value> CElmToolbar::Item::GetSeparator(Local<String>, const AccessorInfo &info)
{
   return Boolean::New(elm_toolbar_item_separator_get(Unwrap(info)->object_item));
}

void CElmToolbar::Item::SetEnable(Local<String>, Local<Value> value, const AccessorInfo &info)
{
   elm_object_item_disabled_set(Unwrap(info)->object_item, !value->BooleanValue());
}

Handle<Value> CElmToolbar::Item::GetEnable(Local<String>, const AccessorInfo &info)
{
   return Boolean::New(!elm_object_item_disabled_get(Unwrap(info)->object_item));
}

void CElmToolbar::Item::SetElement(Local<String> key, Local<Value> value, const AccessorInfo &info)
{
   if (!value->IsObject())
     return;

   HandleScope scope;
   Item *item = Unwrap(info);
   Handle<Object> self = item->ToObject();
   Local<Value> parent = self->GetHiddenValue(String::NewSymbol("parent"));

   Handle<Value> realised = Realise(value->ToObject(), parent);
   elm_object_item_part_content_set(item->object_item, "object",
                                    GetEvasObjectFromJavascript(realised));

   self->SetHiddenValue(key, realised);
}

Handle<Value> CElmToolbar::Item::GetElement(Local<String> key, const AccessorInfo &info)
{
   return Unwrap(info)->ToObject()->GetHiddenValue(key);
}

void CElmToolbar::Item::SetStates(Local<String> key, Local<Value> value, const AccessorInfo &info)
{
   HandleScope scope;
   static Persistent<ObjectTemplate> tmpl;

   if (tmpl.IsEmpty())
     {
        Local<FunctionTemplate> klass = FunctionTemplate::New();
        klass->SetClassName(String::NewSymbol("ToolbarItemStates"));
        tmpl = Persistent<ObjectTemplate>::New(klass->InstanceTemplate());
        tmpl->SetNamedPropertyHandler(StatesGetter, StatesSetter);
     }

   if (!value->IsObject())
     return;

   Local<Object> obj = tmpl->NewInstance();
   obj->SetHiddenValue(String::NewSymbol("item"), info.This());
   Unwrap(info)->ToObject()->SetHiddenValue(key, obj);

   Local<Object> val = value->ToObject();
   Local<Array> props = val->GetOwnPropertyNames();

   for (unsigned int i = 0; i < props->Length(); i++)
     {
        Local<Value> _key = props->Get(i);
        obj->Set(_key, val->Get(_key));
     }
}

Handle<Value> CElmToolbar::Item::GetStates(Local<String> key, const AccessorInfo &info)
{
   return Unwrap(info)->ToObject()->GetHiddenValue(key);
}

Handle<Value> CElmToolbar::Item::StatesSetter(Local<String> attr, Local<Value> val, const AccessorInfo& info)
{
   Local<Value> item = info.This()->GetHiddenValue(String::NewSymbol("item"));
   info.This()->ForceSet(attr, External::Wrap((new State(item->ToObject(), val))->state));
   return val;
}

Handle<Value> CElmToolbar::Item::StatesGetter(Local<String>, const AccessorInfo&)
{
   return Handle<Value>();
}

Handle<Value> CElmToolbar::Item::GetState(Local<String>, const AccessorInfo &info)
{
   HandleScope scope;
   Item *item = Unwrap(info);
   Elm_Toolbar_Item_State *state = elm_toolbar_item_state_get(item->object_item);

   Handle<Object> states = item->ToObject()->Get(String::NewSymbol("states"))->ToObject();
   Local<Array> props = states->GetOwnPropertyNames();

   for (unsigned int i = 0; i < props->Length(); i++)
     {
        Local<Value> key = props->Get(i);
        if ((void *)state == External::Unwrap(states->Get(key)))
          return key;
     }

   return Undefined();
}

void CElmToolbar::Item::SetState(Local<String>, Local<Value> value, const AccessorInfo &info)
{
   HandleScope scope;
   static Persistent<ObjectTemplate> tmpl;
   Item *item = Unwrap(info);

   if (value->IsUndefined())
     {
        elm_toolbar_item_state_unset(item->object_item);
        return;
     }

   if (value->IsString())
     value = info.This()->Get(String::NewSymbol("states"))->ToObject()->Get(value);

   elm_toolbar_item_state_set(item->object_item, (Elm_Toolbar_Item_State *)External::Unwrap(value));
}

Handle<Value> CElmToolbar::Pack(Handle<Value> value, Handle<Value> replace)
{
   HandleScope scope;
   Local<Object> obj = value->ToObject();

   Local<String> priority = String::NewSymbol("priority");
   if (obj->Get(priority)->IsUndefined() && replace->IsObject())
     obj->Set(priority, replace->ToObject()->GetHiddenValue(priority));

   Local<String> str = String::NewSymbol("before");
   Local<Value> before = obj->Get(str);

   if (before->IsUndefined() && replace->IsObject())
     before = replace->ToObject()->Get(str);
   else if (before->IsString() || before->IsNumber())
     before = jsObject->Get(String::NewSymbol("elements"))->ToObject()->Get(before);

   obj->Set(str, before);

   return scope.Close((new Item(value, jsObject))->ToObject());
}

Handle<Value> CElmToolbar::Unpack(Handle<Value> value)
{
   HandleScope scope;
   Item *item = Item::Unwrap(value);

   if (!item)
     return value;

   Elm_Object_Item *next = elm_toolbar_item_next_get(item->object_item);
   Local<Object> obj = value->ToObject();

   if (next)
     obj->Set(String::NewSymbol("before"), External::Wrap(next));

   Local<String> priority = String::NewSymbol("priority");
   obj->SetHiddenValue(priority, obj->Get(priority));

   delete item;
   return value;
}

void CElmToolbar::Item::OnSelect(void *data, Evas_Object *, void *)
{
   HandleScope scope;
   Handle<Object> self = static_cast<Item *>(data)->ToObject();

   Local<Function> callback
      (Function::Cast(*self->Get(String::NewSymbol("on_select"))));

   if (!callback->IsFunction())
     return;

   Handle<Value> args[1] = { self };
   callback->Call(self, 1, args);
}

void CElmToolbar::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Toolbar"), GetTemplate()->GetFunction());
}

Handle<Value> CElmToolbar::icon_size_get() const
{
   return Number::New(elm_toolbar_icon_size_get(eo));
}

void CElmToolbar::icon_size_set(Handle<Value> value)
{
   if (value->IsInt32())
     elm_toolbar_icon_size_set(eo, value->Int32Value());
}

Handle<Value> CElmToolbar::icon_order_lookup_get() const
{
   switch (elm_toolbar_icon_order_lookup_get(eo)) {
     case ELM_ICON_LOOKUP_FDO_THEME:
       return String::NewSymbol("freedektoptheme");
     case ELM_ICON_LOOKUP_THEME_FDO:
       return String::NewSymbol("themefreedektop");
     case ELM_ICON_LOOKUP_FDO:
       return String::NewSymbol("freedektop");
     case ELM_ICON_LOOKUP_THEME:
       return String::NewSymbol("theme");
     default:
       return String::NewSymbol("unknown");
   }
}

void CElmToolbar::icon_order_lookup_set(Handle<Value> value)
{
   if (!value->IsInt32())
     return;

   elm_toolbar_icon_order_lookup_set(eo, (Elm_Icon_Lookup_Order) value->Int32Value());
}

Handle<Value> CElmToolbar::select_mode_get() const
{
   switch (elm_toolbar_select_mode_get(eo)) {
     case ELM_OBJECT_SELECT_MODE_ALWAYS:
       return String::NewSymbol("always");
     case ELM_OBJECT_SELECT_MODE_NONE:
       return String::NewSymbol("none");
     case ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY:
       return String::NewSymbol("display-only");
     default:
       return String::NewSymbol("default");
   }

   return String::NewSymbol("unknown");
}

void CElmToolbar::select_mode_set(Handle<Value> value)
{
   String::Utf8Value mode(value->ToString());

   if (!strcmp(*mode, "default"))
     elm_toolbar_select_mode_set(eo, ELM_OBJECT_SELECT_MODE_DEFAULT);
   else if (!strcmp(*mode, "always"))
     elm_toolbar_select_mode_set(eo, ELM_OBJECT_SELECT_MODE_ALWAYS);
   else if (!strcmp(*mode, "none"))
     elm_toolbar_select_mode_set(eo, ELM_OBJECT_SELECT_MODE_NONE);
   else if (!strcmp(*mode, "display-only"))
     elm_toolbar_select_mode_set(eo, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
}

Handle<Value> CElmToolbar::homogeneous_get() const
{
   return Boolean::New(elm_toolbar_homogeneous_get(eo));
}

void CElmToolbar::homogeneous_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_toolbar_homogeneous_set(eo, value->BooleanValue());
}

Handle<Value> CElmToolbar::item_align_get() const
{
   return Number::New(elm_toolbar_align_get(eo));
}

void CElmToolbar::item_align_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_toolbar_align_set(eo, value->NumberValue());
}

Handle<Value> CElmToolbar::shrink_mode_get() const
{
   switch (elm_toolbar_shrink_mode_get(eo)) {
     case ELM_TOOLBAR_SHRINK_NONE:
       return String::NewSymbol("none");
     case ELM_TOOLBAR_SHRINK_HIDE:
       return String::NewSymbol("hide");
     case ELM_TOOLBAR_SHRINK_SCROLL:
       return String::NewSymbol("scroll");
     case ELM_TOOLBAR_SHRINK_MENU:
       return String::NewSymbol("menu");
     case ELM_TOOLBAR_SHRINK_EXPAND:
       return String::NewSymbol("expand");
     case ELM_TOOLBAR_SHRINK_LAST:
       return String::NewSymbol("last");
     default:
       return String::NewSymbol("unknown");
   }
}

void CElmToolbar::shrink_mode_set(Handle<Value> value)
{
   String::Utf8Value mode_string(value->ToString());

   if (!strcmp(*mode_string, "none"))
     elm_toolbar_shrink_mode_set(eo, ELM_TOOLBAR_SHRINK_NONE);
   else if (!strcmp(*mode_string, "hide"))
     elm_toolbar_shrink_mode_set(eo, ELM_TOOLBAR_SHRINK_HIDE);
   else if (!strcmp(*mode_string, "scroll"))
     elm_toolbar_shrink_mode_set(eo, ELM_TOOLBAR_SHRINK_SCROLL);
   else if (!strcmp(*mode_string, "menu"))
     elm_toolbar_shrink_mode_set(eo, ELM_TOOLBAR_SHRINK_MENU);
   else if (!strcmp(*mode_string, "expand"))
     elm_toolbar_shrink_mode_set(eo, ELM_TOOLBAR_SHRINK_EXPAND);
   else if (!strcmp(*mode_string, "last"))
     elm_toolbar_shrink_mode_set(eo, ELM_TOOLBAR_SHRINK_LAST);
}

void CElmToolbar::horizontal_set(Handle<Value> value)
{
   if (value->IsBoolean())
      elm_toolbar_horizontal_set(eo, value->BooleanValue());
}

Handle<Value> CElmToolbar::horizontal_get() const
{
   return Boolean::New(elm_toolbar_horizontal_get(eo));
}

Handle<Value> CElmToolbar::standard_priority_get() const
{
   return Number::New(elm_toolbar_standard_priority_get(eo));
}

void CElmToolbar::standard_priority_set(Handle<Value> value)
{
   if (value->IsInt32())
      elm_toolbar_standard_priority_set(eo, value->Int32Value());
}

Handle<Value> CElmToolbar::items_count_get() const
{
   return Number::New(elm_toolbar_items_count(eo));
}

}
