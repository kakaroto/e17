#include "CElmFileSelectorButton.h"

CElmFileSelectorButton::CElmFileSelectorButton(CEvasObject *parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_fileselector_button_add (parent->top_widget_get());
   construct(eo, obj);
}

Handle<Value> CElmFileSelectorButton::win_title_get() const
{
   return String::New(elm_fileselector_button_window_title_get(eo));
}

void CElmFileSelectorButton::win_title_set(Handle<Value> val)
{
   if (val->IsString() || val->IsNumber())
     elm_fileselector_button_window_title_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmFileSelectorButton::path_get() const
{
   return String::New(elm_fileselector_button_path_get(eo));
}

void CElmFileSelectorButton::path_set(Handle<Value> val)
{
   if (val->IsString() || val->IsNumber())
     elm_fileselector_button_path_set(eo, *String::Utf8Value(val));
}

void CElmFileSelectorButton::win_size_set(Handle<Value> val)
{
   HandleScope scope;

   if (!val->IsObject())
     return;

   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("w"));
   Local<Value> h = obj->Get(String::New("h"));

   if (w->IsInt32() && h->IsInt32())
     elm_fileselector_button_window_size_set(eo, w->Int32Value(), h->Int32Value());
}

Handle<Value> CElmFileSelectorButton::win_size_get(void) const
{
   HandleScope scope;
   Evas_Coord w, h;

   elm_fileselector_button_window_size_get(eo,  &w, &h);
   Local<Object> obj = Object::New();
   obj->Set(String::New("w"), Number::New(w));
   obj->Set(String::New("h"), Number::New(h));

   return scope.Close(obj);
}

Handle<Value> CElmFileSelectorButton::expandable_get() const
{
   return Boolean::New(elm_fileselector_button_expandable_get(eo));
}

void CElmFileSelectorButton::expandable_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_fileselector_button_expandable_set(eo, val->BooleanValue());
}

Handle<Value> CElmFileSelectorButton::folder_only_get() const
{
   return Boolean::New(elm_fileselector_button_folder_only_get(eo));
}

void CElmFileSelectorButton::folder_only_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_fileselector_button_folder_only_set(eo, val->BooleanValue());
}

Handle<Value> CElmFileSelectorButton::is_save_get() const
{
   return Boolean::New(elm_fileselector_button_is_save_get (eo));
}

void CElmFileSelectorButton::is_save_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_fileselector_button_is_save_set(eo, val->BooleanValue());
}

Handle<Value> CElmFileSelectorButton::inwin_mode_get() const
{
   return Boolean::New(elm_fileselector_button_inwin_mode_get(eo));
}

void CElmFileSelectorButton::inwin_mode_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_fileselector_button_inwin_mode_set(eo, val->BooleanValue());
}

void CElmFileSelectorButton::on_click(void *event_info)
{
   HandleScope handle_scope;
   Handle<Object> obj = get_object();
   Handle<Value> val = on_clicked_val;

   if (!event_info)
     return;

   // FIXME: Also provide click coordinates.
   Handle<String> path = String::New((const char *)event_info);
   Handle<Value> args[2] = { obj, path };
   assert(val->IsFunction());
   Handle<Function> fn(Function::Cast(*val));
   fn->Call(obj, 2, args);
}

void CElmFileSelectorButton::on_clicked_set(Handle<Value> val)
{
   on_clicked_val.Dispose();

   on_clicked_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_smart_callback_add(eo, "file,chosen", &eo_on_click, this);
   else
     evas_object_smart_callback_del(eo, "file,chosen", &eo_on_click);
}

Handle<Value> CElmFileSelectorButton::on_clicked_get(void) const
{
   return on_clicked_val;
}

PROPERTIES_OF(CElmFileSelectorButton) = {
   PROP_HANDLER(CElmFileSelectorButton, win_title),
   PROP_HANDLER(CElmFileSelectorButton, win_size),
   PROP_HANDLER(CElmFileSelectorButton, path),
   PROP_HANDLER(CElmFileSelectorButton, expandable),
   PROP_HANDLER(CElmFileSelectorButton, folder_only),
   PROP_HANDLER(CElmFileSelectorButton, is_save),
   PROP_HANDLER(CElmFileSelectorButton, inwin_mode),
   { NULL }
};
