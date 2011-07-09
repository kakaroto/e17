#include "Empower.h"
#include "config.h"

void setup_window()
{		
  Evas_Object *ok_button = NULL;
  Evas_Object *cancel_button = NULL;
  Evas_Object *progtext = NULL;
  Evas_Object *vbox = NULL, *vbox_frame2 = NULL, *vbox_frame = NULL;
  Evas_Object *image = NULL;
  Evas_Object *bg = NULL;
  Evas_Object *title = NULL;
  Evas_Object *execscroller = NULL, *passscroller = NULL;
  Evas_Object *scrollerframe = NULL, *scrollerframe2 = NULL;
  Evas_Object *exec_frame = NULL, *exec_frame2 = NULL;
  Evas_Object *pass_frame = NULL, *pass_frame2 = NULL;
  Evas_Object *inwin = NULL;
  char buf[PATH_MAX];

  entry = NULL;
  win = NULL;
  exec = NULL;

  win = elm_win_add(NULL, "Empower", ELM_WIN_DIALOG_BASIC);
  elm_win_title_set(win, "Empower");
  elm_win_raise(win);
  elm_win_borderless_set(win, 1);
  elm_win_autodel_set(win, 1);
  elm_win_alpha_set(win,1);
  elm_win_fullscreen_set(win, 1);
  elm_win_keyboard_mode_set(win, ELM_WIN_KEYBOARD_ON);
  evas_object_event_callback_add(win, EVAS_CALLBACK_SHOW, win_show, win);
  if(evas_object_key_grab(win, "Escape", 0, 0, 1))
    evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN, key_down_cb, NULL);

  bg = elm_image_add(win);
  snprintf(buf, sizeof(buf), "%s/data/trans.png", PACKAGE_DATA_DIR);
  elm_image_file_set(bg, buf, NULL);
  elm_win_resize_object_add(win, bg);
  evas_object_size_hint_weight_set(bg, 1.0, 1.0);
  evas_object_size_hint_align_set(bg, -1.0, -1.0);
  elm_image_fill_outside_set(bg, 1);
  evas_object_show(bg);

  vbox = elm_box_add(win);
  elm_box_horizontal_set(vbox, 0);
  evas_object_size_hint_weight_set(vbox, 0.5, 0.5);
  evas_object_size_hint_align_set(vbox, 0.0, 0.0);
  evas_object_size_hint_min_set(vbox, 100, 100);
  evas_object_size_hint_max_set(vbox, 600, 600);
  elm_box_homogenous_set(vbox, 1);
  evas_object_show(vbox);

  inwin = elm_win_inwin_add(win);
  evas_object_size_hint_weight_set(inwin, 0.1, 0.1);
  elm_object_style_set(inwin, "minimal_vertical");
  elm_win_inwin_content_set(inwin, vbox);
  evas_object_show(inwin);

  if(mode == SUDOPROG)
  {
    exec = elm_entry_add(win);
    evas_object_size_hint_weight_set(exec, 1.0, 0.0);
    evas_object_size_hint_align_set(exec, -1.0, 0.0);
    evas_object_name_set(exec, "exec");
    elm_entry_editable_set(exec, 1);
    elm_entry_single_line_set(exec, 1);
    elm_object_focus(exec);

    scrollerframe2 = elm_frame_add(win);
    evas_object_size_hint_weight_set(scrollerframe2, 1.0, 0.0);
    evas_object_size_hint_align_set(scrollerframe2, -1.0, 0.0);
    elm_object_style_set(scrollerframe2, "pad_small");
    elm_frame_content_set(scrollerframe2, exec);

    execscroller = elm_scroller_add(win);
    evas_object_size_hint_weight_set(execscroller, 1.0, 1.0);
    evas_object_size_hint_align_set(execscroller, -1.0, 1.0);
    elm_scroller_content_min_limit(execscroller, 0, 1);
    evas_object_size_hint_padding_set(execscroller, 5, 5, 5, 5);
    elm_scroller_content_set(execscroller, scrollerframe2);
    
    scrollerframe = elm_frame_add(win);
    evas_object_size_hint_weight_set(scrollerframe, 1.0, 0.0);
    evas_object_size_hint_align_set(scrollerframe, -1.0, 0.0);
    elm_object_style_set(scrollerframe, "pad_medium");
    elm_frame_content_set(scrollerframe, execscroller);

    // Add callbacks
    evas_object_smart_callback_add(exec, "activated", check_pass_cb, NULL);

    exec_frame2 = elm_frame_add(win);
    evas_object_size_hint_weight_set(exec_frame2, 1.0, 0.0);
    evas_object_size_hint_align_set(exec_frame2, -1.0, 0.0);
    elm_frame_content_set(exec_frame2, scrollerframe);
    elm_frame_label_set(exec_frame2, _("Execute"));

    exec_frame = elm_frame_add(win);
    evas_object_size_hint_weight_set(exec_frame, 1.0, 0.0);
    evas_object_size_hint_align_set(exec_frame, -1.0, 0.0);
    elm_object_style_set(exec_frame, "pad_small");
    elm_frame_content_set(exec_frame, exec_frame2);
    elm_box_pack_end(vbox, exec_frame);

    // show exec entry
    evas_object_show(exec);
    evas_object_show(exec_frame2);
    evas_object_show(exec_frame);
  }

  if(!auth_passed)
  {
    entry = elm_entry_add(win);
    evas_object_size_hint_weight_set(entry, 0.0, 0.0);
    evas_object_size_hint_align_set(entry, -1.0, 0.0);
    evas_object_name_set(entry, "password");
    elm_entry_editable_set(entry, 1);
    elm_entry_single_line_set(entry, 1);
    elm_entry_password_set(entry, 1);

    scrollerframe2 = elm_frame_add(win);
    evas_object_size_hint_weight_set(scrollerframe2, 1.0, 0.0);
    evas_object_size_hint_align_set(scrollerframe2, -1.0, 0.0);
    elm_object_style_set(scrollerframe2, "pad_small");
    elm_frame_content_set(scrollerframe2, entry);
    
    passscroller = elm_scroller_add(win);
    evas_object_size_hint_weight_set(passscroller, 1.0, 1.0);
    evas_object_size_hint_align_set(passscroller, -1.0, 1.0);
    elm_scroller_content_min_limit(passscroller, 0, 1);
    elm_scroller_content_set(passscroller, scrollerframe2);

    scrollerframe = elm_frame_add(win);
    evas_object_size_hint_weight_set(scrollerframe, 1.0, 0.0);
    evas_object_size_hint_align_set(scrollerframe, -1.0, 0.0);
    elm_object_style_set(scrollerframe, "pad_medium");
    elm_frame_content_set(scrollerframe, passscroller);
    
    // Add callbacks
    evas_object_smart_callback_add(entry, "activated", check_pass_cb, NULL);

    pass_frame2 = elm_frame_add(win);
    evas_object_size_hint_weight_set(pass_frame2, 1.0, 0.0);
    evas_object_size_hint_align_set(pass_frame2, -1.0, 0.0);
    elm_frame_content_set(pass_frame2, scrollerframe);
    elm_frame_label_set(pass_frame2, _("Password"));

    pass_frame = elm_frame_add(win);
    evas_object_size_hint_weight_set(pass_frame, 1.0, 0.0);
    evas_object_size_hint_align_set(pass_frame, -1.0, 0.0);
    elm_object_style_set(pass_frame, "pad_small");
    elm_frame_content_set(pass_frame, pass_frame2);
    elm_box_pack_end(vbox, pass_frame);
  }

  if(entry)
  {
    evas_object_show(entry);
    evas_object_show(pass_frame2);
    evas_object_show(pass_frame);
    if(mode != SUDOPROG)
      elm_object_focus(entry);
  }

  startup = 0;
  failure = 0;
}

void display_window()
{
  Ecore_X_Window ewin;

  if(win)
  {
    if(entry)
      elm_entry_entry_set(entry, "");
    failure = 1;
  }
  else
  {
    setup_window();
  }

  if(exec)
  {
    elm_object_focus(exec);
  }
  else
  {
    elm_object_focus(entry);
  }

  evas_object_show(win);
}
