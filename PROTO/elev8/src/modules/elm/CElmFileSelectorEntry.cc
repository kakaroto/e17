#include "CElmFileSelectorEntry.h"

namespace elm {

using namespace v8;

//TODO : Add support for more events.
//"changed"
//"activated"
//"press"
//"longpressed"
//"clicked"
//"clicked,double"
//"focused"
//"unfocused"
//"selection,paste"
//"selection,copy"
//"selection,cut"
//"unpressed"

GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorEntry, win_title);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorEntry, win_size);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorEntry, path);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorEntry, expandable);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorEntry, folder_only);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorEntry, is_save);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorEntry, inwin_mode);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorEntry, selected);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelectorEntry, on_file_choose);

GENERATE_TEMPLATE(CElmFileSelectorEntry,
                  PROPERTY(win_title),
                  PROPERTY(win_size),
                  PROPERTY(path),
                  PROPERTY(expandable),
                  PROPERTY(folder_only),
                  PROPERTY(is_save),
                  PROPERTY(inwin_mode),
                  PROPERTY(selected),
                  PROPERTY(on_file_choose));

CElmFileSelectorEntry::CElmFileSelectorEntry(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_fileselector_entry_add(parent->GetEvasObject()))
{
}

void CElmFileSelectorEntry::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("FileSelectorEntry"), GetTemplate()->GetFunction());
}

CElmFileSelectorEntry::~CElmFileSelectorEntry()
{
   on_file_choose_set(Undefined());
}

Handle<Value> CElmFileSelectorEntry::win_title_get() const
{
   return String::New(elm_fileselector_entry_window_title_get(eo));
}

void CElmFileSelectorEntry::win_title_set(Handle<Value> val)
{
   if (val->IsString() || val->IsNumber())
     elm_fileselector_entry_window_title_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmFileSelectorEntry::selected_get() const
{
   const char *entry_selected = elm_fileselector_entry_selected_get(eo);
   return entry_selected ? String::New(entry_selected) : Undefined();
}

void CElmFileSelectorEntry::selected_set(Handle<Value> val)
{
   if (val->IsString())
     elm_fileselector_entry_selected_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmFileSelectorEntry::path_get() const
{
   return String::New(elm_fileselector_entry_path_get(eo));
}

void CElmFileSelectorEntry::path_set(Handle<Value> val)
{
   if (val->IsString() || val->IsNumber())
     elm_fileselector_entry_path_set(eo, *String::Utf8Value(val));
}

void CElmFileSelectorEntry::win_size_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("w"));
   Local<Value> h = obj->Get(String::New("h"));

   if (w->IsInt32() && h->IsInt32())
     elm_fileselector_entry_window_size_set(eo,  w->Int32Value(), h->Int32Value());
}

Handle<Value> CElmFileSelectorEntry::win_size_get(void) const
{
   Evas_Coord w, h;
   elm_fileselector_entry_window_size_get  (eo,  &w, &h);
   Local<Object> obj = Object::New();
   obj->Set(String::New("w"), Number::New(w));
   obj->Set(String::New("h"), Number::New(h));
   return obj;
}

Handle<Value> CElmFileSelectorEntry::expandable_get() const
{
   return Boolean::New(elm_fileselector_entry_expandable_get(eo));
}

void CElmFileSelectorEntry::expandable_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_fileselector_entry_expandable_set(eo, val->BooleanValue());
}
Handle<Value> CElmFileSelectorEntry::folder_only_get() const
{
   return Boolean::New(elm_fileselector_entry_folder_only_get(eo));
}

void CElmFileSelectorEntry::folder_only_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_fileselector_entry_folder_only_set(eo, val->BooleanValue());
}
Handle<Value> CElmFileSelectorEntry::is_save_get() const
{
   return Boolean::New(elm_fileselector_entry_is_save_get (eo));
}

void CElmFileSelectorEntry::is_save_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_fileselector_entry_is_save_set(eo, val->BooleanValue());
}

Handle<Value> CElmFileSelectorEntry::inwin_mode_get() const
{
   return Boolean::New(elm_fileselector_entry_inwin_mode_get(eo));
}

void CElmFileSelectorEntry::inwin_mode_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_fileselector_entry_inwin_mode_set(eo, val->BooleanValue());
}


void CElmFileSelectorEntry::OnFileChoose(void *event_info)
{
   if (event_info == NULL)
     return;

   Handle<Function> callback(Function::Cast(*cb.file_choose));
   Handle<Value> args[2] = { jsObject, String::New((const char *)event_info) };
   callback->Call(jsObject, 2, args);
}

void CElmFileSelectorEntry::OnFileChooseWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmFileSelectorEntry*>(data)->OnFileChoose(event_info);
}

Handle<Value> CElmFileSelectorEntry::on_file_choose_get() const
{
   return cb.file_choose;
}

void CElmFileSelectorEntry::on_file_choose_set(Handle<Value> val)
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
