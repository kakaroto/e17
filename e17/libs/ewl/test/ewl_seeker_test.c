#include "ewl_test.h"

static Ewl_Widget *seeker_button;

void
__destroy_seeker_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(seeker_button, EWL_CALLBACK_CLICKED,
			    __create_seeker_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__print_value(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Seeker *s;

	s = EWL_SEEKER(w);

	printf("Seeker set to %g\n", ewl_seeker_get_value(s));
}

void
__create_seeker_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *seeker_win;
	Ewl_Widget *hseeker, *vseeker;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_seeker_test_window);

	seeker_button = w;

	seeker_win = ewl_window_new();
	ewl_object_set_minimum_size(EWL_OBJECT(seeker_win), 300, 300);
	ewl_callback_append(seeker_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_seeker_test_window, NULL);
	ewl_widget_show(seeker_win);

	hseeker = ewl_hseeker_new();
	ewl_object_request_position(EWL_OBJECT(hseeker), 30, 0);
	ewl_callback_append(hseeker, EWL_CALLBACK_VALUE_CHANGED, __print_value,
			NULL);
	ewl_container_append_child(EWL_CONTAINER(seeker_win), hseeker);
	ewl_widget_show(hseeker);

	vseeker = ewl_vseeker_new();
	ewl_object_request_position(EWL_OBJECT(vseeker), 0, 30);
	ewl_callback_append(vseeker, EWL_CALLBACK_VALUE_CHANGED, __print_value,
			NULL);
	ewl_container_append_child(EWL_CONTAINER(seeker_win), vseeker);
	ewl_widget_show(vseeker);

	return;
	ev_data = NULL;
	user_data = NULL;
}
