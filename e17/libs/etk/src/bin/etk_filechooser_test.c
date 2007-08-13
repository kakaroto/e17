#include "etk_test.h"

Etk_Widget *filechooser;

enum check_btns
{
   IS_SAVE,
   SELECT_MULTI,
   SHOW_HIDDEN
};

void on_checks_clicked(Etk_Button *button, void *data)
{
   Etk_Bool btn_status = etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(button));
   
   switch ((int)data)
   {
      case IS_SAVE:
         etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(filechooser),btn_status);
         break;
      case SELECT_MULTI:
         etk_filechooser_widget_select_multiple_set(ETK_FILECHOOSER_WIDGET(filechooser),btn_status);
         break;
      case SHOW_HIDDEN:
         etk_filechooser_widget_show_hidden_set(ETK_FILECHOOSER_WIDGET(filechooser),btn_status);
         break;
   }
}

/* Creates the window for the file chooser test */
void etk_test_filechooser_window_create(void *data)
{
   static Etk_Widget *dialog = NULL;
   Etk_Widget *is_save_toggle;
   Etk_Widget *select_multiple_toggle;
   Etk_Widget *show_hidden_toggle;

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
   
   is_save_toggle = etk_check_button_new_with_label("Is save");
   etk_dialog_pack_widget_in_action_area(ETK_DIALOG(dialog),
      is_save_toggle, ETK_BOX_START, ETK_BOX_EXPAND_FILL,0);
   etk_signal_connect("toggled", ETK_OBJECT(is_save_toggle), ETK_CALLBACK(on_checks_clicked), (void*)IS_SAVE);
   
   select_multiple_toggle = etk_check_button_new_with_label("Select multiple");
   etk_dialog_pack_widget_in_action_area(ETK_DIALOG(dialog),
      select_multiple_toggle, ETK_BOX_START, ETK_BOX_EXPAND_FILL,0);
   etk_signal_connect("toggled", ETK_OBJECT(select_multiple_toggle), ETK_CALLBACK(on_checks_clicked), (void*)SELECT_MULTI);
   
   show_hidden_toggle = etk_check_button_new_with_label("Show hidden");
   etk_dialog_pack_widget_in_action_area(ETK_DIALOG(dialog),
      show_hidden_toggle, ETK_BOX_START, ETK_BOX_EXPAND_FILL,0);
   etk_signal_connect("toggled", ETK_OBJECT(show_hidden_toggle), ETK_CALLBACK(on_checks_clicked), (void*)SHOW_HIDDEN);
   
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DOCUMENT_OPEN, 1);
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_CLOSE, 2);
   etk_dialog_action_area_alignment_set(ETK_DIALOG(dialog), 0.5);
   
   etk_widget_show_all(dialog);
}
