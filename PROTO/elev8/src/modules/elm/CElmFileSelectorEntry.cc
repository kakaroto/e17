#include "CElmFileSelectorEntry.h"

CElmFileSelectorEntry::CElmFileSelectorEntry(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_fileselector_entry_add (parent->top_widget_get());
   construct(eo, obj);
}

Handle<Value> CElmFileSelectorEntry::win_title_get() const
{
   return String::New(elm_fileselector_entry_window_title_get(eo));

}

void CElmFileSelectorEntry::win_title_set(Handle<Value> val)
{
   if (val->IsString() || val->IsNumber())
     {
        String::Utf8Value str(val);
        elm_fileselector_entry_window_title_set(eo, *str);
     }
}

Handle<Value> CElmFileSelectorEntry::selected_get() const
{
   return String::New(elm_fileselector_entry_selected_get(eo));

}

void CElmFileSelectorEntry::selected_set(Handle<Value> val)
{
   if (val->IsString() || val->IsNumber())
     {
        String::Utf8Value str(val);
        elm_fileselector_entry_selected_set(eo, *str);
     }
}

Handle<Value> CElmFileSelectorEntry::path_get() const
{
   return String::New(elm_fileselector_entry_path_get(eo));

}

void CElmFileSelectorEntry::path_set(Handle<Value> val)
{
   if (val->IsString() || val->IsNumber())
     {
        String::Utf8Value str(val);
        elm_fileselector_entry_path_set(eo, *str);
     }
}
void CElmFileSelectorEntry::win_size_set(Handle<Value> val)
{
   if (!val->IsObject())
     return ;
   Evas_Coord width, height;
   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("w"));
   Local<Value> h = obj->Get(String::New("h"));
   if (!w->IsInt32() || !h->IsInt32())
     return;
   width = w->Int32Value();
   height = h->Int32Value();
   elm_fileselector_entry_window_size_set (eo,  width, height);
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

void CElmFileSelectorEntry::on_click(void *event_info)
{
   Handle<Object> obj = get_object();
   HandleScope handle_scope;
   Handle<Value> val = on_clicked_val;

   // also provide x and y positions where it was clicked
   //
   if (event_info!=NULL)
     {
        Handle<String> path = String::New((const char *)event_info);
        Handle<Value> args[2] = { obj, path };
        assert(val->IsFunction());
        Handle<Function> fn(Function::Cast(*val));
        elm_fileselector_entry_path_set(eo, (const char *)event_info);
        fn->Call(obj, 2, args);
     }
}
void CElmFileSelectorEntry::on_clicked_set(Handle<Value> val)
{
   on_clicked_val.Dispose();
   on_clicked_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_smart_callback_add(eo, "file,chosen", &eo_on_click, this);
   else
     evas_object_smart_callback_del(eo, "file,chosen", &eo_on_click);
}

Handle<Value> CElmFileSelectorEntry::on_clicked_get(void) const
{
   return on_clicked_val;
}

PROPERTIES_OF(CElmFileSelectorEntry) = {
  PROP_HANDLER(CElmFileSelectorEntry, win_title),
  PROP_HANDLER(CElmFileSelectorEntry, win_size),
  PROP_HANDLER(CElmFileSelectorEntry, path),
  PROP_HANDLER(CElmFileSelectorEntry, expandable),
  PROP_HANDLER(CElmFileSelectorEntry, folder_only),
  PROP_HANDLER(CElmFileSelectorEntry, is_save),
  PROP_HANDLER(CElmFileSelectorEntry, inwin_mode),
  PROP_HANDLER(CElmFileSelectorEntry, selected),
  { NULL }
};
