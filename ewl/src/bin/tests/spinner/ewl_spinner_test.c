/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_separator.h"
#include "ewl_spinner.h"
#include "ewl_text.h"

#include <stdio.h>

static int create_test(Ewl_Container *box);
static void cb_value_show(Ewl_Widget * w, void *ev, void *data);

static Ewl_Widget *spinner[4];
extern Ewl_Unit_Test spinner_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Spinner";
        test->tip = "Provides a field for entering numerical\n"
                        "values, along with buttons to increment\n"
                        "and decrement the value.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
        test->unit_tests = spinner_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *spinner_row, *text, *separator;

        spinner_row = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), spinner_row);
        ewl_widget_show(spinner_row);

        text = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(text), "X");
        ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(spinner_row), text);
        ewl_widget_show(text);

        spinner[0] = ewl_spinner_new();
        ewl_spinner_digits_set(EWL_SPINNER(spinner[0]), 0);
        ewl_range_minimum_value_set(EWL_RANGE(spinner[0]), 0);
        ewl_range_maximum_value_set(EWL_RANGE(spinner[0]), 1280);
        ewl_range_value_set(EWL_RANGE(spinner[0]), 0.0);
        ewl_range_step_set(EWL_RANGE(spinner[0]), 1.0);
        ewl_container_child_append(EWL_CONTAINER(spinner_row), spinner[0]);
        ewl_widget_show(spinner[0]);

        separator = ewl_hseparator_new();
        ewl_container_child_append(EWL_CONTAINER(box), separator);
        ewl_widget_show(separator);

        spinner_row = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), spinner_row);
        ewl_widget_show(spinner_row);

        text = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(text), "Y");
        ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(spinner_row), text);
        ewl_widget_show(text);

        spinner[1] = ewl_spinner_new();
        ewl_spinner_digits_set(EWL_SPINNER(spinner[1]), 0);
        ewl_range_minimum_value_set(EWL_RANGE(spinner[1]), 0);
        ewl_range_maximum_value_set(EWL_RANGE(spinner[1]), 1024);
        ewl_range_value_set(EWL_RANGE(spinner[1]), 0.0);
        ewl_range_step_set(EWL_RANGE(spinner[1]), 1.0);
        ewl_container_child_append(EWL_CONTAINER(spinner_row), spinner[1]);
        ewl_widget_show(spinner[1]);

        separator = ewl_hseparator_new();
        ewl_container_child_append(EWL_CONTAINER(box), separator);
        ewl_widget_show(separator);

        spinner_row = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), spinner_row);
        ewl_widget_show(spinner_row);

        text = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(text), "W");
        ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(spinner_row), text);
        ewl_widget_show(text);

        spinner[2] = ewl_spinner_new();
        ewl_spinner_digits_set(EWL_SPINNER(spinner[2]), 0);
        ewl_range_minimum_value_set(EWL_RANGE(spinner[2]), 0.0);
        ewl_range_maximum_value_set(EWL_RANGE(spinner[2]), 1280);
        ewl_range_value_set(EWL_RANGE(spinner[2]), 0.0);
        ewl_range_step_set(EWL_RANGE(spinner[2]), 1.0);
        ewl_container_child_append(EWL_CONTAINER(spinner_row), spinner[2]);
        ewl_widget_show(spinner[2]);

        separator = ewl_hseparator_new();
        ewl_container_child_append(EWL_CONTAINER(box), separator);
        ewl_widget_show(separator);

        spinner_row = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), spinner_row);
        ewl_widget_show(spinner_row);

        text = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(text), "H");
        ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(spinner_row), text);
        ewl_widget_show(text);

        spinner[3] = ewl_spinner_new();
        ewl_spinner_digits_set(EWL_SPINNER(spinner[3]), 0);
        ewl_range_minimum_value_set(EWL_RANGE(spinner[3]), 0.0);
        ewl_range_maximum_value_set(EWL_RANGE(spinner[3]), 1024);
        ewl_range_value_set(EWL_RANGE(spinner[3]), 0.0);
        ewl_range_step_set(EWL_RANGE(spinner[3]), 1.0);
        ewl_container_child_append(EWL_CONTAINER(spinner_row), spinner[3]);
        ewl_widget_show(spinner[3]);

        separator = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(separator), "Get Values");
        ewl_container_child_append(EWL_CONTAINER(box), separator);
        ewl_callback_append(separator, EWL_CALLBACK_CLICKED,
                                        cb_value_show, NULL);
        ewl_widget_show(separator);

        return 1;
}

static void
cb_value_show(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        printf("X %d, Y %d, W %d, H %d\n",
                (int)ewl_range_value_get(EWL_RANGE(spinner[0])),
                (int)ewl_range_value_get(EWL_RANGE(spinner[1])),
                (int)ewl_range_value_get(EWL_RANGE(spinner[2])),
                (int)ewl_range_value_get(EWL_RANGE(spinner[3])));
}


