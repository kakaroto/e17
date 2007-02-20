#include "etk_test.h"

/* Creates the window for the slider test */
void etk_test_slider_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *table;
   Etk_Widget *slider;
   Etk_Widget *spinner;
   Etk_Widget *separator;
   
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
   slider = etk_hslider_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_slider_label_set(ETK_SLIDER(slider), "%.0f");
   etk_widget_size_request_set(slider, 130, 130);
   etk_table_attach_default(ETK_TABLE(table), slider, 0, 0, 0, 0);
   
   /* Create the vertical slider */
   slider = etk_vslider_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_slider_label_set(ETK_SLIDER(slider), "%.0f");
   etk_widget_size_request_set(slider, 130, 130);
   etk_table_attach_default(ETK_TABLE(table), slider, 1, 1, 0, 0);
   
   separator = etk_hseparator_new();
   etk_table_attach_default(ETK_TABLE(table), separator, 0, 1, 1, 1);
   
   /* Create the spinner */
   spinner = etk_spinner_new(0.0, 255.0, 128.00, 1.0, 10.0);
   etk_table_attach(ETK_TABLE(table), spinner, 0, 1, 2, 2, 0, 0, ETK_TABLE_NONE);
   
   etk_widget_show_all(win);
}
