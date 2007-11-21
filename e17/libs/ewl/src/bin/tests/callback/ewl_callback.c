/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include <limits.h>
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int append_test_id(char *buf, int len);
static int prepend_test_id(char *buf, int len);
static int insert_after_test_id(char *buf, int len);
static int shared_test_id(char *buf, int len);
static int unique_test_id(char *buf, int len);
static int del_test_call(char *buf, int len);
static int clear_test_call(char *buf, int len);
static int append_test_call(char *buf, int len);
static int prepend_test_call(char *buf, int len);

static void base_callback(Ewl_Widget *w, void *event, void *data);
static void differing_callback(Ewl_Widget *w, void *event, void *data);

static Ewl_Unit_Test callback_unit_tests[] = {
		{"append/get id", append_test_id, NULL, -1, 0},
		{"prepend/get id", prepend_test_id, NULL, -1, 0},
		{"insert after/get id", insert_after_test_id, NULL, -1, 0},
		{"shared id", shared_test_id, NULL, -1, 0},
		{"unique id", unique_test_id, NULL, -1, 0},
		{"del/call", del_test_call, NULL, -1, 0},
		{"clear/call", clear_test_call, NULL, -1, 0},
		{"append/call", append_test_call, NULL, -1, 0},
		{"prepend/call", prepend_test_call, NULL, -1, 0},
		{NULL, NULL, NULL, -1, 0}
	};

void
test_info(Ewl_Test *test)
{
	test->name = "Callback";
	test->tip = "The base callback manipulation.";
	test->filename = __FILE__;
	test->type = EWL_TEST_TYPE_MISC;
	test->unit_tests = callback_unit_tests;
}

/*
 * Append a callback and verify that the returned id is valid.
 */
static int
append_test_id(char *buf, int len)
{
	Ewl_Widget *w;
	int id;
	int ret = 0;

	w = ewl_widget_new();
	id = ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, base_callback,
			NULL);

	if (id)
		ret = 1;
	else
		snprintf(buf, len, "invalid callback id returned");

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Prepend a callback and verify that the returned id is valid.
 */
static int
prepend_test_id(char *buf, int len)
{
	Ewl_Widget *w;
	int id;
	int ret = 0;

	w = ewl_widget_new();
	id = ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback,
			NULL);

	if (id)
		ret = 1;
	else
		snprintf(buf, len, "invalid callback id returned");

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Insert a callback and verify that the returned id is valid.
 */
static int
insert_after_test_id(char *buf, int len)
{
	Ewl_Widget *w;
	int id;
	int ret = 0;

	w = ewl_widget_new();
	ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback,
			NULL);
	id = ewl_callback_insert_after(w, EWL_CALLBACK_CONFIGURE, base_callback,
			w, base_callback, NULL);

	if (id)
		ret = 1;
	else
		snprintf(buf, len, "invalid callback id returned");

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Append a duplicate callback and verify that the id's match.
 */
static int
shared_test_id(char *buf, int len)
{
	Ewl_Widget *w;
	int id, id2;
	int ret = 0;

	w = ewl_widget_new();
	id = ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, base_callback,
			NULL);
	id2 = ewl_callback_append(w, EWL_CALLBACK_REALIZE, base_callback,
			NULL);

	if (id == id2)
		ret = 1;
	else
		snprintf(buf, len, "callback id's don't match");

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Append a callbacks that vary in the function and data, and verify that the
 * id's differ.
 */
static int
unique_test_id(char *buf, int len)
{
	Ewl_Widget *w;
	int id, id2;
	int ret = 0;

	w = ewl_widget_new();
	id = ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, base_callback,
			NULL);
	id2 = ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, base_callback,
			w);

	if (id != id2) {
		id = ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
				differing_callback, w);
		if (id != id2)
			ret = 1;
		else
			snprintf(buf, len, "callback with different functions"
				       " id's match");
	}
	else
		snprintf(buf, len, "callback with different data id's match");

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Prepend a callback and verify that clearing the chain prevents it from being
 * called.
 */
static int
del_test_call(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback, NULL);
	ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
	ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

	if ((long)ewl_widget_data_get(w, w) != 1)
		ret = 1;
	else
		snprintf(buf, len, "del_type failed to remove callback");

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Prepend a callback and verify that clearing the chain prevents it from being
 * called.
 */
static int
clear_test_call(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback, NULL);
	ewl_callback_clear(w);
	ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

	if ((long)ewl_widget_data_get(w, w) != 1)
		ret = 1;
	else
		snprintf(buf, len, "clear failed to remove callback");

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Append a callback and verify that calling the chain triggers the callback.
 */
static int
append_test_call(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, base_callback, NULL);
	ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

	if ((long)ewl_widget_data_get(w, w) == 1)
		ret = 1;
	else
		snprintf(buf, len, "callback function not called");

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Prepend a callback and verify that calling the chain triggers the callback.
 */
static int
prepend_test_call(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_callback_del_type(w, EWL_CALLBACK_CONFIGURE);
	ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, base_callback, NULL);
	ewl_callback_call(w, EWL_CALLBACK_CONFIGURE);

	if ((long)ewl_widget_data_get(w, w) == 1)
		ret = 1;
	else
		snprintf(buf, len, "callback function not called");

	ewl_widget_destroy(w);

	return ret;
}

static void
base_callback(Ewl_Widget *w, void *event, void *data)
{
	ewl_widget_data_set(w, w, (void *)(long)1);
	event = data = NULL;
	return;
}

static void
differing_callback(Ewl_Widget *w, void *event, void *data)
{
	ewl_widget_data_set(w, w, (void *)(long)2);
	event = data = NULL;
	return;
}
