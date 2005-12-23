#include "etk_test.h"

/* Creates the window for the notebook test */
void etk_test_notebook_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *notebook;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Notebook Test"));
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);	
   
   notebook = etk_notebook_new();
   etk_container_add(ETK_CONTAINER(win), notebook);
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Tab 1");
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "This is a test");
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Hey, another tab");
   
   etk_widget_show_all(win);
}
