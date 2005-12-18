#include "ewl_test.h"

static Ewl_Widget *paned_button;

static void
__destroy_paned_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(paned_button, EWL_CALLBACK_CLICKED,
			    __create_paned_test_window, NULL);
}

void
__create_paned_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Widget *paned_win, *box, *pane1, *pane2, *o;

	paned_button = w;

	paned_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(paned_win), "Paned Test");
	ewl_window_name_set(EWL_WINDOW(paned_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(paned_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
					__create_paned_test_window);
		ewl_callback_append(paned_win, EWL_CALLBACK_DELETE_WINDOW,
				    __destroy_paned_test_window, NULL);
	} else
		ewl_callback_append(paned_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(paned_win);

	/*
	 * Create the main box for holding the widgets
	 */
	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(paned_win), box);
	ewl_widget_show(box);

	pane1 = ewl_vpaned_new();
	ewl_container_child_append(EWL_CONTAINER(box), pane1);
	ewl_widget_show(pane1);

	ewl_paned_active_area_set(EWL_PANED(pane1), EWL_POSITION_TOP);

	pane2 = ewl_hpaned_new();
	ewl_container_child_append(EWL_CONTAINER(pane1), pane2);
	ewl_widget_show(pane2);

	ewl_paned_active_area_set(EWL_PANED(pane2), EWL_POSITION_LEFT);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "left");
	ewl_container_child_append(EWL_CONTAINER(pane2), o);
	ewl_widget_show(o);

	ewl_paned_active_area_set(EWL_PANED(pane2), EWL_POSITION_RIGHT);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "right");
	ewl_container_child_append(EWL_CONTAINER(pane2), o);
	ewl_widget_show(o);

	ewl_paned_active_area_set(EWL_PANED(pane1), EWL_POSITION_BOTTOM);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "bottom");
	ewl_container_child_append(EWL_CONTAINER(pane1), o);
	ewl_widget_show(o);
}


