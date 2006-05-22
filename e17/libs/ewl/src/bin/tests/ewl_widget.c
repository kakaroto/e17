#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int appearance_test_set_get(char *buf, int len);

static Ewl_Unit_Test widget_unit_tests[] = {
		{"widget appearance set/get", appearance_test_set_get},	
		{NULL, NULL}
	};

void 
test_info(Ewl_Test *test)
{
	test->name = "Widget";
	test->tip = "The base widget type.";
	test->filename = "ewl_widget.c";
	test->type = EWL_TEST_TYPE_UNIT;
	test->unit_tests = widget_unit_tests;
}

static int
appearance_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	ewl_widget_appearance_set(w, "my_appearance");
	if (strcmp("my_appearance", ewl_widget_appearance_get(w)))
		snprintf(buf, len, "appearance_get dosen't match appearance_set");
	else 
		ret = 1;

	return ret;
}

