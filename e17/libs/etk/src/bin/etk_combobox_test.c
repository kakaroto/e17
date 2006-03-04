#include "etk_test.h"

/* Creates the window for the combobox test */
void etk_test_combobox_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *combobox;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Combobox Test"));
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(win), vbox);

   combobox = etk_combobox_new();
   etk_box_pack_start(ETK_BOX(vbox), combobox, ETK_FALSE, ETK_FALSE, 0);

   etk_widget_show_all(win);
}
