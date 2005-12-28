#include "ewl_test.h"

static Ewl_Widget *paned_button;

static void
ewl_paned_test_cb_clicked(Ewl_Widget *w, void *ev __UNUSED__,
							void *data)
{
	printf("Clicked %s\n", (char *)data);

	ewl_widget_hide(w);
}

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
	ewl_object_size_request(EWL_OBJECT(paned_win), 400, 600);

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

	pane2 = ewl_hpaned_new();
	ewl_container_child_append(EWL_CONTAINER(pane1), pane2);
	ewl_widget_show(pane2);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Fill");
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(pane2), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
			ewl_paned_test_cb_clicked, "Fill");
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "HFill | VShrink");
	ewl_object_fill_policy_set(EWL_OBJECT(o), 
			EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);
	ewl_container_child_append(EWL_CONTAINER(pane2), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
			ewl_paned_test_cb_clicked, "HFill | VShrink");
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "HShrink | VFill");
	ewl_object_fill_policy_set(EWL_OBJECT(o), 
			EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VFILL);
	ewl_container_child_append(EWL_CONTAINER(pane2), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
			ewl_paned_test_cb_clicked, "HShrink | VFill");
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "shrink");
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(pane2), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
			ewl_paned_test_cb_clicked, "Shrink");
	ewl_widget_show(o);

	pane2 = ewl_hpaned_new();
	ewl_container_child_append(EWL_CONTAINER(pane1), pane2);
	ewl_widget_show(pane2);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Left Top");
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(o), 
				EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);
	ewl_container_child_append(EWL_CONTAINER(pane2), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
			ewl_paned_test_cb_clicked, "Left Top");
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Center");
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(pane2), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
			ewl_paned_test_cb_clicked, "Center");
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Right Bottom");
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(o), 
			EWL_FLAG_ALIGN_RIGHT | EWL_FLAG_ALIGN_BOTTOM);
	ewl_container_child_append(EWL_CONTAINER(pane2), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
			ewl_paned_test_cb_clicked, "Right Bottom");
	ewl_widget_show(o);
}


