#include <Ewl.h>

static char *long_text = "This is a long string to test\n"
			"the effectiveness of\n"
			"the new text area.\n"
			"It's possible that it contains\n"
			"unprintable characters such\n"
			"as  or extremely long\n"
			"lines. Beware, it may\n"
			"trigger bugs in etox or\n"
			"ewl\nso\ntrace\ncarefully. Now it's time for some "
			"gibberish to make this thing longer and to test the "
			"scrollbars. There's probably not enough text here\n"
			"to really push etox, but hopefully it will give a "
			"better idea of the speed\n"
			"Right now selection of text and click to position\n"
			"need quite a bit of work, as well as keyboard\n"
			"navigation and focus control\n";

static Ewl_Widget *textarea_button;

void __create_textarea_test_window(Ewl_Widget * w, void *ev_data,
				   void *user_data);


void
__destroy_textarea_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy_recursive(w);

	ewl_callback_append(textarea_button, EWL_CALLBACK_CLICKED,
			    __create_textarea_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_textarea_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *textarea_win;
	Ewl_Widget *textarea;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_textarea_test_window);

	textarea_button = w;

	textarea_win = ewl_window_new();
	ewl_box_set_spacing(EWL_BOX(textarea_win), 10);
	ewl_callback_append(textarea_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_textarea_test_window, NULL);
	ewl_widget_show(textarea_win);

	textarea = ewl_textarea_new();
	ewl_textarea_set_text(EWL_TEXTAREA(textarea), long_text);
	ewl_object_set_padding(EWL_OBJECT(textarea), 10, 10, 10, 10);
	ewl_container_append_child(EWL_CONTAINER(textarea_win), textarea);
	ewl_widget_show(textarea);

	return;
	ev_data = NULL;
	user_data = NULL;
}
