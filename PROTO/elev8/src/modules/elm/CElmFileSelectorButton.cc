#include "CElmFileSelectorButton.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorButton, win_title);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorButton, win_size);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorButton, path);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorButton, expandable);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorButton, folder_only);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorButton, is_save);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorButton, inwin_mode);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorButton, on_file_choose);

GENERATE_TEMPLATE(CElmFileSelectorButton,
                  PROPERTY(win_title),
                  PROPERTY(win_size),
                  PROPERTY(path),
                  PROPERTY(expandable),
                  PROPERTY(folder_only),
                  PROPERTY(is_save),
                  PROPERTY(inwin_mode),
                  PROPERTY(on_file_choose));

CElmFileSelectorButton::CElmFileSelectorButton(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_fileselector_button_add(parent->GetEvasObject()))
{
}

void CElmFileSelectorButton::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("FileSelectorButton"), GetTemplate()->GetFunction());
}

CElmFileSelectorButton::~CElmFileSelectorButton()
{
   on_file_choose_set(Undefined());
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

   Local<Value> w = val->ToObject()->Get(String::NewSymbol("width"));
   Local<Value> h = val->ToObject()->Get(String::NewSymbol("height"));

   if (w->IsInt32() && h->IsInt32())
     elm_fileselector_button_window_size_set(eo, w->Int32Value(), h->Int32Value());
}

Handle<Value> CElmFileSelectorButton::win_size_get(void) const
{
   Local<Object> obj = Object::New();
   Evas_Coord w, h;

   elm_fileselector_button_window_size_get(eo,  &w, &h);
   obj->Set(String::NewSymbol("width"), Number::New(w));
   obj->Set(String::NewSymbol("height"), Number::New(h));

   return obj;
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

void CElmFileSelectorButton::OnFileChoose(void *event_info)
{
   if (event_info == NULL)
     return;

   Handle<Function> callback(Function::Cast(*cb.file_choose));
   Handle<Value> args[2] = { jsObject, String::New((const char *)event_info) };
   callback->Call(jsObject, 2, args);
}

void CElmFileSelectorButton::OnFileChooseWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmFileSelectorButton*>(data)->OnFileChoose(event_info);
}

Handle<Value> CElmFileSelectorButton::on_file_choose_get() const
{
   return cb.file_choose;
}

void CElmFileSelectorButton::on_file_choose_set(Handle<Value> val)
{
   if (!cb.file_choose.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "file,chosen", &OnFileChooseWrapper);
        cb.file_choose.Dispose();
     }

   if (!val->IsFunction())
     return;

   cb.file_choose = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "file,chosen", &OnFileChooseWrapper, this);
}

}
