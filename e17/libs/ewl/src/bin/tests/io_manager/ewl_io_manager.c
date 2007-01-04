/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include <stdio.h>
#include <stdlib.h>
#include "ewl_test_private.h"

static int create_test(Ewl_Container *box);

static void cb_clicked(Ewl_Widget *w, void *ev, void *data);
static void cb_open(Ewl_Widget *w, void *ev, void *data);
static void cb_fd_delete(Ewl_Widget *w, void *ev, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "IO Manager";
	test->tip = "Defines a system for doing IO of URIs";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_SIMPLE;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *o, *o2;

	o2 = ewl_button_new();
	ewl_stock_type_set(EWL_STOCK(o2), EWL_STOCK_OPEN);
	ewl_callback_append(o2, EWL_CALLBACK_CLICKED, cb_clicked, NULL);
	ewl_object_fill_policy_set(EWL_OBJECT(o2), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(box, o2);
	ewl_widget_show(o2);

	o = ewl_scrollpane_new();
	ewl_container_child_append(box, o);
	ewl_widget_name_set(o, "scroll");
	ewl_widget_show(o);

	return 1;
}

static void
cb_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Widget *fd;

	fd = ewl_filedialog_new();
	ewl_filedialog_filter_add(EWL_FILEDIALOG(fd), "txt", "*.txt");
	ewl_filedialog_filter_add(EWL_FILEDIALOG(fd), "png", "*.png");
	ewl_filedialog_filter_add(EWL_FILEDIALOG(fd), "jpg", "*.jpg");
	ewl_callback_append(fd, EWL_CALLBACK_DELETE_WINDOW, cb_fd_delete, NULL);
	ewl_callback_append(fd, EWL_CALLBACK_VALUE_CHANGED, cb_open, NULL);
	ewl_widget_show(fd);
}

static void
cb_fd_delete(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	ewl_widget_destroy(w);
}

static void
cb_open(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
	Ewl_Dialog_Event *e;
	Ewl_Filedialog *fd;
	Ewl_Widget *scroll, *t;
	char *s, path[1024];

	e = ev;

	if (e->response == EWL_STOCK_CANCEL)
		return;

	fd = EWL_FILEDIALOG(w);
	s = ewl_filedialog_selected_file_get(fd);
	snprintf(path, sizeof(path), "%s/%s", 
			ewl_filedialog_directory_get(fd), s);
	if (s) free(s);
	ewl_widget_destroy(w);

	scroll = ewl_widget_name_find("scroll");
	ewl_container_reset(EWL_CONTAINER(scroll));

	t = ewl_io_manager_uri_read(path);
	if (!t)
	{
		printf("Unable to create widget from file (%s)\n", path);
		return;
	}
	ewl_container_child_append(EWL_CONTAINER(scroll), t);
	ewl_widget_show(t);
}


