#include "ewl_test.h"

static Ewl_Widget *progressbar_button = NULL;
static Ecore_Timer *progress_timer = NULL;

int __increment_progress(Ewl_Widget *progressbar)
{
	double val;

	val = ewl_progressbar_get_value(EWL_PROGRESSBAR(progressbar));
	if (val >= 1.00) {
		ecore_timer_del(progress_timer);
		progress_timer = NULL;
		return 0;
	}

	val += 0.05;
	ewl_progressbar_set_value(EWL_PROGRESSBAR(progressbar), val);

	return 1;
}

void
__destroy_progressbar_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	if (progress_timer)
		ecore_timer_del(progress_timer);
	ewl_widget_destroy(w);
	ewl_callback_append(progressbar_button, EWL_CALLBACK_CLICKED,
			__create_progressbar_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_progressbar_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *progressbar_win;
	Ewl_Widget     *progressbar_box;
	Ewl_Widget     *progressbar;


	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_progressbar_test_window);

	progressbar_button = w;

	progressbar_win = ewl_window_new();
	ewl_object_request_size(EWL_OBJECT(progressbar_win), 200, 50);
	ewl_callback_append(progressbar_win, EWL_CALLBACK_DELETE_WINDOW,
			__destroy_progressbar_test_window, NULL);
	ewl_widget_show(progressbar_win);

	progressbar_box = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(progressbar_win), 
			progressbar_box);
	ewl_box_set_spacing(EWL_BOX(progressbar_box), 10);
	ewl_widget_show(progressbar_box);

	progressbar = ewl_progressbar_new();
	ewl_progressbar_set_value (EWL_PROGRESSBAR(progressbar), 0.0);
	ewl_widget_show (progressbar);

	progress_timer = ecore_timer_add(0.2, __increment_progress,
					 progressbar);

	ewl_container_append_child(EWL_CONTAINER(progressbar_box), progressbar);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
