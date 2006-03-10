#include "Ewl_Test.h"
#include <stdio.h>

static int create_test(Ewl_Container *box);

void 
test_info(Ewl_Test *test)
{
	test->name = "Fileselector";
	test->tip = "A simple file selection widget";
	test->filename = "ewl_fileselector.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_ADVANCED;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *w;

	w = ewl_fileselector_new();
	ewl_container_child_append(box, w);
	ewl_widget_show(w);

	return 1;
}

