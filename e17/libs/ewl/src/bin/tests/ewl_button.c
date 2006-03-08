#include "Ewl_Test2.h"
#include <stdio.h>

static int create_test(Ewl_Container *win);

void 
test_info(Ewl_Test *test)
{
	test->name = "Button";
	test->tip = "The button class is a basic button\n"
		"with a label. This class inherits from\n"
		"the Ewl_Box to allow for placing any\n"
		"other widgets inside the button.";
	test->filename = "ewl_button.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_SIMPLE;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *hbox;
	Ewl_Widget *vbox;
	Ewl_Widget *separator[2];
	Ewl_Widget *button[2];
	Ewl_Widget *check_button[3];
	Ewl_Widget *radio_button[2];

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
	ewl_object_alignment_set(EWL_OBJECT(check_button[0]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(check_button[0]);

	/*
	 * Create a check button with a label and checked.
	 */
	check_button[1]  = ewl_checkbutton_new();
	ewl_button_label_set(EWL_BUTTON(check_button[1] ), "With Label and checked");
	ewl_checkbutton_checked_set(EWL_CHECKBUTTON(check_button[1]), TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), check_button[1]);
	ewl_object_alignment_set(EWL_OBJECT(check_button[1]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(check_button[1]);

	/*
	 * Create a check button w/o a label.
	 */
	check_button[2]  = ewl_checkbutton_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), check_button[2]);
	ewl_object_alignment_set(EWL_OBJECT(check_button[2]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(check_button[2]);

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
	ewl_radiobutton_checked_set(radio_button[0], TRUE);
	ewl_container_child_append(EWL_CONTAINER(vbox), radio_button[0]);
	ewl_object_alignment_set(EWL_OBJECT(radio_button[0]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(radio_button[0]);

	radio_button[1]  = ewl_radiobutton_new();
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(radio_button[1]),
				  EWL_RADIOBUTTON(radio_button[0]));
	ewl_container_child_append(EWL_CONTAINER(vbox), radio_button[1]);
	ewl_object_alignment_set(EWL_OBJECT(radio_button[1]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(radio_button[1]);

	return 1;
}

