#include <Ewl.h>

static Ewl_Widget *fx_button = NULL;

void __create_fx_test_window(Ewl_Widget * w, void *ev_data, void *user_data);

void
__destroy_fx_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy_recursive(w);

	ewl_callback_append(fx_button, EWL_CALLBACK_CLICKED,
			    __create_fx_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_fx_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *fx_win;
	Ewl_Widget *test_button[5];

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_fx_test_window);

	fx_button = w;

	fx_win = ewl_window_new();
	ewl_window_resize(fx_win, 145, 230);
	ewl_window_set_min_size(fx_win, 145, 230);
	ewl_box_set_spacing(EWL_BOX(fx_win), 10);
	ewl_callback_append(fx_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_fx_test_window, NULL);
	ewl_widget_show(fx_win);

	test_button[0] = ewl_button_new("fade_in");
	ewl_object_set_custom_size(EWL_OBJECT(test_button[0]), 100, 20);
	ewl_object_set_padding(EWL_OBJECT(test_button[0]), 10, 0, 10, 0);
	ewl_container_append_child(EWL_CONTAINER(fx_win), test_button[0]);
	ewl_fx_add_all(test_button[0], "fade_in",
		       EWL_CALLBACK_SHOW, EWL_CALLBACK_HIDE);
	ewl_widget_show(test_button[0]);

	test_button[1] = ewl_button_new("fade_out");
	ewl_object_set_custom_size(EWL_OBJECT(test_button[1]), 100, 20);
	ewl_object_set_padding(EWL_OBJECT(test_button[1]), 10, 0, 10, 0);
	ewl_container_append_child(EWL_CONTAINER(fx_win), test_button[1]);
	ewl_fx_add_all(test_button[1], "fade_out",
		       EWL_CALLBACK_SHOW, EWL_CALLBACK_HIDE);
	ewl_widget_show(test_button[1]);

	test_button[2] = ewl_button_new("glow");
	ewl_object_set_custom_size(EWL_OBJECT(test_button[2]), 100, 20);
	ewl_object_set_padding(EWL_OBJECT(test_button[2]), 10, 0, 10, 0);
	ewl_container_append_child(EWL_CONTAINER(fx_win), test_button[2]);
	ewl_fx_add_all(test_button[2], "glow",
		       EWL_CALLBACK_SHOW, EWL_CALLBACK_HIDE);
	ewl_widget_show(test_button[2]);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
