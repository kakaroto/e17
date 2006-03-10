#include "Ewl_Test.h"
#include <stdio.h>

static int create_test(Ewl_Container *box);

void 
test_info(Ewl_Test *test)
{
	test->name = "Scrollpane";
	test->tip = "A scrollpane.";
	test->filename = "ewl_scrollpane.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *scrollpane;
	Ewl_Widget *button;

	scrollpane = ewl_scrollpane_new();
	ewl_container_child_append(box, scrollpane);
	ewl_widget_show(scrollpane);

	button = ewl_button_new();
	ewl_object_size_request(EWL_OBJECT(button), 500, 500);
	ewl_object_padding_set(EWL_OBJECT(button), 20, 20, 20, 20);
	ewl_container_child_append(EWL_CONTAINER(scrollpane), button);
	ewl_widget_show(button);

	return 1;
}

