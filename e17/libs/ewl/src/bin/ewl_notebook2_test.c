#include "ewl_test.h"

static Ewl_Widget *nb2_button;
static int count = 1;

static void
__destroy_notebook2_test_window(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);

	ewl_callback_append(nb2_button, EWL_CALLBACK_CLICKED,
			    __create_notebook2_test_window, NULL);
}

static void
notebook2_cb_toggle_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
							void *data __UNUSED__)
{
	Ewl_Widget *n;

	n = ewl_widget_name_find("notebook");
	ewl_notebook2_tabbar_visible_set(EWL_NOTEBOOK2(n),
			!ewl_notebook2_tabbar_visible_get(EWL_NOTEBOOK2(n)));
}

static void
notebook2_change_align(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
							void *data)
{
	Ewl_Widget *n;
	unsigned int align = EWL_FLAG_ALIGN_CENTER;
	char *pos;
	
	pos = data;
	if (!strcmp(pos, "top"))
		align = EWL_FLAG_ALIGN_TOP;
	else if (!strcmp(pos, "bottom"))
		align = EWL_FLAG_ALIGN_BOTTOM;
	else if (!strcmp(pos, "left"))
		align = EWL_FLAG_ALIGN_LEFT;
	else if (!strcmp(pos, "right"))
		align = EWL_FLAG_ALIGN_RIGHT;
	else if (!strcmp(pos, "center"))
		align = EWL_FLAG_ALIGN_CENTER;
		
	n = ewl_widget_name_find("notebook");
	ewl_notebook2_tabbar_alignment_set(EWL_NOTEBOOK2(n), align);
}

static void
notebook2_change_position(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
							void *data)
{
	Ewl_Widget *n;
	unsigned int align = EWL_POSITION_TOP;
	char *pos;
	
	pos = data;
	if (!strcmp(pos, "top"))
		align = EWL_POSITION_TOP;
	else if (!strcmp(pos, "bottom"))
		align = EWL_POSITION_BOTTOM;
	else if (!strcmp(pos, "left"))
		align = EWL_POSITION_LEFT;
	else if (!strcmp(pos, "right"))
		align = EWL_POSITION_RIGHT;
		
	n = ewl_widget_name_find("notebook");
	ewl_notebook2_tabbar_position_set(EWL_NOTEBOOK2(n), align);
}

static void
notebook2_append_page(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
							void *data)
{
	printf("APPENDING TO %p\n", data);
}

static void
notebook2_prepend_page(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
							void *data)
{
	printf("PREPENDING TO %p\n", data);
}

static void
notebook2_delete_page(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
							void *data)
{
	printf("DELETE PAGE %p\n", data);
}

static Ewl_Widget *
create_main_page(void)
{
	Ewl_Widget *box2, *o, *o2 = NULL, *body, *border;
	unsigned int i;
	char *alignment[] = {"top", "left", "center", "right", "bottom"};
	char *position[] = {"top", "left", "right", "bottom"}; 

	/* box to hold everything */
	body = ewl_vbox_new();

	/* title label */
	o = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(o), "Main");
	ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(body), o);
	ewl_widget_show(o);

	/* box to hold alignment and position lists */
	box2 = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(body), box2);
	ewl_widget_show(box2);

	/* box to hold alignment list */
	border = ewl_border_new();
	ewl_border_text_set(EWL_BORDER(border), "Tabbar Alignment");
	ewl_container_child_append(EWL_CONTAINER(box2), border);
	ewl_widget_show(border);

	for (i = 0; i < (sizeof(alignment) / sizeof(char *)); i++)
	{
		o = ewl_radiobutton_new();
		ewl_button_label_set(EWL_BUTTON(o), alignment[i]);
		ewl_container_child_append(EWL_CONTAINER(border), o);
		ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED, 
					notebook2_change_align, alignment[i]);

		if (o2) ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o), 
							EWL_RADIOBUTTON(o2));

		if (!strcmp(alignment[i], "center"))
			ewl_radiobutton_checked_set(EWL_RADIOBUTTON(o), TRUE);

		ewl_widget_show(o);

		o2 = o;
	}

	/* box to hold position list */
	border = ewl_border_new();
	ewl_border_text_set(EWL_BORDER(border), "Tabbar Position");
	ewl_container_child_append(EWL_CONTAINER(box2), border);
	ewl_widget_show(border);

	o2 = NULL;
	for (i = 0; i < (sizeof(position) / sizeof(char *)); i++)
	{
		o = ewl_radiobutton_new();
		ewl_button_label_set(EWL_BUTTON(o), position[i]);
		ewl_container_child_append(EWL_CONTAINER(border), o);
		ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED, 
					notebook2_change_position, position[i]);

		if (o2) ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o), 
							EWL_RADIOBUTTON(o2));

		if (!strcmp(alignment[i], "top"))
			ewl_radiobutton_checked_set(EWL_RADIOBUTTON(o), TRUE);

		ewl_widget_show(o);

		o2 = o;
	}

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Toggle tabbar");
	ewl_container_child_append(EWL_CONTAINER(body), o);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, notebook2_cb_toggle_clicked, NULL);
	ewl_widget_show(o);

	return body;
}

static Ewl_Widget *
create_page(void)
{
	Ewl_Widget *box, *box2, *o;
	char buf[20];

	snprintf(buf, sizeof(buf), "Page %d", count++);

	box = ewl_vbox_new();
	ewl_box_spacing_set(EWL_BOX(box), 10);
	ewl_widget_show(box);

	o = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(o), buf);
	ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	box2 = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), box2);
	ewl_object_alignment_set(EWL_OBJECT(box2), EWL_FLAG_ALIGN_CENTER);
	ewl_widget_show(box2);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Append Page");
	ewl_container_child_append(EWL_CONTAINER(box2), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
				notebook2_append_page, box);
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Prepend Page");
	ewl_container_child_append(EWL_CONTAINER(box2), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
				notebook2_prepend_page, box);
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Remove This Page");
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
				notebook2_delete_page, box);
	ewl_widget_show(o);

	return box;
}

void
__create_notebook2_test_window(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Widget *win, *box, *n, *o, *o2;
	char buf[10];
	int i;

	nb2_button = w;

	win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(win), "Notebook2 Test");
	ewl_window_name_set(EWL_WINDOW(win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED,
				 __create_notebook2_test_window);
		ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
				    __destroy_notebook2_test_window, NULL);
	} else 
		ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
						__close_main_window, NULL);
	ewl_widget_show(win);

	box = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(win), box);
	ewl_widget_show(box);

	n = ewl_notebook2_new();
	ewl_container_child_append(EWL_CONTAINER(box), n);
	ewl_widget_name_set(n, "notebook");
	ewl_widget_show(n);

	o = create_main_page();
	ewl_container_child_prepend(EWL_CONTAINER(n), o);
	ewl_notebook2_page_tab_text_set(EWL_NOTEBOOK2(n), o, "Main");
	ewl_widget_show(o);

	for (i = 1; i < 4; i++)
	{
		o2 = ewl_label_new();
		snprintf(buf, sizeof(buf), "Page %d", i);
		ewl_label_text_set(EWL_LABEL(o2), buf);
		ewl_widget_show(o2);

		o = create_page();
		ewl_container_child_append(EWL_CONTAINER(n), o);
		ewl_notebook2_page_tab_widget_set(EWL_NOTEBOOK2(n), o, o2);
		ewl_widget_show(o);
	}
}

