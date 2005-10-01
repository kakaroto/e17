#include "etk_test.h"
#include "../../config.h"

static Etk_Bool _etk_test_table_window_deleted_cb (void *data)
{
   Etk_Window *win = data;
   etk_widget_hide (ETK_WIDGET(win));
   return 1;
}

void etk_test_table_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *widget[21];
   Etk_Widget *vbox, *hbox, *table;
   Etk_Widget *image;

	if (win)
	{
		etk_widget_show_all(ETK_WIDGET(win));
		return;
	}	

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Table Test");

   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_table_window_deleted_cb), win);
	
   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/open.png");
   widget[0] = etk_button_new_with_label("Set Icon");
   etk_button_image_set(ETK_BUTTON(widget[0]), ETK_IMAGE(image));

   widget[20] = etk_alignment_new(0.5, 0.5, 0.0, 0.0);
   etk_container_add(ETK_CONTAINER(widget[20]), widget[0]);

   widget[1] = etk_label_new("App name");
   widget[2] = etk_entry_new();

   widget[3] = etk_label_new("Generic Info");
   widget[4] = etk_entry_new();

   widget[5] = etk_label_new("Comments");
   widget[6] = etk_entry_new();

   widget[7] = etk_label_new("Executable");
   widget[8] = etk_entry_new();

   widget[9] = etk_label_new("Window name");
   widget[10] = etk_entry_new();

   widget[11] = etk_label_new("Window class");
   widget[12] = etk_entry_new();

   widget[13] = etk_label_new("Startup notify");
   widget[14] = etk_check_button_new();

   widget[15] = etk_label_new("Wait exit");
   widget[16] = etk_check_button_new();

   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/close.png");
   widget[17] = etk_button_new_with_label("Cancel");
   etk_button_image_set(ETK_BUTTON(widget[17]), ETK_IMAGE(image));

   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/save.png");
   widget[18] = etk_button_new_with_label("Save");
   etk_button_image_set(ETK_BUTTON(widget[18]), ETK_IMAGE(image));
   
   widget[19] = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/test.png");

   vbox = etk_vbox_new(FALSE, 0);
   hbox = etk_hbox_new(FALSE, 0);
   table = etk_table_new(2, 10, FALSE);

   etk_container_add(ETK_CONTAINER(win), vbox);

   etk_box_pack_start(ETK_BOX(vbox), table, FALSE, FALSE, 0);
   etk_box_pack_end(ETK_BOX(vbox), hbox, FALSE, FALSE, 0);
   etk_box_pack_end(ETK_BOX(hbox), widget[18], FALSE, FALSE, 0);
   etk_box_pack_end(ETK_BOX(hbox), widget[17], FALSE, FALSE, 0);

   etk_table_attach(ETK_TABLE(table), widget[19], 0, 0, 0, 0, 0, 0, ETK_FILL_POLICY_NONE);
   etk_table_attach(ETK_TABLE(table), widget[20], 1, 1, 0, 0, 0, 0, ETK_FILL_POLICY_HEXPAND | ETK_FILL_POLICY_HFILL);
   etk_table_attach(ETK_TABLE(table), widget[1], 0, 0, 2, 2, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[2], 1, 1, 2, 2);
   etk_table_attach(ETK_TABLE(table), widget[3], 0, 0, 3, 3, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[4], 1, 1, 3, 3);
   etk_table_attach(ETK_TABLE(table), widget[5], 0, 0, 4, 4, 0, 0, ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VEXPAND);
   etk_table_attach_defaults(ETK_TABLE(table), widget[6], 1, 1, 4, 4);
   etk_table_attach(ETK_TABLE(table), widget[7], 0, 0, 5, 5, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[8], 1, 1, 5, 5);
   etk_table_attach(ETK_TABLE(table), widget[9], 0, 0, 6, 6, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[10], 1, 1, 6, 6);
   etk_table_attach(ETK_TABLE(table), widget[11], 0, 0, 7, 7, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[12], 1, 1, 7, 7);
   etk_table_attach(ETK_TABLE(table), widget[13], 0, 0, 8, 8, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[14], 1, 1, 8, 8);
   etk_table_attach(ETK_TABLE(table), widget[15], 0, 0, 9, 9, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[16], 1, 1, 9, 9);

   etk_widget_show_all(win);
}
