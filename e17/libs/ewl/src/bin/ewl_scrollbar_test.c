#include "ewl_test.h"

static Ewl_Widget *scrollbar_button;

static void
__destroy_scrollbar_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(scrollbar_button, EWL_CALLBACK_CLICKED,
			    __create_scrollbar_test_window, NULL);
}

void
__create_scrollbar_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Widget *scrollbar_win;
	Ewl_Widget *hscrollbar, *vscrollbar;

	scrollbar_button = w;

	scrollbar_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(scrollbar_win), "Scrollbar Test");
	ewl_window_name_set(EWL_WINDOW(scrollbar_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(scrollbar_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED,
				 __create_scrollbar_test_window);
		ewl_callback_append(scrollbar_win, EWL_CALLBACK_DELETE_WINDOW,
				    __destroy_scrollbar_test_window, NULL);
	} else
		ewl_callback_append(scrollbar_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_win, NULL);
	ewl_widget_show(scrollbar_win);

	hscrollbar = ewl_hscrollbar_new();
	ewl_object_set_padding(EWL_OBJECT(hscrollbar), 10, 10, 10, 0);
	ewl_container_child_append(EWL_CONTAINER(scrollbar_win), hscrollbar);
	ewl_widget_show(hscrollbar);

	vscrollbar = ewl_vscrollbar_new();
	ewl_object_set_padding(EWL_OBJECT(vscrollbar), 10, 10, 10, 10);
	ewl_container_child_append(EWL_CONTAINER(scrollbar_win), vscrollbar);
	ewl_widget_show(vscrollbar);
}

