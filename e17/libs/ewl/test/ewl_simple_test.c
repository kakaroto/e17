#include "ewl-config.h"
#include <Ewl.h>

void
entrance_text(void *data, Evas_Object *obj, const char *emission,
		const char *source)
{
	Ewl_Widget *label = data;
	char *text = "\nEntrance [en-'trans], v.\n"
			"  1: to carry away with delight,\n"
			"     wonder, or rapture\n"
			"  2: to put into a trance\n"
			"Entrance is the Enlightenment\n"
			"Display Manager. And like\n"
			"Enlightenment, it takes beauty\n"
			"and customization to new levels\n";

	ewl_text_align_set(EWL_TEXT(label), EWL_FLAG_ALIGN_LEFT);
	ewl_text_font_set(EWL_TEXT(label), "Vera", 12);
	ewl_text_style_set(EWL_TEXT(label), "soft_shadow");
	ewl_text_text_set(EWL_TEXT(label), "Entrance");

	ewl_text_style_set(EWL_TEXT(label), "none");
	ewl_text_font_set(EWL_TEXT(label), "Vera", 7);
	ewl_text_text_append(EWL_TEXT(label), text);
}

void
elicit_text(void *data, Evas_Object *obj, const char *emission,
		const char *source)
{
	Ewl_Widget *label = data;
	char *text = "\nElicit is a tool for examining\n"
		     "images on your desktop,\n"
		     "providing both a global color\n"
		     "picker and a zoom tool.\n"
		     "Graphic artists and designers\n"
		     "can quickly examine graphics\n"
		     "without needed to rely on\n"
		     "larger tools for simple\n"
		     "examinations and color checks.";

	ewl_text_align_set(EWL_TEXT(label), EWL_FLAG_ALIGN_LEFT);
	ewl_text_font_set(EWL_TEXT(label), "Vera", 12);
	ewl_text_style_set(EWL_TEXT(label), "soft_shadow");
	ewl_text_text_set(EWL_TEXT(label), "Elicit");

	ewl_text_style_set(EWL_TEXT(label), "none");
	ewl_text_font_set(EWL_TEXT(label), "Vera", 7);
	ewl_text_text_append(EWL_TEXT(label), text);
}

void start_text(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Widget *label = user_data;

	ewl_object_set_alignment(EWL_OBJECT(label), EWL_FLAG_ALIGN_CENTER);
	ewl_text_align_set(EWL_TEXT(label), EWL_FLAG_ALIGN_CENTER);
	ewl_text_font_set(EWL_TEXT(label), "Vera", 12);
	ewl_text_style_set(EWL_TEXT(label), "soft_shadow");
	ewl_text_text_append(EWL_TEXT(label), "Welcome to Enlightenment!\n");
	ewl_text_style_set(EWL_TEXT(label), "none");
	ewl_text_font_set(EWL_TEXT(label), "Vera", 7);
	ewl_text_text_append(EWL_TEXT(label), "We hope you enjoy your stay.\n"
					      "Please visit us at:\n");
	ewl_text_color_set(EWL_TEXT(label), 0, 0, 255, 190);
	ewl_text_text_append(EWL_TEXT(label), "http://www.enlightenment.org/");
	ewl_text_color_set(EWL_TEXT(label), 0, 0, 0, 255);
}

void realize_logo_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	edje_object_signal_callback_add(w->theme_object, "elicit", "tour",
			elicit_text, user_data);
	edje_object_signal_callback_add(w->theme_object, "entrance", "tour",
			entrance_text, user_data);
}

void test_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	printf("%s at: %d, %d (%d x %d)\n", (char *)user_data, CURRENT_X(w),
			CURRENT_Y(w), CURRENT_W(w), CURRENT_H(w));
}

void button_down(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Widget *logo = user_data;
	ewl_widget_set_state(logo, "start_tour");
}

void quit_demo(Ewl_Widget *w, void *ev_data, void *user_data)
{
	ewl_main_quit();
}

int main(int argc, char **argv)
{
	Ewl_Widget *win;
	Ewl_Widget *vbox;
	Ewl_Widget *hbox;
	Ewl_Widget *button;
	Ewl_Widget *logo;
	Ewl_Widget *label;
	char tmp[PATH_MAX];

	ewl_init(&argc, argv);

	snprintf(tmp, PATH_MAX, "%s/images/bg.eet", PACKAGE_DATA_DIR);

	win = ewl_window_new();
	ewl_callback_append(win, EWL_CALLBACK_CONFIGURE, test_cb,
			"configure window");
	ewl_object_request_size(EWL_OBJECT(win), 240, 320);
	ewl_theme_data_set_str(win, "/window/file", tmp);
	ewl_theme_data_set_str(win, "/window/group", "bg");
	ewl_widget_show(win);

	vbox = ewl_vbox_new();
	ewl_theme_data_set_str(vbox, "/vbox/file", tmp);
	ewl_theme_data_set_str(vbox, "/vbox/group", "box");
	ewl_object_set_alignment(EWL_OBJECT(vbox), EWL_FLAG_ALIGN_TOP);
	ewl_object_set_fill_policy(EWL_OBJECT(vbox), EWL_FLAG_FILL_HFILL);
	ewl_container_append_child(EWL_CONTAINER(win), vbox);
	ewl_widget_show(vbox);

	/*
	 * Create the label first so we can pass it to the logo's callbacks
	 */
	label = ewl_text_new(NULL);

	/*
	 * Setup the custom logo which becomes the tour display area.
	 */
	logo = NEW(Ewl_Widget, 1);
	ewl_widget_init(logo, "demo_logo");
	ewl_callback_append(logo, EWL_CALLBACK_REALIZE, realize_logo_cb, label);
	ewl_callback_append(logo, EWL_CALLBACK_CONFIGURE, test_cb, "configure logo");
	ewl_theme_data_set_str(logo, "/demo_logo/file", tmp);
	ewl_theme_data_set_str(logo, "/demo_logo/group", "tour");
	ewl_object_set_preferred_size(EWL_OBJECT(logo), 150, 150);
	ewl_object_set_fill_policy(EWL_OBJECT(logo), EWL_FLAG_FILL_NONE);
	ewl_object_set_alignment(EWL_OBJECT(logo), EWL_FLAG_ALIGN_CENTER);
	ewl_container_append_child(EWL_CONTAINER(vbox), logo);
	ewl_widget_show(logo);

	start_text(label, NULL, label);
	ewl_container_append_child(EWL_CONTAINER(vbox), label);
	ewl_widget_show(label);

	hbox = ewl_hbox_new();
	ewl_box_set_spacing(EWL_BOX(hbox), 10);
	ewl_container_append_child(EWL_CONTAINER(vbox), hbox);
	ewl_object_set_fill_policy(EWL_OBJECT(hbox), EWL_FLAG_FILL_NONE);
	ewl_object_set_alignment(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
	ewl_widget_show(hbox);

	button = ewl_button_new("Take the tour!");
	ewl_container_append_child(EWL_CONTAINER(hbox), button);
	ewl_object_set_fill_policy(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, button_down, logo);
	ewl_widget_show(button);

	button = ewl_button_new("Quit");
	ewl_container_append_child(EWL_CONTAINER(hbox), button);
	ewl_object_set_fill_policy(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, quit_demo, NULL);
	ewl_widget_show(button);

	ewl_widget_configure(win);

	ewl_main();

	return 0;
}
