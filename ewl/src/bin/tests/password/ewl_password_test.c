/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_entry.h"

#include <stdio.h>
#include <stdlib.h>

static Ewl_Widget *password[2];

static int create_test(Ewl_Container *win);
static void cb_fetch_password_text(Ewl_Widget *w, void *ev, void *data);
static void cb_set_password_text(Ewl_Widget *w, void *ev, void *data);

extern Ewl_Unit_Test password_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Password";
        test->tip = "Defines the Ewl_Password class to allow\n"
                        "for single line obscured text.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
        test->unit_tests = password_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *button_hbox, *button[3];

        password[0] = ewl_password_new();
        ewl_text_text_set(EWL_TEXT(password[0]), "Play with me ?");
        ewl_object_padding_type_set(EWL_OBJECT(password[0]),
                        EWL_PADDING_MEDIUM);
        ewl_object_padding_type_bottom_set(EWL_OBJECT(password[0]),
                        EWL_PADDING_DEFAULT);
        ewl_container_child_append(box, password[0]);
        ewl_callback_append(password[0], EWL_CALLBACK_VALUE_CHANGED,
                            cb_fetch_password_text, NULL);
        ewl_widget_show(password[0]);

        password[1] = ewl_password_new();
        ewl_text_obscure_set(EWL_TEXT(password[1]), "•");
        ewl_text_color_set(EWL_TEXT(password[1]), 255, 0, 0, 255);
        ewl_text_text_append(EWL_TEXT(password[1]), "E W L ! ! !");
        ewl_object_padding_type_left_set(EWL_OBJECT(password[1]),
                        EWL_PADDING_MEDIUM);
        ewl_object_padding_type_right_set(EWL_OBJECT(password[1]),
                        EWL_PADDING_MEDIUM);
        ewl_container_child_append(box, password[1]);
        ewl_callback_append(password[1], EWL_CALLBACK_VALUE_CHANGED,
                            cb_fetch_password_text, NULL);
        ewl_widget_show(password[1]);

        password[2] = ewl_password_new();
        ewl_text_obscure_set(EWL_TEXT(password[2]), "");
        ewl_text_text_append(EWL_TEXT(password[2]), "hidden text");
        ewl_object_padding_type_left_set(EWL_OBJECT(password[2]),
                        EWL_PADDING_MEDIUM);
        ewl_object_padding_type_right_set(EWL_OBJECT(password[2]),
                        EWL_PADDING_MEDIUM);
        ewl_container_child_append(box, password[2]);
        ewl_callback_append(password[2], EWL_CALLBACK_VALUE_CHANGED,
                            cb_fetch_password_text, NULL);
        ewl_widget_show(password[2]);

        button_hbox = ewl_hbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(button_hbox), EWL_FLAG_FILL_VFILL);
        ewl_object_alignment_set(EWL_OBJECT(button_hbox), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(box, button_hbox);
        ewl_box_spacing_set(EWL_BOX(button_hbox), 5);
        ewl_widget_show(button_hbox);

        button[0] = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button[0]), "Fetch text");
        ewl_container_child_append(EWL_CONTAINER(button_hbox), button[0]);
        ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
                            cb_fetch_password_text, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(button[0]), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(button[0]), EWL_FLAG_ALIGN_CENTER);
        ewl_widget_show(button[0]);

        button[1] = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button[1]), "Set Text");
        ewl_container_child_append(EWL_CONTAINER(button_hbox), button[1]);
        ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
                            cb_set_password_text, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(button[1]), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(button[1]), EWL_FLAG_ALIGN_CENTER);
        ewl_widget_show(button[1]);

        return 1;
}

static void
cb_fetch_password_text(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        char *s;

        s = ewl_text_text_get(EWL_TEXT(password[0]));
        printf("First password covers: %s\n", s);
        free(s);

        s = ewl_text_text_get(EWL_TEXT(password[1]));
        printf("Second password covers: %s\n", s);
        free(s);
        
        s = ewl_text_text_get(EWL_TEXT(password[2]));
        printf("Third password covers: %s\n", s);
        free(s);
}

static void
cb_set_password_text(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        ewl_text_text_set(EWL_TEXT(password[0]), "Play with me ?");
        ewl_text_text_set(EWL_TEXT(password[1]), "E W L ! ! !");
        ewl_text_text_set(EWL_TEXT(password[2]), "hidden text");
}

