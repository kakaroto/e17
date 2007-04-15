/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>

static int counter = 0;
static int create_test(Ewl_Container *win);
static void cb_click(Ewl_Widget *w, void *ev, void *data);

void
test_info(Ewl_Test *test)
{
	test->name = "Label";
	test->tip = "Defines a widget for displaying a label";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_SIMPLE;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *body, *o2, *o;

	body = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), body);
	ewl_widget_show(body);

	o = ewl_label_new();
	ewl_container_child_append(EWL_CONTAINER(body), o);
	ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
	ewl_label_text_set(EWL_LABEL(o), "First label");
	ewl_widget_show(o);

	o2 = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o2), "Change Label");
	ewl_container_child_append(EWL_CONTAINER(body), o2);
	ewl_callback_append(o2, EWL_CALLBACK_CLICKED, cb_click, o);
	ewl_widget_show(o2);

	return 1;
}

void
cb_click(Ewl_Widget *w __UNUSED__, void *e __UNUSED__, void *data)
{
	if ((counter % 2) == 0)
		ewl_label_text_set(EWL_LABEL(data), "Second Label");
	else
		ewl_label_text_set(EWL_LABEL(data), "First label");

	counter ++;
}


