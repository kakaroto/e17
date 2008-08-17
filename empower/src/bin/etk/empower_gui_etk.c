#include "Empower.h"

void display_window()
{
   Etk_Widget *button_ok;
   Etk_Widget *button_cancel;
   Etk_Size size;

   etk_init(NULL, NULL);
   
   em->dialog = etk_dialog_new();
   em->hbox = etk_hbox_new(ETK_FALSE, 0);
   em->label = etk_label_new("Password:");
   em->entry = etk_entry_new();
   em->icon = etk_image_new_from_stock(ETK_STOCK_SYSTEM_LOCK_SCREEN, ETK_STOCK_BIG);
   
   etk_window_title_set(ETK_WINDOW(em->dialog), "Empower");
   etk_window_wmclass_set(ETK_WINDOW(em->dialog), "Empower", "Empower");
   etk_signal_connect("destroyed", ETK_OBJECT(em->dialog), ETK_CALLBACK(_em_dialog_delete_event_cb), em);
   etk_signal_connect("size-requested", ETK_OBJECT(em->dialog), ETK_CALLBACK(_em_dialog_size_request_cb), em);
   etk_signal_connect("response", ETK_OBJECT(em->dialog), ETK_CALLBACK(_em_dialog_response_cb), em);
   
   etk_signal_connect("key-down", ETK_OBJECT(em->entry), ETK_CALLBACK(_em_entry_key_down), em);
   etk_entry_password_mode_set(ETK_ENTRY(em->entry), ETK_TRUE);
   
   etk_dialog_pack_in_main_area(ETK_DIALOG(em->dialog), em->hbox, ETK_BOX_START, ETK_BOX_NONE, 4);
   etk_box_append(ETK_BOX(em->hbox), em->icon, ETK_BOX_START, ETK_BOX_NONE, 3);
   etk_box_append(ETK_BOX(em->hbox), em->label, ETK_BOX_START, ETK_BOX_NONE, 2);   
   etk_box_append(ETK_BOX(em->hbox), em->entry, ETK_BOX_START, ETK_BOX_NONE, 2);

   button_cancel = etk_dialog_button_add_from_stock(ETK_DIALOG(em->dialog), ETK_STOCK_DIALOG_CANCEL, 0);
   button_ok = etk_dialog_button_add_from_stock(ETK_DIALOG(em->dialog), ETK_STOCK_DIALOG_OK, 1);
   
   etk_widget_show_all(em->dialog);
   
   etk_widget_size_request(button_cancel, &size);
   etk_widget_size_request_set(button_ok, size.w, size.h);
}
