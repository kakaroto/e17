#include "elm.h"
#include "CElmToolbar.h"

namespace elm {

using namespace v8;

GENERATE_METHOD_CALLBACKS(CElmToolbar, append);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, icon_size);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, icon_order_lookup);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, homogeneous);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, align);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, shrink_mode);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, horizontal);
GENERATE_PROPERTY_CALLBACKS(CElmToolbar, standard_priority);
GENERATE_RO_PROPERTY_CALLBACKS(CElmToolbar, items_count);

GENERATE_TEMPLATE(CElmToolbar,
                  METHOD(append),
                  PROPERTY(icon_size),
                  PROPERTY(icon_order_lookup),
                  PROPERTY(homogeneous),
                  PROPERTY(align),
                  PROPERTY(shrink_mode),
                  PROPERTY(horizontal),
                  PROPERTY(standard_priority),
                  PROPERTY_RO(items_count));

CElmToolbar::CElmToolbar(Local <Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_toolbar_add(parent->GetEvasObject()))
{
}

void CElmToolbar::DidRealiseElement(Local<Value> val)
{
   Local<Object> obj = val->ToObject();
   append(obj->Get(String::NewSymbol("icon")), obj->Get(String::NewSymbol("label")),
          obj->Get(String::NewSymbol("data")), obj->Get(String::NewSymbol("on_select")));
}

void CElmToolbar::append(Local<Value> icon, Local<Value> label,
                         Local<Value> data, Local<Value> callback)
{
   elm_toolbar_item_append(eo,
                           icon->IsUndefined() ? NULL : *String::Utf8Value(icon),
                           label->IsUndefined() ? NULL : *String::Utf8Value(label),
                           OnSelect, new Item(jsObject, data, callback));
}

Handle<Value> CElmToolbar::append(const Arguments& args)
{
   append(args[0], args[1], args[2], args[3]);
   return Undefined();
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

Handle<Value> CElmToolbar::homogeneous_get() const
{
   return Boolean::New(elm_toolbar_homogeneous_get(eo));
}

void CElmToolbar::homogeneous_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_toolbar_homogeneous_set(eo, value->BooleanValue());
}

Handle<Value> CElmToolbar::align_get() const
{
   return Number::New(elm_toolbar_align_get(eo));
}

void CElmToolbar::align_set(Handle<Value> value)
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
