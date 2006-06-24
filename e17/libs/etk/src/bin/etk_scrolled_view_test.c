#include "etk_test.h"

/* Creates the window for the scrolled view test */
void etk_test_scrolled_view_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *scrolled_view;
   Etk_Widget *button;
   
   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Scrolled View Test");
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_widget_size_request_set(win, 180, 180);
	
   scrolled_view = etk_scrolled_view_new();
   etk_container_add(ETK_CONTAINER(win), scrolled_view);

   button = etk_button_new_with_label("Scrolled View Test");
   etk_widget_size_request_set(button, 300, 300);
   etk_scrolled_view_add_with_viewport(ETK_SCROLLED_VIEW(scrolled_view), button);
   
   etk_widget_show_all(win);
}
