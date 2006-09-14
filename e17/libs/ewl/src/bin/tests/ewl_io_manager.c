#include "Ewl_Test.h"
#include <stdio.h>
#include "ewl_test_private.h"

static int create_test(Ewl_Container *box);
static int ext_to_icon_name_test(char *buf, int len);
static int mime_to_icon_name_test(char *buf, int len);
static int uri_mime_type_get_test(char *buf, int len);

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
	Ewl_Widget *t;

	t = ewl_io_manager_uri_read(PACKAGE_DATA_DIR "/examples/ewl_io_manager.c");
	if (!t)
	{
		printf("Unable to create text widget from file");
		return 0;
	}
	ewl_container_child_append(box, t);
	ewl_widget_show(t);

	return 1;
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


