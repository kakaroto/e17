#include <Ewl.h>

static Ewl_Widget *notebook_button;

void __create_notebook_test_window(Ewl_Widget * w, void *ev_data,
				   void *user_data);
void __notebook_append_page(Ewl_Widget * w, void *ev_data, void *user_data);
void __notebook_prepend_page(Ewl_Widget * w, void *ev_data, void *user_data);


void
__destroy_notebook_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(notebook_button, EWL_CALLBACK_CLICKED,
			    __create_notebook_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__notebook_append_page(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *main_vbox, *hbox;
	Ewl_Widget *tab;
	Ewl_Widget *text;
	Ewl_Widget *button[3];
	static int num = 1;
	char label[20];

	if (num > 3)
		return;

	snprintf(label, 20, "Page %i", num++);

	tab = ewl_text_new();
	ewl_text_set_text(tab, label);
	ewl_text_set_font_size(tab, 8);
	ewl_widget_show(tab);

	main_vbox = ewl_vbox_new();
	ewl_box_set_spacing(main_vbox, 10);
	ewl_theme_data_set(main_vbox, "/appearance/box/vertical/base/visible",
			   "no");
	ewl_widget_show(main_vbox);

	text = ewl_text_new();
	ewl_text_set_text(text, label);
	ewl_object_set_alignment(EWL_OBJECT(text), EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), text);
	ewl_widget_show(text);

	hbox = ewl_hbox_new();
	ewl_box_set_spacing(hbox, 5);
	ewl_theme_data_set(hbox, "/appearance/box/horizontal/base/visible",
			   "no");
	ewl_widget_set_data(hbox, "/y_padding", (void *) 20);
	ewl_object_set_custom_size(EWL_OBJECT(hbox), 225, 17);
	ewl_object_set_alignment(EWL_OBJECT(hbox), EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), hbox);
	ewl_widget_show(hbox);

	button[0] = ewl_button_new("Append Page");
	ewl_object_set_custom_size(button[0], 110, 17);
	ewl_container_append_child(EWL_CONTAINER(hbox), button[0]);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
			    __notebook_append_page, user_data);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new("Prepend Page");
	ewl_object_set_custom_size(button[1], 110, 17);
	ewl_container_append_child(EWL_CONTAINER(hbox), button[1]);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
			    __notebook_prepend_page, user_data);
	ewl_widget_show(button[1]);

	button[2] = ewl_button_new("Remove This Page");
	ewl_object_set_custom_size(button[2], 110, 17);
	ewl_object_set_alignment(EWL_OBJECT(button[2]), EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), button[2]);
	ewl_widget_show(button[2]);

	ewl_notebook_append_page(user_data, main_vbox, tab);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__notebook_prepend_page(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *main_vbox, *hbox;
	Ewl_Widget *tab;
	Ewl_Widget *text;
	Ewl_Widget *button[3];
	static int num = 1;
	char label[20];

	if (num > 3)
		return;

	snprintf(label, 20, "Page %i", num++);

	tab = ewl_text_new();
	ewl_text_set_text(tab, label);
	ewl_text_set_font_size(tab, 8);
	ewl_widget_show(tab);

	main_vbox = ewl_vbox_new();
	ewl_box_set_spacing(main_vbox, 10);
	ewl_theme_data_set(main_vbox, "/appearance/box/vertical/base/visible",
			   "no");
	ewl_widget_show(main_vbox);

	text = ewl_text_new();
	ewl_text_set_text(text, label);
	ewl_object_set_alignment(EWL_OBJECT(text), EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), text);
	ewl_widget_show(text);

	hbox = ewl_hbox_new();
	ewl_box_set_spacing(hbox, 5);
	ewl_theme_data_set(hbox, "/appearance/box/horizontal/base/visible",
			   "no");
	ewl_widget_set_data(hbox, "/y_padding", (void *) 20);
	ewl_object_set_custom_size(EWL_OBJECT(hbox), 225, 17);
	ewl_object_set_alignment(EWL_OBJECT(hbox), EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), hbox);
	ewl_widget_show(hbox);

	button[0] = ewl_button_new("Append Page");
	ewl_object_set_custom_size(button[0], 110, 17);
	ewl_container_append_child(EWL_CONTAINER(hbox), button[0]);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
			    __notebook_append_page, user_data);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new("Prepend Page");
	ewl_object_set_custom_size(button[1], 110, 17);
	ewl_container_append_child(EWL_CONTAINER(hbox), button[1]);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
			    __notebook_prepend_page, user_data);
	ewl_widget_show(button[1]);

	button[2] = ewl_button_new("Remove This Page");
	ewl_object_set_custom_size(button[2], 110, 17);
	ewl_object_set_alignment(EWL_OBJECT(button[2]), EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), button[2]);
	ewl_widget_show(button[2]);

	ewl_notebook_prepend_page(user_data, main_vbox, tab);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_notebook_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *notebook_win;
	Ewl_Widget *notebook;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_notebook_test_window);

	notebook_button = w;

	notebook_win = ewl_window_new();
	ewl_window_set_min_size(notebook_win, 256, 256);
	ewl_callback_append(notebook_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_notebook_test_window, NULL);
	ewl_widget_show(notebook_win);

	notebook = ewl_notebook_new();
	ewl_container_append_child(EWL_CONTAINER(notebook_win), notebook);
	ewl_widget_show(notebook);

	__notebook_append_page(NULL, NULL, notebook);
	__notebook_append_page(NULL, NULL, notebook);
	__notebook_append_page(NULL, NULL, notebook);

	return;
	ev_data = NULL;
	user_data = NULL;
}
