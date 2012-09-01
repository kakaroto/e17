#include "Empower.h"

static void      _gui_win_show_cb(void *data, Evas *e, Evas_Object *w, void *event);
static void      _gui_key_down_cb(void *data, Evas *e, Evas_Object *w, void *event);
static void      _gui_check_pass_cb(void *data, Evas_Object *w, void *event);
static void      _gui_id_change(void *data, Evas_Object *w, void *event);
static Eina_Bool _gui_grab_keyboard(void *data);

static struct
{
  Evas_Object *win;        // elm_win
  Evas_Object *frame;      // elm_frame
  Evas_Object *password;   // elm_entry
  Evas_Object *icon;       // elm_icon
  Evas_Object *message;    // elm_label
  Evas_Object *identity;   // elm_hoversel
  Empower_Identity *id;
} _gui;

Eina_Bool gui_init()
{
  char buf[PATH_MAX];
  Evas_Object *vbox;
  Evas_Object *vbox2;
  Evas_Object *hbox;
  Evas_Object *bg;
  Evas_Object *frame;
  Evas_Object *frame2;
  Evas_Object *frame3;
  Evas_Object *label;
  Evas_Object *grid;

  _gui.win = elm_win_add(NULL, "Empower", ELM_WIN_DIALOG_BASIC);
  elm_win_title_set(_gui.win, "Empower");
  elm_win_raise(_gui.win);
  elm_win_borderless_set(_gui.win, 1);
  elm_win_autodel_set(_gui.win, 1);
  elm_win_alpha_set(_gui.win,1);
  elm_win_fullscreen_set(_gui.win, 1);
  elm_win_keyboard_mode_set(_gui.win, ELM_WIN_KEYBOARD_ON);

  bg = elm_image_add(_gui.win);
  snprintf(buf, sizeof(buf), "%s/data/trans.png", PACKAGE_DATA_DIR);
  elm_image_file_set(bg, buf, NULL);
  elm_win_resize_object_add(_gui.win, bg);
  evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_image_fill_outside_set(bg, 1);
  evas_object_show(bg);

  grid = elm_gengrid_add(_gui.win);

  vbox2 = elm_box_add(_gui.win);
  evas_object_size_hint_weight_set(vbox2, 0.5, 0.25);
  evas_object_size_hint_align_set(vbox2, 0.5, 0.5);
  elm_win_resize_object_add(_gui.win, vbox2);
  evas_object_show(vbox2);

  _gui.frame = elm_frame_add(_gui.win);
  evas_object_size_hint_weight_set(_gui.frame, 0.25, 0.25);
  evas_object_size_hint_align_set(_gui.frame, 0.5, 0.5);
  elm_box_pack_end(vbox2, _gui.frame);
  elm_object_text_set(_gui.frame, _("Empower"));

  frame2 = elm_frame_add(_gui.win);
  evas_object_size_hint_weight_set(frame2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(frame2, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_object_style_set(frame2, "pad_medium");

  vbox = elm_box_add(_gui.win);
  elm_box_horizontal_set(vbox, EINA_FALSE);
  evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(vbox, 0.5, 0.5);
  elm_box_homogeneous_set(vbox, EINA_TRUE);

  hbox = elm_box_add(_gui.win);
  elm_box_horizontal_set(hbox, EINA_TRUE);
  evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(hbox, 0.0, 0.0);

  _gui.icon = elm_icon_add(_gui.win);
  elm_image_file_set(_gui.icon, "", "");
  elm_image_aspect_fixed_set(_gui.icon, EINA_TRUE);
  elm_image_fill_outside_set(_gui.icon, EINA_FALSE);
  evas_object_size_hint_weight_set(_gui.icon, 0.2, EVAS_HINT_EXPAND);
  elm_box_pack_end(hbox, _gui.icon);
  evas_object_show(_gui.icon);

  _gui.message = elm_label_add(_gui.win);
  elm_object_text_set(_gui.message, "");
  evas_object_size_hint_weight_set(_gui.message, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_box_pack_end(hbox, _gui.message);
  evas_object_show(_gui.message);

  elm_box_pack_end(vbox, hbox);
  evas_object_show(hbox);

  hbox = elm_box_add(_gui.win);
  elm_box_horizontal_set(hbox, EINA_TRUE);
  evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(hbox, EVAS_HINT_FILL, EVAS_HINT_FILL);

  label = elm_label_add(_gui.win);
  elm_object_text_set(label, "User: ");
  evas_object_size_hint_weight_set(label, 0.0, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(label, 0.0, 0.5);
  elm_box_pack_end(hbox, label);
  evas_object_show(label);

  _gui.identity = elm_hoversel_add(_gui.win);
  evas_object_size_hint_weight_set(_gui.identity, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(_gui.identity, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_box_pack_end(hbox, _gui.identity);
  evas_object_show(_gui.identity);

  elm_box_pack_end(vbox, hbox);
  evas_object_show(hbox);

  hbox = elm_box_add(_gui.win);
  elm_box_horizontal_set(hbox, EINA_TRUE);
  evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(hbox, EVAS_HINT_FILL, EVAS_HINT_FILL);

  label = elm_label_add(_gui.win);
  elm_object_text_set(label, "Password: ");
  evas_object_size_hint_weight_set(label, 0.0, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(label, 0.0, 0.5);
  elm_box_pack_end(hbox, label);
  evas_object_show(label);

  _gui.password = elm_entry_add(_gui.win);
  elm_entry_scrollable_set(_gui.password, EINA_TRUE);
  evas_object_size_hint_weight_set(_gui.password, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(_gui.password, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_name_set(_gui.password, "password");
  elm_entry_editable_set(_gui.password, EINA_TRUE);
  elm_entry_single_line_set(_gui.password, EINA_TRUE);
  elm_entry_password_set(_gui.password, EINA_TRUE);
  elm_box_pack_end(hbox, _gui.password);
  evas_object_show(_gui.password);

  elm_box_pack_end(vbox, hbox);
  evas_object_show(hbox);

  elm_object_content_set(frame2, vbox);
  evas_object_show(frame2);

  elm_object_content_set(_gui.frame, frame2);
  evas_object_show(vbox);
  evas_object_show(_gui.frame);

  return EINA_TRUE;
}

void gui_show(Empower_Auth_Info *info)
{
  Eina_List *l;
  Empower_Identity *id;

  elm_object_text_set(_gui.message, info->message);
  elm_icon_standard_set(_gui.icon, info->icon);
  elm_entry_entry_set(_gui.password, "");

  elm_hoversel_clear(_gui.identity);
  _gui.id = NULL;
  EINA_LIST_FOREACH(info->identities, l, id)
  {
    switch (id->kind)
    {
      case EMPOWER_IDENTITY_USER:
        elm_hoversel_item_add(_gui.identity, id->details.user.name, NULL, 0, _gui_id_change, id);
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
