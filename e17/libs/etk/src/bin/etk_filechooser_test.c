#include "etk_test.h"

/* Creates the window for the file chooser test */
void etk_test_filechooser_window_create(void *data)
{
   static Etk_Widget *dialog = NULL;
   Etk_Widget *filechooser;

   if (dialog)
   {
      etk_widget_show_all(ETK_WIDGET(dialog));
      return;
   }
   
   dialog = etk_dialog_new();
   etk_window_title_set(ETK_WINDOW(dialog), "Etk Filechooser Test");
   etk_signal_connect("delete_event", ETK_OBJECT(dialog), ETK_CALLBACK(etk_window_hide_on_delete), NULL);	
   
   filechooser = etk_filechooser_widget_new();
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), filechooser, ETK_TRUE, ETK_TRUE, 0, ETK_FALSE);
   etk_dialog_button_add(ETK_DIALOG(dialog), "Open", 1);
   etk_dialog_button_add(ETK_DIALOG(dialog), "Close", 2);
   
   etk_widget_show_all(dialog);
}
