#include <Ewl.h>

static Ewl_Widget *notebook_button;

void __create_notebook_test_window(Ewl_Widget * w, void *ev_data,
				   void *user_data);
void __notebook_append_page(Ewl_Widget * w, void *ev_data,
			    void *user_data);
void __notebook_prepend_page(Ewl_Widget * w, void *ev_data,
			     void *user_data);

Ewl_Widget *button_aleft, *button_acenter, *button_aright, *button_atop,
    *button_abottom;
Ewl_Widget *button_pleft, *button_pright, *button_ptop, *button_pbottom;

void
__destroy_notebook_test_window(Ewl_Widget * w, void *ev_data,
			       void *user_data)
{
	ewl_widget_destroy_recursive(w);

	ewl_callback_append(notebook_button, EWL_CALLBACK_CLICKED,
			    __create_notebook_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__notebook_change_alignment(Ewl_Widget * w, void *ev_data, void *user_data)
{
	if (!ewl_radiobutton_is_checked(w))
		return;

	if (w == button_aleft)
		ewl_notebook_set_tabs_alignment(user_data,
						EWL_ALIGNMENT_LEFT);
	else if (w == button_acenter)
		ewl_notebook_set_tabs_alignment(user_data,
						EWL_ALIGNMENT_CENTER);
	else if (w == button_aright)
		ewl_notebook_set_tabs_alignment(user_data,
						EWL_ALIGNMENT_RIGHT);
	else if (w == button_atop)
		ewl_notebook_set_tabs_alignment(user_data,
						EWL_ALIGNMENT_TOP);
	else if (w == button_abottom)
		ewl_notebook_set_tabs_alignment(user_data,
						EWL_ALIGNMENT_BOTTOM);

	return;
	ev_data = NULL;
}

void
__notebook_change_position(Ewl_Widget * w, void *ev_data, void *user_data)
{
	if (!ewl_radiobutton_is_checked(w))
		return;

	if (w == button_pleft)
		ewl_notebook_set_tabs_position(user_data,
					       EWL_POSITION_LEFT);
	else if (w == button_pright)
		ewl_notebook_set_tabs_position(user_data,
					       EWL_POSITION_RIGHT);
	else if (w == button_ptop)
		ewl_notebook_set_tabs_position(user_data,
					       EWL_POSITION_TOP);
	else if (w == button_pbottom)
		ewl_notebook_set_tabs_position(user_data,
					       EWL_POSITION_BOTTOM);

	return;
	ev_data = NULL;
}

Ewl_Widget **
__notebook_create_main_page(Ewl_Widget * notebook)
{
	Ewl_Widget **n;
	Ewl_Widget *main_vbox;
	Ewl_Widget *tab;
	Ewl_Widget *text;
	Ewl_Widget *avbox, *alabel;
	Ewl_Widget *pvbox, *plabel;

	n = NEW(Ewl_Widget, 2);

	tab = ewl_text_new();
	ewl_text_set_text(tab, "Main");
	ewl_text_set_font_size(tab, 8);
	ewl_widget_show(tab);

	main_vbox = ewl_vbox_new();
	ewl_box_set_spacing(main_vbox, 10);
	ewl_theme_data_set(main_vbox,
			   "/appearance/box/vertical/base/visible", "no");
	ewl_widget_show(main_vbox);

	text = ewl_text_new();
	ewl_text_set_text(text, "Main");
	ewl_object_set_alignment(EWL_OBJECT(text), EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), text);
	ewl_widget_show(text);

	avbox = ewl_vbox_new();
	ewl_object_set_custom_size(EWL_OBJECT(avbox), 120, 104);
	ewl_object_set_alignment(EWL_OBJECT(avbox), EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), avbox);
	ewl_widget_show(avbox);

	alabel = ewl_text_new();
	ewl_text_set_text(alabel, "Tabs Alignment");
	ewl_container_append_child(EWL_CONTAINER(avbox), alabel);
	ewl_widget_show(alabel);

	button_atop = ewl_radiobutton_new("Top");
	ewl_container_append_child(EWL_CONTAINER(avbox), button_atop);
	ewl_callback_append(button_atop, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_alignment, notebook);
	ewl_widget_show(button_atop);

	button_aleft = ewl_radiobutton_new("Left");
	ewl_radiobutton_set_checked(button_aleft, 1);
	ewl_container_append_child(EWL_CONTAINER(avbox), button_aleft);
	ewl_radiobutton_set_chain(button_aleft, button_atop);
	ewl_callback_append(button_aleft, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_alignment, notebook);
	ewl_widget_show(button_aleft);

	button_acenter = ewl_radiobutton_new("Center");
	ewl_radiobutton_set_chain(button_acenter, button_aleft);
	ewl_container_append_child(EWL_CONTAINER(avbox), button_acenter);
	ewl_callback_append(button_acenter, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_alignment, notebook);
	ewl_widget_show(button_acenter);

	button_aright = ewl_radiobutton_new("Right");
	ewl_radiobutton_set_chain(button_aright, button_acenter);
	ewl_container_append_child(EWL_CONTAINER(avbox), button_aright);
	ewl_callback_append(button_aright, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_alignment, notebook);
	ewl_widget_show(button_aright);

	button_abottom = ewl_radiobutton_new("Bottom");
	ewl_radiobutton_set_chain(button_abottom, button_aright);
	ewl_container_append_child(EWL_CONTAINER(avbox), button_abottom);
	ewl_callback_append(button_abottom, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_alignment, notebook);
	ewl_widget_show(button_abottom);

	pvbox = ewl_vbox_new();
	ewl_object_set_custom_size(EWL_OBJECT(pvbox), 120, 87);
	ewl_object_set_alignment(EWL_OBJECT(pvbox), EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), pvbox);
	ewl_widget_show(pvbox);

	plabel = ewl_text_new();
	ewl_text_set_text(plabel, "Tabs Position");
	ewl_container_append_child(EWL_CONTAINER(pvbox), plabel);
	ewl_widget_show(plabel);

	button_pleft = ewl_radiobutton_new("Left");
	ewl_container_append_child(EWL_CONTAINER(pvbox), button_pleft);
	ewl_callback_append(button_pleft, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_position, notebook);
	ewl_widget_show(button_pleft);

	button_pright = ewl_radiobutton_new("Right");
	ewl_radiobutton_set_chain(button_pright, button_pleft);
	ewl_container_append_child(EWL_CONTAINER(pvbox), button_pright);
	ewl_callback_append(button_pright, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_position, notebook);
	ewl_widget_show(button_pright);

	button_ptop = ewl_radiobutton_new("Top");
	ewl_radiobutton_set_checked(button_ptop, 1);
	ewl_radiobutton_set_chain(button_ptop, button_pright);
	ewl_container_append_child(EWL_CONTAINER(pvbox), button_ptop);
	ewl_callback_append(button_ptop, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_position, notebook);
	ewl_widget_show(button_ptop);

	button_pbottom = ewl_radiobutton_new("Bottom");
	ewl_radiobutton_set_chain(button_pbottom, button_ptop);
	ewl_container_append_child(EWL_CONTAINER(pvbox), button_pbottom);
	ewl_callback_append(button_pbottom, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_position, notebook);
	ewl_widget_show(button_pbottom);

	n[0] = main_vbox;
	n[1] = tab;

	return n;
}

Ewl_Widget **
__notebook_generate_page(Ewl_Widget * notebook)
{
	Ewl_Widget **n;
	Ewl_Widget *main_vbox, *hbox;
	Ewl_Widget *tab;
	Ewl_Widget *text;
	Ewl_Widget *button[3];
	static int num = 1;
	char label[20];

	snprintf(label, 20, "Page %i", num++);

	n = NEW(Ewl_Widget, 2);

	tab = ewl_text_new();
	ewl_text_set_text(tab, label);
	ewl_text_set_font_size(tab, 8);
	ewl_widget_show(tab);

	main_vbox = ewl_vbox_new();
	ewl_box_set_spacing(main_vbox, 10);
	ewl_theme_data_set(main_vbox,
			   "/appearance/box/vertical/base/visible", "no");
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
	ewl_object_set_padding(EWL_OBJECT(hbox), 0, 0, 20, 0);
	ewl_object_set_custom_size(EWL_OBJECT(hbox), 225, 17);
	ewl_object_set_alignment(EWL_OBJECT(hbox), EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), hbox);
	ewl_widget_show(hbox);

	button[0] = ewl_button_new("Append Page");
	ewl_object_set_custom_size(button[0], 110, 17);
	ewl_container_append_child(EWL_CONTAINER(hbox), button[0]);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
			    __notebook_append_page, notebook);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new("Prepend Page");
	ewl_object_set_custom_size(button[1], 110, 17);
	ewl_container_append_child(EWL_CONTAINER(hbox), button[1]);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
			    __notebook_prepend_page, notebook);
	ewl_widget_show(button[1]);

	button[2] = ewl_button_new("Remove This Page");
	ewl_object_set_custom_size(button[2], 110, 17);
	ewl_object_set_alignment(EWL_OBJECT(button[2]),
				 EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), button[2]);
	ewl_widget_show(button[2]);

	n[0] = main_vbox;
	n[1] = tab;

	return n;
}

void
__notebook_append_page(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget **n;

	n = __notebook_generate_page(user_data);

	ewl_notebook_append_page(user_data, n[0], n[1]);

	FREE(n);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__notebook_prepend_page(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget **n;

	n = __notebook_generate_page(user_data);

	ewl_notebook_prepend_page(user_data, n[0], n[1]);

	FREE(n);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_notebook_test_window(Ewl_Widget * w, void *ev_data,
			      void *user_data)
{
	Ewl_Widget *notebook_win;
	Ewl_Widget *notebook;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_notebook_test_window);

	notebook_button = w;

	notebook_win = ewl_window_new();
	ewl_window_resize(notebook_win, 305, 256);
	ewl_window_set_min_size(notebook_win, 305, 256);
	ewl_callback_append(notebook_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_notebook_test_window, NULL);
	ewl_widget_show(notebook_win);

	notebook = ewl_notebook_new();
	ewl_container_append_child(EWL_CONTAINER(notebook_win), notebook);
	ewl_widget_show(notebook);

	__notebook_append_page(NULL, NULL, notebook);
	__notebook_append_page(NULL, NULL, notebook);
	__notebook_append_page(NULL, NULL, notebook);

	{
		Ewl_Widget **n;

		n = __notebook_create_main_page(notebook);

		ewl_notebook_prepend_page(notebook, n[0], n[1]);
	}

	return;
	ev_data = NULL;
	user_data = NULL;
}
