#include "Ewl_Test.h"
#include <stdio.h>
#include <stdlib.h>
#include "ewl_test_private.h"

static int create_test(Ewl_Container *box);
static int ext_to_icon_name_test(char *buf, int len);
static int mime_to_icon_name_test(char *buf, int len);
static int uri_mime_type_get_test(char *buf, int len);

static void cb_clicked(Ewl_Widget *w, void *ev, void *data);
static void cb_open(Ewl_Widget *w, void *ev, void *data);
static void cb_fd_delete(Ewl_Widget *w, void *ev, void *data);

static Ewl_Unit_Test io_manager_unit_tests[] = {
		{"extension to icon name mapping", ext_to_icon_name_test},
		{"mime type to icon name mapping", mime_to_icon_name_test},
		{"uri mime type get", uri_mime_type_get_test},
		{NULL, NULL}
	};

void 
test_info(Ewl_Test *test)
{
	test->name = "IO Manager";
	test->tip = "Defines a system for doing IO of URIs";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_SIMPLE;
	test->unit_tests = io_manager_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *o, *o2;

	o2 = ewl_button_new();
	ewl_button_stock_type_set(EWL_BUTTON(o2), EWL_STOCK_OPEN);
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
cb_clicked(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Widget *fd;

	fd = ewl_filedialog_new();
	ewl_filedialog_filter_add(EWL_FILEDIALOG(fd), "txt", "*.txt");
	ewl_callback_append(fd, EWL_CALLBACK_DELETE_WINDOW, cb_fd_delete, NULL);
	ewl_callback_append(fd, EWL_CALLBACK_VALUE_CHANGED, cb_open, NULL);
	ewl_widget_show(fd);
}

static void
cb_fd_delete(Ewl_Widget *w, void *ev, void *data)
{
	ewl_widget_destroy(w);
}

static void
cb_open(Ewl_Widget *w, void *ev, void *data)
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
	snprintf(path, sizeof(path), "%s/%s", ewl_filedialog_directory_get(fd), s);
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

static int
ext_to_icon_name_test(char *buf, int len)
{
	return 1;
}

static int
mime_to_icon_name_test(char *buf, int len)
{
	return 1;
}

static int
uri_mime_type_get_test(char *buf, int len)
{
	return 1;
}


