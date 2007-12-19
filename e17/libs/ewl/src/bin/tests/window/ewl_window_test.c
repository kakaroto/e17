/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_window.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int title_set_get(char *buf, int len);
static int name_set_get(char *buf, int len);
static int class_set_get(char *buf, int len);


/*
 * This set of tests is targeted at window
 */

static Ewl_Unit_Test window_unit_tests[] = {
		{"title set/get", title_set_get, NULL, -1, 0},
		{"name set/get", name_set_get, NULL, -1, 0},
		{"class set/get", class_set_get, NULL, -1, 0},
		{NULL, NULL, NULL, -1, 0}
	};

void
test_info(Ewl_Test *test)
{
	test->name = "Window";
	test->tip = "The window system.";
	test->filename = __FILE__;
	test->type = EWL_TEST_TYPE_MISC;
	test->unit_tests = window_unit_tests;
}

/*
 * Set a string to a new window title and retrieve it again
 */
static int
title_set_get(char *buf, int len)
{
	Ewl_Widget *win;
	const char *title;
	int ret = 0;

	win = ewl_window_new();
	title = ewl_window_title_get(EWL_WINDOW(win));

	if (title && *title)
		snprintf(buf, len, "default title set to %s", title);
	else {
		ewl_window_title_set(EWL_WINDOW(win), "A title");
		title = ewl_window_title_get(EWL_WINDOW(win));

		if (strcmp(title, "A title"))
			snprintf(buf, len, "incorrect title set");
		else
			ret = 1;
	}

	ewl_widget_destroy(win);

	return ret;
}

/*
 * Set a string to a new window name and retrieve it again
 */
static int
name_set_get(char *buf, int len)
{
	Ewl_Widget *win;
	const char *name;
	int ret = 0;

	win = ewl_window_new();
	name = ewl_window_name_get(EWL_WINDOW(win));

	if (name && *name)
		snprintf(buf, len, "default name set to %s", name);
	else {
		ewl_window_name_set(EWL_WINDOW(win), "A name");
		name = ewl_window_name_get(EWL_WINDOW(win));

		if (strcmp(name, "A name"))
			snprintf(buf, len, "incorrect name set");
		else
			ret = 1;
	}

	ewl_widget_destroy(win);

	return ret;
}

/*
 * Set a string to a new window class and retrieve it again
 */
static int
class_set_get(char *buf, int len)
{
	Ewl_Widget *win;
	const char *class;
	int ret = 0;

	win = ewl_window_new();
	class = ewl_window_class_get(EWL_WINDOW(win));

	if (class && *class)
		snprintf(buf, len, "default class set to %s", class);
	else {
		ewl_window_class_set(EWL_WINDOW(win), "A class");
		class = ewl_window_class_get(EWL_WINDOW(win));

		if (strcmp(class, "A class"))
			snprintf(buf, len, "incorrect class set");
		else
			ret = 1;
	}

	ewl_widget_destroy(win);

	return ret;
}
