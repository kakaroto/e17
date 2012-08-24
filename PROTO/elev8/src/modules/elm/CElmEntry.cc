#include "elm.h"
#include "CElmEntry.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmEntry, password);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, editable);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, line_wrap);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, scrollable);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, single_line);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, on_activate);
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
GENERATE_PROPERTY_CALLBACKS(CElmEntry, input_panel_layout);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, autocapital_type);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, input_panel_language);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, input_panel_return_key_type);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, cnp_mode);
GENERATE_PROPERTY_CALLBACKS(CElmEntry, scrollbar_policy);
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
GENERATE_METHOD_CALLBACKS(CElmEntry, cursor_line_begin);
GENERATE_METHOD_CALLBACKS(CElmEntry, cursor_line_end);
GENERATE_METHOD_CALLBACKS(CElmEntry, text_style_user_pop);
GENERATE_METHOD_CALLBACKS(CElmEntry, text_style_user_push);
GENERATE_METHOD_CALLBACKS(CElmEntry, calc_force);
GENERATE_METHOD_CALLBACKS(CElmEntry, select_none);
GENERATE_METHOD_CALLBACKS(CElmEntry, select_all);
GENERATE_METHOD_CALLBACKS(CElmEntry, file_save);

GENERATE_TEMPLATE(CElmEntry,
                  PROPERTY(password),
                  PROPERTY(editable),
                  PROPERTY(line_wrap),
                  PROPERTY(scrollable),
                  PROPERTY(single_line),
                  PROPERTY(on_activate),
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
                  PROPERTY(input_panel_layout),
                  PROPERTY(autocapital_type),
                  PROPERTY(input_panel_language),
                  PROPERTY(input_panel_return_key_type),
                  PROPERTY(cnp_mode),
                  PROPERTY(scrollbar_policy),
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
                  METHOD(text_style_user_peek),
                  METHOD(cursor_line_begin),
                  METHOD(cursor_line_end),
                  METHOD(text_style_user_pop),
                  METHOD(text_style_user_push),
                  METHOD(calc_force),
                  METHOD(select_none),
                  METHOD(select_all),
                  METHOD(file_save));

CElmEntry::CElmEntry(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_entry_add(parent->GetEvasObject()))
{
}

CElmEntry::~CElmEntry()
{
   on_activate_set(Undefined());
   on_change_set(Undefined());
   icon_visible.Dispose();
   end_visible.Dispose();
   enabled_status.Dispose();
   scrollbar_policy.Dispose();
}

void CElmEntry::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Entry"),
               GetTemplate()->GetFunction());
}

void CElmEntry::on_activate_set(Handle<Value> val)
{
   if (!cb.on_activate.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "activated", &OnActivateWrapper);
        cb.on_activate.Dispose();
        cb.on_activate.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.on_activate = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "activated", &OnActivateWrapper, this);
}

Handle<Value> CElmEntry::on_activate_get() const
{
   return cb.on_activate;
}

void CElmEntry::OnActivate()
{
   Handle<Function> callback(Function::Cast(*cb.on_activate));
   Handle<Value> args[1] = { entry_get() };
   callback->Call(jsObject, 1, args);
}

void CElmEntry::OnActivateWrapper(void *data, Evas_Object *, void *)
{
   static_cast<CElmEntry *>(data)->OnActivate();
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
   Handle<Value> args[1] = { jsObject };
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
   const char* entry = elm_entry_entry_get(eo);

   return entry ? String::New(entry) : Undefined();
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
   elm_entry_bounce_get(eo, NULL, &vertical);
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
   const char* hover_style = elm_entry_anchor_hover_style_get(eo);

   return hover_style ? String::New(hover_style) : Undefined() ;
}

void CElmEntry::anchor_hover_style_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   elm_entry_anchor_hover_style_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmEntry::input_panel_layout_get() const
{
   switch (elm_entry_input_panel_layout_get(eo)) {
     case ELM_INPUT_PANEL_LAYOUT_NORMAL:
       return String::NewSymbol("default");
     case ELM_INPUT_PANEL_LAYOUT_NUMBER:
       return String::NewSymbol("number");
     case ELM_INPUT_PANEL_LAYOUT_EMAIL:
       return String::NewSymbol("email");
     case ELM_INPUT_PANEL_LAYOUT_URL:
       return String::NewSymbol("url");
     case ELM_INPUT_PANEL_LAYOUT_PHONENUMBER:
       return String::NewSymbol("phonenumber");
     case ELM_INPUT_PANEL_LAYOUT_IP:
       return String::NewSymbol("ip");
     case ELM_INPUT_PANEL_LAYOUT_MONTH:
       return String::NewSymbol("month");
     case ELM_INPUT_PANEL_LAYOUT_NUMBERONLY:
       return String::NewSymbol("numberonly");
     case ELM_INPUT_PANEL_LAYOUT_INVALID:
       return String::NewSymbol("invalid");
     case ELM_INPUT_PANEL_LAYOUT_HEX:
       return String::NewSymbol("hexadecimal");
     case ELM_INPUT_PANEL_LAYOUT_TERMINAL:
       return String::NewSymbol("terminal");
     case ELM_INPUT_PANEL_LAYOUT_PASSWORD:
       return String::NewSymbol("password");
     default:
       return String::NewSymbol("unknown");
   }
}

void CElmEntry::input_panel_layout_set(Handle<Value> val)
{
   String::Utf8Value mode_string(val->ToString());

   if (!strcmp(*mode_string, "default"))
     elm_entry_input_panel_layout_set(eo, ELM_INPUT_PANEL_LAYOUT_NORMAL);
   else if (!strcmp(*mode_string, "number"))
     elm_entry_input_panel_layout_set(eo, ELM_INPUT_PANEL_LAYOUT_NUMBER);
   else if (!strcmp(*mode_string, "email"))
     elm_entry_input_panel_layout_set(eo, ELM_INPUT_PANEL_LAYOUT_EMAIL);
   else if (!strcmp(*mode_string, "url"))
     elm_entry_input_panel_layout_set(eo, ELM_INPUT_PANEL_LAYOUT_URL);
   else if (!strcmp(*mode_string, "phonenumber"))
     elm_entry_input_panel_layout_set(eo, ELM_INPUT_PANEL_LAYOUT_PHONENUMBER);
   else if (!strcmp(*mode_string, "ip"))
     elm_entry_input_panel_layout_set(eo, ELM_INPUT_PANEL_LAYOUT_IP);
   else if (!strcmp(*mode_string, "month"))
     elm_entry_input_panel_layout_set(eo, ELM_INPUT_PANEL_LAYOUT_MONTH);
   else if (!strcmp(*mode_string, "numberonly"))
     elm_entry_input_panel_layout_set(eo, ELM_INPUT_PANEL_LAYOUT_NUMBERONLY);
   else if (!strcmp(*mode_string, "invalid"))
     elm_entry_input_panel_layout_set(eo, ELM_INPUT_PANEL_LAYOUT_INVALID);
   else if (!strcmp(*mode_string, "hexadecimal"))
     elm_entry_input_panel_layout_set(eo, ELM_INPUT_PANEL_LAYOUT_HEX);
   else if (!strcmp(*mode_string, "terminal"))
     elm_entry_input_panel_layout_set(eo, ELM_INPUT_PANEL_LAYOUT_TERMINAL);
   else if (!strcmp(*mode_string, "password"))
     elm_entry_input_panel_layout_set(eo, ELM_INPUT_PANEL_LAYOUT_PASSWORD);
}

Handle<Value> CElmEntry::autocapital_type_get() const
{
   switch (elm_entry_autocapital_type_get(eo)) {
     case ELM_AUTOCAPITAL_TYPE_NONE:
       return String::NewSymbol("none");
     case ELM_AUTOCAPITAL_TYPE_WORD:
       return String::NewSymbol("word");
     case ELM_AUTOCAPITAL_TYPE_SENTENCE:
       return String::NewSymbol("sentence");
     case ELM_AUTOCAPITAL_TYPE_ALLCHARACTER:
       return String::NewSymbol("allcharacter");
     default:
       return String::NewSymbol("unknown");
   }
}

void CElmEntry::autocapital_type_set(Handle<Value> val)
{
   String::Utf8Value mode_string(val->ToString());

   if (!strcmp(*mode_string, "none"))
     elm_entry_autocapital_type_set(eo, ELM_AUTOCAPITAL_TYPE_NONE);
   else if (!strcmp(*mode_string, "word"))
     elm_entry_autocapital_type_set(eo, ELM_AUTOCAPITAL_TYPE_WORD);
   else if (!strcmp(*mode_string, "sentence"))
     elm_entry_autocapital_type_set(eo, ELM_AUTOCAPITAL_TYPE_SENTENCE);
   else if (!strcmp(*mode_string, "allcharacter"))
     elm_entry_autocapital_type_set(eo, ELM_AUTOCAPITAL_TYPE_ALLCHARACTER);
}

Handle<Value> CElmEntry::input_panel_language_get() const
{
   switch (elm_entry_input_panel_language_get(eo)) {
     case ELM_INPUT_PANEL_LANG_AUTOMATIC:
       return String::NewSymbol("automatic");
     case ELM_INPUT_PANEL_LANG_ALPHABET:
       return String::NewSymbol("alphabet");
     default:
       return String::NewSymbol("unknown");
   }
}

void CElmEntry::input_panel_language_set(Handle<Value> val)
{
   String::Utf8Value mode_string(val->ToString());

   if (!strcmp(*mode_string, "automatic"))
     elm_entry_input_panel_language_set(eo, ELM_INPUT_PANEL_LANG_AUTOMATIC);
   else if (!strcmp(*mode_string, "alphabet"))
     elm_entry_input_panel_language_set(eo, ELM_INPUT_PANEL_LANG_ALPHABET);
}

Handle<Value> CElmEntry::input_panel_return_key_type_get() const
{
   switch (elm_entry_input_panel_return_key_type_get(eo)) {
     case ELM_INPUT_PANEL_RETURN_KEY_TYPE_DEFAULT:
       return String::NewSymbol("default");
     case ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE:
       return String::NewSymbol("done");
     case ELM_INPUT_PANEL_RETURN_KEY_TYPE_GO:
       return String::NewSymbol("go");
     case ELM_INPUT_PANEL_RETURN_KEY_TYPE_JOIN:
       return String::NewSymbol("join");
     case ELM_INPUT_PANEL_RETURN_KEY_TYPE_LOGIN:
       return String::NewSymbol("login");
     case ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT:
       return String::NewSymbol("next");
     case ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEARCH:
       return String::NewSymbol("search");
     case ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEND:
       return String::NewSymbol("send");
     default:
       return String::NewSymbol("unknown");
   }
}

void CElmEntry::input_panel_return_key_type_set(Handle<Value> val)
{
   String::Utf8Value mode_string(val->ToString());

   if (!strcmp(*mode_string, "default"))
     elm_entry_input_panel_return_key_type_set(eo, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DEFAULT);
   else if (!strcmp(*mode_string, "done"))
     elm_entry_input_panel_return_key_type_set(eo, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
   else if (!strcmp(*mode_string, "go"))
     elm_entry_input_panel_return_key_type_set(eo, ELM_INPUT_PANEL_RETURN_KEY_TYPE_GO);
   else if (!strcmp(*mode_string, "join"))
     elm_entry_input_panel_return_key_type_set(eo, ELM_INPUT_PANEL_RETURN_KEY_TYPE_JOIN);
   else if (!strcmp(*mode_string, "login"))
     elm_entry_input_panel_return_key_type_set(eo, ELM_INPUT_PANEL_RETURN_KEY_TYPE_LOGIN);
   else if (!strcmp(*mode_string, "next"))
     elm_entry_input_panel_return_key_type_set(eo, ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT);
   else if (!strcmp(*mode_string, "search"))
     elm_entry_input_panel_return_key_type_set(eo, ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEARCH);
   else if (!strcmp(*mode_string, "send"))
     elm_entry_input_panel_return_key_type_set(eo, ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEND);
}

Handle<Value> CElmEntry::cnp_mode_get() const
{
   switch (elm_entry_cnp_mode_get(eo)) {
     case ELM_CNP_MODE_MARKUP:
       return String::NewSymbol("markup");
     case ELM_CNP_MODE_NO_IMAGE:
       return String::NewSymbol("noimage");
     case ELM_CNP_MODE_PLAINTEXT:
       return String::NewSymbol("plaintext");
      default:
       return String::NewSymbol("unknown");
   }
}

void CElmEntry::cnp_mode_set(Handle<Value> val)
{
   String::Utf8Value mode_string(val->ToString());

   if (!strcmp(*mode_string, "markup"))
     elm_entry_cnp_mode_set(eo, ELM_CNP_MODE_MARKUP);
   else if (!strcmp(*mode_string, "noimage"))
     elm_entry_cnp_mode_set(eo, ELM_CNP_MODE_NO_IMAGE);
   else if (!strcmp(*mode_string, "plaintext"))
     elm_entry_cnp_mode_set(eo, ELM_CNP_MODE_PLAINTEXT);
}

Handle<Value> CElmEntry::scrollbar_policy_get() const
{
   return scrollbar_policy;
}

void CElmEntry::scrollbar_policy_set(Handle<Value> val)
{
   if (!val->IsArray())
     return;

   Local<Object> policy = val->ToObject();
   elm_entry_scrollbar_policy_set(eo,
        (Elm_Scroller_Policy) policy->Get(0)->ToNumber()->Value(),
        (Elm_Scroller_Policy) policy->Get(1)->ToNumber()->Value());

   scrollbar_policy.Dispose();
   scrollbar_policy = Persistent<Value>::New(val);
}

Handle<Value> CElmEntry::is_empty_get() const
{
   return Boolean::New(elm_entry_is_empty(eo));
}

Handle<Value> CElmEntry::selection_get() const
{
   const char* selection = elm_entry_selection_get(eo);

   return selection ? String::New(selection) : Undefined();
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
   const char* user_peek = elm_entry_text_style_user_peek(eo);

   return user_peek ? String::New(user_peek) : Undefined() ;
}

Handle<Value> CElmEntry::cursor_line_begin(const Arguments&)
{
   elm_entry_cursor_line_begin_set(eo);
   return Undefined();
}

Handle<Value> CElmEntry::cursor_line_end(const Arguments&)
{
   elm_entry_cursor_line_end_set(eo);
   return Undefined();
}

Handle<Value> CElmEntry::text_style_user_pop(const Arguments&)
{
   elm_entry_text_style_user_pop(eo);
   return Undefined();
}

Handle<Value> CElmEntry::text_style_user_push(const Arguments& args)
{
   if (!args[0]->IsString())
     elm_entry_text_style_user_push(eo, *String::Utf8Value(args[0]));

   return Undefined();
}

Handle<Value> CElmEntry::calc_force(const Arguments&)
{
   elm_entry_calc_force(eo);
   return Undefined();
}

Handle<Value> CElmEntry::select_none(const Arguments&)
{
   elm_entry_select_none(eo);
   return Undefined();
}

Handle<Value> CElmEntry::select_all(const Arguments&)
{
   elm_entry_select_all(eo);
   return Undefined();
}

Handle<Value> CElmEntry::file_save(const Arguments&)
{
   elm_entry_file_save(eo);
   return Undefined();
}

}
