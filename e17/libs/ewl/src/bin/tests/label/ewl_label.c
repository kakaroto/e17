/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_label.h"

#include <stdio.h>
#include <string.h>

static int counter = 0;
static int create_test(Ewl_Container *win);
static void cb_click(Ewl_Widget *w, void *ev, void *data);

static int label_null_test_get(char *buf, int len);
static int label_null_test_set_get(char *buf, int len);
static int label_test_set_get(char *buf, int len);

static Ewl_Unit_Test label_unit_tests[] = {
		{"label null get", label_null_test_get, -1, NULL},
		{"label null set/get", label_null_test_set_get, -1, NULL},
		{"label set/get", label_test_set_get, -1, NULL},
		{NULL, NULL, -1, NULL}
	};

void
test_info(Ewl_Test *test)
{
	test->name = "Label";
	test->tip = "Defines a widget for displaying a label";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_SIMPLE;
	test->unit_tests = label_unit_tests;
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

static int
label_null_test_get(char *buf, int len)
{
	Ewl_Widget *label;
	int ret = 0;

	label = ewl_label_new();

	if (ewl_label_text_get(EWL_LABEL(label)))
		snprintf(buf, len, "text_get not NULL");
	else
		ret = 1;

	ewl_widget_destroy(label);

	return ret;
}

static int
label_null_test_set_get(char *buf, int len)
{
	Ewl_Widget *label;
	const char *val;
	int ret = 0;

	label = ewl_label_new();

	ewl_label_text_set(EWL_LABEL(label), "some text");
	ewl_label_text_set(EWL_LABEL(label), NULL);

	val = ewl_label_text_get(EWL_LABEL(label));
	if (val)
		snprintf(buf, len, "text_set_get %s instead of NULL", val);
	else
		ret = 1;

	ewl_widget_destroy(label);

	return ret;
}

static int
label_test_set_get(char *buf, int len)
{
	Ewl_Widget *label;
	const char *val;
	int ret = 0;

	label = ewl_label_new();

	ewl_label_text_set(EWL_LABEL(label), "some text");

	val = ewl_label_text_get(EWL_LABEL(label));
	if (strcmp(val, "some text"))
		snprintf(buf, len, "%s is not 'some text'", val);
	else
		ret = 1;

	ewl_widget_destroy(label);

	return ret;
}
