#include "Ewl_Test.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @addtogroup Ewl_Text
 * @section text_tut Tutorial
 *
 * You can use the text widget to display text stuff.
 * This is another line.
 *
 * @code
 * ewl_text_text_set(EWL_TEXT(t), "foo");
 * @endcode
 */

static int create_test(Ewl_Container *box);
static void trigger_cb_mouse_out(Ewl_Widget *w, void *ev, void *data);
static void trigger_cb_mouse_in(Ewl_Widget *w, void *ev, void *data);
static void trigger_cb(Ewl_Widget *w, void *ev, void *data);

static int text_test_set_get(char *buf, int len);

static Ewl_Unit_Test text_unit_tests[] = {
		{"text set/get", text_test_set_get},	
		{NULL, NULL}
	};

void 
test_info(Ewl_Test *test)
{
	test->name = "Text";
	test->tip = "Defines a class for multi-line text layout\n"
		"and formatting.";
	test->filename = "ewl_text.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_SIMPLE;
	test->unit_tests = text_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *o;
	Ewl_Text_Trigger *trigger;
	int len;

       o = ewl_text_new();
	ewl_widget_name_set(o, "text");
	ewl_text_selectable_set(EWL_TEXT(o), TRUE);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	ewl_text_text_insert(EWL_TEXT(o), "The first bunch of text\n", 0); /* 24 */

	ewl_text_cursor_position_set(EWL_TEXT(o), 10);
	ewl_text_color_apply(EWL_TEXT(o), 0, 0, 255, 255, 5);
	ewl_text_text_append(EWL_TEXT(o), "The second bunch of text\n"); /* 25 */

	ewl_text_font_size_set(EWL_TEXT(o), 20);
	ewl_text_styles_set(EWL_TEXT(o), EWL_TEXT_STYLE_DOUBLE_UNDERLINE
						| EWL_TEXT_STYLE_OUTLINE
						| EWL_TEXT_STYLE_SOFT_SHADOW);
	ewl_text_double_underline_color_set(EWL_TEXT(o), 50, 50, 50, 255);
	ewl_text_shadow_color_set(EWL_TEXT(o), 128, 128, 128, 128);
	ewl_text_outline_color_set(EWL_TEXT(o), 200, 200, 200, 200);
	ewl_text_text_append(EWL_TEXT(o), "The third bunch of text\n"); /* 24 */

	ewl_text_text_insert(EWL_TEXT(o), "The fourth bunch of text\n", 31); /* 25 */

	trigger = ewl_text_trigger_new(EWL_TEXT_TRIGGER_TYPE_TRIGGER);
	ewl_text_trigger_start_pos_set(trigger, ewl_text_length_get(EWL_TEXT(o)));
	ewl_text_cursor_position_set(EWL_TEXT(o), ewl_text_length_get(EWL_TEXT(o)));
	ewl_text_styles_set(EWL_TEXT(o), EWL_TEXT_STYLE_NONE);
	ewl_text_text_append(EWL_TEXT(o), "This is the link."); /* 17 */

	len = ewl_text_cursor_position_get(EWL_TEXT(o)) -
			ewl_text_trigger_start_pos_get(trigger);
	ewl_text_trigger_length_set(trigger, len);

	ewl_container_child_append(EWL_CONTAINER(o), EWL_WIDGET(trigger));
	ewl_callback_append(EWL_WIDGET(trigger), EWL_CALLBACK_MOUSE_UP,
			trigger_cb, "You clicked the trigger, have a cookie.");
	ewl_callback_append(EWL_WIDGET(trigger), EWL_CALLBACK_MOUSE_IN,
			trigger_cb_mouse_in, NULL);
	ewl_callback_append(EWL_WIDGET(trigger), EWL_CALLBACK_MOUSE_OUT,
			trigger_cb_mouse_out, NULL);

	ewl_text_text_insert(EWL_TEXT(o), "The fifth bunch of text\n", 0); /* 24 */

	ewl_text_cursor_position_set(EWL_TEXT(o), 0);
	ewl_text_color_apply(EWL_TEXT(o), 255, 0, 0, 255, 24);

	ewl_text_text_insert(EWL_TEXT(o), "The sixth bunch of text\n", 24); /* 24 */

	ewl_text_cursor_position_set(EWL_TEXT(o), 43);
	ewl_text_color_apply(EWL_TEXT(o), 0, 255, 0, 255, 14);

	ewl_text_cursor_position_set(EWL_TEXT(o), ewl_text_length_get(EWL_TEXT(o)));
	ewl_text_color_set(EWL_TEXT(o), 255, 0, 0, 255);
	ewl_text_text_append(EWL_TEXT(o), "And in red\n"); /* 11 */

	ewl_text_color_set(EWL_TEXT(o), 0, 0, 0, 255);
	ewl_text_text_append(EWL_TEXT(o), "Once more with feeling. ");

	trigger = ewl_text_trigger_new(EWL_TEXT_TRIGGER_TYPE_TRIGGER);
	ewl_text_trigger_start_pos_set(trigger, ewl_text_length_get(EWL_TEXT(o)));

	ewl_text_text_append(EWL_TEXT(o), "This is the multi\n\nline link."); /* 28 */
	len = ewl_text_cursor_position_get(EWL_TEXT(o)) -
			ewl_text_trigger_start_pos_get(trigger);
	ewl_text_trigger_length_set(trigger, len);

	ewl_container_child_append(EWL_CONTAINER(o), EWL_WIDGET(trigger));
	ewl_callback_append(EWL_WIDGET(trigger), EWL_CALLBACK_MOUSE_UP,
			trigger_cb, "You clicked the multi-line trigger, have a coke.");
	ewl_callback_append(EWL_WIDGET(trigger), EWL_CALLBACK_MOUSE_IN,
			trigger_cb_mouse_in, NULL);
	ewl_callback_append(EWL_WIDGET(trigger), EWL_CALLBACK_MOUSE_OUT,
			trigger_cb_mouse_out, NULL);

	ewl_text_color_set(EWL_TEXT(o), 255, 0, 255, 255);
	ewl_text_text_append(EWL_TEXT(o), "ONE MORE SEGV");

#if 0
	printf("-- DUMP --\n");
	ewl_text_tree_dump((EWL_TEXT(o))->formatting, "");
	printf("-- DUMP DONE --\n");
#endif

	return 1;
}

static void
trigger_cb(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data)
{
	printf("%s\n", (char *)data);
}

static void
trigger_cb_mouse_in(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Text_Trigger *t;

	t = EWL_TEXT_TRIGGER(w);

	ewl_text_cursor_position_set(EWL_TEXT(t->text_parent), t->pos);
	ewl_text_color_apply(EWL_TEXT(t->text_parent), 255, 0, 0, 255, t->len);
}

static void
trigger_cb_mouse_out(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Text_Trigger *t;

	t = EWL_TEXT_TRIGGER(w);

	ewl_text_cursor_position_set(EWL_TEXT(t->text_parent), t->pos);
	ewl_text_color_apply(EWL_TEXT(t->text_parent), 0, 0, 0, 255, t->len);
}

static int
text_test_set_get(char *buf, int len)
{
	Ewl_Widget *o;
	char *t;
	int ret = 0;

	o = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(o), "This is the test text.");
	t = ewl_text_text_get(EWL_TEXT(o));

	if (strcmp(t, "This is the test text."))
		snprintf(buf, len, "text_get did not match text_set.");
	else
		ret = 1;

	return ret;
}


