#include <Ewl.h>

static Ewl_Widget *seeker_button;

void __create_seeker_test_window(Ewl_Widget * w, void *ev_data,
				 void *user_data);


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
__create_seeker_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *seeker_win;
	Ewl_Widget *hseeker, *vseeker;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_seeker_test_window);

	seeker_button = w;

	seeker_win = ewl_window_new();
	ewl_callback_append(seeker_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_seeker_test_window, NULL);
	ewl_widget_show(seeker_win);

	hseeker = ewl_hseeker_new();
	ewl_object_set_padding(EWL_OBJECT(hseeker), 10, 10, 10, 0);
	ewl_container_append_child(EWL_CONTAINER(seeker_win), hseeker);
	ewl_widget_show(hseeker);

	vseeker = ewl_vseeker_new();
	ewl_object_set_padding(EWL_OBJECT(vseeker), 10, 10, 10, 10);
	ewl_container_append_child(EWL_CONTAINER(seeker_win), vseeker);
	ewl_widget_show(vseeker);

	return;
	ev_data = NULL;
	user_data = NULL;
}
