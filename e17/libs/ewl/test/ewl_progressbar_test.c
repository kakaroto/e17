#include "ewl_test.h"

static Ewl_Widget *progressbar_button = NULL;
static Ecore_Timer *progress_timer = NULL;

int __increment_progress(Ewl_Widget *progressbar)
{
	double val;
	double value, range;
	char c[30];

	val = ewl_progressbar_get_value(EWL_PROGRESSBAR(progressbar));
	if (val >= p->range) {
		ecore_timer_del(progress_timer);
		progress_timer = NULL;
		return 0;
	}

	val += 1;
	ewl_progressbar_set_value(EWL_PROGRESSBAR(progressbar), val);


	if (val >= 20 && val < 35 ) {
		ewl_progressbar_set_custom_label (EWL_PROGRESSBAR(progressbar),
				"%.0lf / %.0lf kbytes");
	}

	if (val >= 35 && val < 60) {
		value = ewl_progressbar_get_value (EWL_PROGRESSBAR(progressbar));
		range = ewl_progressbar_get_range (EWL_PROGRESSBAR(progressbar));

		snprintf (c, sizeof (c), "%.0lf of %.0lf beers", value, range);
		ewl_progressbar_set_label (EWL_PROGRESSBAR(progressbar), c);
	}

	if (val == 60) 
		ewl_progressbar_label_hide (EWL_PROGRESSBAR(progressbar));

	if (val == 70)
		ewl_progressbar_label_show (EWL_PROGRESSBAR(progressbar));

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
	ewl_box_set_spacing(EWL_BOX(progressbar_box), 0);
	ewl_widget_show(progressbar_box);

	progressbar = ewl_progressbar_new();
	ewl_progressbar_set_value (EWL_PROGRESSBAR(progressbar), 0);
	ewl_widget_show (progressbar);

	ewl_progressbar_set_range (EWL_PROGRESSBAR(progressbar), 123);

	progress_timer = ecore_timer_add(0.1, __increment_progress,
					 progressbar);

	ewl_container_append_child(EWL_CONTAINER(progressbar_box), progressbar);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
