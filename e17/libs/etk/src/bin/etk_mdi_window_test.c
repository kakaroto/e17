#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_test.h"

#include <Evas.h>
#include <stdlib.h>

/* Creates the window for the mdi test */
void etk_test_mdi_window_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Evas *evas;
   Etk_Widget *mdi_window;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_resize(ETK_WINDOW(win), 300, 300);
   etk_window_title_set(ETK_WINDOW(win), "Etk Mdi Window Test");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("delete-event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_widget_show_all(win);

   evas = etk_widget_toplevel_evas_get(ETK_WIDGET(win));

   mdi_window = etk_mdi_window_new(evas);
   etk_mdi_window_title_set(ETK_MDI_WINDOW(mdi_window), "Mdi Window 1");
   etk_mdi_window_resize(ETK_MDI_WINDOW(mdi_window), 100, 100);
   etk_widget_show_all(mdi_window);

   mdi_window = etk_mdi_window_new(evas);
   etk_mdi_window_title_set(ETK_MDI_WINDOW(mdi_window), "Mdi Window 2");
   etk_mdi_window_move(ETK_MDI_WINDOW(mdi_window), 100, 100);
   etk_mdi_window_resize(ETK_MDI_WINDOW(mdi_window), 100, 100);
   etk_widget_show_all(mdi_window);
}
