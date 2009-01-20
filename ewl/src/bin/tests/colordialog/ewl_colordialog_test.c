/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_colordialog.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern Ewl_Unit_Test colordialog_unit_tests[];

static int create_test(Ewl_Container *win);
static void colordialog_cb_launch(Ewl_Widget *w, void *ev, void *data);
static void colordialog_cb_value_changed(Ewl_Widget *w, void *ev,
                                                        void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "Colordialog";
        test->tip = "Defines a dialog with a colour picker.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_ADVANCED;
        test->unit_tests = colordialog_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *o;

        o = ewl_widget_new();
        ewl_widget_appearance_set(o, "rectangle");
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_object_minimum_size_set(EWL_OBJECT(o), 150, 20);
        ewl_widget_name_set(o, "colour_preview");
        ewl_widget_color_set(o, 255, 255, 255, 255);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_button_label_set(EWL_BUTTON(o), "Launch Colour Dialog");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, colordialog_cb_launch, NULL);
        ewl_widget_show(o);

        return 1;
}

static void
colordialog_cb_value_changed(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Event_Action_Response *cd_ev;

        cd_ev = ev;
        if (cd_ev->response == EWL_STOCK_OK)
        {
                Ewl_Widget *o;
                unsigned int r, g, b, a;

                o = ewl_widget_name_find("colour_preview");

                ewl_colordialog_current_rgb_get(EWL_COLORDIALOG(w), &r, &g, &b);
                a = ewl_colordialog_alpha_get(EWL_COLORDIALOG(w));

                ewl_widget_color_set(o, r, g, b, a);
        }
        ewl_widget_destroy(w);
}

static void
colordialog_cb_launch(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Widget *o;
        unsigned int r, g, b, a;

        o = ewl_widget_name_find("colour_preview");
        ewl_widget_color_get(o, &r, &g, &b, &a);

        o = ewl_colordialog_new();
        ewl_colordialog_previous_rgba_set(EWL_COLORDIALOG(o), r, g, b, a);
        ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                colordialog_cb_value_changed, NULL);
        ewl_widget_show(o);
}
