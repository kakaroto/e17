#include "etk_test.h"
#include "../../config.h"

static Etk_Bool _etk_test_image_window_deleted_cb (void *data)
{
   Etk_Window *win = data;
   etk_widget_hide (ETK_WIDGET(win));
   return 1;
}

void etk_test_image_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *images[2];
   Etk_Widget *labels[2];
   Etk_Widget *table;

	if (win)
	{
		etk_widget_show_all(ETK_WIDGET(win));
		return;
	}
	
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Image Test"));

   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_image_window_deleted_cb), win);	
	
   images[0] = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/test.png");
   etk_image_keep_aspect_set(ETK_IMAGE(images[0]), TRUE);
   images[1] = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/test.png");
   etk_image_keep_aspect_set(ETK_IMAGE(images[1]), FALSE);

   labels[0] = etk_label_new(_("Keep aspect"));
   labels[1] = etk_label_new(_("Don't keep aspect"));
   table = etk_table_new(2, 2, FALSE);

   etk_table_attach_defaults(ETK_TABLE(table), images[0], 0, 0, 0, 0);
   etk_table_attach_defaults(ETK_TABLE(table), images[1], 1, 1, 0, 0);
   etk_table_attach(ETK_TABLE(table), labels[0], 0, 0, 1, 1, 2, 0, ETK_FILL_POLICY_HEXPAND);
   etk_table_attach(ETK_TABLE(table), labels[1], 1, 1, 1, 1, 2, 0, ETK_FILL_POLICY_HEXPAND);

   etk_container_add(ETK_CONTAINER(win), table);

   etk_widget_show_all(win);
}
