#include "Empower.h"

void setup_window()
{		
  Ewl_Widget *ok_button = NULL;
  Ewl_Widget *cancel_button = NULL;
  Ewl_Widget *progtext = NULL;
  Ewl_Widget *vbox = NULL, *hbox = NULL;
  Ewl_Widget *image = NULL;
  entry = NULL;
  win = NULL;
  exec = NULL;
  
  win = ewl_dialog_new();
  ewl_window_title_set(EWL_WINDOW(win), "Empower!");
  ewl_window_name_set(EWL_WINDOW(win), "Empower!");
  ewl_window_class_set(EWL_WINDOW(win), "Empower!");
  ewl_window_dialog_set(EWL_WINDOW(win), 1);
  ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, destroy_cb, NULL);
  ewl_callback_prepend(win, EWL_CALLBACK_REVEAL, reveal_cb, NULL);
  ewl_callback_append(win, EWL_CALLBACK_KEY_DOWN, key_down_cb, NULL);
  ewl_window_keyboard_grab_set(EWL_WINDOW(win), 1);	
  ewl_dialog_active_area_set(EWL_DIALOG(win), EWL_POSITION_TOP);

  hbox = ewl_hbox_new();
  ewl_container_child_append(EWL_CONTAINER(win), hbox);
  ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
  ewl_object_padding_set(EWL_OBJECT(hbox), 15, 15, 15, 5);
  ewl_widget_show(hbox);

  image = ewl_image_new();
  ewl_image_file_set(EWL_IMAGE(image), 
    ewl_icon_theme_icon_path_get(EWL_ICON_SYSTEM_LOCK_SCREEN, 46), 
    EWL_ICON_SYSTEM_LOCK_SCREEN);
  ewl_container_child_append(EWL_CONTAINER(hbox), image);
  ewl_object_padding_set(EWL_OBJECT(image), 5, 20, 0, 0);
  ewl_widget_show(image);

  vbox = ewl_vbox_new();
  ewl_container_child_append(EWL_CONTAINER(hbox), vbox);
  ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_VFILL | EWL_FLAG_FILL_HFILL);
  ewl_box_homogeneous_set(EWL_OBJECT(vbox), 1);
  ewl_widget_show(vbox);

  if(mode == SUDOPROG)
  {
    exec = ewl_entry_new();
    ewl_container_child_append(EWL_CONTAINER(vbox), exec);
    ewl_widget_name_set(exec, "exec");
    ewl_callback_append(exec, EWL_CALLBACK_VALUE_CHANGED, check_pass_cb, 
              entry);
    ewl_callback_append(exec, EWL_CALLBACK_FOCUS_IN, focus_cb,
            "Execute");
    ewl_callback_append(exec, EWL_CALLBACK_FOCUS_OUT, unfocus_cb,
            "Execute");
    ewl_text_align_set(EWL_TEXT(exec),EWL_FLAG_ALIGN_LEFT);
    ewl_text_text_set(EWL_TEXT(exec),"Execute");
    ewl_text_cursor_position_set(EWL_TEXT(exec),0);
    ewl_text_color_apply(EWL_TEXT(exec),100,100,100,200,strlen("Execute"));
    ewl_object_minimum_h_set(EWL_OBJECT(exec), 20);
    ewl_widget_show(exec);
  }	

  entry = ewl_entry_new();
  ewl_container_child_append(EWL_CONTAINER(vbox), entry);
  ewl_widget_name_set(entry, "password");
  ewl_callback_append(entry, EWL_CALLBACK_VALUE_CHANGED, check_pass_cb, 
              entry);
  ewl_callback_append(entry, EWL_CALLBACK_FOCUS_IN, focus_cb,
          "Password");
  ewl_callback_append(entry, EWL_CALLBACK_FOCUS_OUT, unfocus_cb,
          "Password");
  ewl_text_align_set(EWL_TEXT(entry),EWL_FLAG_ALIGN_LEFT);
  ewl_text_text_set(EWL_TEXT(entry),"Password");
  ewl_text_cursor_position_set(EWL_TEXT(entry),0);
  ewl_text_color_apply(EWL_TEXT(entry),100,100,100,200,strlen("Password"));
  ewl_object_minimum_h_set(EWL_OBJECT(entry), 20);
  
  if(!auth_passed)
    ewl_widget_show(entry);
  
  startup = 0;
  failure = 0;
}

void display_window()
{
  if(win)
  {		
    ewl_widget_enable(win);
    
    ewl_text_clear(EWL_TEXT(entry));
    failure = 1;
  }
  else
  {
    setup_window();
    ewl_widget_show(win);
  }
}
