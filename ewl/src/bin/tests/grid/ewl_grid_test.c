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

/* unit tests */
static int dimensions_set_get(char *buf, int len);
static int column_fixed_set_get(char *buf, int len);
static int row_fixed_set_get(char *buf, int len);
static int column_relative_set_get(char *buf, int len);
static int row_relative_set_get(char *buf, int len);
static int column_size_remove(char *buf, int len);
static int row_size_remove(char *buf, int len);
static int hhomogeneous_set_get(char *buf, int len);
static int vhomogeneous_set_get(char *buf, int len);
static int homogeneous_set_get(char *buf, int len);
static int position_set_get(char *buf, int len);
static int floating_position_get(char *buf, int len);
static int dimensions_auto_resize(char *buf, int len);
static int orientation_set_get(char *buf, int len);

static Ewl_Unit_Test grid_unit_tests[] = {
                {"set and get dimensions", dimensions_set_get, NULL, -1, 0},
                {"set and get fixed column size", column_fixed_set_get, NULL, -1, 0},
                {"set and get fixed row size", row_fixed_set_get, NULL, -1, 0},
                {"set and get relative column size", column_relative_set_get, NULL, -1, 0},
                {"set and get relative row size", row_relative_set_get, NULL, -1, 0},
                {"remove fixed column size", column_size_remove, NULL, -1, 0},
                {"remove row column size", row_size_remove, NULL, -1, 0},
                {"set and get vertiacal homogeneous", vhomogeneous_set_get, NULL, -1, 0},
                {"set and get horizontal homogeneous", hhomogeneous_set_get, NULL, -1, 0},
                {"set and get homogeneous", homogeneous_set_get, NULL, -1, 0},
                {"set and get position", position_set_get, NULL, -1, 0},
                {"get position of floating widgets", floating_position_get, NULL, -1, 0},
                {"auto resize dimensions", dimensions_auto_resize, NULL, -1, 0},
                {"set and get the orientation", orientation_set_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

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

/* unit tests */
static int 
dimensions_set_get(char *buf, int len)
{
        Ewl_Widget *grid;
        int row, col;
        int ret = 1;

        grid = ewl_grid_new();
        ewl_grid_dimensions_set(EWL_GRID(grid), 5, 6);
        ewl_grid_dimensions_get(EWL_GRID(grid), &col, &row);

        if (col != 5 || row != 6) {
                snprintf(buf, len, "dimensions are %dx%d, but should be 5x6", 
                                col, row);
                ret = 0;
        }
        
        ewl_widget_destroy(grid);

        return ret;
}

static int
column_fixed_set_get(char *buf, int len)
{
        Ewl_Widget *grid;
        int fixed;
        int ret = 1;

        grid = ewl_grid_new();
        fixed = ewl_grid_column_fixed_w_get(EWL_GRID(grid), 1);
        if (fixed != 0) {
                snprintf(buf, len, "the column has a fixed size (%d), where"
                                " it shouldn't", fixed);
                ret = 0;
                goto CLEANUP;
        }

        /* now set a fixed size and see if we get it back */
        ewl_grid_column_fixed_w_set(EWL_GRID(grid), 1, 123);
        fixed = ewl_grid_column_fixed_w_get(EWL_GRID(grid), 1);
        if (fixed != 123) {
                snprintf(buf, len, "the column has a wrong fixed size (%d)!",
                                fixed);
                ret = 0;
                goto CLEANUP;
        }

CLEANUP:
        ewl_widget_destroy(grid);

        return ret;
}

static int
row_fixed_set_get(char *buf, int len)
{
        Ewl_Widget *grid;
        int fixed;
        int ret = 1;

        grid = ewl_grid_new();
        fixed = ewl_grid_row_fixed_h_get(EWL_GRID(grid), 1);
        if (fixed != 0) {
                snprintf(buf, len, "the row has a fixed size (%d), where"
                                " it shouldn't", fixed);
                ret = 0;
                goto CLEANUP;
        }

        /* now set a fixed size and see if we get it back */
        ewl_grid_row_fixed_h_set(EWL_GRID(grid), 1, 123);
        fixed = ewl_grid_row_fixed_h_get(EWL_GRID(grid), 1);
        if (fixed != 123) {
                snprintf(buf, len, "the row has a wrong fixed size (%d)!",
                                fixed);
                ret = 0;
                goto CLEANUP;
        }

CLEANUP:
        ewl_widget_destroy(grid);

        return ret;
}


static int
column_relative_set_get(char *buf, int len)
{
        Ewl_Widget *grid;
        float rel;
        int ret = 1;

        grid = ewl_grid_new();
        rel = ewl_grid_column_relative_w_get(EWL_GRID(grid), 1);
        if (rel != 0.0) {
                snprintf(buf, len, "the column has a relative size (%f), where"
                                " it shouldn't", rel);
                ret = 0;
                goto CLEANUP;
        }

        /* now set a relative size and see if we get it back */
        ewl_grid_column_relative_w_set(EWL_GRID(grid), 1, 0.5);
        rel = ewl_grid_column_relative_w_get(EWL_GRID(grid), 1);
        if (rel != 0.5) {
                snprintf(buf, len, "the column has a wrong relative size (%f)!",
                                rel);
                ret = 0;
                goto CLEANUP;
        }

CLEANUP:
        ewl_widget_destroy(grid);

        return ret;
}

static int
row_relative_set_get(char *buf, int len)
{
        Ewl_Widget *grid;
        float rel;
        int ret = 1;

        grid = ewl_grid_new();
        rel = ewl_grid_row_relative_h_get(EWL_GRID(grid), 1);
        if (rel != 0) {
                snprintf(buf, len, "the row has a rel size (%f), where"
                                " it shouldn't", rel);
                ret = 0;
                goto CLEANUP;
        }

        /* now set a rel size and see if we get it back */
        ewl_grid_row_relative_h_set(EWL_GRID(grid), 1, 0.5);
        rel = ewl_grid_row_relative_h_get(EWL_GRID(grid), 1);
        if (rel != 0.5) {
                snprintf(buf, len, "the row has a wrong relative size (%f)!",
                                rel);
                ret = 0;
                goto CLEANUP;
        }

CLEANUP:
        ewl_widget_destroy(grid);

        return ret;
}

static int
column_size_remove(char *buf, int len)
{
        Ewl_Widget *grid;
        int fixed;
        int ret = 1;

        grid = ewl_grid_new();
        ewl_grid_column_fixed_w_set(EWL_GRID(grid), 1, 102);
        ewl_grid_column_w_remove(EWL_GRID(grid), 1);
        fixed = ewl_grid_column_fixed_w_get(EWL_GRID(grid), 1);
        if (fixed != 0) {
                snprintf(buf, len, "the column has a fixed size (%d), where"
                                " it shouldn't", fixed);
                ret = 0;
                goto CLEANUP;
        }

CLEANUP:
        ewl_widget_destroy(grid);

        return ret;
}

static int
row_size_remove(char *buf, int len)
{
        Ewl_Widget *grid;
        int fixed;
        int ret = 1;

        grid = ewl_grid_new();
        ewl_grid_row_fixed_h_set(EWL_GRID(grid), 1, 102);
        ewl_grid_row_h_remove(EWL_GRID(grid), 1);
        fixed = ewl_grid_row_fixed_h_get(EWL_GRID(grid), 1);
        if (fixed != 0) {
                snprintf(buf, len, "the row has a fixed size (%d), where"
                                " it shouldn't", fixed);
                ret = 0;
                goto CLEANUP;
        }

CLEANUP:
        ewl_widget_destroy(grid);

        return ret;
}

static int
vhomogeneous_set_get(char *buf, int len)
{
        Ewl_Widget *grid;
        int ret = 1;

        grid = ewl_grid_new();

        /* test true */
        ewl_grid_vhomogeneous_set(EWL_GRID(grid), TRUE);
        if (!ewl_grid_vhomogeneous_get(EWL_GRID(grid))) {
                LOG_FAILURE(buf, len, "grid is not vhomogeneous");
                ret = 0;
                goto CLEANUP;
        }
        
        /* test false */
        ewl_grid_vhomogeneous_set(EWL_GRID(grid), FALSE);
        if (ewl_grid_vhomogeneous_get(EWL_GRID(grid))) {
                LOG_FAILURE(buf, len, "grid is vhomogeneous");
                ret = 0;
                goto CLEANUP;
        }
CLEANUP:
        ewl_widget_destroy(grid);

        return ret;
}

static int
hhomogeneous_set_get(char *buf, int len)
{
        Ewl_Widget *grid;
        int ret = 1;

        grid = ewl_grid_new();

        /* test true */
        ewl_grid_hhomogeneous_set(EWL_GRID(grid), TRUE);
        if (!ewl_grid_hhomogeneous_get(EWL_GRID(grid))) {
                LOG_FAILURE(buf, len, "grid is not hhomogeneous");
                ret = 0;
                goto CLEANUP;
        }
        
        /* test false */
        ewl_grid_hhomogeneous_set(EWL_GRID(grid), FALSE);
        if (ewl_grid_hhomogeneous_get(EWL_GRID(grid))) {
                LOG_FAILURE(buf, len, "grid is hhomogeneous");
                ret = 0;
                goto CLEANUP;
        }
CLEANUP:
        ewl_widget_destroy(grid);

        return ret;
}

static int
homogeneous_set_get(char *buf, int len)
{
        Ewl_Widget *grid;
        int ret = 1;

        grid = ewl_grid_new();

        /* test true */
        ewl_grid_homogeneous_set(EWL_GRID(grid), TRUE);
        if (!(ewl_grid_hhomogeneous_get(EWL_GRID(grid))
                        || ewl_grid_vhomogeneous_get(EWL_GRID(grid)))) {
                LOG_FAILURE(buf, len, "grid is not homogeneous");
                ret = 0;
                goto CLEANUP;
        }
        
        /* test false */
        ewl_grid_homogeneous_set(EWL_GRID(grid), FALSE);
        if (ewl_grid_hhomogeneous_get(EWL_GRID(grid))
                        || ewl_grid_vhomogeneous_get(EWL_GRID(grid))) {
                LOG_FAILURE(buf, len, "grid is homogeneous");
                ret = 0;
                goto CLEANUP;
        }
        
CLEANUP:
        ewl_widget_destroy(grid);

        return ret;
}

static int
position_set_get(char *buf, int len)
{
        Ewl_Widget *grid;
        Ewl_Widget *child;
        int ret = 1;
        int col_start, col_end, row_start, row_end;

        grid = ewl_grid_new();
        ewl_grid_dimensions_set(EWL_GRID(grid), 5, 6);
        ewl_widget_show(grid);

        child = ewl_cell_new();
        ewl_container_child_prepend(EWL_CONTAINER(grid), child);
        ewl_grid_child_position_set(EWL_GRID(grid), child, 0, 2, 1, 3);
        ewl_widget_show(child);

        ewl_grid_child_position_get(EWL_GRID(grid), child, &col_start, &col_end, 
                        &row_start, &row_end);

        if (col_start != 0 || col_end != 2 || row_start != 1 || row_end != 3) {
                LOG_FAILURE(buf, len, "child has wrong position");
                ret = 0;
        }

        ewl_widget_destroy(grid);

        return ret;
}

static int 
floating_position_get(char *buf, int len)
{
        Ewl_Widget *grid;
        Ewl_Widget *w[5];
        Ewl_Widget *pl;
        int ret = 1, i;
        int row = 0, col = 0;

        grid = ewl_grid_new();
        ewl_grid_dimensions_set(EWL_GRID(grid), 2, 3);
        ewl_widget_show(grid);

        /* we place one widget */
        pl = ewl_cell_new();
        ewl_container_child_append(EWL_CONTAINER(grid), pl);
        ewl_grid_child_position_set(EWL_GRID(grid), pl, 0, 0, 1, 1);
        ewl_widget_show(pl);

        /* first add the children to the grid */
        for (i = 0; i < 5; i++) {
                w[i] = ewl_cell_new();
                ewl_container_child_append(EWL_CONTAINER(grid), w[i]);
                ewl_widget_show(w[i]);
        }

        /* now check the positions */
        for (i = 0; i < 5; i++) {
                int r, c;
                
                ewl_grid_child_position_get(EWL_GRID(grid), w[i], &c, NULL, &r,
                                NULL);
                if (c != col || r != row) {
                        LOG_FAILURE(buf, len, "child isn't placed right");
                        ret = 0;
                        break;
                }
                col++;
                if (col > 1) {
                        col = 0;
                        row++;
                }
                /* here is our placed widget we don't want to check this
                 * position */
                if (col == 0 && row == 1)
                        col = 1;
        }

        ewl_widget_destroy(grid);

        return ret;
}

static int
dimensions_auto_resize(char *buf, int len)
{
        Ewl_Widget *grid;
        int ret = 1, i, number = 26;

        grid = ewl_grid_new();
        ewl_grid_dimensions_set(EWL_GRID(grid), 2, 0);
        ewl_widget_show(grid);

        for (i = 0; i < number; i++) {
                Ewl_Widget *child;

                child = ewl_cell_new();
                ewl_container_child_append(EWL_CONTAINER(grid), child);
                ewl_widget_show(child);
        }

        i = 0;
        ewl_grid_dimensions_get(EWL_GRID(grid), NULL, &i);

        if (i != number / 2) {
                LOG_FAILURE(buf, len, "the grid doesn't have the expected dimensions");
                ret = 0;
        }

        ewl_widget_destroy(grid);

        return ret;
}

static int
orientation_set_get(char *buf, int len)
{
        Ewl_Widget *grid;
        int ret = 1;

        grid = ewl_grid_new();

        ewl_grid_orientation_set(EWL_GRID(grid), EWL_ORIENTATION_HORIZONTAL);
        if (ewl_grid_orientation_get(EWL_GRID(grid)) 
                        != EWL_ORIENTATION_HORIZONTAL) {
                LOG_FAILURE(buf, len, "orientation is not horizontal");
                ret = 0;
                goto CLEANUP;
        }
        
        ewl_grid_orientation_set(EWL_GRID(grid), EWL_ORIENTATION_VERTICAL);
        if (ewl_grid_orientation_get(EWL_GRID(grid)) 
                        != EWL_ORIENTATION_VERTICAL) {
                LOG_FAILURE(buf, len, "orientation is not vertiacal");
                ret = 0;
                goto CLEANUP;
        }

CLEANUP:
        ewl_widget_destroy(grid);
        return ret;
}

