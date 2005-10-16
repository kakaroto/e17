#include "etk_test.h"

static Etk_Bool _etk_test_scale_window_deleted_cb(void *data)
{
   etk_widget_hide(ETK_WIDGET(data));
   return 1;
}

void etk_test_scale_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *hbox;
   Etk_Widget *scale;
   
	if (win)
	{
		etk_widget_show(ETK_WIDGET(win));
		return;
	}
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Scale test"));
   
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_scale_window_deleted_cb), win);	
	
   hbox = etk_hbox_new(TRUE, 3);
   etk_container_add(ETK_CONTAINER(win), hbox);
   etk_widget_show(hbox);
   
   scale = etk_widget_new(ETK_BIN_TYPE, "theme_group", "hscale", "focusable", TRUE, NULL);
   etk_widget_size_request_set(scale, 70, -1);
   etk_box_pack_start(ETK_BOX(hbox), scale, TRUE, TRUE, 0);
   etk_widget_show(scale);
   scale = etk_widget_new(ETK_BIN_TYPE, "theme_group", "vscale", "focusable", TRUE, NULL);
   etk_widget_size_request_set(scale, -1, 70);
   etk_box_pack_start(ETK_BOX(hbox), scale, TRUE, TRUE, 0);
   etk_widget_show(scale);
   
   etk_widget_show(win);
}
