#include <Ewl.h>

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

void            __create_textarea_test_window(Ewl_Widget * w, void *ev_data,
					      void *user_data);


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
	Ewl_Widget     *textarea_box;
	Ewl_Widget     *textarea;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_textarea_test_window);

	textarea_button = w;

	textarea_win = ewl_window_new();
	ewl_object_set_fill_policy(EWL_OBJECT(textarea_win),
			EWL_FILL_POLICY_FILL);
	ewl_object_set_minimum_size(EWL_OBJECT(textarea_win), 200, 200);
	ewl_object_set_maximum_size(EWL_OBJECT(textarea_win), 400, 400);
	ewl_callback_append(textarea_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_textarea_test_window, NULL);
	ewl_widget_show(textarea_win);

	/*
	 * Create the main box for holding the widgets
	 */
	textarea_box = ewl_vbox_new();
	ewl_box_set_spacing(EWL_BOX(textarea_box), 10);
	ewl_container_append_child(EWL_CONTAINER(textarea_win), textarea_box);
	ewl_object_set_fill_policy(EWL_OBJECT(textarea_box),
			EWL_FILL_POLICY_FILL | EWL_FILL_POLICY_SHRINK);
	ewl_widget_show(textarea_box);

	/*
	 * Create a scrollpane to hold the text.
	 */
	scrollpane = ewl_scrollpane_new();
	ewl_container_append_child(EWL_CONTAINER(textarea_box), scrollpane);
	ewl_widget_show(scrollpane);

	/*
	 * Create the textarea to be displayed.
	 */
	textarea = ewl_textarea_new(long_text);
	ewl_object_set_padding(EWL_OBJECT(textarea), 10, 10, 10, 10);
	ewl_container_append_child(EWL_CONTAINER(scrollpane), textarea);
	ewl_widget_show(textarea);

	return;
	ev_data = NULL;
	user_data = NULL;
}
