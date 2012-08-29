#include "CElmCtxPopup.h"

namespace elm {

using namespace v8;

struct CbItemStruct {
  CElmCtxPopup *self;
  Persistent<Value> item;
};

GENERATE_PROPERTY_CALLBACKS(CElmCtxPopup, on_item_select)
GENERATE_PROPERTY_CALLBACKS(CElmCtxPopup, horizontal);
GENERATE_PROPERTY_CALLBACKS(CElmCtxPopup, hover_parent);
GENERATE_PROPERTY_CALLBACKS(CElmCtxPopup, on_dismiss);
GENERATE_RO_PROPERTY_CALLBACKS(CElmCtxPopup, direction);
GENERATE_METHOD_CALLBACKS(CElmCtxPopup, show);
GENERATE_METHOD_CALLBACKS(CElmCtxPopup, dismiss);

GENERATE_TEMPLATE_FULL(CElmObject, CElmCtxPopup,
                        PROPERTY(on_item_select),
                        PROPERTY(horizontal),
                        PROPERTY(hover_parent),
                        PROPERTY(on_dismiss),
                        PROPERTY_RO(direction),
                        METHOD(show),
                        METHOD(dismiss));

static void _dismissed(void *data, Evas_Object *, void*)
{
   CElmCtxPopup *self = static_cast<CElmCtxPopup *>(data);
   self->Dismissed();
}

static void _item_selected_cb(void *data, Evas_Object *, void *)
{
   CbItemStruct *cis = (CbItemStruct *)data;
   cis->self->ItemSelected(cis->item);
}

CElmCtxPopup::CElmCtxPopup(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject,
   elm_ctxpopup_add(parent->GetEvasObject()))
{
   evas_object_smart_callback_add(eo, "dismissed", _dismissed, this);
   ctxpopup_parent = parent->GetEvasObject();
}

CElmCtxPopup::~CElmCtxPopup()
{
   on_item_select.Dispose();
   on_dismiss.Dispose();
   hover_parent.Dispose();
}

void CElmCtxPopup::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("CtxPopup"), GetTemplate()->GetFunction());
}

void CElmCtxPopup::ItemSelected(Handle<Value> item)
{
   if (on_item_select.IsEmpty() || !on_item_select->IsFunction())
     return;

   Handle<Function> callback(Function::Cast(*on_item_select));
   Handle<Value> args[1] = { item };
   callback->Call(jsObject, 1, args);
}

void CElmCtxPopup::Dismissed()
{
   if (!on_dismiss.IsEmpty() && on_dismiss->IsFunction()) {
     Handle<Function> callback(Function::Cast(*on_dismiss));
     Handle<Value> args[] = { };
     callback->Call(jsObject, 0, args);
   }

   //As it's impossible to show a ctxpopup twice, I create a new one here,
   //copying original's one properties.
   Evas_Object *tmp = elm_ctxpopup_add(ctxpopup_parent);
   Elm_Ctxpopup_Direction first, second, third, fourth;
   elm_ctxpopup_direction_priority_get(eo, &first, &second, &third, &fourth);
   elm_ctxpopup_direction_priority_set(tmp, first, second, third, fourth);
   elm_ctxpopup_hover_parent_set(tmp, elm_ctxpopup_hover_parent_get(eo));
   elm_ctxpopup_horizontal_set(tmp, elm_ctxpopup_horizontal_get(eo));

   evas_object_del(eo);
   eo = tmp;
   evas_object_data_set(eo, "this", this);
   evas_object_smart_callback_add(eo, "dismissed", _dismissed, this);
}

Handle<Value> CElmCtxPopup::AddItems(Handle<Value> value)
{
   Handle<Array> items = value->ToObject()->GetPropertyNames();

   int n_items = items->Length();
   for (int i = 0; i < n_items; i++)
     {
       Local<Value> item = value->ToObject()->Get(items->Get(i));

       CbItemStruct *cis = new CbItemStruct();
       cis->self = this;
       cis->item = Persistent<Value>::New(item);

       if (item->IsObject())
         {
            Handle<Value> text = item->ToObject()->Get(String::NewSymbol("text"));
            Handle<Value> icon = item->ToObject()->Get(String::NewSymbol("icon"));
            Evas_Object *eo_icon = (!icon.IsEmpty() && icon->IsObject()) ?
               GetEvasObjectFromJavascript(Realise(icon->ToObject(), jsObject)) : NULL;

            elm_ctxpopup_item_append(eo, *String::Utf8Value(text->ToString()),
                                     eo_icon, _item_selected_cb, cis);
         }
       else
         elm_ctxpopup_item_append(eo, *String::Utf8Value(item->ToString()),
                                     NULL, _item_selected_cb, cis);

     }

   return Undefined();
}

Handle<Value> CElmCtxPopup::horizontal_get() const
{
   return Boolean::New(elm_ctxpopup_horizontal_get(eo));
}

void CElmCtxPopup::horizontal_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_ctxpopup_horizontal_set(eo, value->BooleanValue());
}

void CElmCtxPopup::on_item_select_set(Handle<Value> val)
{
   on_item_select.Dispose();
   on_item_select.Clear();

   if (val->IsFunction())
     on_item_select = Persistent<Value>::New(val);
}

Handle<Value> CElmCtxPopup::on_item_select_get(void) const
{
   return on_item_select;
}

void CElmCtxPopup::on_dismiss_set(Handle<Value> val)
{
   on_dismiss.Dispose();
   on_dismiss.Clear();

   if (val->IsFunction())
     on_dismiss = Persistent<Value>::New(val);
}

Handle<Value> CElmCtxPopup::on_dismiss_get(void) const
{
   return on_dismiss;
}

Handle<Value> CElmCtxPopup::hover_parent_get() const
{
   return hover_parent;
}

void CElmCtxPopup::hover_parent_set(Handle<Value> value)
{
   if (!value->IsObject())
     return;

   elm_ctxpopup_hover_parent_set(eo, GetEvasObjectFromJavascript(value));

   hover_parent.Dispose();
   hover_parent = Persistent<Value>::New(value);
}

Handle<Value> CElmCtxPopup::direction_get() const
{
   switch(elm_ctxpopup_direction_get(eo))
     {
       case ELM_CTXPOPUP_DIRECTION_DOWN:
         return String::NewSymbol("down");
       case ELM_CTXPOPUP_DIRECTION_RIGHT:
         return String::NewSymbol("right");
       case ELM_CTXPOPUP_DIRECTION_LEFT:
         return String::NewSymbol("left");
       case ELM_CTXPOPUP_DIRECTION_UP:
         return String::NewSymbol("up");
       case ELM_CTXPOPUP_DIRECTION_UNKNOWN:
       default:
         return String::NewSymbol("unknown");
     }
}

Handle<Value> CElmCtxPopup::dismiss(const Arguments &)
{
   elm_ctxpopup_dismiss(eo);
   return Undefined();
}

Handle<Value> CElmCtxPopup::show(const Arguments &args)
{
   AddItems(args.This()->ToObject()->Get(String::NewSymbol("elements"))->ToObject());

   evas_object_show(eo);

   return Undefined();
}

}
