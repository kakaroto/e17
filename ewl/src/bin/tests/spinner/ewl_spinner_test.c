/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_spinner.h"
#include "ewl_grid.h"
#include "ewl_label.h"

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
        Ewl_Widget *grid, *button;
        int i;
        const struct
        {
                const char *label;
                double min, max, step;
        } spinner_vals[] = {
                {"X", 0.0, 1280.0, 1.0},
                {"Y", 0.0, 1280.0, 1.0},
                {"W", 0.0, 1280.0, 1.0},
                {"H", 0.0, 1280.0, 1.0},
                {NULL, 0.0, 0.0, 0.0}
        };

        grid = ewl_grid_new();
        ewl_grid_dimensions_set(EWL_GRID(grid), 2, 3);
        ewl_grid_column_preferred_w_use(EWL_GRID(grid), 0);
        ewl_container_child_append(EWL_CONTAINER(box), grid);
        ewl_widget_show(grid);

        for (i = 0; spinner_vals[i].label; i++)
        {
                Ewl_Widget *label;

                label = ewl_label_new();
                ewl_label_text_set(EWL_LABEL(label), spinner_vals[i].label);
                ewl_container_child_append(EWL_CONTAINER(grid), label);
                ewl_widget_show(label);

                spinner[i] = ewl_spinner_new();
                ewl_spinner_digits_set(EWL_SPINNER(spinner[i]), 0);
                ewl_range_minimum_value_set(EWL_RANGE(spinner[i]), 
                                spinner_vals[i].min);
                ewl_range_maximum_value_set(EWL_RANGE(spinner[i]),
                                spinner_vals[i].max);
                ewl_range_step_set(EWL_RANGE(spinner[i]),
                                spinner_vals[i].step);
                ewl_container_child_append(EWL_CONTAINER(grid), spinner[i]);
                ewl_widget_show(spinner[i]);
        }

        button = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button), "Get Values");
        ewl_container_child_append(EWL_CONTAINER(box), button);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED,
                                        cb_value_show, NULL);
        ewl_widget_show(button);

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


