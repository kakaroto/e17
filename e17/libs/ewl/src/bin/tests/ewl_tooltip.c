#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>

static int create_test(Ewl_Container *win);

void 
test_info(Ewl_Test *test)
{
	test->name = "Tooltip";
	test->tip = "Defines a widget for displaying short\n"
			"messages after a delay.";
	test->filename = "ewl_tooltip.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_SIMPLE;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *button;

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Hoover on this button");
	ewl_container_child_append(EWL_CONTAINER(box), button);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	ewl_attach_tooltip_text_set(button, "and see.");
	ewl_widget_show(button);

	return 1;
}


