#include "etk_test.h"
#include "../../config.h"

static Etk_Bool _etk_test_canvas_window_deleted_cb (void *data)
{
   Etk_Window *win = data;
   etk_widget_hide (ETK_WIDGET(win));
   return 1;
}

void etk_test_colorpicker_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *cp;

	if (win)
	{
		etk_widget_show_all(ETK_WIDGET(win));
		return;
	}
	

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Color Picker");

   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_canvas_window_deleted_cb), win);	
	
   cp = etk_colorpicker_new();
   etk_container_add(ETK_CONTAINER(win), cp);
   
   etk_widget_show_all(win);
}
