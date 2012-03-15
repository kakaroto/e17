#include "CElmEntry.h"

CElmEntry::CElmEntry(CEvasObject * parent, Local <Object> obj)
    : CEvasObject()
    , prop_handler(property_list_base)
{
    eo = elm_entry_add(parent->get());
    construct(eo, obj);
}

Handle <Value> CElmEntry::password_get() const
{
    return Boolean::New(elm_entry_password_get(eo));
}

void CElmEntry::password_set(Handle <Value> value)
{
    if (value->IsBoolean())
        elm_entry_password_set(eo, value->BooleanValue());
}

Handle <Value> CElmEntry::editable_get() const
{
    return Boolean::New(elm_entry_editable_get(eo));
}

void CElmEntry::editable_set(Handle <Value> value)
{
    if (value->IsBoolean())
        elm_entry_editable_set(eo, value->BooleanValue());
}

Handle <Value> CElmEntry::line_wrap_get() const
{
    return Integer::New(elm_entry_line_wrap_get(eo));
}

void CElmEntry::line_wrap_set(Handle <Value> value)
{
    if (value->IsNumber())
        elm_entry_line_wrap_set(eo, (Elm_Wrap_Type) value->Int32Value());
}

Handle <Value> CElmEntry::scrollable_get() const
{
    return Boolean::New(elm_entry_scrollable_get(eo));
}

void CElmEntry::scrollable_set(Handle <Value> value)
{
    if (value->IsBoolean())
        elm_entry_scrollable_set(eo, value->BooleanValue());
}

Handle <Value> CElmEntry::single_line_get() const
{
    return Integer::New(elm_entry_single_line_get(eo));
}

void CElmEntry::single_line_set(Handle <Value> value)
{
    if (value->IsNumber())
        elm_entry_single_line_set(eo, (Elm_Wrap_Type) value->Int32Value());
}

PROPERTIES_OF(CElmEntry) = {
    PROP_HANDLER(CElmEntry, password),
    PROP_HANDLER(CElmEntry, editable),
    PROP_HANDLER(CElmEntry, line_wrap),
    PROP_HANDLER(CElmEntry, scrollable),
    PROP_HANDLER(CElmEntry, single_line),
    { NULL }
};

