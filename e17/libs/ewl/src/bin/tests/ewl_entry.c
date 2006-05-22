#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @addtogroup Ewl_Entry
 * @section text_tut Tutorial
 *
 * The Ewl_Entry provides a widget for editing single line or multi-line text.
 * Since the entry widget inherits from Ewl_Text, all operations that
 * can be performed on text can be used on the entry, including programmatic
 * control of formatting, selections and text contents.
 *
 * @code
 * Ewl_Widget *entry;
 * entry = ewl_entry_new();
 * ewl_text_text_set(EWL_TEXT(entry), "Text to display");
 * ewl_widget_show(entry);
 * @endcode
 *
 * Almost all manipulation of the entry widget is performed through the
 * Ewl_Text API, with some minor exceptions. Since the entry widget takes user
 * input, it requires an API to enable or disable the editing capability.
 *
 * @code
 * void ewl_entry_editable_set(Ewl_Entry *e, unsigned int editable);
 * unsigned int ewl_entry_editable_get(Ewl_Entry *e);
 * @endcode
 *
 * It is also important to be able to indicate if the text entry can contain
 * multiple lines of text, as the reaction to the Enter/Return keys changes.
 * When multiline is enabled, Enter will insert a carriage return in the text,
 * otherwise it raises an EWL_CALLBACK_VALUE_CHANGED callback on the entry
 * widget.
 *
 * @code
 * void ewl_entry_multiline_set(Ewl_Entry *e, unsigned int multiline);
 * unsigned int ewl_entry_multiline_get(Ewl_Entry *e);
 * @endcode
 *
 * By inheriting from Ewl_Text, the capabalities of Ewl_Entry are expanded to
 * the full API available for text manipulation. This reduces the number of
 * calls for the user to learn without sacrificing functionality.
 */

static Ewl_Widget *entry[3];

static int create_test(Ewl_Container *box);

void 
test_info(Ewl_Test *test)
{
	test->name = "Entry";
	test->tip = "Defines the Ewl_Entry class to allow\n"
		"for single line editable text.";
	test->filename = "ewl_entry.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_SIMPLE;
}

static void
fetch_entry_text(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, 
				void *user_data __UNUSED__)
{
	char *s;

	s = ewl_text_text_get(EWL_TEXT(entry[0]));
	if (s) {
		printf("First entry covers: %s\n", s);
		free(s);
	}

	s = ewl_text_text_get(EWL_TEXT(entry[1]));
	if (s) {
		printf("Second entry covers: %s\n", s);
		free(s);
	}

	s = ewl_text_text_get(EWL_TEXT(entry[2]));
	if (s) {
		printf("Third entry covers: %s\n", s);
		free(s);
	}
}

static void
set_entry_text(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
{
	ewl_text_color_set(EWL_TEXT(entry[0]), 0, 0, 0, 255);
	ewl_text_color_set(EWL_TEXT(entry[1]), 0, 0, 0, 255);
	ewl_text_color_set(EWL_TEXT(entry[2]), 0, 0, 0, 255);

	ewl_text_text_set(EWL_TEXT(entry[0]), "Play with me ?");
	ewl_text_text_set(EWL_TEXT(entry[1]), "E W L ! ! !");

	ewl_text_clear(EWL_TEXT(entry[2]));
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *entry_box;
	Ewl_Widget *button_hbox, *o;
	Ewl_Widget *button[2];

	/*
	 * Create the main box for holding the widgets
	 */
	entry_box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), entry_box);
	ewl_box_spacing_set(EWL_BOX(entry_box), 10);
	ewl_widget_show(entry_box);

	entry[0] = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(entry[0]), "Play with me ?");
	ewl_entry_multiline_set(EWL_ENTRY(entry[0]), 1);
	ewl_text_color_set(EWL_TEXT(entry[0]), 255, 0, 0, 255);
	ewl_object_padding_set(EWL_OBJECT(entry[0]), 5, 5, 5, 0);
	ewl_container_child_append(EWL_CONTAINER(entry_box), entry[0]);
	ewl_widget_show(entry[0]);

	entry[1] = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(entry[1]), "E W L ! ! !");
	ewl_text_color_set(EWL_TEXT(entry[1]), 255, 0, 0, 255);
	ewl_object_padding_set(EWL_OBJECT(entry[1]), 5, 5, 0, 0);
	ewl_container_child_append(EWL_CONTAINER(entry_box), entry[1]);
	ewl_widget_show(entry[1]);

	entry[2] = ewl_entry_new();
	ewl_object_padding_set(EWL_OBJECT(entry[2]), 5, 5, 0, 0);
	ewl_container_child_append(EWL_CONTAINER(entry_box), entry[2]);
	ewl_widget_show(entry[2]);

	o = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(o), "disabled");
	ewl_object_padding_set(EWL_OBJECT(o), 5, 5, 0, 0);
	ewl_container_child_append(EWL_CONTAINER(entry_box), o);
	ewl_widget_disable(o);
	ewl_widget_show(o);

	button_hbox = ewl_hbox_new();
	ewl_object_alignment_set(EWL_OBJECT(button_hbox), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(entry_box), button_hbox);
	ewl_box_spacing_set(EWL_BOX(button_hbox), 5);
	ewl_widget_show(button_hbox);

	button[0] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[0]), "Fetch text");
	ewl_container_child_append(EWL_CONTAINER(button_hbox), button[0]);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
				fetch_entry_text, NULL);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[1]), "Set Text");
	ewl_container_child_append(EWL_CONTAINER(button_hbox), button[1]);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
				set_entry_text, NULL);
	ewl_widget_show(button[1]);

	o = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(o), "Text insert \"\"");
	ewl_container_child_append(EWL_CONTAINER(button_hbox), o);
	ewl_widget_show(o);

	o = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(o), "");
	ewl_container_child_append(EWL_CONTAINER(button_hbox), o);
	ewl_widget_show(o);

	return 1;
}


