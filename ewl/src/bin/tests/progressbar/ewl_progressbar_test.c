/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_progressbar.h"

#include <stdio.h>
#include <stdlib.h>

static int create_test(Ewl_Container *box);
static void cb_rerun_progressbars(Ewl_Widget *w, void *ev, void *data);
static void cb_set_new_range(Ewl_Widget *w, void *ev, void *data);
static void cb_destroy_progressbar_test(Ewl_Widget *w, void *ev,
                                                        void *data);
static int cb_increment_progress(void *data);

static Ecore_Timer *progress_timer[3];
static Ewl_Widget *progressbar[4];

extern Ewl_Unit_Test progressbar_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Progressbar";
        test->tip = "Provides a progress bar from a given value.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_SIMPLE;
        test->unit_tests = progressbar_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *progressbar_vbox, *button, *progressbar_box;
        int i;

        progressbar_vbox = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), progressbar_vbox);
        ewl_box_spacing_set(EWL_BOX(progressbar_vbox), 0);
        ewl_callback_prepend(EWL_WIDGET(progressbar_vbox), EWL_CALLBACK_DESTROY,
                                        cb_destroy_progressbar_test, NULL);
        ewl_widget_show(progressbar_vbox);


        progressbar_box = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(progressbar_vbox),
                                                        progressbar_box);
        ewl_box_spacing_set(EWL_BOX(progressbar_box), 0);
        ewl_widget_show(progressbar_box);

        /*
         * First and second progressbar
         */
        for (i = 0; i < 2; i++) {
                progressbar[i] = ewl_progressbar_new();
                if (i == 0)
                        ewl_range_invert_set(EWL_RANGE(progressbar[0]), TRUE);
                ewl_range_value_set(EWL_RANGE(progressbar[i]), 0);
                ewl_widget_show(progressbar[i]);

                progress_timer[i] = ecore_timer_add(0.1, cb_increment_progress,
                                (Ewl_Progressbar *) progressbar[i]);

                ewl_container_child_append(EWL_CONTAINER(progressbar_box),
                                                        progressbar[i]);
        }

        /*
         * Third big progressbar
         */
        progressbar[2] = ewl_progressbar_new();
        ewl_range_value_set(EWL_RANGE(progressbar[2]), 0);
        ewl_widget_show(progressbar[2]);

        progress_timer[2] = ecore_timer_add(0.1, cb_increment_progress,
                        (Ewl_Progressbar *) progressbar[2]);

        ewl_container_child_append(EWL_CONTAINER(progressbar_vbox), progressbar[2]);

        /*
         * A bouncy progressbar
         */
        progressbar[3] = ewl_progressbar_new();
        ewl_range_unknown_set(EWL_RANGE(progressbar[3]), TRUE);
        ewl_container_child_append(EWL_CONTAINER(progressbar_vbox), progressbar[3]);
        ewl_widget_show(progressbar[3]);

        /*
         * Add buttons at the bottom
         */
        progressbar_box = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(progressbar_vbox),
                                                progressbar_box);
        ewl_object_fill_policy_set(EWL_OBJECT(progressbar_box), 
                        EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(progressbar_box), 
                                                EWL_FLAG_ALIGN_CENTER);
        ewl_widget_show(progressbar_box);

        button = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button), "Rerun");
        ewl_container_child_append(EWL_CONTAINER(progressbar_box), button);
        ewl_callback_prepend(button, EWL_CALLBACK_CLICKED,
                        cb_rerun_progressbars, NULL);
        ewl_widget_show (button);

        button = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button), "Set a random range from 0-500");
        ewl_container_child_append(EWL_CONTAINER(progressbar_box), button);
        ewl_callback_prepend(button, EWL_CALLBACK_CLICKED,
                        cb_set_new_range, NULL);
        ewl_widget_show (button);

        return 1;
}

static int
cb_increment_progress(void *data)
{
        double val, value, range;
        char c[30];
        int i;
        Ewl_Range *r;

        r = EWL_RANGE(data);
        val = ewl_range_value_get(r);

        if (val >= r->max_val) {
                for (i = 0; i < 3; i++) {
                        if (progress_timer[i]) {
                                ecore_timer_del(progress_timer[i]);
                                progress_timer[i] = NULL;
                        }
                }
                return 0;
        }

        val += 1;
        ewl_range_value_set(r, val);

        if (val >= 20 && val < 35 ) {
                ewl_progressbar_custom_label_set(EWL_PROGRESSBAR(r),
                                "%.0lf / %.0lf kbytes");
        }

        if (val >= 35 && val < 60) {
                value = ewl_range_value_get(r);
                range = ewl_range_maximum_value_get(r);

                snprintf(c, sizeof (c), "%.0lf of %.0lf beers", value, range);
                ewl_progressbar_label_set(EWL_PROGRESSBAR(r), c);
        }

        if (val == 60)
                ewl_progressbar_label_hide(EWL_PROGRESSBAR(r));

        if (val == 70)
                ewl_progressbar_label_show(EWL_PROGRESSBAR(r));

        return 1;
}

static void
cb_destroy_progressbar_test(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        int i;

        for (i = 0; i < 3; i++) {
                if (progress_timer[i]) {
                        ecore_timer_del(progress_timer[i]);
                        progress_timer[i] = NULL;
                }
        }
}

static void
cb_set_new_range(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        int i, j;

        j = rand() % 500;
        printf ("New random value: %d\n", j);

        for (i = 0; i < 3; i++) {
                ewl_range_maximum_value_set(EWL_RANGE(progressbar[i]), j);

                if (ewl_range_value_get(EWL_RANGE(progressbar[i])) >= j)
                        cb_rerun_progressbars(EWL_WIDGET (progressbar[i]),
                                                                NULL, NULL);
        }
}

static void
cb_rerun_progressbars (Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        int i;

        for (i = 0; i < 3; i++) {
                /*
                 * Make sure to autolabel the bar on start again,
                 * if we stop a place where it labels manually.
                 * (since the auto label is turned off when you label manually)
                 */
                ewl_progressbar_label_show (EWL_PROGRESSBAR (progressbar[i]));
                ewl_range_value_set(EWL_RANGE(progressbar[i]), 0);

                if (progress_timer[i]) {
                        ecore_timer_del(progress_timer[i]);
                        progress_timer[i] = NULL;
                }

                progress_timer[i] = ecore_timer_add(0.1, cb_increment_progress,
                                                                progressbar[i]);
        }
}

