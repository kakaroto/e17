/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_floater.h"
#include "ewl_radiobutton.h"
#include "ewl_separator.h"
#include <stdio.h>

static int create_test(Ewl_Container *box);

void
test_info(Ewl_Test *test)
{
        test->name = "Floater";
        test->tip = "Defines a widget for layering above other\n "
                        "widgets in EWL's drawing area, with\n"
                        "the ability to follow the movement of\n"
                        "another widget.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *separator, *button[2], *check_button[2];
        Ewl_Widget *radio_button[2], *floater;

        radio_button[0] = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(radio_button[0]), "With Label");
        ewl_container_child_append(EWL_CONTAINER(box), radio_button[0]);
        ewl_object_alignment_set(EWL_OBJECT(radio_button[0]),
                                 EWL_FLAG_ALIGN_LEFT);
        ewl_widget_show(radio_button[0]);

        radio_button[1] = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(radio_button[1]), NULL);
        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(radio_button[1]),
                                  EWL_RADIOBUTTON(radio_button[0]));
        ewl_container_child_append(EWL_CONTAINER(box), radio_button[1]);
        ewl_object_alignment_set(EWL_OBJECT(radio_button[1]),
                                 EWL_FLAG_ALIGN_LEFT);
        ewl_widget_show(radio_button[1]);

        floater = ewl_floater_new();
        ewl_floater_follow_set(EWL_FLOATER(floater), radio_button[1]);
        ewl_container_child_append(EWL_CONTAINER(box), floater);
        ewl_floater_position_set(EWL_FLOATER(floater), 20, 20);
        ewl_widget_show(floater);

        button[0] = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button[0]), "With Label");
        ewl_container_child_append(EWL_CONTAINER(floater), button[0]);
        ewl_object_alignment_set(EWL_OBJECT(button[0]), EWL_FLAG_ALIGN_LEFT);
        ewl_object_custom_size_set(EWL_OBJECT(button[0]), 100, 17);
        ewl_widget_show(button[0]);

        button[1] = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button[1]), NULL);
        ewl_container_child_append(EWL_CONTAINER(floater), button[1]);
        ewl_object_alignment_set(EWL_OBJECT(button[1]), EWL_FLAG_ALIGN_LEFT);
        ewl_object_custom_size_set(EWL_OBJECT(button[1]), 100, 17);
        ewl_widget_show(button[1]);

        separator = ewl_hseparator_new();
        ewl_container_child_append(EWL_CONTAINER(floater), separator);
        ewl_widget_show(separator);

        ewl_object_padding_type_set(EWL_OBJECT(separator), EWL_PADDING_SMALL);
        ewl_object_padding_type_top_set(EWL_OBJECT(separator),
                        EWL_PADDING_MEDIUM);
        ewl_object_padding_type_bottom_set(EWL_OBJECT(separator),
                        EWL_PADDING_MEDIUM);

        check_button[0] = ewl_checkbutton_new();
        ewl_button_label_set(EWL_BUTTON(check_button[0]), "With Label");
        ewl_container_child_append(EWL_CONTAINER(floater), check_button[0]);
        ewl_object_alignment_set(EWL_OBJECT(check_button[0]),
                                 EWL_FLAG_ALIGN_LEFT);
        ewl_widget_show(check_button[0]);

        check_button[1] = ewl_checkbutton_new();
        ewl_button_label_set(EWL_BUTTON(check_button[1]), NULL);
        ewl_container_child_append(EWL_CONTAINER(floater), check_button[1]);
        ewl_object_alignment_set(EWL_OBJECT(check_button[1]),
                                 EWL_FLAG_ALIGN_LEFT);
        ewl_widget_show(check_button[1]);

        return 1;
}

