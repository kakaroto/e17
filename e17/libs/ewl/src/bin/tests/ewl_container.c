#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int switch_parents(char *buf, int len);

static Ewl_Unit_Test container_unit_tests[] = {
		{"switch parents", switch_parents},	
		{NULL, NULL}
	};

void 
test_info(Ewl_Test *test)
{
	test->name = "Container";
	test->tip = "The base container type.";
	test->filename = __FILE__;
	test->type = EWL_TEST_TYPE_UNIT;
	test->unit_tests = container_unit_tests;
}

static int
switch_parents(char *buf, int len)
{
	Ewl_Widget *p1, *p2, *w;

	p1 = ewl_vbox_new();
	ewl_widget_show(p1);

	w = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(p1), w);
	ewl_widget_show(w);

	if (!(w->parent == p1))
	{
		snprintf(buf, len, "initial parent incorrect (%p vs %p)", w->parent, p1);
		return FALSE;
	}

	p2 = ewl_vbox_new();
	ewl_widget_show(p2);

	ewl_container_child_append(EWL_CONTAINER(p2), w);
	if (!(w->parent == p2))
	{
		snprintf(buf, len, "reparent incorrect (%p vs %p)", w->parent, p2);
		return FALSE;
	}

	return TRUE;
}


