/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_entry.h"
#include "ewl_label.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Ewl_Widget *entry[3];

static int create_test(Ewl_Container *box);

/* unit tests */
static int constructor_test(char *buf, int len);

static Ewl_Unit_Test entry_unit_tests[] = {
                {"constructor", constructor_test, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

void
test_info(Ewl_Test *test)
{
        test->name = "Entry";
        test->tip = "Defines the Ewl_Entry class to allow\n"
                "for single line editable text.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
        test->unit_tests = entry_unit_tests;
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
        ewl_text_text_set(EWL_TEXT(entry[0]), "Play with me ?");
        ewl_text_clear(EWL_TEXT(entry[1]));
        ewl_text_text_set(EWL_TEXT(entry[2]), "Not more than 30 characters");
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
        ewl_object_padding_set(EWL_OBJECT(entry[0]), 5, 5, 5, 0);
        ewl_container_child_append(EWL_CONTAINER(entry_box), entry[0]);
        ewl_widget_show(entry[0]);

        entry[1] = ewl_entry_new();
        ewl_object_padding_set(EWL_OBJECT(entry[1]), 5, 5, 0, 0);
        ewl_container_child_append(EWL_CONTAINER(entry_box), entry[1]);
        ewl_widget_show(entry[1]);

        entry[2] = ewl_entry_new();
        ewl_text_text_set(EWL_TEXT(entry[2]), "Not more than 30 characters");
        ewl_text_length_maximum_set(EWL_TEXT(entry[2]), 30);
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

static int
constructor_test(char *buf, int len)
{
        Ewl_Widget *e;
        int ret = 0;

        e = ewl_entry_new();

        if (!EWL_ENTRY_IS(e))
        {
                LOG_FAILURE(buf, len, "returned widget is not of the type"
                                " " EWL_ENTRY_TYPE);
                goto DONE;
        }
        if (ewl_object_fill_policy_get(EWL_OBJECT(e)) != (EWL_FLAG_FILL_HSHRINK
                                | EWL_FLAG_FILL_HFILL))
        {
                LOG_FAILURE(buf, len, "default fill policy is wrong");
                goto DONE;
        }
        if (!!strcmp(ewl_widget_appearance_get(e), EWL_ENTRY_TYPE))
        {
                LOG_FAILURE(buf, len, "appearance is not " EWL_ENTRY_TYPE);
                goto DONE;
        }
        if (!ewl_widget_focusable_get(e))
        {
                LOG_FAILURE(buf, len, "entry is not focusable");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(e);

        return ret;
}

