#include "elm.h"
#include "CElmPopup.h"

namespace elm {

using namespace v8;

GENERATE_METHOD_CALLBACKS(CElmPopup, toast)
GENERATE_METHOD_CALLBACKS(CElmPopup, message)
GENERATE_PROPERTY_CALLBACKS(CElmPopup, on_button_press)
GENERATE_PROPERTY_CALLBACKS(CElmPopup, on_dismiss)
GENERATE_PROPERTY_CALLBACKS(CElmPopup, on_item_select)
GENERATE_PROPERTY_CALLBACKS(CElmPopup, on_timeout)
GENERATE_PROPERTY_CALLBACKS(CElmPopup, on_realize_content)

GENERATE_TEMPLATE(CElmPopup,
                  METHOD(toast),
                  METHOD(message),
                  PROPERTY(on_button_press),
                  PROPERTY(on_dismiss),
                  PROPERTY(on_item_select),
                  PROPERTY(on_timeout),
                  PROPERTY(on_realize_content));

CElmPopup::CElmPopup(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject,
		elm_popup_add(elm_object_top_widget_get(parent->GetEvasObject())))
{
   popup_parent = elm_object_top_widget_get(parent->GetEvasObject());
   evas_object_size_hint_weight_set(eo, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_data_set(eo, "this", this);
}

CElmPopup::~CElmPopup()
{
   cb.button_press.Dispose();
   cb.item_select.Dispose();
   cb.dismiss.Dispose();
   cb.timeout.Dispose();
   cb.realize_content.Dispose();
   cached.content.Dispose();
   cached.items.Dispose();
}

void CElmPopup::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Popup"), GetTemplate()->GetFunction());
}

void CElmPopup::on_realize_content_set(Handle<Value> val)
{
   cb.realize_content.Dispose();
   cb.realize_content.Clear();

   if (val->IsFunction())
     cb.realize_content = Persistent<Value>::New(val);
}

Handle<Value> CElmPopup::on_realize_content_get(void) const
{
   return cb.realize_content;
}

void CElmPopup::on_button_press_set(Handle<Value> val)
{
   cb.button_press.Dispose();
   cb.button_press.Clear();

   if (val->IsFunction())
     cb.button_press = Persistent<Value>::New(val);
}

Handle<Value> CElmPopup::on_button_press_get(void) const
{
   return cb.button_press;
}

void CElmPopup::on_item_select_set(Handle<Value> val)
{
   cb.item_select.Dispose();
   cb.item_select.Clear();

   if (val->IsFunction())
     cb.item_select = Persistent<Value>::New(val);
}

Handle<Value> CElmPopup::on_item_select_get(void) const
{
   return cb.item_select;
}

void CElmPopup::on_dismiss_set(Handle<Value> val)
{
   cb.dismiss.Dispose();
   cb.dismiss.Clear();

   if (val->IsFunction())
     cb.dismiss = Persistent<Value>::New(val);
}

Handle<Value> CElmPopup::on_dismiss_get(void) const
{
   return cb.dismiss;
}

void CElmPopup::on_timeout_set(Handle<Value> val)
{
   cb.timeout.Dispose();
   cb.timeout.Clear();

   if (val->IsFunction())
     cb.timeout = Persistent<Value>::New(val);
}

Handle<Value> CElmPopup::on_timeout_get(void) const
{
   return cb.timeout;
}

void CElmPopup::ItemSelected(int item_index)
{
   if (cb.item_select.IsEmpty() || !cb.item_select->IsFunction())
     return;
   Handle<Value> item = cached.items->Get(item_index);
   if (item.IsEmpty())
     return;
   Handle<Function> callback(Function::Cast(*cb.item_select));
   Handle<Value> args[1] = { item };
   callback->Call(jsObject, 1, args);
}

void CElmPopup::PressButton(Handle<String> label)
{
   if (cb.button_press.IsEmpty() || !cb.button_press->IsFunction())
     return;
   Handle<Function> callback(Function::Cast(*cb.button_press));
   Handle<Value> args[1] = { label };
   callback->Call(jsObject, 1, args);
}

void CElmPopup::Dismiss()
{
   if (cb.dismiss.IsEmpty() || !cb.dismiss->IsFunction())
     return;
   Handle<Function> callback(Function::Cast(*cb.dismiss));
   Handle<Value> args[] = { };
   callback->Call(jsObject, 0, args);
}

void CElmPopup::Timeout()
{
   if (cb.timeout.IsEmpty() || !cb.timeout->IsFunction())
     return;
   Handle<Function> callback(Function::Cast(*cb.timeout));
   Handle<Value> args[] = { };
   callback->Call(jsObject, 0, args);
}

void CElmPopup::Hide()
{
   cached.content.Dispose();
   cached.content.Clear();
   cached.items.Dispose();
   cached.items.Clear();
   evas_object_hide(eo);
   evas_object_del(eo);
   eo = elm_popup_add(popup_parent);
   evas_object_data_set(eo, "this", this);
}

void CElmPopup::DidRealizeContent()
{
   if (cb.realize_content.IsEmpty() || !cb.realize_content->IsFunction())
     return;
   Handle<Function> callback(Function::Cast(*cb.realize_content));
   Handle<Value> args[1] = { cached.content };
   callback->Call(jsObject, 1, args);
}

static void _timeout_cb(void *data, Evas_Object *, void *)
{
   CElmPopup *self = static_cast<CElmPopup *>(data);
   self->Hide();
   self->Timeout();
}

static void _block_clicked_cb(void *data, Evas_Object *obj, void *)
{
   CElmPopup *self = static_cast<CElmPopup *>(data);
   self->Hide();
   self->Dismiss();
   evas_object_del(obj);
}

Handle<Value> CElmPopup::toast(const Arguments &args)
{
   if (!args[0]->IsString())
     return ThrowException(Exception::Error(String::New("First argument must be a string.")));

   double timeout = args[1]->IsNumber() ? args[1]->NumberValue() : 2000.0;

   Hide();
   elm_object_text_set(eo, *String::Utf8Value(args[0]->ToString()));
   elm_popup_content_text_wrap_type_set(eo, ELM_WRAP_MIXED);
   elm_popup_orient_set(eo, ELM_POPUP_ORIENT_BOTTOM);
   elm_popup_timeout_set(eo, timeout / 1000.0);
   evas_object_smart_callback_add(eo, "timeout", _timeout_cb, this);
   evas_object_show(eo);

   return Undefined();
}

static Elm_Popup_Orient get_orientation(Local<String> orientation)
{
   String::Utf8Value v(orientation);

#define O(string_,value_) do { if (!strcmp(*v, (string_))) return (ELM_POPUP_ORIENT_ ## value_); } while(0)
   O("top", TOP);
   O("center", CENTER);
   O("bottom", BOTTOM);
   O("left", LEFT);
   O("right", RIGHT);
   O("top_left", TOP_LEFT);
   O("top_right", TOP_RIGHT);
   O("bottom_left", BOTTOM_LEFT);
   O("bottom_right", BOTTOM_RIGHT);
#undef O

   return ELM_POPUP_ORIENT_CENTER;
}

static void _button_pressed_cb(void *data, Evas_Object *button, void *)
{
   CElmPopup *self = static_cast<CElmPopup *>(data);
   char *button_label = strdup(elm_object_text_get(button));
   self->Hide();
   self->PressButton(String::New(button_label));
   free(button_label);
}

static void _item_selected_cb(void *data, Evas_Object *popup, void *)
{
   CElmPopup *self = static_cast<CElmPopup *>(evas_object_data_get(popup, "this"));
   if (self)
     self->ItemSelected((int)(long)data);
}

Handle<Value> CElmPopup::message(const Arguments &args)
{
   if (!args[0]->IsObject())
     return ThrowException(Exception::Error(String::New("Argument must be an object.")));

   Local<Object> o = args[0]->ToObject();

   Hide();
   if (o->Has(String::NewSymbol("orientation")))
     elm_popup_orient_set(eo, get_orientation(o->Get(String::NewSymbol("orientation"))->ToString()));

   if (o->Has(String::NewSymbol("icon")))
     {
        String::Utf8Value path(o->Get(String::NewSymbol("icon")));
        Evas_Object *icon = elm_icon_add(eo);
        if (!access(*path, R_OK))
          elm_image_file_set(icon, *path, NULL);
        else
          elm_icon_standard_set(icon, *path);
        elm_object_part_content_set(eo, "title,icon", icon);
     }

   if (o->Has(String::NewSymbol("buttons")))
     {
        Handle<Array> buttons = Handle<Array>::Cast(o->Get(String::NewSymbol("buttons"))->ToObject());

        int n_buttons = buttons->Length();
        if (n_buttons > 3)
          n_buttons = 3;

        char btn_part[] = "buttonX";
        for (int i = 0; i < n_buttons; ++i)
          {
             btn_part[6] = '0' + i + 1;

             Evas_Object *button = elm_button_add(eo);
             elm_object_text_set(button, *String::Utf8Value(buttons->Get(i)->ToString()));
             elm_object_part_content_set(eo, btn_part, button);
             evas_object_smart_callback_add(button, "clicked", _button_pressed_cb, this);
          }
     }

   if (o->Has(String::NewSymbol("text")))
     {
        elm_object_text_set(eo, *String::Utf8Value(o->Get(String::NewSymbol("text"))));
        elm_popup_content_text_wrap_type_set(eo, ELM_WRAP_MIXED);
     }
   else if (o->Has(String::NewSymbol("content")))
     {
        cached.content.Dispose();
        cached.content = Persistent<Value>::New(Realise(o->Get(String::NewSymbol("content")->ToObject()), jsObject));
        elm_object_content_set(eo, GetEvasObjectFromJavascript(cached.content));
        DidRealizeContent();
     }
   else if (o->Has(String::NewSymbol("items")))
     {
        Handle<Array> items = Handle<Array>::Cast(o->Get(String::NewSymbol("items"))->ToObject());
        cached.items.Dispose();
        cached.items = Persistent<Object>::New(items);

        int n_items = items->Length();
        for (int i = 0; i < n_items; ++i)
          {
             Local<Value> item = items->Get(i);

             if (item->IsObject())
               {
                  Handle<Value> text = item->ToObject()->Get(String::NewSymbol("text"));
                  Handle<Value> icon = item->ToObject()->Get(String::NewSymbol("icon"));
                  Evas_Object *eo_icon = (!icon.IsEmpty() && icon->IsObject()) ?
                     GetEvasObjectFromJavascript(Realise(icon->ToObject(), jsObject)) : NULL;

                   elm_popup_item_append(eo, *String::Utf8Value(text->ToString()), eo_icon, _item_selected_cb, (void *)(long)i);
               }
             else
               elm_popup_item_append(eo, *String::Utf8Value(item->ToString()), NULL, _item_selected_cb, (void *)(long)i);
          }
     }

   if (o->Has(String::NewSymbol("title")))
     elm_object_part_text_set(eo, "title,text", *String::Utf8Value(o->Get(String::NewSymbol("title"))));

   if (o->Has(String::NewSymbol("timeout")))
     {
        elm_popup_timeout_set(eo, o->Get(String::NewSymbol("timeout"))->NumberValue() / 1000.0);
        evas_object_smart_callback_add(eo, "timeout", _timeout_cb, this);
     }

   evas_object_smart_callback_add(eo, "block,clicked", _block_clicked_cb, this);
   evas_object_show(eo);
   return Undefined();
}

}
