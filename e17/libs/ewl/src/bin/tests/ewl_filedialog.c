#include "Ewl_Test.h"
#include <stdio.h>

static int create_test(Ewl_Container *box);
static void dialog_cb_single_clicked(Ewl_Widget *w, void *ev, void *data);
static void dialog_cb_multi_clicked(Ewl_Widget *w, void *ev, void *data);
static void fd_cb_delete(Ewl_Widget *w, void *ev, void *data);
static void fd_cb_value_changed(Ewl_Widget *w, void *ev, void *data);
static void fd_append(void *value, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Filedialog";
	test->tip = "The filedialog is intended to be used for a\n"
			"single or multi-file chooser.";
	test->filename = "ewl_filedialog.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_ADVANCED;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *o, *o2;

	o = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(o), "Selected Files");
	ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(box, o);
	ewl_widget_show(o);

	o = ewl_vbox_new();
	ewl_widget_name_set(o, "file_list");
	ewl_container_child_append(box, o);
	ewl_widget_show(o);

	o = ewl_hbox_new();
	ewl_container_child_append(box, o);
	ewl_widget_show(o);

	o2 = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o2), "Single select");
	ewl_container_child_append(EWL_CONTAINER(o), o2);
	ewl_callback_append(o2, EWL_CALLBACK_CLICKED,
				dialog_cb_single_clicked, NULL);
	ewl_widget_show(o2);

	o2 = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o2), "Multi select");
	ewl_container_child_append(EWL_CONTAINER(o), o2);
	ewl_callback_append(o2, EWL_CALLBACK_CLICKED,
				dialog_cb_multi_clicked, NULL);
	ewl_widget_show(o2);

	return 1;
}

static void
dialog_cb_single_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	Ewl_Widget *fd;

	fd = ewl_filedialog_new();
	ewl_callback_append(fd, EWL_CALLBACK_DELETE_WINDOW, 
					fd_cb_delete, NULL);
	ewl_callback_append(fd, EWL_CALLBACK_VALUE_CHANGED,
					fd_cb_value_changed, NULL);
	ewl_widget_show(fd);
}

static void
dialog_cb_multi_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	Ewl_Widget *fd;

	fd = ewl_filedialog_multiselect_new();
	ewl_callback_append(fd, EWL_CALLBACK_DELETE_WINDOW, 
					fd_cb_delete, NULL);
	ewl_callback_append(fd, EWL_CALLBACK_VALUE_CHANGED,
					fd_cb_value_changed, NULL);
	ewl_widget_show(fd);
}

static void
fd_cb_delete(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	ewl_widget_destroy(w);
}

static void
fd_cb_value_changed(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
	Ewl_Dialog_Event *e;
	Ewl_Widget *list;
	Ewl_Filedialog *fd;

	e = ev;

	list = ewl_widget_name_find("file_list");
	ewl_container_reset(EWL_CONTAINER(list));

	/* nothing to do on a cancel */
	if (e->response == EWL_STOCK_CANCEL)
		return;

	fd = EWL_FILEDIALOG(w);
	if (ewl_filedialog_multiselect_get(fd))
	{
		Ecore_List *l;
		l = ewl_filedialog_select_list_get(fd);
		ecore_list_for_each(l, fd_append, list);
	}
	else
		fd_append(ewl_filedialog_file_get(fd), list);

	ewl_widget_destroy(w);
}

static void
fd_append(void *value, void *data)
{
	Ewl_Widget *list, *o;
	const char *file;

	file = value;
	list = data;

	o = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(o), file);
	ewl_container_child_append(EWL_CONTAINER(list), o);
	ewl_widget_show(o);
}

