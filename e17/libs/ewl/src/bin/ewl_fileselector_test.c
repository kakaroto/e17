#include "ewl_test.h"

static Ewl_Widget *fs_button = NULL;

static void
__destroy_fileselector_test_window(Ewl_Widget *w, void *ev_data __UNUSED__,
				   void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(fs_button, EWL_CALLBACK_CLICKED,
			    __create_fileselector_test_window, NULL);
}

void
__create_fileselector_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
				  void *user_data __UNUSED__)
{
	Ewl_Widget     *fs;
	Ewl_Widget     *fs_win;

	fs_button = w;

	fs_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(fs_win), "File Selector Test");
	ewl_window_name_set(EWL_WINDOW(fs_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(fs_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
				__create_fileselector_test_window);
		ewl_callback_append(fs_win, EWL_CALLBACK_DELETE_WINDOW,
				__destroy_fileselector_test_window, NULL);
	} else
		ewl_callback_append(fs_win, EWL_CALLBACK_DELETE_WINDOW,
						__close_main_window, NULL);
	ewl_widget_show(fs_win);

	fs = ewl_fileselector_new();
	ewl_object_fill_policy_set(EWL_OBJECT(fs), EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(fs_win), fs);
	ewl_widget_show(fs);
}
