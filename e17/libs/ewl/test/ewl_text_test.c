#include <Ewl.h>

static Ewl_Widget *text_button;

void            __create_text_test_window(Ewl_Widget * w, void *ev_data,
					  void *user_data);


void
__destroy_text_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(text_button, EWL_CALLBACK_CLICKED,
			    __create_text_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_text_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *text_win;
	Ewl_Widget     *text_box;
	Ewl_Widget     *text;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_text_test_window);

	text_button = w;

	text_win = ewl_window_new();
	ewl_box_set_spacing(EWL_BOX(text_win), 10);
	ewl_callback_append(text_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_text_test_window, NULL);
	ewl_widget_show(text_win);

	/*
	 * Create the main box for holding the widgets
	 */
	text_box = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(text_win), text_box);
	ewl_widget_show(text_box);

	text = ewl_text_new("Enlightenment Widget Library\n"
			  "\n"
			  "Bla bla bla bla bla bla bla bla bla\n"
			  "Bla bla bla bla bla bla bla bla bla\n"
			  "Bla bla bla bla bla bla bla bla bla\n"
			  "Bla bla bla bla bla bla bla bla bla\n"
			  "Bla bla bla bla bla bla bla bla bla\n"
			  "Bla bla bla bla bla bla bla bla bla\n"
			  "\n" "            Bla bla bla bla bla bla\n");
	ewl_object_set_alignment(EWL_OBJECT(text), EWL_ALIGNMENT_CENTER);
	ewl_object_set_padding(EWL_OBJECT(text), 0, 0, 20, 0);
	ewl_container_append_child(EWL_CONTAINER(text_box), text);
	ewl_widget_show(text);

	return;
	ev_data = NULL;
	user_data = NULL;
}
