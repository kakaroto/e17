/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_entry.h"
#include "ewl_grid.h"
#include "ewl_separator.h"

#include <stdio.h>

static int test_constructor(char *buf, int len);
static int test_dimensions_set_get(char *buf, int len);
static int test_column_fixed_set_get(char *buf, int len);
static int test_row_fixed_set_get(char *buf, int len);
static int test_column_relative_set_get(char *buf, int len);
static int test_row_relative_set_get(char *buf, int len);
static int test_column_size_remove(char *buf, int len);
static int test_row_size_remove(char *buf, int len);
static int test_htest_homogeneous_set_get(char *buf, int len);
static int test_vtest_homogeneous_set_get(char *buf, int len);
static int test_homogeneous_set_get(char *buf, int len);
static int test_position_set_get(char *buf, int len);
static int test_floating_position_get(char *buf, int len);
static int test_dimensions_auto_resize(char *buf, int len);
static int test_orientation_set_get(char *buf, int len);

Ewl_Unit_Test grid_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {"set and get dimensions", test_dimensions_set_get, NULL, -1, 0},
                {"set and get fixed column size", test_column_fixed_set_get, NULL, -1, 0},
                {"set and get fixed row size", test_row_fixed_set_get, NULL, -1, 0},
                {"set and get relative column size", test_column_relative_set_get, NULL, -1, 0},
                {"set and get relative row size", test_row_relative_set_get, NULL, -1, 0},
                {"remove fixed column size", test_column_size_remove, NULL, -1, 0},
                {"remove row column size", test_row_size_remove, NULL, -1, 0},
                {"set and get vertiacal homogeneous", test_vtest_homogeneous_set_get, NULL, -1, 0},
                {"set and get horizontal homogeneous", test_htest_homogeneous_set_get, NULL, -1, 0},
                {"set and get homogeneous", test_homogeneous_set_get, NULL, -1, 0},
                {"set and get position", test_position_set_get, NULL, -1, 0},
                {"get position of floating widgets", test_floating_position_get, NULL, -1, 0},
                {"auto resize dimensions", test_dimensions_auto_resize, NULL, -1, 0},
                {"set and get the orientation", test_orientation_set_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int 
test_constructor(char *buf, int len)
{
        Ewl_Widget *grid;
        int ret = 0;
        int col, row;

        grid = ewl_grid_new();
        ewl_grid_dimensions_get(EWL_GRID(grid), &col, &row);

        if (col != 2 && row != 2)
                LOG_FAILURE(buf, len, "dimensions are not 2x2");
        else if (ewl_grid_column_fixed_w_get(EWL_GRID(grid), 0))
                LOG_FAILURE(buf, len, "fixed width set for column 0");
        else if (ewl_grid_column_fixed_w_get(EWL_GRID(grid), 1))
                LOG_FAILURE(buf, len, "fixed width set for column 1");
        else if (ewl_grid_column_relative_w_get(EWL_GRID(grid), 0) != 0.0)
                LOG_FAILURE(buf, len, "relative width set for column 0");
        else if (ewl_grid_column_relative_w_get(EWL_GRID(grid), 1) != 0.0)
                LOG_FAILURE(buf, len, "relative width set for column 1");
        else if (ewl_grid_row_fixed_h_get(EWL_GRID(grid), 0))
                LOG_FAILURE(buf, len, "fixed heigth set for row 0");
        else if (ewl_grid_row_fixed_h_get(EWL_GRID(grid), 1))
                LOG_FAILURE(buf, len, "fixed heigth set for row 1");
        else if (ewl_grid_row_relative_h_get(EWL_GRID(grid), 0) != 0.0)
                LOG_FAILURE(buf, len, "relative heigth set for row 0");
        else if (ewl_grid_row_relative_h_get(EWL_GRID(grid), 1) != 0.0)
                LOG_FAILURE(buf, len, "relative height set for row 1");
        else if (ewl_grid_orientation_get(EWL_GRID(grid)) 
                        != EWL_ORIENTATION_HORIZONTAL)
                LOG_FAILURE(buf, len, "orientation is not horizontal");
        else if (ewl_grid_hhomogeneous_get(EWL_GRID(grid)))
                LOG_FAILURE(buf, len, "grid is vertical homogeneous");
        else if (ewl_grid_vhomogeneous_get(EWL_GRID(grid)))
                LOG_FAILURE(buf, len, "grid is horizontal homogeneous");
        else
                ret = 1;
        
        ewl_widget_destroy(grid);

        return ret;
}

static int 
test_dimensions_set_get(char *buf, int len)
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
test_column_fixed_set_get(char *buf, int len)
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
test_row_fixed_set_get(char *buf, int len)
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
test_column_relative_set_get(char *buf, int len)
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
test_row_relative_set_get(char *buf, int len)
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
test_column_size_remove(char *buf, int len)
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
test_row_size_remove(char *buf, int len)
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
test_vtest_homogeneous_set_get(char *buf, int len)
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
test_htest_homogeneous_set_get(char *buf, int len)
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
test_homogeneous_set_get(char *buf, int len)
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
test_position_set_get(char *buf, int len)
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
test_floating_position_get(char *buf, int len)
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
test_dimensions_auto_resize(char *buf, int len)
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
test_orientation_set_get(char *buf, int len)
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

