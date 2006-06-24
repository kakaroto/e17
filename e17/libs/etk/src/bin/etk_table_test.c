#include "etk_test.h"
#include "config.h"

/* Creates the window for the table test */
void etk_test_table_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox, *hbox;
   Etk_Widget *table;
   Etk_Widget *image;
   Etk_Widget *alignment;
   Etk_Widget *buttons[5];
   Etk_Widget *labels[8];
   Etk_Widget *entries[6];
   int i;
   
   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Table Test");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   
   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/test.png");
   
   buttons[0] = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_OPEN);
   buttons[1] = etk_check_button_new();
   buttons[2] = etk_check_button_new();
   buttons[3] = etk_button_new_from_stock(ETK_STOCK_DIALOG_CANCEL);
   buttons[4] = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE);
   
   alignment = etk_alignment_new(0.5, 0.5, 0.0, 0.0);
   etk_container_add(ETK_CONTAINER(alignment), buttons[0]);
   
   labels[0] = etk_label_new("App Name");
   labels[1] = etk_label_new("Generic Info");
   labels[2] = etk_label_new("Comments");
   labels[3] = etk_label_new("Executable");
   labels[4] = etk_label_new("Window Name");
   labels[5] = etk_label_new("Window Class");
   labels[6] = etk_label_new("Startup Notify");
   labels[7] = etk_label_new("Wait Exit");
   
   for (i = 0; i < 6; i++)
      entries[i] = etk_entry_new();
   

   table = etk_table_new(2, 10, ETK_FALSE);
   etk_table_attach(ETK_TABLE(table), image, 0, 0, 0, 0, 0, 0, ETK_FILL_POLICY_NONE);
   etk_table_attach(ETK_TABLE(table), alignment, 1, 1, 0, 0, 0, 0, ETK_FILL_POLICY_HEXPAND | ETK_FILL_POLICY_HFILL);
   
   for (i = 0; i < 6; i++)
   {
      etk_table_attach(ETK_TABLE(table), labels[i], 0, 0, 2 + i, 2 + i, 0, 0, ETK_FILL_POLICY_HFILL);
      etk_table_attach_defaults(ETK_TABLE(table), entries[i], 1, 1, 2 + i, 2 + i);
   }
   
   etk_table_attach(ETK_TABLE(table), labels[6], 0, 0, 8, 8, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), buttons[1], 1, 1, 8, 8);
   etk_table_attach(ETK_TABLE(table), labels[7], 0, 0, 9, 9, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), buttons[2], 1, 1, 9, 9);


   vbox = etk_vbox_new(ETK_FALSE, 0);
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);

   etk_box_pack_start(ETK_BOX(vbox), table, ETK_FALSE, ETK_FALSE, 0);
   etk_box_pack_end(ETK_BOX(vbox), hbox, ETK_FALSE, ETK_FALSE, 0);
   etk_box_pack_end(ETK_BOX(hbox), buttons[3], ETK_FALSE, ETK_FALSE, 0);
   etk_box_pack_end(ETK_BOX(hbox), buttons[4], ETK_FALSE, ETK_FALSE, 0);

   etk_widget_show_all(win);
}
