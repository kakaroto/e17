#include "elm.h"
#include "CElmEntry.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmEntry, password);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, editable);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, line_wrap);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, scrollable);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, single_line);

GENERATE_TEMPLATE(CElmEntry,
                  PROPERTY(password),
                  PROPERTY(editable),
                  PROPERTY(line_wrap),
                  PROPERTY(scrollable),
                  PROPERTY(single_line));

CElmEntry::CElmEntry(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_entry_add(parent->GetEvasObject()))
{
}

void CElmEntry::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Entry"),
               GetTemplate()->GetFunction());
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
   return Integer::New(elm_entry_single_line_get(eo));
}

void CElmEntry::single_line_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_entry_single_line_set(eo, (Elm_Wrap_Type) value->Int32Value());
}

}
