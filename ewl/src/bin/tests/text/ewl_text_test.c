/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_text_fmt.h"
#include "ewl_text_trigger.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int create_test(Ewl_Container *box);
static void trigger_cb_mouse_out(Ewl_Widget *w, void *ev, void *data);
static void trigger_cb_mouse_in(Ewl_Widget *w, void *ev, void *data);
static void trigger_cb(Ewl_Widget *w, void *ev, void *data);

extern Ewl_Unit_Test text_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Text";
        test->tip = "Defines a class for multi-line text layout\n"
                "and formatting.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
        test->unit_tests = text_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *o, *trigger;
        int len;

        o = ewl_text_new();
        ewl_widget_name_set(o, "text");
        ewl_text_selectable_set(EWL_TEXT(o), TRUE);
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        printf("Insert 'The first bunch of text\\n' [24]\n");
        ewl_text_text_insert(EWL_TEXT(o), "The first bunch of text\n", 0); /* 24 */

        printf("Cursor position\n");
        ewl_text_cursor_position_set(EWL_TEXT(o), 10);

        printf("Colour apply\n");
        ewl_text_color_apply(EWL_TEXT(o), 0, 0, 255, 255, 5);

        printf("Appending 'The second bunch of text\\n' [49]\n");
        ewl_text_text_append(EWL_TEXT(o), "The second bunch of text\n"); /* 25 */

        printf("Font size set\n");
        ewl_text_font_size_set(EWL_TEXT(o), 20);

        printf("Styles set\n");
        ewl_text_styles_set(EWL_TEXT(o), EWL_TEXT_STYLE_DOUBLE_UNDERLINE
                                                | EWL_TEXT_STYLE_OUTLINE
                                                | EWL_TEXT_STYLE_SOFT_SHADOW);
        printf("Double underline colour set\n");
        ewl_text_double_underline_color_set(EWL_TEXT(o), 50, 50, 50, 255);

        printf("Shadow colour set\n");
        ewl_text_shadow_color_set(EWL_TEXT(o), 128, 128, 128, 128);

        printf("Outline colour set\n");
        ewl_text_outline_color_set(EWL_TEXT(o), 200, 200, 200, 200);

        printf("Appending 'The third bunch of text\\n' [73]\n");
        ewl_text_text_append(EWL_TEXT(o), "The third bunch of text\n"); /* 24 */

        printf("Inserting 'The fourth bunch of text\\n' [98]\n");
        ewl_text_text_insert(EWL_TEXT(o), "The fourth bunch of text\n", 31); /* 25 */

        printf("Creating trigger [115]\n");
        trigger = ewl_text_trigger_new(EWL_TEXT_TRIGGER_TYPE_TRIGGER);
        ewl_text_trigger_start_pos_set(EWL_TEXT_TRIGGER(trigger), 
                                        ewl_text_length_get(EWL_TEXT(o)));
        ewl_text_cursor_position_set(EWL_TEXT(o), ewl_text_length_get(EWL_TEXT(o)));
        ewl_text_styles_set(EWL_TEXT(o), EWL_TEXT_STYLE_NONE);
        ewl_text_text_append(EWL_TEXT(o), "This is the link."); /* 17 */

        len = ewl_text_cursor_position_get(EWL_TEXT(o)) -
                        ewl_text_trigger_start_pos_get(
                                                EWL_TEXT_TRIGGER(trigger));
        ewl_text_trigger_length_set(EWL_TEXT_TRIGGER(trigger), len);

        ewl_container_child_append(EWL_CONTAINER(o), trigger);
        ewl_callback_append(trigger, EWL_CALLBACK_MOUSE_UP, trigger_cb, 
                        "You clicked the trigger, have a cookie.");
        ewl_callback_append(trigger, EWL_CALLBACK_MOUSE_IN, trigger_cb_mouse_in,
                        NULL);
        ewl_callback_append(trigger, EWL_CALLBACK_MOUSE_OUT,
                        trigger_cb_mouse_out, NULL);
        ewl_attach_mouse_cursor_set(trigger, EWL_MOUSE_CURSOR_HAND2);

        printf("Inserting 'The fifth bunch of text\\n' [139]\n");
        ewl_text_text_insert(EWL_TEXT(o), "The fifth bunch of text\n", 0); /* 24 */

        printf("Cursor position\n");
        ewl_text_cursor_position_set(EWL_TEXT(o), 0);

        printf("Colour apply\n");
        ewl_text_color_apply(EWL_TEXT(o), 255, 0, 0, 255, 24);

        printf("Inserting 'The sixth bunch of text\\n' [163]\n");
        ewl_text_text_insert(EWL_TEXT(o), "The sixth bunch of text\n", 24); /* 24 */

        printf("Cursor position\n");
        ewl_text_cursor_position_set(EWL_TEXT(o), 43);

        printf("Colour apply\n");
        ewl_text_color_apply(EWL_TEXT(o), 0, 255, 0, 255, 14);

        printf("Cursor position\n");
        ewl_text_cursor_position_set(EWL_TEXT(o), ewl_text_length_get(EWL_TEXT(o)));

        printf("Colour set\n");
        ewl_text_color_set(EWL_TEXT(o), 255, 0, 0, 255);

        printf("Appending 'And in red\\n' [174]\n");
        ewl_text_text_append(EWL_TEXT(o), "And in red\n"); /* 11 */

        printf("Colour set\n");
        ewl_text_color_set(EWL_TEXT(o), 0, 0, 0, 255);

        printf("Appending 'Once more with feeling. ' [198]\n");
        ewl_text_text_append(EWL_TEXT(o), "Once more with feeling. "); /* 24 */

        printf("Trigger\n");
        trigger = ewl_text_trigger_new(EWL_TEXT_TRIGGER_TYPE_TRIGGER);
        ewl_text_trigger_start_pos_set(EWL_TEXT_TRIGGER(trigger),
                                        ewl_text_length_get(EWL_TEXT(o)));

        printf("Appending 'This is the multi\\n\\nline link.' [226]\n");
        ewl_text_text_append(EWL_TEXT(o), "This is the multi\n\nline link."); /* 29 */
        len = ewl_text_cursor_position_get(EWL_TEXT(o)) -
                        ewl_text_trigger_start_pos_get(
                                        EWL_TEXT_TRIGGER(trigger));
        ewl_text_trigger_length_set(EWL_TEXT_TRIGGER(trigger), len);

        ewl_container_child_append(EWL_CONTAINER(o), trigger);
        ewl_callback_append(trigger, EWL_CALLBACK_MOUSE_UP, trigger_cb,
                        "You clicked the multi-line trigger, have a coke.");
        ewl_callback_append(trigger, EWL_CALLBACK_MOUSE_IN, trigger_cb_mouse_in,
                        NULL);
        ewl_callback_append(trigger, EWL_CALLBACK_MOUSE_OUT,
                        trigger_cb_mouse_out, NULL);
        ewl_attach_mouse_cursor_set(trigger, EWL_MOUSE_CURSOR_HAND2);

        printf("Colour set\n");
        ewl_text_color_set(EWL_TEXT(o), 255, 0, 255, 255);

        printf("Appending 'ONE MORE SEGV\\n\\n' [241]\n");
        ewl_text_text_append(EWL_TEXT(o), "ONE MORE SEGV\n\n"); /* 15 */

        printf("Colour set\n");
        ewl_text_color_set(EWL_TEXT(o), 0, 0, 0, 255);

        printf("Appending 'Align Left\\n' [252]\n");
        ewl_text_text_append(EWL_TEXT(o), "Align Left\n");  /* 11 */

        printf("Align set\n");
        ewl_text_align_set(EWL_TEXT(o), EWL_FLAG_ALIGN_CENTER);

        printf("Appending 'Align Center.\\n' [266]\n");
        ewl_text_text_append(EWL_TEXT(o), "Align Center.\n");  /* 14 */

        printf("Align set\n");
        ewl_text_align_set(EWL_TEXT(o), EWL_FLAG_ALIGN_RIGHT);

        printf("Appending 'Align Right.\\n' [279]\n");
        ewl_text_text_append(EWL_TEXT(o), "Align Right.\n");  /* 13 */

        ewl_text_bg_color_set(EWL_TEXT(o), 255, 255, 255, 255);
        ewl_text_align_set(EWL_TEXT(o), EWL_FLAG_ALIGN_LEFT);
        ewl_text_text_append(EWL_TEXT(o), "White Background\n");

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

        ewl_text_cursor_position_set(EWL_TEXT(t->text_parent), t->char_pos);
        ewl_text_color_apply(EWL_TEXT(t->text_parent), 255, 0, 0, 255, t->char_len);
}

static void
trigger_cb_mouse_out(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Text_Trigger *t;

        t = EWL_TEXT_TRIGGER(w);

        ewl_text_cursor_position_set(EWL_TEXT(t->text_parent), t->char_pos);
        ewl_text_color_apply(EWL_TEXT(t->text_parent), 0, 0, 0, 255, t->char_len);
}
