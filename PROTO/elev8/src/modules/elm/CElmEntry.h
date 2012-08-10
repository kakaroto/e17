#ifndef C_ELM_ENTRY_H
#define C_ELM_ENTRY_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

class CElmEntry : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmEntry(Local<Object> _jsObject, CElmObject *parent);
   ~CElmEntry();

   static Handle<FunctionTemplate> GetTemplate();

   struct {
      Persistent<Value> on_activate;
      Persistent<Value> on_change;
   } cb;

   Persistent<Value> icon_visible;
   Persistent<Value> end_visible;
   Persistent<Value> enabled_status;
   Persistent<Value> scrollbar_policy;

public:
   static void Initialize(Handle<Object> val);

   Handle<Value> on_activate_get() const;
   void on_activate_set(Handle<Value> value);
   static void OnActivateWrapper(void *data, Evas_Object *, void *);
   void OnActivate();

   Handle<Value> on_change_get() const;
   void on_change_set(Handle<Value> value);
   static void OnChangeWrapper(void *data, Evas_Object *, void *);
   void OnChange();

   Handle<Value> password_get() const;
   void password_set(Handle<Value> value);

   Handle<Value> editable_get() const;
   void editable_set(Handle<Value> value);

   Handle<Value> line_wrap_get() const;
   void line_wrap_set(Handle<Value> value);

   Handle<Value> scrollable_get() const;
   void scrollable_set(Handle<Value> value);

   Handle<Value> single_line_get() const;
   void single_line_set(Handle<Value> value);

   Handle<Value> entry_get() const;
   void entry_set(Handle<Value> value);
   void entry_append(Handle<Value> value);
   void entry_insert(Handle<Value> value);

   Handle<Value> cursor_pos_get() const;
   void cursor_pos_set(Handle<Value> value);

   Handle<Value> cursor_begin_get() const;
   void cursor_begin_set(Handle<Value>);

   Handle<Value> cursor_end_get() const;
   void cursor_end_set(Handle<Value>);

   Handle<Value> icon_visible_get() const;
   void icon_visible_set(Handle<Value>);

   Handle<Value> context_menu_disabled_get() const;
   void context_menu_disabled_set(Handle<Value> value);

   Handle<Value> autosave_get() const;
   void autosave_set(Handle<Value> value);

   Handle<Value> end_visible_get() const;
   void end_visible_set(Handle<Value>);

   Handle<Value> h_bounce_get() const;
   void h_bounce_set(Handle<Value> value);

   Handle<Value> v_bounce_get() const;
   void v_bounce_set(Handle<Value> value);

   Handle<Value> input_panel_enabled_get() const;
   void input_panel_enabled_set(Handle<Value> value);

   Handle<Value> input_panel_return_key_disabled_get() const;
   void input_panel_return_key_disabled_set(Handle<Value> value);

   Handle<Value> input_panel_return_key_autoenabled_get() const;
   void input_panel_return_key_autoenabled_set(Handle<Value> value);

   Handle<Value> prediction_allow_get() const;
   void prediction_allow_set(Handle<Value> value);

   Handle<Value> anchor_hover_style_get() const;
   void anchor_hover_style_set(Handle<Value> value);

   Handle<Value> input_panel_layout_get() const;
   void input_panel_layout_set(Handle<Value> value);

   Handle<Value> autocapital_type_get() const;
   void autocapital_type_set(Handle<Value> value);

   Handle<Value> input_panel_language_get() const;
   void input_panel_language_set(Handle<Value> value);

   Handle<Value> input_panel_return_key_type_get() const;
   void input_panel_return_key_type_set(Handle<Value> value);

   Handle<Value> cnp_mode_get() const;
   void cnp_mode_set(Handle<Value> value);

   Handle<Value> scrollbar_policy_get() const;
   void scrollbar_policy_set(Handle<Value> value);

   Handle<Value> is_empty_get() const;

   Handle<Value> selection_get() const;

   Handle<Value> cursor_content_get() const;

   Handle<Value> cursor_is_format_get() const;

   Handle<Value> cursor_is_visible_format_get() const;

   Handle<Value> cursor_geometry_get() const;

   Handle<Value> cursor_next(const Arguments&);
   Handle<Value> cursor_prev(const Arguments&);
   Handle<Value> cursor_up(const Arguments&);
   Handle<Value> cursor_down(const Arguments&);
   Handle<Value> cursor_selection_begin(const Arguments&);
   Handle<Value> cursor_selection_end(const Arguments&);
   Handle<Value> cursor_line_begin(const Arguments&);
   Handle<Value> cursor_line_end(const Arguments&);

   Handle<Value> selection_cut(const Arguments&);
   Handle<Value> selection_copy(const Arguments&);
   Handle<Value> selection_paste(const Arguments&);

   Handle<Value> context_menu_clear(const Arguments&);

   Handle<Value> markup_to_utf8(const Arguments& args);
   Handle<Value> utf8_to_markup(const Arguments& args);

   Handle<Value> input_panel_show(const Arguments&);
   Handle<Value> input_panel_hide(const Arguments&);

   Handle<Value> imf_context_reset(const Arguments&);

   Handle<Value> anchor_hover_end(const Arguments&);

   Handle<Value> text_style_user_peek(const Arguments&);
   Handle<Value> text_style_user_pop(const Arguments&);
   Handle<Value> text_style_user_push(const Arguments& args);

   Handle<Value> calc_force(const Arguments&);

   Handle<Value> select_none(const Arguments&);
   Handle<Value> select_all(const Arguments&);

   Handle<Value> file_save(const Arguments&);

   friend Handle<Value> CElmObject::New<CElmEntry>(const Arguments& args);
};

}

#endif
