#include "ewl_test.h"

static Ewl_Widget *paned_button;

void
__destroy_paned_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(paned_button, EWL_CALLBACK_CLICKED,
			    __create_paned_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_paned_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *paned_win, *box, *pane, *o;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_entry_test_window);

	paned_button = w;

	paned_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(paned_win), "Paned Test");
	ewl_window_name_set(EWL_WINDOW(paned_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(paned_win), "EFL Test Application");
	ewl_callback_append(paned_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_paned_test_window, NULL);
	ewl_widget_show(paned_win);

	/*
	 * Create the main box for holding the widgets
	 */
	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(paned_win), box);
	ewl_widget_show(box);

	pane = ewl_hpaned_new();
	ewl_container_child_append(EWL_CONTAINER(box), pane);
	ewl_widget_show(pane);

	o = ewl_text_new("left");
	ewl_paned_first_pane_append(EWL_PANED(pane), o);
	ewl_widget_show(o);

	o = ewl_text_new("right");
	ewl_paned_second_pane_append(EWL_PANED(pane), o);
	ewl_widget_show(o);

	return;
	ev_data = NULL;
	user_data = NULL;
}


