#include "etk_test.h"

static void _inverted_toggled_cb(Etk_Object *object, void *data);

/* Creates the window for the slider test */
void etk_test_slider_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *table;
   Etk_Widget *hslider;
   Etk_Widget *vslider;
   Etk_Widget *separator;
   Etk_Widget *frame;
   Etk_Widget *vbox;
   Etk_Widget *check;
   
   if (win)
   {
      etk_widget_show(ETK_WIDGET(win));
      return;
   }
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Slider Test");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   table = etk_table_new(2, 3, ETK_FALSE);
   etk_container_add(ETK_CONTAINER(win), table);
   
   /* Create the horizontal slider */
   hslider = etk_hslider_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_slider_label_set(ETK_SLIDER(hslider), "%.0f");
   etk_widget_size_request_set(hslider, 130, 130);
   etk_table_attach_default(ETK_TABLE(table), hslider, 0, 0, 0, 0);
   
   /* Create the vertical slider */
   vslider = etk_vslider_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_slider_label_set(ETK_SLIDER(vslider), "%.0f");
   etk_widget_size_request_set(vslider, 130, 130);
   etk_table_attach_default(ETK_TABLE(table), vslider, 1, 1, 0, 0);
   
   separator = etk_hseparator_new();
   etk_table_attach(ETK_TABLE(table), separator, 0, 1, 1, 1, 0, 0, ETK_TABLE_HFILL);
   
   
   /* Create the settings frame for the horizontal slider */
   frame = etk_frame_new("Horizontal");
   etk_table_attach(ETK_TABLE(table), frame, 0, 0, 2, 2, 0, 0, ETK_TABLE_HEXPAND | ETK_TABLE_FILL);
   vbox = etk_vbox_new(ETK_TRUE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox);
   
   check = etk_check_button_new_with_label("Inverted");
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check), ETK_CALLBACK(_inverted_toggled_cb), hslider);
   
   check = etk_check_button_new_with_label("Continuous Update");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   
   
   /* Create the settings frame for the vertical slider */
   frame = etk_frame_new("Vertical");
   etk_table_attach(ETK_TABLE(table), frame, 1, 1, 2, 2, 0, 0, ETK_TABLE_HEXPAND | ETK_TABLE_FILL);
   vbox = etk_vbox_new(ETK_TRUE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox);
   
   check = etk_check_button_new_with_label("Inverted");
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check), ETK_CALLBACK(_inverted_toggled_cb), vslider);
   
   check = etk_check_button_new_with_label("Continuous Update");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   
   
   etk_widget_show_all(win);
}

/* Called when one of the "Inverted" check buttons is toggled */
static void _inverted_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *toggle;
   Etk_Slider *slider;
   
   if (!(toggle = ETK_TOGGLE_BUTTON(object)) || !(slider = ETK_SLIDER(data)))
      return;
   etk_slider_inverted_set(slider, etk_toggle_button_active_get(toggle));
}
