/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_overlay.h"
#include "ewl_button.h"
#include "ewl_spectrum.h"

#include <stdio.h>

static int create_test(Ewl_Container *win);
static void spectrum_show(Ewl_Widget *w, void *ev_data, void *user_data);
static void spectrum_hide(Ewl_Widget *w, void *ev_data, void *user_data);
static void layer_change(Ewl_Widget *w, void *ev_data, void *user_data);
static void configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);

void
test_info(Ewl_Test *test)
{
        test->name = "Layer";
        test->tip = "sets the layer of a widget";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_MISC;
}

static Ewl_Widget * button_row1[20];
static Ewl_Widget * button_row2[20];
static Ewl_Widget * dbutton[2];

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *w, *c, *spec;
        int i;

        /*
         * Create the overlay container
         */
        c = ewl_overlay_new();
        ewl_container_child_append(EWL_CONTAINER(box), c);
        ewl_object_fill_policy_set(EWL_OBJECT(c), EWL_FLAG_FILL_FILL);
        ewl_widget_show(c);

        /*
         * Create the spectrum
         */
        w = ewl_spectrum_new();
        ewl_container_child_append(EWL_CONTAINER(c), w);
        ewl_widget_layer_priority_set(w, 0);
        spec = w;

        /*
         * And now create the first row of buttons
         */
        for (i = 0; i < 20; i++) {
                w = ewl_button_new();
                ewl_button_label_set(EWL_BUTTON(w), "Hide");
                ewl_container_child_append(EWL_CONTAINER(c), w);
                ewl_widget_layer_priority_set(w, i - 10);
                ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
                ewl_callback_append(w, EWL_CALLBACK_CLICKED,
                        spectrum_hide, spec);
                ewl_widget_show(w);

                button_row1[i] = w;
        }

        /*
         * Create the second row
         */
        for (i = 0; i < 20; i++) {
                w = ewl_button_new();
                ewl_button_label_set(EWL_BUTTON(w), "Show");
                ewl_container_child_append(EWL_CONTAINER(c), w);
                ewl_widget_layer_priority_set(w, 10 - i);
                ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
                ewl_callback_append(w, EWL_CALLBACK_CLICKED,
                        spectrum_show, spec);
                ewl_widget_show(w);

                button_row2[i] = w;
        }

        /*
         *  the dynamicly layered button
         */
        w = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(w), "Up/Down");
        ewl_container_child_append(EWL_CONTAINER(c), w);
        ewl_widget_layer_priority_set(w, 3);
        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
        ewl_callback_append(w, EWL_CALLBACK_CLICKED,
                        layer_change, NULL);
        ewl_widget_show(w);
        dbutton[0] = w;

        /*
         * the disabled button
         */
        w = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(w), "Disabled");
        ewl_widget_layer_priority_set(w, 2);
        ewl_container_child_append(EWL_CONTAINER(c), w);
        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
        ewl_widget_disable(w);
        ewl_widget_show(w);
        dbutton[1] = w;

        ewl_callback_append(c, EWL_CALLBACK_CONFIGURE, configure_cb, NULL);

        return 1;
}

static void
spectrum_show(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
                void *user_data)
{
        Ewl_Widget *spec;

        spec = user_data;
        ewl_widget_show(spec);
        return;
}

static void
spectrum_hide(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
                void *user_data)
{
        Ewl_Widget *spec;

        spec = user_data;
        ewl_widget_hide(spec);
        return;
}

static void
layer_change(Ewl_Widget *w, void *ev_data __UNUSED__,
                void *user_data __UNUSED__)
{
        if (ewl_widget_layer_priority_get(w) == 3)
                ewl_widget_layer_priority_set(w, 1);
        else
                ewl_widget_layer_priority_set(w, 3);
}

static void
configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
                void *user_data __UNUSED__)
{
        int x, y;
        int i;

        x = CURRENT_X(w);
        y = CURRENT_Y(w);
        /*
         * place the first row of buttons
         */
        for (i = 0; i < 20; i++)
                ewl_object_position_request(EWL_OBJECT(button_row1[i]),
                                                i * 5 + x, i * 5 + y);
        /*
         * place the second row
         */
        for (i = 0; i < 20; i++)
                ewl_object_position_request(EWL_OBJECT(button_row2[i]),
                                                i * 5 + 40 + x, i * 5 + y);

        ewl_object_position_request(EWL_OBJECT(dbutton[0]), 180 + x, 0 + y);
        ewl_object_position_request(EWL_OBJECT(dbutton[1]), 220 + x, 20 + y);
}

