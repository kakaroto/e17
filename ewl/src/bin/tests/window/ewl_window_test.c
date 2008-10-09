/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_window.h"
#include "ewl_button.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int create_test(Ewl_Container *box);
static void run_window(Ewl_Widget *w, void *ev, void *data);
static void del_window_cb(Ewl_Widget *w, void *ev, void *data);
static void flip_border_cb(Ewl_Widget *w, void *ev, void *data);

extern Ewl_Unit_Test window_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Window";
        test->tip = "The window system.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_MISC;
        test->unit_tests = window_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *o;

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Launch window");
        ewl_container_child_append(box, o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, run_window, NULL);
        ewl_widget_show(o);

        return 1;
}

static void
run_window(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Widget *win, *b, *o;

        win = ewl_window_new();
        ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, del_window_cb, NULL);
        ewl_window_borderless_set(EWL_WINDOW(win), TRUE);
        ewl_widget_show(win);

        b = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(win), b);
        ewl_widget_show(b);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Flip borderless");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, flip_border_cb, win);
        ewl_container_child_append(EWL_CONTAINER(b), o);
        ewl_widget_show(o);
}

static void
del_window_cb(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        ewl_widget_destroy(w);
}

static void
flip_border_cb(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data)
{
        ewl_window_borderless_set(EWL_WINDOW(data),
                !ewl_window_borderless_get(EWL_WINDOW(data)));
}
