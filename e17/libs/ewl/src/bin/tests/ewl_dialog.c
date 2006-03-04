#include "Ewl_Test2.h"
#include <stdio.h>

static int create_test(Ewl_Container *box);
static void dialog_response_cb(Ewl_Widget *w, void *ev, void *data);
static void run_dialog(Ewl_Widget *w, void *ev, void *data);
static void dialog_delete_cb(Ewl_Widget *w, void *ev, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Dialog";
	test->tip = "Defines a dialog window.";
	test->filename = "ewl_dialog.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_ADVANCED;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *o;

	o = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(o), "");
	ewl_container_child_append(box, o);
	ewl_widget_name_set(o, "dialog_label");
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Lanuch Dialog");
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, run_dialog, NULL);
	ewl_container_child_append(box, o);
	ewl_widget_show(o);

	return 1;
}

static void
run_dialog(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Widget *label, *button, *dialog, *hbox;

	dialog = ewl_dialog_new();
	ewl_window_title_set(EWL_WINDOW(dialog), "Dialog Test");
	ewl_window_name_set(EWL_WINDOW(dialog), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(dialog), "EFL Test Application");
	ewl_callback_append(dialog, EWL_CALLBACK_DELETE_WINDOW,
						dialog_delete_cb, NULL);
	ewl_widget_show(dialog);
	ewl_dialog_active_area_set(EWL_DIALOG(dialog), EWL_POSITION_TOP);

        hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(dialog), hbox);
	ewl_widget_show(hbox);

	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), "blah blah blah");
	ewl_container_child_append(EWL_CONTAINER(hbox), label);
	ewl_widget_show(label);

	label = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(label), "This is a test for the Dialog widget");
	ewl_container_child_append(EWL_CONTAINER(hbox), label);
	ewl_widget_show(label);

	button = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_button_label_set(EWL_BUTTON(button), "tooltip");
	ewl_attach_tooltip_text_set(button, "This is a tooltip");
	ewl_widget_show(button);

	ewl_dialog_active_area_set(EWL_DIALOG(dialog), EWL_POSITION_BOTTOM);

	button = ewl_button_new();
	ewl_button_stock_type_set(EWL_BUTTON(button), EWL_STOCK_OK);
	ewl_container_child_append(EWL_CONTAINER(dialog), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED,
			dialog_response_cb, dialog);
	ewl_widget_show(button);

	button = ewl_button_new();
	ewl_button_stock_type_set(EWL_BUTTON(button), EWL_STOCK_CANCEL);
	ewl_container_child_append(EWL_CONTAINER(dialog), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED,
			dialog_response_cb, dialog);
	ewl_widget_show(button);

	ewl_dialog_active_area_set(EWL_DIALOG(dialog), EWL_POSITION_TOP);
	ewl_widget_show(dialog);
}

static void
dialog_response_cb(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	Ewl_Stock_Type response;
	Ewl_Widget *o;

	o = ewl_widget_name_find("dialog_label");
	response = ewl_button_stock_type_get(EWL_BUTTON(w));
	if (response == EWL_STOCK_OK)
		ewl_label_text_set(EWL_LABEL(o), "OK button clicked.");
	else
		ewl_label_text_set(EWL_LABEL(o), "Cancel button clicked.");

	/* get rid of the dialog box */
	ewl_widget_destroy(EWL_WIDGET(data));
}

static void
dialog_delete_cb(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	ewl_widget_destroy(w);
}


