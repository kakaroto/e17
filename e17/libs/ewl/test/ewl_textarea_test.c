#include "ewl_test.h"

static char    *long_text =
	"This is a long string to test\n"
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
	"navigation and focus control\n"
	"We really need to get some better text in here at\n"
	"some point, since this is pretty lame\n"
	"but we need long strings to test it effectively.\n"
	"I have heard a couple reports of major slowdowns\n"
	"when displaying large text regions. This message will "
	"now be repeated to do a little stress testing.\n"
	"\n"
	"\n"
	"This is a long string to test\n"
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
	"navigation and focus control\n"
	"We really need to get some better text in here at\n"
	"some point, since this is pretty lame\n"
	"but we need long strings to test it effectively.\n"
	"I have heard a couple reports of major slowdowns\n"
	"when displaying large text regions. This message will "
	"now be repeated to do a little stress testing.";

static Ewl_Widget *textarea_button;

void
__destroy_textarea_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(textarea_button, EWL_CALLBACK_CLICKED,
			    __create_textarea_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_textarea_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *scrollpane;
	Ewl_Widget     *textarea_win;
	Ewl_Widget     *textarea;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_textarea_test_window);

	textarea_button = w;

	textarea_win = ewl_window_new();
	ewl_window_set_title(EWL_WINDOW(textarea_win), "Textarea Test");
	ewl_window_set_name(EWL_WINDOW(textarea_win), "EWL Test Application");
	ewl_window_set_class(EWL_WINDOW(textarea_win), "EFL Test Application");
	ewl_object_set_fill_policy(EWL_OBJECT(textarea_win), EWL_FLAG_FILL_ALL);
	ewl_object_request_size(EWL_OBJECT(textarea_win), 100, 100);
	ewl_callback_append(textarea_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_textarea_test_window, NULL);
	ewl_widget_show(textarea_win);

	/*
	 * Create a scrollpane to hold the text.
	 */
	scrollpane = ewl_scrollpane_new();
	ewl_container_append_child(EWL_CONTAINER(textarea_win), scrollpane);
	ewl_widget_show(scrollpane);

	/*
	 * Create the textarea to be displayed.
	 */
	textarea = ewl_textarea_new(long_text);
	ewl_container_append_child(EWL_CONTAINER(scrollpane), textarea);
	ewl_widget_show(textarea);

	return;
	ev_data = NULL;
	user_data = NULL;
}
