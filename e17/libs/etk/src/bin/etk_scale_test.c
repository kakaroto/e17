#include "etk_test.h"

static Etk_Bool _etk_test_scale_window_deleted_cb(void *data)
{
   etk_widget_hide(ETK_WIDGET(data));
   return 1;
}

static void _etk_test_scale_value_changed(Etk_Object *object, double value, void *data)
{
   char string[256];

   snprintf(string, 255, "%'.2f", value);
   etk_label_set(ETK_LABEL(data), string);
}

void etk_test_scale_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *table;
   Etk_Widget *scale;
   Etk_Widget *label;
   
	if (win)
	{
		etk_widget_show(ETK_WIDGET(win));
		return;
	}
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Scale test"));
   
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_scale_window_deleted_cb), win);	
	
   table = etk_table_new(2, 2, FALSE);
   etk_container_add(ETK_CONTAINER(win), table);
   
   scale = etk_hscale_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_widget_size_request_set(scale, 130, 130);
   etk_table_attach_defaults(ETK_TABLE(table), scale, 0, 0, 0, 0);
   
   label = etk_label_new("128.00");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1, 0, 0, ETK_FILL_POLICY_NONE);
   etk_signal_connect("value_changed", ETK_OBJECT(scale), ETK_CALLBACK(_etk_test_scale_value_changed), label);
   
   scale = etk_vscale_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_widget_size_request_set(scale, 130, 130);
   etk_table_attach_defaults(ETK_TABLE(table), scale, 1, 1, 0, 0);

   label = etk_label_new("128.00");
   etk_table_attach(ETK_TABLE(table), label, 1, 1, 1, 1, 0, 0, ETK_FILL_POLICY_NONE);
   etk_signal_connect("value_changed", ETK_OBJECT(scale), ETK_CALLBACK(_etk_test_scale_value_changed), label);
   
   etk_widget_show_all(win);
}
