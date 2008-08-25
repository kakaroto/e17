/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int create_test(Ewl_Container *box);
static void ewl_widget_cb_toggle(Ewl_Widget *w, void *ev, void *data);
static void ewl_widget_cb_first_click(Ewl_Widget *w, void *ev, void *data);
static void ewl_widget_cb_second_click(Ewl_Widget *w, void *ev, void *data);
static void ewl_widget_cb_toggle_fullscreen(Ewl_Widget *w, void *ev, void *data);

extern Ewl_Unit_Test widget_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Widget";
        test->tip = "The base widget type.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_MISC;
        test->func = create_test;
        test->unit_tests = widget_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *vbox, *b2, *o;

        vbox = ewl_vbox_new();
        ewl_container_child_append(box, vbox);
        ewl_widget_show(vbox);

        b2 = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), b2);
        ewl_widget_show(b2);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "First button");
        ewl_widget_name_set(o, "first_widget");
        ewl_container_child_append(EWL_CONTAINER(b2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_widget_cb_first_click, NULL);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Second button");
        ewl_widget_name_set(o, "second_widget");
        ewl_container_child_append(EWL_CONTAINER(b2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_widget_cb_second_click, NULL);
        ewl_widget_disable(o);
        ewl_widget_show(o);

        b2 = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), b2);
        ewl_widget_show(b2);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Toggle");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_widget_cb_toggle, NULL);
        ewl_container_child_append(EWL_CONTAINER(b2), o);
        ewl_widget_show(o);

        b2 = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), b2);
        ewl_widget_show(b2);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Change fullscreen setting");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                ewl_widget_cb_toggle_fullscreen, NULL);
        ewl_container_child_append(EWL_CONTAINER(b2), o);
        ewl_widget_show(o);

        return 1;
}

static void
ewl_widget_cb_toggle(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Widget *o, *o2;

        o = ewl_widget_name_find("first_widget");
        o2 = ewl_widget_name_find("second_widget");

        if (DISABLED(o))
        {
                ewl_widget_enable(o);
                ewl_widget_disable(o2);
        }
        else
        {
                ewl_widget_disable(o);
                ewl_widget_enable(o2);
        }
}

static void
ewl_widget_cb_first_click(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                        void *data __UNUSED__)
{
        printf("First clicked\n");
}

static void
ewl_widget_cb_second_click(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                        void *data __UNUSED__)
{
        printf("Second clicked\n");
}

static void
ewl_widget_cb_toggle_fullscreen(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Embed *win;

        win = ewl_embed_widget_find(w);
        ewl_window_fullscreen_set(EWL_WINDOW(win),
                !ewl_window_fullscreen_get(EWL_WINDOW(win)));
}
