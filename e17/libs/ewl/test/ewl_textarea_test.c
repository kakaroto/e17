#include "ewl_test.h"

#define FONTSIZE 12

static char    *long_text =
	"This is a long string to test\n"
	"the effectiveness of\n"
	"the new text area.\n"
	"It's possible that it contains\n"
	"unprintable characters such\n"
	"as \t or extremely long\n"
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
	"\n";

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
__font_get(Ewl_Widget *w, void *ev_data, void *user_data)
{
	int size;
	Ewl_Widget *spinner = user_data;

	size = ewl_entry_font_size_get(EWL_ENTRY(w));
	ewl_spinner_value_set(EWL_SPINNER(spinner), (double)(size));

	return;
	ev_data = NULL;
}

void
__font_set(Ewl_Widget *w, void *ev_data, void *user_data)
{
	double size;
	char *font;
	Ewl_Widget *textarea = user_data;

	font = ewl_entry_font_get(EWL_ENTRY(textarea));
	size = ewl_spinner_value_get(EWL_SPINNER(w));
	ewl_entry_font_set(EWL_ENTRY(textarea), font, (int)(size));

	return;
	ev_data = NULL;
}

void
__create_textarea_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *button_box;
	Ewl_Widget     *scrollpane;
	Ewl_Widget     *spinner;
	Ewl_Widget     *textarea_win;
	Ewl_Widget     *textarea;
	Ewl_Widget     *tmp;
	Ewl_Widget     *vbox;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_textarea_test_window);

	textarea_button = w;

	/*
	 * Setup a window displaying title and class information, also sets up
	 * callbacks for closing the window and a sane default size.
	 */
	textarea_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(textarea_win), "Textarea Test");
	ewl_window_name_set(EWL_WINDOW(textarea_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(textarea_win), "EFL Test Application");
	ewl_object_fill_policy_set(EWL_OBJECT(textarea_win), EWL_FLAG_FILL_ALL);
	ewl_object_size_request(EWL_OBJECT(textarea_win), 200, 150);
	ewl_callback_append(textarea_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_textarea_test_window, NULL);
	ewl_widget_show(textarea_win);

	vbox = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
	ewl_box_homogeneous_set(EWL_BOX(vbox), FALSE);
	ewl_container_child_append(EWL_CONTAINER(textarea_win), vbox);
	ewl_widget_show(vbox);

	/*
	 * Setup a series of buttons for changing properties on the text.
	 */
	button_box = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(button_box), EWL_FLAG_FILL_NONE);
	ewl_box_homogeneous_set(EWL_BOX(button_box), FALSE);
	ewl_container_child_append(EWL_CONTAINER(vbox), button_box);
	ewl_widget_show(button_box);

	tmp = ewl_text_new("Font size:");
	ewl_container_child_append(EWL_CONTAINER(button_box), tmp);
	ewl_object_alignment_set(EWL_OBJECT(tmp), EWL_FLAG_ALIGN_CENTER);
	ewl_widget_show(tmp);

	spinner= ewl_spinner_new();
	ewl_container_child_append(EWL_CONTAINER(button_box), spinner);
	ewl_spinner_min_val_set(EWL_SPINNER(spinner), 1);
	ewl_spinner_max_val_set(EWL_SPINNER(spinner), 72);
	ewl_spinner_value_set(EWL_SPINNER(spinner), 12);
	ewl_spinner_step_set(EWL_SPINNER(spinner), 1);
	ewl_spinner_digits_set(EWL_SPINNER(spinner), 0);
	ewl_widget_show(spinner);

	/*
	tmp = ewl_button_new("Moo");
	ewl_container_child_append(EWL_CONTAINER(vbox), tmp);
	ewl_widget_show(tmp);
	*/

	/*
	 * Create a scrollpane to hold the text.
	 */
	scrollpane = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), scrollpane);
	ewl_widget_show(scrollpane);

	/*
	 * Create the textarea to be displayed.
	 */
	textarea = ewl_entry_new(NULL);
	ewl_container_child_append(EWL_CONTAINER(scrollpane), textarea);
	ewl_object_fill_policy_set(EWL_OBJECT(textarea), EWL_FLAG_FILL_NONE);
	ewl_callback_append(textarea, EWL_CALLBACK_REALIZE, __font_get,
			    spinner);
	ewl_widget_show(textarea);

	ewl_callback_append(spinner, EWL_CALLBACK_VALUE_CHANGED, __font_set,
			    textarea);

	return;
	ev_data = NULL;
	user_data = NULL;
}
