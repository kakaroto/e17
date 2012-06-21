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
GENERATE_PROPERTY_CALLBACKS(CElmEntry, cursor_begin);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, cursor_end);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, icon_visible);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, context_menu_disabled);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, autosave);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, end_visible);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, h_bounce);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, v_bounce);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, input_panel_enabled);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, input_panel_return_key_disabled);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, input_panel_return_key_autoenabled);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, prediction_allow);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, anchor_hover_style);
GENERATE_RO_PROPERTY_CALLBACKS(CElmEntry, is_empty);
GENERATE_RO_PROPERTY_CALLBACKS(CElmEntry, selection);
GENERATE_RO_PROPERTY_CALLBACKS(CElmEntry, cursor_content);
GENERATE_RO_PROPERTY_CALLBACKS(CElmEntry, cursor_is_format);
GENERATE_RO_PROPERTY_CALLBACKS(CElmEntry, cursor_is_visible_format);
GENERATE_RO_PROPERTY_CALLBACKS(CElmEntry, cursor_geometry);
GENERATE_METHOD_CALLBACKS(CElmEntry, cursor_next);
GENERATE_METHOD_CALLBACKS(CElmEntry, cursor_prev);
GENERATE_METHOD_CALLBACKS(CElmEntry, cursor_up);
GENERATE_METHOD_CALLBACKS(CElmEntry, cursor_down);
GENERATE_METHOD_CALLBACKS(CElmEntry, cursor_selection_begin);
GENERATE_METHOD_CALLBACKS(CElmEntry, cursor_selection_end);
GENERATE_METHOD_CALLBACKS(CElmEntry, selection_cut);
GENERATE_METHOD_CALLBACKS(CElmEntry, selection_copy);
GENERATE_METHOD_CALLBACKS(CElmEntry, selection_paste);
GENERATE_METHOD_CALLBACKS(CElmEntry, context_menu_clear);
GENERATE_METHOD_CALLBACKS(CElmEntry, markup_to_utf8);
GENERATE_METHOD_CALLBACKS(CElmEntry, utf8_to_markup);
GENERATE_METHOD_CALLBACKS(CElmEntry, input_panel_show);
GENERATE_METHOD_CALLBACKS(CElmEntry, input_panel_hide);
GENERATE_METHOD_CALLBACKS(CElmEntry, imf_context_reset);
GENERATE_METHOD_CALLBACKS(CElmEntry, anchor_hover_end);
GENERATE_METHOD_CALLBACKS(CElmEntry, text_style_user_peek);

GENERATE_TEMPLATE(CElmEntry,
                  PROPERTY(password),
                  PROPERTY(editable),
                  PROPERTY(line_wrap),
                  PROPERTY(scrollable),
                  PROPERTY(single_line),
                  PROPERTY(on_change),
                  PROPERTY(entry),
                  PROPERTY(cursor_pos),
                  PROPERTY(cursor_begin),
                  PROPERTY(cursor_end),
                  PROPERTY(icon_visible),
                  PROPERTY(context_menu_disabled),
                  PROPERTY(autosave),
                  PROPERTY(end_visible),
                  PROPERTY(h_bounce),
                  PROPERTY(v_bounce),
                  PROPERTY(input_panel_enabled),
                  PROPERTY(input_panel_return_key_disabled),
                  PROPERTY(input_panel_return_key_autoenabled),
                  PROPERTY(prediction_allow),
                  PROPERTY(anchor_hover_style),
                  PROPERTY_RO(is_empty),
                  PROPERTY_RO(selection),
                  PROPERTY_RO(cursor_content),
                  PROPERTY_RO(cursor_is_format),
                  PROPERTY_RO(cursor_is_visible_format),
                  PROPERTY_RO(cursor_geometry),
                  METHOD(cursor_next),
                  METHOD(cursor_prev),
                  METHOD(cursor_up),
                  METHOD(cursor_down),
                  METHOD(cursor_selection_begin),
                  METHOD(cursor_selection_end),
                  METHOD(selection_cut),
                  METHOD(selection_copy),
                  METHOD(selection_paste),
                  METHOD(context_menu_clear),
                  METHOD(markup_to_utf8),
                  METHOD(utf8_to_markup),
                  METHOD(input_panel_show),
                  METHOD(input_panel_hide),
                  METHOD(imf_context_reset),
                  METHOD(anchor_hover_end),
                  METHOD(text_style_user_peek));

CElmEntry::CElmEntry(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_entry_add(parent->GetEvasObject()))
{
}

CElmEntry::~CElmEntry()
{
   on_change_set(Undefined());
   icon_visible.Dispose();
   end_visible.Dispose();
   enabled_status.Dispose();
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

Handle<Value> CElmEntry::cursor_begin_get() const
{
   return Undefined();
}

void CElmEntry::cursor_begin_set(Handle<Value>)
{
   elm_entry_cursor_begin_set(eo);
}

Handle<Value> CElmEntry::cursor_end_get() const
{
   return Undefined();
}

void CElmEntry::cursor_end_set(Handle<Value>)
{
   elm_entry_cursor_end_set(eo);
}

Handle<Value> CElmEntry::icon_visible_get() const
{
   return icon_visible;
}

void CElmEntry::icon_visible_set(Handle<Value> value)
{
   if (!value->IsBoolean())
     return;

   elm_entry_icon_visible_set(eo, value->BooleanValue());

   icon_visible.Dispose();
   icon_visible = Persistent<Value>::New(value);
}

Handle<Value> CElmEntry::context_menu_disabled_get() const
{
   return Boolean::New(elm_entry_context_menu_disabled_get(eo));
}

void CElmEntry::context_menu_disabled_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_entry_context_menu_disabled_set(eo, value->BooleanValue());
}

Handle<Value> CElmEntry::autosave_get() const
{
   return Boolean::New(elm_entry_autosave_get(eo));
}

void CElmEntry::autosave_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_entry_autosave_set(eo, value->BooleanValue());
}

Handle<Value> CElmEntry::end_visible_get() const
{
   return end_visible;
}

void CElmEntry::end_visible_set(Handle<Value> value)
{
   if (!value->IsBoolean())
     return;

   elm_entry_end_visible_set(eo, value->BooleanValue());

   end_visible.Dispose();
   end_visible = Persistent<Value>::New(value);
}

Handle<Value> CElmEntry::h_bounce_get() const
{
   Eina_Bool horizontal;
   elm_entry_bounce_get(eo, &horizontal, NULL);
   return Boolean::New(horizontal);
}

void CElmEntry::h_bounce_set(Handle<Value> value)
{
   if (!value->IsBoolean())
     return;

   Eina_Bool vertical;
   elm_entry_bounce_get(eo, NULL, &vertical);
   elm_entry_bounce_set(eo, value->BooleanValue(), vertical);
}

Handle<Value> CElmEntry::v_bounce_get() const
{
   Eina_Bool vertical;
   elm_entry_bounce_get(eo, &vertical, NULL);
   return Boolean::New(vertical);
}

void CElmEntry::v_bounce_set(Handle<Value> value)
{
   if (!value->IsBoolean())
     return;

   Eina_Bool horizontal;
   elm_entry_bounce_get(eo, &horizontal, NULL);
   elm_entry_bounce_set(eo, horizontal, value->BooleanValue());
}

Handle<Value> CElmEntry::input_panel_enabled_get() const
{
   return Boolean::New(elm_entry_input_panel_enabled_get(eo));
}

void CElmEntry::input_panel_enabled_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_entry_input_panel_enabled_set(eo, value->BooleanValue());
}

Handle<Value> CElmEntry::input_panel_return_key_disabled_get() const
{
   return Boolean::New(elm_entry_input_panel_return_key_disabled_get(eo));
}

void CElmEntry::input_panel_return_key_disabled_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_entry_input_panel_return_key_disabled_set(eo, value->BooleanValue());
}

Handle<Value> CElmEntry::input_panel_return_key_autoenabled_get() const
{
   return enabled_status;
}

void CElmEntry::input_panel_return_key_autoenabled_set(Handle<Value> value)
{
   if (!value->IsBoolean())
     return;

   elm_entry_input_panel_return_key_autoenabled_set(eo, value->BooleanValue());

   enabled_status.Dispose();
   enabled_status = Persistent<Value>::New(value);
}

Handle<Value> CElmEntry::prediction_allow_get() const
{
   return Boolean::New(elm_entry_prediction_allow_get(eo));
}

void CElmEntry::prediction_allow_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_entry_prediction_allow_set(eo, value->BooleanValue());
}

Handle<Value> CElmEntry::anchor_hover_style_get() const
{
   return String::New(elm_entry_anchor_hover_style_get(eo));
}

void CElmEntry::anchor_hover_style_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   elm_entry_anchor_hover_style_set(eo, *String::Utf8Value(val));
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

Handle<Value> CElmEntry::cursor_is_format_get() const
{
   return Boolean::New(elm_entry_cursor_is_format_get(eo));
}

Handle<Value> CElmEntry::cursor_is_visible_format_get() const
{
   return Boolean::New(elm_entry_cursor_is_visible_format_get(eo));
}

Handle<Value> CElmEntry::cursor_geometry_get() const
{
   int x, y;
   int width;
   int height;
   return Boolean::New(elm_entry_cursor_geometry_get(eo, &x, &y, &width, &height));
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

Handle<Value> CElmEntry::cursor_down(const Arguments&)
{
   return Boolean::New(elm_entry_cursor_down(eo));
}

Handle<Value> CElmEntry::cursor_selection_begin(const Arguments&)
{
   elm_entry_cursor_selection_begin(eo);
   return Undefined();
}

Handle<Value> CElmEntry::cursor_selection_end(const Arguments&)
{
   elm_entry_cursor_selection_end(eo);
   return Undefined();
}

Handle<Value> CElmEntry::selection_cut(const Arguments&)
{
   elm_entry_selection_cut(eo);
   return Undefined();
}

Handle<Value> CElmEntry::selection_copy(const Arguments&)
{
   elm_entry_selection_copy(eo);
   return Undefined();
}

Handle<Value> CElmEntry::selection_paste(const Arguments&)
{
   elm_entry_selection_paste(eo);
   return Undefined();
}

Handle<Value> CElmEntry::context_menu_clear(const Arguments&)
{
   elm_entry_context_menu_clear(eo);
   return Undefined();
}

Handle<Value> CElmEntry::markup_to_utf8(const Arguments& args)
{
   if (args[0]->IsString())
     return String::New(elm_entry_markup_to_utf8(*String::Utf8Value(args[0])));

   return Undefined();
}

Handle<Value> CElmEntry::utf8_to_markup(const Arguments& args)
{
   if (!args[0]->IsString())
     return String::New(elm_entry_utf8_to_markup(*String::Utf8Value(args[0])));

   return Undefined();
}

Handle<Value> CElmEntry::input_panel_show(const Arguments&)
{
   elm_entry_input_panel_show(eo);
   return Undefined();
}

Handle<Value> CElmEntry::input_panel_hide(const Arguments&)
{
   elm_entry_input_panel_hide(eo);
   return Undefined();
}

Handle<Value> CElmEntry::imf_context_reset(const Arguments&)
{
   elm_entry_imf_context_reset(eo);
   return Undefined();
}

Handle<Value> CElmEntry::anchor_hover_end(const Arguments&)
{
   elm_entry_anchor_hover_end(eo);
   return Undefined();
}

Handle<Value> CElmEntry::text_style_user_peek(const Arguments&)
{
   return String::New(elm_entry_text_style_user_peek(eo));
}

}
