#include "etk_test.h"

/* Creates the window for the color picker test */
void etk_test_colorpicker_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *cp;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Color Picker Test");
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);	
   
   cp = etk_colorpicker_new();
   etk_container_add(ETK_CONTAINER(win), cp);
   
   etk_widget_show_all(win);
}
