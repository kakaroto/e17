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
   etk_signal_connect("delete-event", ETK_OBJECT(dialog), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   filechooser = etk_filechooser_widget_new();
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), filechooser, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DOCUMENT_OPEN, 1);
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_CLOSE, 2);
   etk_dialog_action_area_alignment_set(ETK_DIALOG(dialog), 1.0);
   
   etk_widget_show_all(dialog);
}
