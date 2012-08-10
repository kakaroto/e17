#ifndef _CELM_WINDOW_H
#define _CELM_WINDOW_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmWindow : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;
   static Evas_Object *main;

protected:
   CElmWindow(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();
   static void quit(void *, Evas_Object *, void *);
   ~CElmWindow();

   Persistent<Value> size_step;
   Persistent<Value> size_base;
   Persistent<Value> prop_focus;

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> title_get() const;
   void title_set(Handle<Value> val);

   Handle<Value> conformant_get() const;
   void conformant_set(Handle<Value> val);

   Handle<Value> autodel_get() const;
   void autodel_set(Handle<Value> val);

   Handle<Value> borderless_get() const;
   void borderless_set(Handle<Value> val);

   Handle<Value> shaped_get() const;
   void shaped_set(Handle<Value> val);

   Handle<Value> alpha_get() const;
   void alpha_set(Handle<Value> val);

   Handle<Value> override_get() const;
   void override_set(Handle<Value> val);

   Handle<Value> fullscreen_get() const;
   void fullscreen_set(Handle<Value> val);

   Handle<Value> maximized_get() const;
   void maximized_set(Handle<Value> val);

   Handle<Value> iconified_get() const;
   void iconified_set(Handle<Value> val);

   Handle<Value> withdrawn_get() const;
   void withdrawn_set(Handle<Value> val);

   Handle<Value> urgent_get() const;
   void urgent_set(Handle<Value> val);

   Handle<Value> demand_attention_get() const;
   void demand_attention_set(Handle<Value> val);

   Handle<Value> modal_get() const;
   void modal_set(Handle<Value> val);

   Handle<Value> sticky_get() const;
   void sticky_set(Handle<Value> val);

   Handle<Value> quickpanel_get() const;
   void quickpanel_set(Handle<Value> val);

   Handle<Value> screen_constrain_get() const;
   void screen_constrain_set(Handle<Value> val);

   Handle<Value> focus_highlight_enabled_get() const;
   void focus_highlight_enabled_set(Handle<Value> val);

   Handle<Value> keyboard_win_get() const;
   void keyboard_win_set(Handle<Value> val);

   Handle<Value> rotation_get() const;
   void rotation_set(Handle<Value> val);
   void rotation_with_resize_set(Handle<Value> val);

   Handle<Value> priority_major_get() const;
   void priority_major_set(Handle<Value> val);

   Handle<Value> priority_minor_get() const;
   void priority_minor_set(Handle<Value> val);

   Handle<Value> quickpanel_zone_get() const;
   void quickpanel_zone_set(Handle<Value> val);

   Handle<Value> size_step_get() const;
   void size_step_set(Handle<Value> val);

   Handle<Value> size_base_get() const;
   void size_base_set(Handle<Value> val);

   Handle<Value> layer_get() const;
   void layer_set(Handle<Value> val);

   Handle<Value> icon_name_get() const;
   void icon_name_set(Handle<Value> val);

   Handle<Value> role_get() const;
   void role_set(Handle<Value> val);

   Handle<Value> focus_highlight_style_get() const;
   void focus_highlight_style_set(Handle<Value> val);

   Handle<Value> aspect_get() const;
   void aspect_set(Handle<Value> val);

   Handle<Value> indicator_mode_get() const;
   void indicator_mode_set(Handle<Value> val);

   Handle<Value> indicator_opacity_get() const;
   void indicator_opacity_set(Handle<Value> val);

   Handle<Value> keyboard_mode_get() const;
   void keyboard_mode_set(Handle<Value> val);

   Handle<Value> prop_focus_skip_get() const;
   void prop_focus_skip_set(Handle<Value> val);

   Handle<Value> focus_get() const;
   Handle<Value> screen_position_get() const;
   Handle<Value> screen_size_get() const;

   Handle<Value> socket_listen(const Arguments&);
   Handle<Value> activate(const Arguments&);
   Handle<Value> lower(const Arguments&);
   Handle<Value> raise(const Arguments&);
   Handle<Value> center(const Arguments& args);

   friend Handle<Value> CElmObject::New<CElmWindow>(const Arguments& args);
};

}

#endif
