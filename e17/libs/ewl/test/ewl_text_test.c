#include <Ewl.h>

static Ewl_Widget *text_button;

void __create_text_test_window (Ewl_Widget * w, void *ev_data,
				void *user_data);


void
__destroy_text_test_window (Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy (w);

	ewl_callback_append (text_button, EWL_CALLBACK_CLICKED,
			     __create_text_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_text_test_window (Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *text_win;
	Ewl_Widget *main_vbox;
	Ewl_Widget *text;

	ewl_callback_del (w, EWL_CALLBACK_CLICKED, __create_text_test_window);

	text_button = w;

	text_win = ewl_window_new ();
	ewl_callback_append (text_win, EWL_CALLBACK_DELETE_WINDOW,
			     __destroy_text_test_window, NULL);
	ewl_widget_show (text_win);

	main_vbox = ewl_vbox_new ();
	ewl_container_append_child (EWL_CONTAINER (text_win), main_vbox);
	ewl_box_set_spacing (main_vbox, 10);
	ewl_widget_show (main_vbox);

	text = ewl_text_new ();
	ewl_text_set_text (EWL_TEXT (text),
			   "The development effort for the 0.17"
			   " a lot larger\nthan for the 0.16 series, mainly because Enlightenment"
			   "\nis going to become a desktop shell.\n"
			   "The subsystems created along the way for purposes of graphics\n"
			   " handling, filesystem access etc are organized in standalone\n"
			   "libraries  that can be used by other projects as well.");
	ewl_container_append_child (EWL_CONTAINER (main_vbox), text);
	ewl_widget_show (text);

	return;
	ev_data = NULL;
	user_data = NULL;
}
