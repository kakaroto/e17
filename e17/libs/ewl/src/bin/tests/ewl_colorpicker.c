#include "Ewl_Test2.h"
#include <stdio.h>

static int create_test(Ewl_Container *box);
static void color_value_changed(Ewl_Widget *w, void *ev, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Colorpicker";
	test->tip = "Defines a colour picker.";
	test->filename = "ewl_colorpicker.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_MISC;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *colorpicker;

	colorpicker = ewl_colorpicker_new();
	ewl_container_child_append(EWL_CONTAINER(box), colorpicker);
	ewl_callback_append(colorpicker, EWL_CALLBACK_VALUE_CHANGED,
					    color_value_changed, NULL);
	ewl_widget_show(colorpicker);

	return 1;
}

static void
color_value_changed(Ewl_Widget *w __UNUSED__, void *ev,
						void *data __UNUSED__)
{
	Ewl_Color_Set *col;

	col = ev;
	printf("value changed to (%i, %i, %i)\n", col->r, col->g, col->b);
}


