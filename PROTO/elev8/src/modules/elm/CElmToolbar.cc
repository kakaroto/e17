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
GENERATE_METHOD_CALLBACKS(CElmToolbar, item_state_get);
GENERATE_METHOD_CALLBACKS(CElmToolbar, item_state_set);

GENERATE_TEMPLATE(CElmToolbar,
                  PROPERTY(icon_size),
                  PROPERTY(icon_order_lookup),
                  PROPERTY(homogeneous),
                  PROPERTY(item_align),
                  PROPERTY(shrink_mode),
                  PROPERTY(horizontal),
                  PROPERTY(standard_priority),
                  PROPERTY_RO(items_count),
                  METHOD(item_state_set),
                  METHOD(item_state_get),
                  PROPERTY(select_mode));

CElmToolbar::CElmToolbar(Local <Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_toolbar_add(parent->GetEvasObject()))
{
}

void CElmToolbar::AddItemStates(Local<Object> obj, Elm_Object_Item *item)
{
   Local<Value> states = obj->Get(String::NewSymbol("states"));
   if (!states->IsObject())
     return;

   Local<Object> states_obj = states->ToObject();
   Local<Array> props = states_obj->GetOwnPropertyNames();
   Handle<Object> wrapped_states = Object::New();

   for (unsigned i = 0; i < props->Length(); ++i)
     {
        Local<Value> key = props->Get(i);
        Local<Value> paramValues = states_obj->Get(key->ToString());
        if (!paramValues->IsObject())
          continue;

        Local<Object> params = paramValues->ToObject();

        Local<Value> icon = params->Get(String::NewSymbol("icon"));
        Local<Value> data = params->Get(String::NewSymbol("data"));
        Local<Value> label = params->Get(String::NewSymbol("label"));
        Local<Value> callback = params->Get(String::NewSymbol("on_select"));

        Elm_Toolbar_Item_State *state = elm_toolbar_item_state_add(item,
            icon->IsUndefined() ? NULL : *String::Utf8Value(icon),
            label->IsUndefined() ? NULL : *String::Utf8Value(label),
            OnSelect, new Item(jsObject, data, callback));

        wrapped_states->Set(key, External::Wrap(state));
     }

   obj->SetHiddenValue(String::NewSymbol("elm::toolbar::states"), wrapped_states);
}

Elm_Object_Item *CElmToolbar::CreateItem(Handle<Value> next,
   Handle<Value> icon, Handle<Value> label,
   Handle<Value> callback, Handle<Value> data)
{
   if (!next.IsEmpty())
     return elm_toolbar_item_insert_before(eo,
                    (Elm_Object_Item *)External::Unwrap(next),
                    icon->IsUndefined() ? NULL : *String::Utf8Value(icon),
                    label->IsUndefined() ? NULL : *String::Utf8Value(label),
                    OnSelect, new Item(jsObject, data, callback));

   return elm_toolbar_item_append(eo,
                                  icon->IsUndefined() ? NULL : *String::Utf8Value(icon),
                                  label->IsUndefined() ? NULL : *String::Utf8Value(label),
                                  OnSelect, new Item(jsObject, data, callback));
}

Elm_Object_Item *CElmToolbar::CreateItem(Handle<Value> next)
{
   if (!next.IsEmpty())
     return elm_toolbar_item_insert_before(eo,
                    (Elm_Object_Item *)External::Unwrap(next),
                    NULL, NULL, NULL, NULL);

   return elm_toolbar_item_append(eo, NULL, NULL, NULL, NULL);
}

Handle<Value> CElmToolbar::Pack(Handle<Value> value, Handle<Value> replace)
{
   HandleScope scope;
   Local<Object> obj = value->ToObject()->Clone();
   Elm_Object_Item *item;
   Local<Value> next;

   if (!replace->IsUndefined())
     next = replace->ToObject()->GetHiddenValue(String::NewSymbol("elm::toolbar::next"));

   if (obj->Has(String::NewSymbol("element")))
     {
        item = CreateItem(next);
        Handle<Value> realised = Realise(obj->Get(String::NewSymbol("element"))->ToObject(), jsObject);
        obj->Set(String::NewSymbol("element"), realised);
        elm_object_item_part_content_set(item, "object", GetEvasObjectFromJavascript(realised));
     }
   else if (obj->Get(String::NewSymbol("separator"))->BooleanValue())
     {
        item = CreateItem(next);
        elm_toolbar_item_separator_set(item, EINA_TRUE);
     }
   else
     {
        item = CreateItem(next, obj->Get(String::NewSymbol("icon")),
             obj->Get(String::NewSymbol("label")), obj->Get(String::NewSymbol("on_select")),
             obj->Get(String::NewSymbol("data")));
        AddItemStates(obj, item);
     }

   if (obj->Has(String::NewSymbol("priority")))
     elm_toolbar_item_priority_set(item, obj->Get(String::NewSymbol("priority"))->Int32Value());

   if (obj->Has(String::NewSymbol("enable")))
     elm_object_item_disabled_set(item, !obj->Get(String::NewSymbol("enable"))->BooleanValue());

   obj->SetHiddenValue(String::NewSymbol("elm::toolbar::item"), External::Wrap(item));

   return scope.Close(obj);
}

Handle<Value> CElmToolbar::Unpack(Handle<Value> value)
{
   HandleScope scope;
   Handle<Object> obj = value->ToObject();
   Handle<Value> item = obj->GetHiddenValue(String::NewSymbol("elm::toolbar::item"));
   Elm_Object_Item *elm_item = (Elm_Object_Item *)External::Unwrap(item);
   Elm_Object_Item *next = elm_toolbar_item_next_get(elm_item);

   if (next)
      obj->SetHiddenValue(String::NewSymbol("elm::toolbar::next"), External::Wrap(next));

   delete static_cast<Item *>(elm_object_item_data_get(elm_item));
   elm_object_item_del(elm_item);
   return obj;
}

void CElmToolbar::OnSelect(void *data, Evas_Object *, void *)
{
   Item *item = static_cast<Item *>(data);

   if (!item->callback->IsFunction())
     return;

   HandleScope scope;
   Local<Function> callback(Function::Cast(*item->callback));

   if (item->data->IsUndefined())
     {
        callback->Call(item->self, 0, NULL);
     }
   else
     {
        Handle<Value> args[1] = { item->data };
        callback->Call(item->self, 1, args);
     }

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

Handle<Value> CElmToolbar::item_state_set(const Arguments& args)
{
   Local<Object> item = args[0]->ToObject();
   if (item->IsUndefined())
     return Undefined();

   Elm_Object_Item *toolbar_item = static_cast<Elm_Object_Item *>(External::Unwrap(item->GetHiddenValue(String::NewSymbol("elm::toolbar::item"))));
   if (!toolbar_item)
     return Undefined();

   if (!args[1]->IsString())
     {
        elm_toolbar_item_state_unset(toolbar_item);
        return Boolean::New(true);
     }

   Handle<Object> states = item->GetHiddenValue(String::NewSymbol("elm::toolbar::states"))->ToObject();
   Elm_Toolbar_Item_State *state = static_cast<Elm_Toolbar_Item_State *>(External::Unwrap(states->Get(args[1]->ToString())));
   if (state)
     return Boolean::New(elm_toolbar_item_state_set(toolbar_item, state));

   return Undefined();
}

Handle<Value> CElmToolbar::item_state_get(const Arguments& args)
{
   Local<Object> item = args[0]->ToObject();
   if (item->IsUndefined())
     return Undefined();

   Elm_Object_Item *toolbar_item = static_cast<Elm_Object_Item *>(External::Unwrap(item->GetHiddenValue(String::NewSymbol("elm::toolbar::item"))));
   if (!toolbar_item)
     return Undefined();

   Handle<Object> states = item->GetHiddenValue(String::NewSymbol("elm::toolbar::states"))->ToObject();
   Local<Array> props = states->GetOwnPropertyNames();
   Elm_Toolbar_Item_State *elm_state = elm_toolbar_item_state_get(toolbar_item);

   for (unsigned i = 0; i < props->Length(); ++i)
     {
        Local<Value> key = states->Get(i);
        if ((void *)elm_state == (void *)External::Unwrap(states->Get(key->ToString())))
          return key;
     }

   return Undefined();
}

}
