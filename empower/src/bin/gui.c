#include "Empower.h"

static void      _gui_win_show_cb(void *data, Evas *e, Evas_Object *w, void *event);
static void      _gui_key_down_cb(void *data, Evas *e, Evas_Object *w, void *event);
static void      _gui_check_pass_cb(void *data, Evas_Object *w, void *event);
static void      _gui_id_change(void *data, Evas_Object *w, void *event);
static Eina_Bool _gui_grab_keyboard(void *data);

static struct
{
  unsigned int h;
  unsigned int w;
  Evas_Object *win;        // elm_win
  Evas_Object *password;   // elm_entry
  Evas_Object *icon;       // elm_icon
  Evas_Object *message;    // evas_object_text
  Evas_Object *identity;   // elm_hoversel
  Evas_Object *grid;       // elm_grid
  Empower_Identity *id;
} _gui;

Eina_Bool gui_init()
{
  char buf[PATH_MAX];
  Evas_Object *bg;
  Evas_Object *o;
  unsigned int w,h;

  _gui.win = elm_win_add(NULL, "Empower", ELM_WIN_DIALOG_BASIC);
  elm_win_title_set(_gui.win, "Empower");
  elm_win_raise(_gui.win);
  elm_win_borderless_set(_gui.win, 1);
  elm_win_autodel_set(_gui.win, 1);
  elm_win_alpha_set(_gui.win,1);
  elm_win_fullscreen_set(_gui.win, 1);
  elm_win_keyboard_mode_set(_gui.win, ELM_WIN_KEYBOARD_ON);

  elm_win_screen_size_get(_gui.win, NULL, NULL, &_gui.w, &_gui.h);

  bg = elm_image_add(_gui.win);
  snprintf(buf, sizeof(buf), "%s/data/trans.png", PACKAGE_DATA_DIR);
  elm_image_file_set(bg, buf, NULL);
  elm_win_resize_object_add(_gui.win, bg);
  elm_image_fill_outside_set(bg, 1);
  evas_object_show(bg);

  _gui.grid = elm_grid_add(_gui.win);
  elm_grid_size_set(_gui.grid, _gui.w, _gui.h);
  elm_win_resize_object_add(_gui.win, _gui.grid);
  evas_object_show(_gui.grid);

  _gui.icon = elm_icon_add(_gui.win);
  elm_image_aspect_fixed_set(_gui.icon, EINA_TRUE);
  elm_image_fill_outside_set(_gui.icon, EINA_TRUE);
  evas_object_size_hint_weight_set(_gui.icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(_gui.icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_grid_pack(_gui.grid, _gui.icon, (_gui.w/2)-64, (_gui.h/2) - 256, 128, 128);
  evas_object_show(_gui.icon);

  _gui.message = evas_object_text_add(evas_object_evas_get(_gui.win));
  evas_object_text_font_set(_gui.message, "sans-serif", 18);
  evas_object_text_style_set(_gui.message, EVAS_TEXT_STYLE_GLOW);
  evas_object_color_set(_gui.message, 0xff, 0xff, 0xff, 0xff);
  evas_object_text_glow_color_set(_gui.message, 0x60, 0x60, 0x60, 0xff);
  elm_grid_pack(_gui.grid, _gui.message, (_gui.w/2), (_gui.h/2) - 120, 0, 0);
  evas_object_show(_gui.message);

  o = evas_object_text_add(evas_object_evas_get(_gui.win));
  evas_object_text_font_set(o, "sans-serif", 14);
  evas_object_text_style_set(o, EVAS_TEXT_STYLE_GLOW);
  evas_object_color_set(o, 0xff, 0xff, 0xff, 0xff);
  evas_object_text_glow_color_set(o, 0x60, 0x60, 0x60, 0xff);
  evas_object_text_text_set(o, "Identity");
  evas_object_geometry_get(o, NULL, NULL, &w, &h);
  elm_grid_pack(_gui.grid, o, (_gui.w/2) - (w/2), ((_gui.h/2) - h - 24), w, h);
  evas_object_show(o);

  _gui.identity = elm_hoversel_add(_gui.win);
  evas_object_name_set(_gui.identity, "identity");
  elm_grid_pack(_gui.grid, _gui.identity, (_gui.w/2) - (_gui.w/8), (_gui.h/2) - 16, (_gui.w/4), 32);
  evas_object_show(_gui.identity);

  o = evas_object_text_add(evas_object_evas_get(_gui.win));
  evas_object_text_font_set(o, "sans-serif", 14);
  evas_object_text_style_set(o, EVAS_TEXT_STYLE_GLOW);
  evas_object_color_set(o, 0xff, 0xff, 0xff, 0xff);
  evas_object_text_glow_color_set(o, 0x60, 0x60, 0x60, 0xff);
  evas_object_text_text_set(o, "Password");
  evas_object_geometry_get(o, NULL, NULL, &w, &h);
  elm_grid_pack(_gui.grid, o, (_gui.w/2) - (w/2), ((_gui.h/2) + 24), w, h);
  evas_object_show(o);

  _gui.password = elm_entry_add(_gui.win);
  elm_entry_scrollable_set(_gui.password, EINA_TRUE);
  evas_object_name_set(_gui.password, "password");
  elm_entry_editable_set(_gui.password, EINA_TRUE);
  elm_entry_single_line_set(_gui.password, EINA_TRUE);
  elm_entry_password_set(_gui.password, EINA_TRUE);
  elm_entry_input_panel_layout_set(_gui.password, ELM_INPUT_PANEL_LAYOUT_PASSWORD);
  elm_entry_autocapital_type_set(_gui.password, ELM_AUTOCAPITAL_TYPE_NONE);
  elm_entry_text_style_user_push(_gui.password, "DEFAULT='font=Sans font_size=20 align=middle valign=baseline linesize=16 color=#ffffff glow_color=#606060 style=glow'");
  elm_grid_pack(_gui.grid, _gui.password, (_gui.w/2) - (_gui.w/8), (_gui.h/2) + 24 + h, (_gui.w/4), 32);
  evas_object_show(_gui.password);

  return EINA_TRUE;
}

void gui_show(Empower_Auth_Info *info)
{
  Eina_List *l;
  Empower_Identity *id;
  Efreet_Icon_Theme *theme;
  const char *icon_path;
  unsigned int x, y, w, h;

  // Reposition message so it's centered.
  evas_object_text_text_set(_gui.message, info->message);
  evas_object_geometry_get(_gui.message, &x, &y, &w, &h);
  elm_grid_pack_set(_gui.message, (_gui.w/2) - (w/2), (_gui.h/2) - (h/2) - 112, w, h);

  // Clear password field here.  Should already be clear, but be sure.
  elm_entry_entry_set(_gui.password, "");

  // Set icon if it's specified.  I've used efreet directly as
  // elm_icon_standard_set is no good.
  if (info->icon && info->icon[0])
  {
    theme = efreet_icon_theme_find(getenv("E_ICON_THEME"));
    if (theme)
    {
      icon_path = efreet_icon_path_find(theme->name.internal, info->icon, 128);

      if (icon_path)
        elm_image_file_set(_gui.icon, icon_path, NULL);
    }
  }

  // Setup hoversel
  // TODO: Come up with a face browser of sorts instead of this.
  elm_hoversel_clear(_gui.identity);
  _gui.id = NULL;
  EINA_LIST_FOREACH(info->identities, l, id)
  {
    switch (id->kind)
    {
      case EMPOWER_IDENTITY_USER:
        elm_hoversel_item_add(_gui.identity, id->details.user.name, NULL, 0, _gui_id_change, id);

        // If we find the current user in the list then use that as default
        if (id->details.user.uid == empower_uid)
        {
          _gui.id = id;
          elm_object_text_set(_gui.identity, id->details.user.name);
        }
        break;
      case EMPOWER_IDENTITY_GROUP:
        elm_hoversel_item_add(_gui.identity, id->details.group.name, NULL, 0, _gui_id_change, id);
        break;
    }

    if (!_gui.id)
    {
      _gui.id = id;
      elm_object_text_set(_gui.identity, id->details.user.name);
    }
  }

  evas_object_event_callback_add(_gui.win, EVAS_CALLBACK_SHOW, _gui_win_show_cb, _gui.win);

  if(evas_object_key_grab(_gui.win, "Escape", 0, 0, 1))
    evas_object_event_callback_add(_gui.win, EVAS_CALLBACK_KEY_DOWN, _gui_key_down_cb, info);

  evas_object_smart_callback_add(_gui.password, "activated", _gui_check_pass_cb, info);

  evas_object_show(_gui.win);
}

void gui_hide()
{
  evas_object_key_ungrab(_gui.win, "Escape", 0, 0);
  evas_object_event_callback_del(_gui.win, EVAS_CALLBACK_SHOW, _gui_win_show_cb);
  evas_object_event_callback_del(_gui.win, EVAS_CALLBACK_KEY_DOWN, _gui_key_down_cb);
  evas_object_smart_callback_del(_gui.password, "activated", _gui_check_pass_cb);
  evas_object_hide(_gui.win);
}

Eina_Bool gui_visible_get()
{
  return evas_object_visible_get(_gui.win);
}

void _gui_win_show_cb(void *data, Evas *e, Evas_Object *w, void *event)
{
  elm_object_focus_set(_gui.password, EINA_TRUE);
  ecore_timer_add(0.1, _gui_grab_keyboard, data);
}

void _gui_key_down_cb(void *data, Evas *e, Evas_Object *w, void *event)
{
  Evas_Event_Key_Down *ev = event;
  Empower_Auth_Info *info;

  if(strcmp(ev->keyname, "Escape") == 0)
  {
    gui_hide();
    elm_entry_entry_set(_gui.password, "");
    info = data;
    pka_cancel(info);
  }
}

void _gui_check_pass_cb(void *data, Evas_Object *w, void *event)
{
  Empower_Auth_Info *info;
  Empower_Identity *id;
  Eina_List *l, *ll;

  info = data;

  info->password = elm_entry_markup_to_utf8(elm_entry_entry_get(w));
  elm_entry_entry_set(w, "");

  gui_hide();

  // Remove all but the identity we've chosen
  EINA_LIST_FOREACH_SAFE(info->identities, l, ll, id)
  {
    if (id != _gui.id)
      info->identities = eina_list_remove_list(info->identities, l);
  }

  pka_exec(info);
}

Eina_Bool _gui_grab_keyboard(void *data)
{
  static int failures = 0;
  Ecore_X_Window ewin;
  Eina_Bool ret = 0;

  // Get window from elm
  ewin = elm_win_xwindow_get((Evas_Object*)data);

  // Grab keyboard
  ret = ecore_x_keyboard_grab(ewin);

  // If this fails 20 times (2 seconds), exit
  // the program since we can't grab the keyboard
  if(failures++ > 20)
  {
    ERR(_("Unable to grab keyboard, exiting...\n"));
    evas_object_hide(_gui.win);
  }

  // If keyboard is grabbed, disable this timer, otherwise
  // continue to try.
  return (ret == 0);
}

static void _gui_id_change(void *data, Evas_Object *w, void *event)
{
  _gui.id = (Empower_Identity*)data;
  elm_object_text_set(w, _gui.id->details.user.name);
}
