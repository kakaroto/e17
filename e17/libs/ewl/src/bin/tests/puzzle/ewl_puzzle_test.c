/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_grid.h"
#include "ewl_image.h"
#include "ewl_separator.h"
#include "ewl_text.h"

#include <stdio.h>
#include <time.h>
#include <limits.h>

static int create_test(Ewl_Container *box);
static int sign(int x);
static void puzzle_grid_fill(Ewl_Grid *grid);
static void puzzle_clicked_cb(Ewl_Widget *w, void *e, void *data);
static void puzzle_mouse_up_cb(Ewl_Widget *w, void *e, void *data);

static Ewl_Widget *childs[4][4];
static int free_col, free_row;

void
test_info(Ewl_Test *test)
{
        test->name = "Puzzle";
        test->tip = "Play with the grid";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_MISC;
        test->func = create_test;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *w;
        Ewl_Widget *grid;
        Ewl_Widget *hbox;
        Ewl_Widget *c;

        /*
         * set up the outer grid
         */
        c = ewl_grid_new();
        ewl_container_child_append(box, c);
        ewl_grid_dimensions_set(EWL_GRID(c), 1, 2);
        ewl_grid_row_preferred_h_use(EWL_GRID(c), 1);
        ewl_widget_show(c);

        /*
         * the first hbox
         */
        hbox = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(c), hbox);
        ewl_widget_show(hbox);


        /*
         * set up the grid
         */
        grid = ewl_grid_new();
        ewl_grid_dimensions_set(EWL_GRID(grid), 4, 4);
        ewl_grid_homogeneous_set(EWL_GRID(grid), TRUE);
        ewl_container_child_append(EWL_CONTAINER(hbox), grid);
        ewl_widget_show(grid);

        puzzle_grid_fill(EWL_GRID(grid));

        /* the seperator */
        w = ewl_vseparator_new();
        ewl_container_child_append(EWL_CONTAINER(hbox), w);
        ewl_widget_show(w);

        /*
         * the real image
         */
        w = ewl_image_new();
        ewl_image_file_path_set(EWL_IMAGE(w), PACKAGE_DATA_DIR
                                                "/ewl/images/e-logo.png");
        ewl_image_proportional_set(EWL_IMAGE(w), FALSE);
        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);
        ewl_container_child_append(EWL_CONTAINER(hbox), w);
        ewl_widget_show(w);

        /*
         * the hbox
         */
        hbox = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(c), hbox);
        ewl_widget_show(hbox);

        /*
         * some text
         */
        w = ewl_text_new();
        ewl_object_insets_set(EWL_OBJECT(w), 10, 10, 10, 10);
        ewl_text_wrap_set(EWL_TEXT(w), TRUE);
        ewl_object_w_request(EWL_OBJECT(w), 400);
        ewl_text_text_append(EWL_TEXT(w),
                        "The rules are really simple. Just try "
                        "to have the same image on your left "
                        "like you have on your right hand.");
        ewl_container_child_append(EWL_CONTAINER(hbox), w);
        ewl_widget_show(w);

        /*
         * the shuffle button
         */
        w = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(w), "shuffle");
        ewl_callback_append(w, EWL_CALLBACK_CLICKED, puzzle_clicked_cb, grid);
        ewl_container_child_append(EWL_CONTAINER(hbox), w);
        ewl_widget_show(w);

        return 1;
}

static void
puzzle_grid_fill(Ewl_Grid *grid)
{
        Ewl_Widget *w;
        int i, random;
        int unsort[6][15] =
                {{4, 2, 5, 14, 0, 11, 6, 13, 10, 12, 1, 9, 8, 7, 3},
                 {0, 2, 3, 7, 8, 4, 1, 6, 12, 5, 10, 11, 9, 13, 14},
                 {1, 2, 6, 3, 13, 7, 10, 11, 0, 5, 12, 14, 4, 8, 9},
                 {1, 10, 2, 3, 7, 9, 11, 6, 0, 8, 12, 5, 14, 4, 13},
                 {0, 5, 1, 2, 9, 3, 7, 10, 4, 6, 11, 14, 8, 12, 13},
                 {1, 3, 11, 14, 0, 7, 2, 13, 8, 12, 6, 10, 4, 9, 5}};

        /* a really stupid randomizer */
        random = time(NULL) % 6;

        /*
         * fill the grid with content
         */
        for (i = 0; i < 15; i++)
        {
                char buf[PATH_MAX];

                snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR
                                "/ewl/images/e-logo-%i.png", unsort[random][i]);

                w = ewl_image_new();
                ewl_image_file_path_set(EWL_IMAGE(w), buf);
                ewl_image_proportional_set(EWL_IMAGE(w), FALSE);
                ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);
                ewl_container_child_append(EWL_CONTAINER(grid), w);
                ewl_grid_child_position_set(EWL_GRID(grid), w,
                                i % 4, i % 4, i / 4, i / 4);
                ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP,
                                        puzzle_mouse_up_cb, grid);
                ewl_widget_show(w);

                childs[i % 4][i / 4] = w;
        }
        free_col = free_row = 3;
}

static void
puzzle_clicked_cb(Ewl_Widget *w __UNUSED__, void *e __UNUSED__, void *data)
{
        ewl_container_reset(EWL_CONTAINER(data));
        puzzle_grid_fill(EWL_GRID(data));
}

static void
puzzle_mouse_up_cb(Ewl_Widget *w, void *e, void *data)
{
        int cx, cy, cw, ch;
        int col, row;
        Ewl_Event_Mouse *ev;
        Ewl_Widget *g;

        g = data;
        ev = e;
        /*
         * get the current geometry of the widget
         */
        cx = ewl_object_current_x_get(EWL_OBJECT(w));
        cy = ewl_object_current_y_get(EWL_OBJECT(w));
        cw = ewl_object_current_w_get(EWL_OBJECT(w));
        ch = ewl_object_current_h_get(EWL_OBJECT(w));

        if (ev->x > cx && ev->y > cy && ev->x < cx + cw && ev->y < cy + ch)
                return;

        ewl_grid_child_position_get(EWL_GRID(g), w, &col, NULL, &row, NULL);

        if (col == free_col && ev->x > cx && ev->x < cx + cw) {
                int direction, i;

                if (ev->y < cy)
                        direction = -1;
                else
                        direction = 1;

                if (sign(free_row - row) != direction)
                        return;

                for (i = free_row; i != row; i -= direction) {
                        Ewl_Widget *move;

                        move = childs[col][i - direction];
                        ewl_grid_child_position_set(EWL_GRID(g),
                                move, col, col, i, i);
                        childs[col][i] = move;
                }
                free_row = row;
        }
        else if (row == free_row && ev->y > cy && ev->y < cy + ch) {
                int direction, i;

                if (ev->x < cx)
                        direction = -1;
                else
                        direction = 1;

                if (sign(free_col - col) != direction)
                        return;

                for (i = free_col; i != col; i -= direction) {
                        Ewl_Widget *move;

                        move = childs[i - direction][row];
                        ewl_grid_child_position_set(EWL_GRID(g),
                                move, i, i, row, row);
                        childs[i][row] = move;
                }
                free_col = col;
        }
}

static int
sign(int x)
{
        if (x > 0)
                return 1;
        if (x < 0)
                return -1;

        return 0;
}

