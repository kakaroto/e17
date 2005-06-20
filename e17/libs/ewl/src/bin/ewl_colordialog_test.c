#include "ewl_test.h"

static Ewl_Widget *colordialog_button = NULL;

static void
__destroy_colordialog_test_window(Ewl_Widget *w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(colordialog_button, EWL_CALLBACK_CLICKED,
			    __create_colordialog_test_window, NULL);
}

void
__create_colordialog_test_window(Ewl_Widget *w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	Ewl_Widget *colordialog_win;
	Ewl_Widget *colordialog;

	colordialog_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(colordialog_win), "Colordialog Test");
	ewl_window_name_set(EWL_WINDOW(colordialog_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(colordialog_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
				__create_colordialog_test_window);
		ewl_callback_append(colordialog_win, EWL_CALLBACK_DELETE_WINDOW,
				    __destroy_colordialog_test_window, NULL);
	} else
		ewl_callback_append(colordialog_win, EWL_CALLBACK_DELETE_WINDOW,
				__close_main_window, NULL);
	ewl_widget_show(colordialog_win);

	colordialog = ewl_colordialog_new(0, 0, 0);
	ewl_container_child_append(EWL_CONTAINER(colordialog_win), colordialog);
	ewl_widget_show(colordialog);
}
