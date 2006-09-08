#include "ephoto.h"
Ecore_Timer *timer;

void destroy_slideshow(Ewl_Widget *w, void *event, void *data)
{
 ecore_timer_del(timer);
 ewl_widget_destroy(w);
}

int change_picture(void *data)
{
 char *image_path;
 Ewl_Widget *w;
 
 w = data;
 ecore_dlist_next(current_thumbs);
 image_path = ecore_dlist_current(current_thumbs);
 if(image_path)
 {
  ewl_image_file_set(EWL_IMAGE(w), image_path, NULL);
 }
 else 
 {
  ecore_timer_del(timer);
  ewl_widget_destroy(w->parent->parent);
 }
}
 
void start_slideshow(Ewl_Widget *w, void *event, void *data)
{
 Ewl_Widget *window;
 Ewl_Widget *vbox;
 Ewl_Widget *image;
 char *image_path;
 
 image_path = ecore_dlist_goto_first(current_thumbs);
 
 if (!image_path) return;
 
 window = ewl_window_new();
 ewl_window_fullscreen_set(EWL_WINDOW(window), 1);
 ewl_callback_append(window, EWL_CALLBACK_DELETE_WINDOW, destroy_slideshow, NULL);
 ewl_callback_append(window, EWL_CALLBACK_CLICKED, destroy_slideshow, NULL); 
 ewl_widget_show(window);

 vbox = ewl_vbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(window), vbox);
 ewl_widget_show(vbox);

 image = ewl_image_new();
 ewl_image_file_set(EWL_IMAGE(image), image_path, NULL);
 ewl_object_fill_policy_set(EWL_OBJECT(image), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(vbox), image);
 ewl_widget_show(image);

 timer = ecore_timer_add(3, change_picture, image);
}

void config_cancel(Ewl_Widget *w, void *event, void *data)
{
 Ewl_Widget *win;
 win = data;
 ewl_widget_destroy(win);
}

void create_slideshow_config(Ewl_Widget *w, void *event, void *data)
{
 Ewl_Widget *win;
 Ewl_Widget *vbox;
 Ewl_Widget *hbox;
 Ewl_Widget *window_border;
 Ewl_Widget *transition_border;
 Ewl_Widget *order_border;
 Ewl_Widget *fullscreen;
 Ewl_Widget *custom;
 Ewl_Widget *spinner;
 Ewl_Widget *show_name;
 Ewl_Widget *random;
 Ewl_Widget *loop;
 Ewl_Widget *text;
 Ewl_Widget *wentry;
 Ewl_Widget *hentry;
 Ewl_Widget *save;
 Ewl_Widget *cancel;

 win = ewl_window_new();
 ewl_window_title_set(EWL_WINDOW(win), "Slideshow Configuration");
 ewl_window_name_set(EWL_WINDOW(win), "Slideshow Configuration");
 ewl_window_dialog_set(EWL_WINDOW(win), 1);
 ewl_object_size_request(EWL_OBJECT(win), 400, 200);
 ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, config_cancel, win);
 ewl_widget_show(win);

 vbox = ewl_vbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(win), vbox);
 ewl_widget_show(vbox);

 hbox = ewl_hbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
 ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(hbox);
      
 window_border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(window_border), "Window Size");
 ewl_border_label_alignment_set(EWL_BORDER(window_border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), window_border);
 ewl_object_alignment_set(EWL_OBJECT(window_border), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(window_border);

 order_border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(order_border), "Order");
 ewl_border_label_alignment_set(EWL_BORDER(order_border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), order_border);
 ewl_object_alignment_set(EWL_OBJECT(order_border), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(order_border);
 
 transition_border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(transition_border), "Transitions");
 ewl_border_label_alignment_set(EWL_BORDER(transition_border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(vbox), transition_border);
 ewl_object_alignment_set(EWL_OBJECT(transition_border), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(transition_border);
 
 hbox = ewl_hbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
 ewl_container_child_append(EWL_CONTAINER(window_border), hbox);
 ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(hbox);

 fullscreen = ewl_radiobutton_new();
 ewl_button_label_set(EWL_BUTTON(fullscreen), "Fullscreen");
 ewl_container_child_append(EWL_CONTAINER(hbox), fullscreen);
 ewl_radiobutton_checked_set(EWL_RADIOBUTTON(fullscreen), TRUE);
 ewl_object_alignment_set(EWL_OBJECT(fullscreen), EWL_FLAG_ALIGN_LEFT);
 ewl_widget_show(fullscreen);	
 
 custom = ewl_radiobutton_new();
 ewl_button_label_set(EWL_BUTTON(custom), "Custom");
 ewl_container_child_append(EWL_CONTAINER(hbox), custom);
 ewl_radiobutton_checked_set(EWL_RADIOBUTTON(custom), FALSE);
 ewl_radiobutton_chain_set(EWL_RADIOBUTTON(fullscreen), EWL_RADIOBUTTON(custom));
 ewl_object_alignment_set(EWL_OBJECT(custom), EWL_FLAG_ALIGN_RIGHT);
 ewl_widget_show(custom);

 hbox = ewl_hbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
 ewl_container_child_append(EWL_CONTAINER(window_border), hbox);
 ewl_widget_show(hbox);
 
 text = ewl_text_new();
 ewl_text_text_set(EWL_TEXT(text), "Width");
 ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), text);
 ewl_widget_show(text);

 wentry = ewl_entry_new();
 ewl_text_text_set(EWL_TEXT(wentry), "640");
 ewl_container_child_append(EWL_CONTAINER(hbox), wentry);
 ewl_object_size_request(EWL_OBJECT(wentry), 35, 15);
 ewl_widget_disable(wentry);
 ewl_widget_show(wentry);

 text = ewl_text_new();
 ewl_text_text_set(EWL_TEXT(text), "Height");
 ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), text);
 ewl_widget_show(text);

 loop = ewl_checkbutton_new();
 ewl_button_label_set(EWL_BUTTON(loop), "Loop Slideshow");
 ewl_checkbutton_checked_set(EWL_CHECKBUTTON(loop), FALSE);
 ewl_container_child_append(EWL_CONTAINER(order_border), loop);
 ewl_object_alignment_set(EWL_OBJECT(loop), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(loop);

 random = ewl_checkbutton_new();
 ewl_button_label_set(EWL_BUTTON(random), "Random Order");
 ewl_checkbutton_checked_set(EWL_CHECKBUTTON(random), FALSE);
 ewl_container_child_append(EWL_CONTAINER(order_border), random);
 ewl_object_alignment_set(EWL_OBJECT(random), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(random);
 
 hentry = ewl_entry_new();
 ewl_text_text_set(EWL_TEXT(hentry), "480");
 ewl_container_child_append(EWL_CONTAINER(hbox), hentry);
 ewl_object_size_request(EWL_OBJECT(hentry), 35, 15);
 ewl_widget_disable(hentry);
 ewl_widget_show(hentry);
 
 hbox = ewl_hbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
 ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(transition_border), hbox);
 ewl_widget_show(hbox);
 
 spinner = ewl_spinner_new();
 ewl_spinner_digits_set(EWL_SPINNER(spinner), 0);
 ewl_range_value_set(EWL_RANGE(spinner), 3);
 ewl_range_step_set(EWL_RANGE(spinner), 1);
 ewl_range_minimum_value_set(EWL_RANGE(spinner), 1.0);
 ewl_range_maximum_value_set(EWL_RANGE(spinner), 1000);
 ewl_container_child_append(EWL_CONTAINER(hbox), spinner);
 ewl_object_alignment_set(EWL_OBJECT(spinner), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(spinner);
 
 show_name = ewl_checkbutton_new();
 ewl_button_label_set(EWL_BUTTON(show_name), "Show File Name On Image Change");
 ewl_checkbutton_checked_set(EWL_CHECKBUTTON(show_name), FALSE);
 ewl_container_child_append(EWL_CONTAINER(hbox), show_name);
 ewl_object_alignment_set(EWL_OBJECT(show_name), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(show_name);
      
 hbox = ewl_hbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
 ewl_widget_show(hbox);
 
 save = ewl_button_new();
 ewl_button_stock_type_set(EWL_BUTTON(save), EWL_STOCK_SAVE);
 ewl_container_child_append(EWL_CONTAINER(hbox), save);
 ewl_object_fill_policy_set(EWL_OBJECT(save), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(save), EWL_FLAG_ALIGN_CENTER);
 //ewl_callback_append(save, EWL_CALLBACK_CLICKED, save_slide_config, NULL);
 ewl_widget_show(save);

 cancel = ewl_button_new();
 ewl_button_stock_type_set(EWL_BUTTON(cancel), EWL_STOCK_CANCEL);
 ewl_container_child_append(EWL_CONTAINER(hbox), cancel);
 ewl_object_fill_policy_set(EWL_OBJECT(cancel), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(cancel), EWL_FLAG_ALIGN_CENTER);
 ewl_callback_append(cancel, EWL_CALLBACK_CLICKED, config_cancel, win);
 ewl_widget_show(cancel);   
}
