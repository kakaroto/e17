#include "Ewl_Test.h"
#include <stdio.h>
#include <stdlib.h>
#include "ewl_test_private.h"

static int create_test(Ewl_Container *box);
static int ext_to_icon_name_test(char *buf, int len);
static int mime_to_icon_name_test(char *buf, int len);
static int uri_mime_type_get_test(char *buf, int len);

static void cb_clicked(Ewl_Widget *w, void *ev, void *data);

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
	Ewl_Widget *o, *o2, *hbox;

	hbox = ewl_hbox_new();
	ewl_container_child_append(box, hbox);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
	ewl_widget_show(hbox);

	o = ewl_entry_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_widget_show(o);

	o2 = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o2), "fetch");
	ewl_container_child_append(EWL_CONTAINER(hbox), o2);
	ewl_callback_append(o2, EWL_CALLBACK_CLICKED, cb_clicked, o);
	ewl_object_fill_policy_set(EWL_OBJECT(o2), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show(o2);

	o = ewl_scrollpane_new();
	ewl_scrollpane_hscrollbar_flag_set(EWL_SCROLLPANE(o),
						EWL_SCROLLPANE_FLAG_ALWAYS_HIDDEN);
	ewl_container_child_append(box, o);
	ewl_widget_name_set(o, "scroll");
	ewl_widget_show(o);

	return 1;
}

static void
cb_clicked(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Widget *scroll, *t, *entry;
	char *s;

	entry = data;

	scroll = ewl_widget_name_find("scroll");
	ewl_container_reset(EWL_CONTAINER(scroll));

	s = ewl_text_text_get(EWL_TEXT(entry));
	t = ewl_io_manager_uri_read(s);
	if (!t)
	{
		printf("Unable to create text widget from file (%s)", s);
		return;
	}
	ewl_container_child_append(EWL_CONTAINER(scroll), t);
	ewl_widget_show(t);

	if (s) free(s);
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


