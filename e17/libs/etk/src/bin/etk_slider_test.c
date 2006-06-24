#include "etk_test.h"

static void _etk_test_slider_value_changed(Etk_Object *object, double value, void *data);

/* Creates the window for the paned test */
void etk_test_slider_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *table;
   Etk_Widget *slider;
   Etk_Widget *label;
   
   if (win)
   {
      etk_widget_show(ETK_WIDGET(win));
      return;
   }
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Slider Test");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
        
   table = etk_table_new(2, 2, ETK_FALSE);
   etk_container_add(ETK_CONTAINER(win), table);
   
   slider = etk_hslider_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_widget_size_request_set(slider, 130, 130);
   etk_table_attach_defaults(ETK_TABLE(table), slider, 0, 0, 0, 0);
   
   label = etk_label_new("128.00");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1, 0, 0, ETK_FILL_POLICY_NONE);
   etk_signal_connect("value_changed", ETK_OBJECT(slider), ETK_CALLBACK(_etk_test_slider_value_changed), label);
   
   slider = etk_vslider_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_widget_size_request_set(slider, 130, 130);
   etk_table_attach_defaults(ETK_TABLE(table), slider, 1, 1, 0, 0);

   label = etk_label_new("128.00");
   etk_table_attach(ETK_TABLE(table), label, 1, 1, 1, 1, 0, 0, ETK_FILL_POLICY_NONE);
   etk_signal_connect("value_changed", ETK_OBJECT(slider), ETK_CALLBACK(_etk_test_slider_value_changed), label);
   
   etk_widget_show_all(win);
}

/* Called when the value of the slider is changed */
static void _etk_test_slider_value_changed(Etk_Object *object, double value, void *data)
{
   char string[256];

   snprintf(string, 255, "%'.2f", value);
   etk_label_set(ETK_LABEL(data), string);
}
