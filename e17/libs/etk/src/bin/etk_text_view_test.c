#include "etk_test.h"

/* Creates the window for the text view test */
void etk_test_text_view_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *text_view;
   
   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }	
  
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Text View Test"));
   etk_widget_size_request_set(ETK_WIDGET(win), 400, 300);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);

   text_view = etk_text_view_new();
   etk_box_pack_start(ETK_BOX(vbox), text_view, ETK_TRUE, ETK_TRUE, 0);
   
   etk_widget_show_all(win);
}
