#include <Ewl.h>

static Ewl_Widget *seeker_button;

void __create_seeker_test_window(Ewl_Widget * w, void *ev_data,
				 void *user_data);


void
__destroy_seeker_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy_recursive(w);

	ewl_callback_append(seeker_button, EWL_CALLBACK_CLICKED,
			    __create_seeker_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_seeker_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *seeker_win;
	Ewl_Widget *seeker;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_seeker_test_window);

	seeker_button = w;

	seeker_win = ewl_window_new();
	ewl_box_set_spacing(EWL_BOX(seeker_win), 10);
	ewl_callback_append(seeker_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_seeker_test_window, NULL);
	ewl_widget_show(seeker_win);

	seeker = ewl_hseeker_new();
	ewl_container_append_child(EWL_CONTAINER(seeker_win), seeker);
	ewl_widget_show(seeker);

	return;
	ev_data = NULL;
	user_data = NULL;
}
