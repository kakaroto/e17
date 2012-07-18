#include "CElmFileSelector.h"

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

GENERATE_PROPERTY_CALLBACKS(CElmFileSelector, path);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelector, expandable);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelector, folder_only);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelector, is_save);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelector, mode);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelector, selected);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelector, on_done);
GENERATE_PROPERTY_CALLBACKS(CElmFileSelector, buttons);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmFileSelector,
                  PROPERTY(path),
                  PROPERTY(expandable),
                  PROPERTY(folder_only),
                  PROPERTY(is_save),
                  PROPERTY(mode),
                  PROPERTY(selected),
                  PROPERTY(on_done),
                  PROPERTY(buttons));

CElmFileSelector::CElmFileSelector(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_fileselector_add(_parent->GetEvasObject()))
{
}

void CElmFileSelector::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("FileSelector"), GetTemplate()->GetFunction());
}

CElmFileSelector::~CElmFileSelector()
{
   on_done_set(Undefined());
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

void CElmFileSelector::OnDone(void *event_info)
{
   if (event_info == NULL)
     return;

   Handle<Function> callback(Function::Cast(*cb.done));
   Handle<Value> args[2] = { jsObject, String::New((const char *)event_info) };
   callback->Call(jsObject, 2, args);
}

void CElmFileSelector::OnDoneWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmFileSelector*>(data)->OnDone(event_info);
}

Handle<Value> CElmFileSelector::on_done_get() const
{
   return cb.done;
}

void CElmFileSelector::on_done_set(Handle<Value> val)
{
   if (!cb.done.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "done", &OnDoneWrapper);
        cb.done.Dispose();
     }

   if (!val->IsFunction())
     return;

   cb.done = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "done", &OnDoneWrapper, this);
}

Handle<Value> CElmFileSelector::buttons_get() const
{
   return Boolean::New(elm_fileselector_buttons_ok_cancel_get(eo));
}

void CElmFileSelector::buttons_set(Handle<Value> val)
{
   if (val->IsBoolean())
       elm_fileselector_buttons_ok_cancel_set(eo, val->BooleanValue());
}

}
