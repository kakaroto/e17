/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_colordialog.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int create_test(Ewl_Container *win);
static void colordialog_cb_launch(Ewl_Widget *w, void *ev, void *data);
static void colordialog_cb_value_changed(Ewl_Widget *w, void *ev,
                                                        void *data);

/* unit tests */
static int constructor_test(char *buf, int len);

static Ewl_Unit_Test colordialog_unit_tests[] = {
                {"constructor", constructor_test, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };


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

        o = calloc(1, sizeof(Ewl_Widget));
        ewl_widget_init(o);
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

static int
constructor_test(char *buf, int len)
{
        Ewl_Widget *c;
        int ret = 0;

        c = ewl_colordialog_new();

        if (!EWL_COLORDIALOG_IS(c))
        {
                LOG_FAILURE(buf, len, "returned color dialog is not of the type"
                                " " EWL_COLORDIALOG_TYPE);
                goto DONE;
        }
        if (!!strcmp(ewl_window_title_get(EWL_WINDOW(c)), "Ewl Colordialog"))
        {
                LOG_FAILURE(buf, len, "window title is not Ewl Colordialog");
                goto DONE;
        }
        if (!!strcmp(ewl_window_name_get(EWL_WINDOW(c)), "Ewl Colordialog"))
        {
                LOG_FAILURE(buf, len, "window name is not Ewl Colordialog");
                goto DONE;
        }
        if (!!strcmp(ewl_window_title_get(EWL_WINDOW(c)), "Ewl Colordialog"))
        {
                LOG_FAILURE(buf, len, "window group is not Ewl Colordialog");
                goto DONE;
        }
        if (!ewl_colordialog_has_alpha_get(EWL_COLORDIALOG(c)))
        {
                LOG_FAILURE(buf, len, "colordialog has no alpha channel");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(c);

        return ret;
}

