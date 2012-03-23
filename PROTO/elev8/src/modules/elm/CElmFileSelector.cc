#include "CElmFileSelector.h"

CElmFileSelector::CElmFileSelector(CEvasObject *parent, Local<Object> obj) 
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_fileselector_add (parent->top_widget_get());
   construct(eo, obj);
}

Handle<Value> CElmFileSelector::selected_get() const
{
   return String::New(elm_fileselector_selected_get(eo));
}

void CElmFileSelector::selected_set(Handle<Value> val)
{
   if (val->IsString() || val->IsNumber())
     elm_fileselector_selected_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmFileSelector::path_get() const
{
   return String::New(elm_fileselector_path_get(eo));
}

void CElmFileSelector::path_set(Handle<Value> val)
{
   if (val->IsString() || val->IsNumber())
     elm_fileselector_path_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmFileSelector::expandable_get() const
{
   return Boolean::New(elm_fileselector_expandable_get(eo));
}

void CElmFileSelector::expandable_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_fileselector_expandable_set(eo, val->BooleanValue());
}
Handle<Value> CElmFileSelector::folder_only_get() const
{
   return Boolean::New(elm_fileselector_folder_only_get(eo));
}

void CElmFileSelector::folder_only_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_fileselector_folder_only_set(eo, val->BooleanValue());
}
Handle<Value> CElmFileSelector::is_save_get() const
{
   return Boolean::New(elm_fileselector_is_save_get (eo));
}

void CElmFileSelector::is_save_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_fileselector_is_save_set(eo, val->BooleanValue());
}

Handle<Value> CElmFileSelector::mode_get() const
{
   return Number::New(elm_fileselector_mode_get(eo));
}

void CElmFileSelector::mode_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_fileselector_mode_set(eo, (Elm_Fileselector_Mode)val->NumberValue());
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

void CElmFileSelector::on_click(void *event_info)
{
   if (event_info == NULL)
     return;

   Handle<Object> obj = get_object();
   Handle<Value> val = on_clicked_val;

   Handle<String> path = String::New((const char *)event_info);
   Handle<Value> args[2] = { obj, path };
   assert(val->IsFunction());
   Handle<Function> fn(Function::Cast(*val));
   elm_fileselector_path_set(eo, (const char *)event_info);
   fn->Call(obj, 2, args);
}

void CElmFileSelector::on_clicked_set(Handle<Value> val)
{
   on_clicked_val.Dispose();
   on_clicked_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_smart_callback_add(eo, "file,chosen", &eo_on_click, this);
   else
     evas_object_smart_callback_del(eo, "file,chosen", &eo_on_click);
}

Handle<Value> CElmFileSelector::on_clicked_get(void) const
{
   return on_clicked_val;
}

PROPERTIES_OF(CElmFileSelector) = {
   PROP_HANDLER(CElmFileSelector, path),
   PROP_HANDLER(CElmFileSelector, expandable),
   PROP_HANDLER(CElmFileSelector, folder_only),
   PROP_HANDLER(CElmFileSelector, is_save),
   PROP_HANDLER(CElmFileSelector, mode),
   PROP_HANDLER(CElmFileSelector, selected),
   { NULL }
};
