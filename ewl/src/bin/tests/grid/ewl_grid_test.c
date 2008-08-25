/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_entry.h"
#include "ewl_grid.h"
#include "ewl_separator.h"

#include <stdio.h>

static int create_test(Ewl_Container *win);
static void child_append_cb(Ewl_Widget *w, void *e, void *d);
static void child_prepend_cb(Ewl_Widget *w, void *e, void *d);
static void button_clicked_cb(Ewl_Widget *w, void *e, void *d);

extern Ewl_Unit_Test grid_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Grid";
        test->tip = "The grid\n";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
        test->unit_tests = grid_unit_tests;        
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *vbox;
        Ewl_Widget *hbox;
        Ewl_Widget *w;
        Ewl_Widget *grid;

        /*
         * Setup the main structure
         */
        vbox = ewl_vbox_new();
        ewl_container_child_append(box, vbox);
        ewl_widget_show(vbox);

        /* the grid */
        grid = ewl_grid_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), grid);
        ewl_object_fill_policy_set(EWL_OBJECT(grid), EWL_FLAG_FILL_FILL);
        ewl_grid_dimensions_set(EWL_GRID(grid), 5, 4);
        ewl_grid_column_relative_w_set(EWL_GRID(grid), 0, 0.25);
        ewl_grid_row_fixed_h_set(EWL_GRID(grid), 3, 50);
        ewl_grid_row_preferred_h_use(EWL_GRID(grid), 2);
        ewl_widget_show(grid);

        /* the seperator */
        w = ewl_hseparator_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), w);
        ewl_widget_show(w);

        /* the hbox */
        hbox = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
        ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
        ewl_box_homogeneous_set(EWL_BOX(hbox), TRUE);
        ewl_widget_show(hbox);

        /* fill the hbox with content */
        w = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(w), "Append");
        ewl_container_child_append(EWL_CONTAINER(hbox), w);
        ewl_callback_append(w, EWL_CALLBACK_CLICKED, child_append_cb, grid);
        ewl_widget_show(w);

        w = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(w), "Prepend");
        ewl_container_child_prepend(EWL_CONTAINER(hbox), w);
        ewl_callback_append(w, EWL_CALLBACK_CLICKED, child_prepend_cb, grid);
        ewl_widget_show(w);

        /*
         * fill the grid with content
         */
        w = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(w), "This column is\n 25% wide");
        ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(grid), w);
        ewl_grid_child_position_set(EWL_GRID(grid), w, 0, 0, 0, 1);
        ewl_widget_show(w);

        w = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(w), "This row is 50px height");
        ewl_container_child_append(EWL_CONTAINER(grid), w);
        ewl_grid_child_position_set(EWL_GRID(grid), w, 1, 2, 3, 3);
        ewl_callback_append(w, EWL_CALLBACK_CLICKED, button_clicked_cb, NULL);
        ewl_widget_show(w);

        w = ewl_entry_new();
        ewl_text_text_set(EWL_TEXT(w), "This row use the\n"
                                        "preferred height");
        ewl_container_child_append(EWL_CONTAINER(grid), w);
        ewl_grid_child_position_set(EWL_GRID(grid), w, 3, 4, 2, 2);
        ewl_widget_show(w);

        return 1;
}

static void
child_append_cb(Ewl_Widget *w __UNUSED__, void *e __UNUSED__, void *d)
{
        Ewl_Widget *button;

        button = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button), "Append");
        ewl_container_child_append(EWL_CONTAINER(d), button);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, button_clicked_cb,
                                        NULL);
        ewl_widget_show(button);
}

static void
child_prepend_cb(Ewl_Widget *w __UNUSED__, void *e __UNUSED__, void *d)
{
        Ewl_Widget *button;

        button = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button), "Prepend");
        ewl_container_child_prepend(EWL_CONTAINER(d), button);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, button_clicked_cb,
                                        NULL);
        ewl_widget_show(button);
}

static void
button_clicked_cb(Ewl_Widget *w, void *e __UNUSED__, void *d __UNUSED__)
{
        Ewl_Widget *g;
        int start_col, end_col, start_row, end_row;

        g = w->parent;
        ewl_grid_child_position_get(EWL_GRID(g), w, &start_col, &end_col,
                                                &start_row, &end_row);
        printf("child position:\n\t\tstart\tend\n"
                " column:\t%i\t%i\n"
                " row:\t\t%i\t%i\n\n", start_col, end_col, start_row, end_row);
}
