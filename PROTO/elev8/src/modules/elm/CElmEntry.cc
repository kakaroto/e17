#include "elm.h"
#include "CElmEntry.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmEntry, password);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, editable);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, line_wrap);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, scrollable);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, single_line);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, on_change);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, entry);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, cursor_pos);
GENERATE_RO_PROPERTY_CALLBACKS(CElmEntry, is_empty);
GENERATE_RO_PROPERTY_CALLBACKS(CElmEntry, selection);
GENERATE_RO_PROPERTY_CALLBACKS(CElmEntry, cursor_content);
GENERATE_METHOD_CALLBACKS(CElmEntry, cursor_next);
GENERATE_METHOD_CALLBACKS(CElmEntry, cursor_prev);
GENERATE_METHOD_CALLBACKS(CElmEntry, cursor_up);

GENERATE_TEMPLATE(CElmEntry,
                  PROPERTY(password),
                  PROPERTY(editable),
                  PROPERTY(line_wrap),
                  PROPERTY(scrollable),
                  PROPERTY(single_line),
                  PROPERTY(on_change),
                  PROPERTY(entry),
                  PROPERTY(cursor_pos),
                  PROPERTY_RO(is_empty),
                  PROPERTY_RO(selection),
                  PROPERTY_RO(cursor_content),
                  METHOD(cursor_next),
                  METHOD(cursor_prev),
                  METHOD(cursor_up));

CElmEntry::CElmEntry(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_entry_add(parent->GetEvasObject()))
{
}

CElmEntry::~CElmEntry()
{
   on_change_set(Undefined());
}

void CElmEntry::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Entry"),
               GetTemplate()->GetFunction());
}

void CElmEntry::on_change_set(Handle<Value> val)
{
   if (!cb.on_change.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "changed", &OnChangeWrapper);
        cb.on_change.Dispose();
        cb.on_change.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.on_change = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "changed", &OnChangeWrapper, this);
}

Handle<Value> CElmEntry::on_change_get() const
{
   return cb.on_change;
}

void CElmEntry::OnChangeWrapper(void *data, Evas_Object *, void *)
{
   static_cast<CElmEntry *>(data)->OnChange();
}

void CElmEntry::OnChange()
{
   Handle<Function> callback(Function::Cast(*cb.on_change));
   Handle<Value> args[1] = jsObject;
   callback->Call(jsObject, 1, args);
}

Handle<Value> CElmEntry::password_get() const
{
   return Boolean::New(elm_entry_password_get(eo));
}

void CElmEntry::password_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_entry_password_set(eo, value->BooleanValue());
}

Handle<Value> CElmEntry::editable_get() const
{
   return Boolean::New(elm_entry_editable_get(eo));
}

void CElmEntry::editable_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_entry_editable_set(eo, value->BooleanValue());
}

Handle<Value> CElmEntry::line_wrap_get() const
{
   return Integer::New(elm_entry_line_wrap_get(eo));
}

void CElmEntry::line_wrap_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_entry_line_wrap_set(eo, (Elm_Wrap_Type) value->Int32Value());
}

Handle<Value> CElmEntry::scrollable_get() const
{
   return Boolean::New(elm_entry_scrollable_get(eo));
}

void CElmEntry::scrollable_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_entry_scrollable_set(eo, value->BooleanValue());
}

Handle<Value> CElmEntry::single_line_get() const
{
   return Boolean::New(elm_entry_single_line_get(eo));
}

void CElmEntry::single_line_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_entry_single_line_set(eo, value->BooleanValue());
}

Handle<Value> CElmEntry::entry_get() const
{
   return String::New(elm_entry_entry_get(eo));
}

void CElmEntry::entry_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   elm_entry_entry_set(eo, *String::Utf8Value(val));
}

void CElmEntry::entry_append(Handle<Value> val)
{
   if (!val->IsString())
     return;

   elm_entry_entry_append(eo, *String::Utf8Value(val));
}

void CElmEntry::entry_insert(Handle<Value> val)
{
   if (!val->IsString())
     return;

   elm_entry_entry_insert(eo, *String::Utf8Value(val));
}

Handle<Value> CElmEntry::cursor_pos_get() const
{
   return Integer::New(elm_entry_cursor_pos_get(eo));
}

void CElmEntry::cursor_pos_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_entry_cursor_pos_set(eo, value->Int32Value());
}

Handle<Value> CElmEntry::is_empty_get() const
{
   return Boolean::New(elm_entry_is_empty(eo));
}

Handle<Value> CElmEntry::selection_get() const
{
   return String::New(elm_entry_selection_get(eo));
}

Handle<Value> CElmEntry::cursor_content_get() const
{
   return String::New(elm_entry_cursor_content_get(eo));
}

Handle<Value> CElmEntry::cursor_next(const Arguments&)
{
   return Boolean::New(elm_entry_cursor_next(eo));
}

Handle<Value> CElmEntry::cursor_prev(const Arguments&)
{
   return Boolean::New(elm_entry_cursor_prev(eo));
}

Handle<Value> CElmEntry::cursor_up(const Arguments&)
{
   return Boolean::New(elm_entry_cursor_up(eo));
}

}
