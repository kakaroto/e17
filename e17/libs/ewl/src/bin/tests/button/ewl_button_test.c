/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_checkbutton.h"
#include "ewl_radiobutton.h"
#include "ewl_separator.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

/**
 * @addtogroup Ewl_Button
 * @section text_tut Tutorial
 *
 * The Ewl_Button includes an icon and label by default but is actually a full
 * container that can hold any type of non-toplevel widget. Because of this
 * flexibility many other widget types inherit from the button.
 *
 * @code
 * Ewl_Widget *button;
 * button = ewl_button_new();
 * ewl_button_label_set(EWL_BUTTON(button), "Button label");
 * ewl_widget_show(button);
 * @endcode
 *
 */

static int create_test(Ewl_Container *win);

static int label_test_set_get(char *buf, int len);
static int image_null_test_get(char *buf, int len);
static int image_null_test_set_get(char *buf, int len);
static int image_nonexist_relative_test_set_get(char *buf, int len);
static int image_size_null_height_test_set_get(char *buf, int len);
static int image_size_null_height_test_set_get(char *buf, int len);
static int image_size_null_width_test_set_get(char *buf, int len);
static int image_size_match_test_set_get(char *buf, int len);
static int image_size_differ_test_set_get(char *buf, int len);
static int image_size_max_int_test_set_get(char *buf, int len);

static Ewl_Unit_Test button_unit_tests[] = {
		{"label set/get", label_test_set_get, NULL, -1, 0},
		{"image null get", image_null_test_get, NULL, -1, 0},
		{"image null set/get", image_null_test_set_get, NULL, -1, 0},
		{"image nonexistent set/get", image_nonexist_relative_test_set_get, NULL, -1, 0},
		{"image size null height set/get", image_size_null_height_test_set_get, NULL, -1 , 0},
		{"image size null width set/get", image_size_null_width_test_set_get, NULL, -1, 0},
		{"image size match set/get", image_size_match_test_set_get, NULL, -1, 0},
		{"image size differ set/get", image_size_differ_test_set_get, NULL, -1, 0},
		{"image size max int set/get", image_size_max_int_test_set_get, NULL, -1, 0},
		{NULL, NULL, NULL, -1, 0}
	};

void
test_info(Ewl_Test *test)
{
	test->name = "Button";
	test->tip = "The button class is a basic button\n"
		"with a label. This class inherits from\n"
		"the Ewl_Box to allow for placing any\n"
		"other widgets inside the button.";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_SIMPLE;
	test->unit_tests = button_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *hbox;
	Ewl_Widget *vbox;
	Ewl_Widget *separator[2];
	Ewl_Widget *button[3];
	Ewl_Widget *check_button[4];
	Ewl_Widget *radio_button[3];

	/*
	 * Pack the button tests horizontally.
	 */
	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), hbox);
	ewl_widget_show(hbox);

	/*
	 * Use a vertical box for the buttons in a test area.
	 */
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), vbox);
	ewl_widget_show(vbox);

	/*
	 * Create a button to be displayed witha label.
	 */
	button[0] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[0]), "With Label");
	ewl_container_child_append(EWL_CONTAINER(vbox), button[0]);
	ewl_object_alignment_set(EWL_OBJECT(button[0]), EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(button[0]);

	/*
	 * Create a button that does not contain a label
	 */
	button[1] = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), button[1]);
	ewl_object_alignment_set(EWL_OBJECT(button[1]), EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(button[1]);

	/*
	 * Create a button that's disabled
	 */
	button[2] = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), button[2]);
	ewl_object_alignment_set(EWL_OBJECT(button[2]), EWL_FLAG_ALIGN_LEFT);
	ewl_button_label_set(EWL_BUTTON(button[2]), "Disabled");
	ewl_widget_disable(button[2]);
	ewl_widget_show(button[2]);

	/*
	 * Add a separator between the classic buttons and the check buttons.
	 */
	separator[0] = ewl_vseparator_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), separator[0]);
	ewl_widget_show(separator[0]);

	/*
	 * Use a vertical box for the buttons in a test area.
	 */
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), vbox);
	ewl_widget_show(vbox);

	/*
	 * Create a check button with a label.
	 */
	check_button[0]  = ewl_checkbutton_new();
	ewl_button_label_set(EWL_BUTTON(check_button[0] ), "With Label");
	ewl_container_child_append(EWL_CONTAINER(vbox), check_button[0]);
	ewl_widget_show(check_button[0]);

	/*
	 * Create a check button with a label and checked.
	 */
	check_button[1]  = ewl_checkbutton_new();
	ewl_button_label_set(EWL_BUTTON(check_button[1] ), 
						"With Label and checked");
	ewl_checkbutton_checked_set(EWL_CHECKBUTTON(check_button[1]), TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), check_button[1]);
	ewl_widget_show(check_button[1]);

	/*
	 * Create a check button w/o a label.
	 */
	check_button[2]  = ewl_checkbutton_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), check_button[2]);
	ewl_widget_show(check_button[2]);

	/*
	 * Create a check button thats disabled
	 */
	check_button[3]  = ewl_checkbutton_new();
	ewl_button_label_set(EWL_BUTTON(check_button[3] ), "Disabled");
	ewl_checkbutton_checked_set(EWL_CHECKBUTTON(check_button[3]), TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), check_button[3]);
	ewl_widget_disable(check_button[3]);
	ewl_widget_show(check_button[3]);

	/*
	 * Add a separator between the check buttons and the radio buttons
	 */
	separator[1] = ewl_vseparator_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), separator[1]);
	ewl_widget_show(separator[1]);

	/*
	 * Use a vertical box for the buttons in a test area.
	 */
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), vbox);
	ewl_widget_show(vbox);

	/*
	 * Add a radio button with
	 */
	radio_button[0]  = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(radio_button[0] ), "With Label");
	ewl_checkbutton_checked_set(EWL_CHECKBUTTON(radio_button[0]), TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), radio_button[0]);
	ewl_widget_show(radio_button[0]);

	radio_button[1]  = ewl_radiobutton_new();
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(radio_button[1]),
				  EWL_RADIOBUTTON(radio_button[0]));
	ewl_container_child_append(EWL_CONTAINER(vbox), radio_button[1]);
	ewl_widget_show(radio_button[1]);

	radio_button[2]  = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(radio_button[2] ), "Disabled");
	ewl_checkbutton_checked_set(EWL_CHECKBUTTON(radio_button[2]), TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), radio_button[2]);
	ewl_widget_disable(radio_button[2]);
	ewl_widget_show(radio_button[2]);

	return 1;
}

static int
label_test_set_get(char *buf, int len)
{
	Ewl_Widget *button;
	int ret = 0;

	button = ewl_button_new();

	ewl_button_label_set(EWL_BUTTON(button), "my_label");
	if (strcmp("my_label", ewl_button_label_get(EWL_BUTTON(button))))
		LOG_FAILURE(buf, len, "label_get dosen't match label_set");
	else
		ret = 1;

	ewl_widget_destroy(button);

	return ret;
}

static int
image_null_test_get(char *buf, int len)
{
	Ewl_Widget *button;
	int ret = 0;

	button = ewl_button_new();

	if (ewl_button_image_get(EWL_BUTTON(button)))
		LOG_FAILURE(buf, len, "image_get not NULL");
	else
		ret = 1;

	ewl_widget_destroy(button);

	return ret;
}

static int
image_null_test_set_get(char *buf, int len)
{
	Ewl_Widget *button;
	const char *val;
	int ret = 0;

	button = ewl_button_new();

	ewl_button_image_set(EWL_BUTTON(button), NULL, NULL);
	val = ewl_button_image_get(EWL_BUTTON(button));
	if (val)
		LOG_FAILURE(buf, len, "image_get %s when set to NULL", val);
	else
		ret = 1;

	ewl_widget_destroy(button);

	return ret;
}

static int
image_nonexist_relative_test_set_get(char *buf, int len)
{
	Ewl_Widget *button;
	int ret = 0;

	button = ewl_button_new();

	ewl_button_image_set(EWL_BUTTON(button), "my_image", NULL);
	if (strcmp("my_image", ewl_button_image_get(EWL_BUTTON(button))))
		LOG_FAILURE(buf, len, "image_get dosen't match image_set");
	else
		ret = 1;

	ewl_widget_destroy(button);

	return ret;
}

static int
image_size_null_height_test_set_get(char *buf, int len)
{
	Ewl_Widget *button;
	int w = 0;
	int ret = 0;

	button = ewl_button_new();

	ewl_button_image_size_get(EWL_BUTTON(button), &w, NULL);
	if (w != 0)
		LOG_FAILURE(buf, len, "image_size_get width not 0");
	else
		ret = 1;

	ewl_widget_destroy(button);

	return ret;
}

static int
image_size_null_width_test_set_get(char *buf, int len)
{
	Ewl_Widget *button;
	int h = 0;
	int ret = 0;

	button = ewl_button_new();

	ewl_button_image_size_get(EWL_BUTTON(button), NULL, &h);
	if (h != 0)
		LOG_FAILURE(buf, len, "image_size_get height not 0");
	else
		ret = 1;

	ewl_widget_destroy(button);

	return ret;
}

static int
image_size_match_test_set_get(char *buf, int len)
{
	Ewl_Widget *button;
	int w = 0, h = 0;
	int ret = 0;

	button = ewl_button_new();

	ewl_button_image_size_set(EWL_BUTTON(button), 32, 32);
	ewl_button_image_size_get(EWL_BUTTON(button), &w, &h);
	if (w != 32 || h != 32)
		LOG_FAILURE(buf, len, "image_size_get width and height don't match");
	else
		ret = 1;

	ewl_widget_destroy(button);

	return ret;
}

static int
image_size_differ_test_set_get(char *buf, int len)
{
	Ewl_Widget *button;
	int w = 0, h = 0;
	int ret = 0;

	button = ewl_button_new();
	ewl_button_image_size_set(EWL_BUTTON(button), 30, 24);
	ewl_button_image_size_get(EWL_BUTTON(button), &w, &h);
	if (w != 30 || h != 24)
		LOG_FAILURE(buf, len, "image_size_get width and height don't differ");
	else
		ret = 1;

	ewl_widget_destroy(button);

	return ret;
}

static int
image_size_max_int_test_set_get(char *buf, int len)
{
	Ewl_Widget *button;
	int w = 0, h = 0;
	int ret = 0;

	button = ewl_button_new();
	ewl_button_image_size_set(EWL_BUTTON(button), INT_MAX, INT_MAX);
	ewl_button_image_size_get(EWL_BUTTON(button), &w, &h);
	if (w != INT_MAX|| h != INT_MAX)
		LOG_FAILURE(buf, len, "image_size_get width and height not INT_MAX");
	else
		ret = 1;

	ewl_widget_destroy(button);

	return ret;
}
