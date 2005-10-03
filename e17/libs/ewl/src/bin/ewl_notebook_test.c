#include "ewl_test.h"

static Ewl_Widget *notebook_button;

static void __notebook_append_page(Ewl_Widget * w, void *ev_data,
						void *user_data);
static void __notebook_prepend_page(Ewl_Widget * w, void *ev_data,
						void *user_data);
static void __notebook_remove_visible_page(Ewl_Widget * w, void *ev_data,
						void *user_data);

static Ewl_Widget *button_aleft, *button_acenter, *button_aright;
static Ewl_Widget *button_atop, *button_abottom;
static Ewl_Widget *button_pleft, *button_pright, *button_ptop, *button_pbottom;

static void
__destroy_notebook_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(notebook_button, EWL_CALLBACK_CLICKED,
			    __create_notebook_test_window, NULL);
}

static void
__notebook_change_alignment(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data)
{
	if (!ewl_radiobutton_is_checked(EWL_RADIOBUTTON(w)))
		return;

	if (w == button_aleft)
		ewl_notebook_tabs_alignment_set(user_data, EWL_FLAG_ALIGN_LEFT);
	else if (w == button_acenter)
		ewl_notebook_tabs_alignment_set(user_data,
						EWL_FLAG_ALIGN_CENTER);
	else if (w == button_aright)
		ewl_notebook_tabs_alignment_set(user_data, EWL_FLAG_ALIGN_RIGHT);
	else if (w == button_atop)
		ewl_notebook_tabs_alignment_set(user_data, EWL_FLAG_ALIGN_TOP);
	else if (w == button_abottom)
		ewl_notebook_tabs_alignment_set(user_data,
						EWL_FLAG_ALIGN_BOTTOM);
}

static void
__notebook_change_position(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data)
{
	if (!ewl_radiobutton_is_checked(w))
		return;

	if (w == button_pleft)
		ewl_notebook_tabs_position_set(user_data, EWL_POSITION_LEFT);
	else if (w == button_pright)
		ewl_notebook_tabs_position_set(user_data, EWL_POSITION_RIGHT);
	else if (w == button_ptop)
		ewl_notebook_tabs_position_set(user_data, EWL_POSITION_TOP);
	else if (w == button_pbottom)
		ewl_notebook_tabs_position_set(user_data, EWL_POSITION_BOTTOM);
}

static void
__notebook_create_main_page(Ewl_Widget * notebook)
{
	Ewl_Widget     *main_vbox;
	Ewl_Widget     *tab;
	Ewl_Widget     *text;
	Ewl_Widget     *avbox, *alabel;
	Ewl_Widget     *pvbox, *plabel;

	tab = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(tab), "Main");
	ewl_widget_show(tab);

	main_vbox = ewl_vbox_new();
	ewl_box_spacing_set(EWL_BOX(main_vbox), 10);
	ewl_widget_show(main_vbox);

	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), "Main");
	ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(main_vbox), text);
	ewl_widget_show(text);

	avbox = ewl_vbox_new();
	ewl_object_alignment_set(EWL_OBJECT(avbox), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(main_vbox), avbox);
	ewl_widget_show(avbox);

	alabel = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(alabel), "Tabs Alignment");
	ewl_container_child_append(EWL_CONTAINER(avbox), alabel);
	ewl_widget_show(alabel);

	button_atop = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(button_atop), "Top");
	ewl_container_child_append(EWL_CONTAINER(avbox), button_atop);
	ewl_callback_append(button_atop, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_alignment, notebook);
	ewl_widget_show(button_atop);

	button_aleft = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(button_aleft), "Left");
	ewl_radiobutton_checked_set(button_aleft, 1);
	ewl_container_child_append(EWL_CONTAINER(avbox), button_aleft);
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_aleft),
				  EWL_RADIOBUTTON(button_atop));
	ewl_callback_append(button_aleft, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_alignment, notebook);
	ewl_widget_show(button_aleft);

	button_acenter = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(button_acenter), "Center");
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_acenter),
				  EWL_RADIOBUTTON(button_aleft));
	ewl_radiobutton_checked_set(button_acenter, 1);
	ewl_container_child_append(EWL_CONTAINER(avbox), button_acenter);
	ewl_callback_append(button_acenter, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_alignment, notebook);
	ewl_widget_show(button_acenter);

	button_aright = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(button_aright), "Right");
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_aright),
				  EWL_RADIOBUTTON(button_acenter));
	ewl_container_child_append(EWL_CONTAINER(avbox), button_aright);
	ewl_callback_append(button_aright, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_alignment, notebook);
	ewl_widget_show(button_aright);

	button_abottom = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(button_abottom), "Bottom");
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_abottom),
				  EWL_RADIOBUTTON(button_aright));
	ewl_container_child_append(EWL_CONTAINER(avbox), button_abottom);
	ewl_callback_append(button_abottom, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_alignment, notebook);
	ewl_widget_show(button_abottom);

	pvbox = ewl_vbox_new();
	ewl_object_alignment_set(EWL_OBJECT(pvbox), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(main_vbox), pvbox);
	ewl_widget_show(pvbox);

	plabel = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(plabel), "Tabs Position");
	ewl_container_child_append(EWL_CONTAINER(pvbox), plabel);
	ewl_widget_show(plabel);

	button_pleft = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(button_pleft), "Left");
	ewl_container_child_append(EWL_CONTAINER(pvbox), button_pleft);
	ewl_callback_append(button_pleft, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_position, notebook);
	ewl_widget_show(button_pleft);

	button_pright = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(button_pright), "Right");
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_pright),
				  EWL_RADIOBUTTON(button_pleft));
	ewl_container_child_append(EWL_CONTAINER(pvbox), button_pright);
	ewl_callback_append(button_pright, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_position, notebook);
	ewl_widget_show(button_pright);

	button_ptop = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(button_ptop), "Top");
	ewl_radiobutton_checked_set(button_ptop, 1);
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_ptop),
				  EWL_RADIOBUTTON(button_pright));
	ewl_container_child_append(EWL_CONTAINER(pvbox), button_ptop);
	ewl_callback_append(button_ptop, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_position, notebook);
	ewl_widget_show(button_ptop);

	button_pbottom = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(button_pbottom), "Bottom");
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(button_pbottom),
				  EWL_RADIOBUTTON(button_ptop));
	ewl_container_child_append(EWL_CONTAINER(pvbox), button_pbottom);
	ewl_callback_append(button_pbottom, EWL_CALLBACK_VALUE_CHANGED,
			    __notebook_change_position, notebook);
	ewl_widget_show(button_pbottom);

	ewl_notebook_page_prepend(EWL_NOTEBOOK(notebook), tab, main_vbox);
}

static void
__notebook_generate_page(Ewl_Widget *notebook, int type)
{
	Ewl_Widget     *main_vbox, *hbox;
	Ewl_Widget     *tab;
	Ewl_Widget     *text;
	Ewl_Widget     *button[3];
	static int      num = 1;
	char            label[20];

	snprintf(label, 20, "Page %i", num++);

	tab = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(tab), label);
	ewl_widget_show(tab);

	main_vbox = ewl_vbox_new();
	ewl_box_spacing_set(EWL_BOX(main_vbox), 10);
	ewl_widget_show(main_vbox);

	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), label);
	ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(main_vbox), text);
	ewl_widget_show(text);

	hbox = ewl_hbox_new();
	ewl_box_spacing_set(EWL_BOX(hbox), 5);
	ewl_object_padding_set(EWL_OBJECT(hbox), 0, 0, 20, 0);
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(main_vbox), hbox);
	ewl_widget_show(hbox);

	button[0] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[0]), "Append Page");
	ewl_container_child_append(EWL_CONTAINER(hbox), button[0]);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
			    __notebook_append_page, notebook);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[1]), "Prepend Page");
	ewl_container_child_append(EWL_CONTAINER(hbox), button[1]);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
			    __notebook_prepend_page, notebook);
	ewl_widget_show(button[1]);

	button[2] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[2]), "Remove This Page");
	ewl_object_alignment_set(EWL_OBJECT(button[2]), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(main_vbox), button[2]);
	ewl_callback_append(button[2], EWL_CALLBACK_CLICKED,
			    __notebook_remove_visible_page, notebook);
	ewl_widget_show(button[2]);

	if (!type)
		ewl_notebook_page_append(EWL_NOTEBOOK(notebook), tab, main_vbox);
	else
		ewl_notebook_page_prepend(EWL_NOTEBOOK(notebook), tab, main_vbox);
}

static void
__notebook_append_page(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
						void *user_data)
{
	__notebook_generate_page(user_data, 0);
}

static void
__notebook_prepend_page(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
						void *user_data)
{
	__notebook_generate_page(user_data, 1);
}

static void
__notebook_remove_visible_page(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
							void *user_data)
{
	ewl_notebook_visible_page_remove(EWL_NOTEBOOK(user_data));
}

void
__create_notebook_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Widget     *notebook_win;
	Ewl_Widget     *notebook_box;
	Ewl_Widget     *notebook;

	notebook_button = w;

	notebook_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(notebook_win), "Notebook Test");
	ewl_window_name_set(EWL_WINDOW(notebook_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(notebook_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_notebook_test_window);
		ewl_callback_append(notebook_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_notebook_test_window, NULL);
	} else 
		ewl_callback_append(notebook_win, EWL_CALLBACK_DELETE_WINDOW,
				__close_main_window, NULL);
	ewl_widget_show(notebook_win);

	/*
	 * Create the main box for holding the widgets
	 */
	notebook_box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(notebook_win), notebook_box);
	ewl_box_spacing_set(EWL_BOX(notebook_box), 10);
	ewl_widget_show(notebook_box);

	notebook = ewl_notebook_new();
	ewl_notebook_tabs_position_set(EWL_NOTEBOOK(notebook),
				       EWL_POSITION_LEFT);
	ewl_container_child_append(EWL_CONTAINER(notebook_box), notebook);
	ewl_widget_show(notebook);

	__notebook_append_page(NULL, NULL, notebook);
	__notebook_append_page(NULL, NULL, notebook);
	__notebook_append_page(NULL, NULL, notebook);

	__notebook_create_main_page(notebook);
}

