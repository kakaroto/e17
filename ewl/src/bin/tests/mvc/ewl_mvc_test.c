/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_mvc.h"
#include "ewl_tree.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int selected_set(char *buf, int len);
static int selected_get(char *buf, int len);
static int selected_range_single_add(char *buf, int len);
static int selected_range_multi_add(char *buf, int len);
static int selected_count_get(char *buf, int len);
static int selected_is_index(char *buf, int len);
static int selected_is_not_index(char *buf, int len);
static int selected_is_range(char *buf, int len);
static int selected_is_not_range(char *buf, int len);
static int selected_clear(char *buf, int len);

static int selected_rm_idx(char *buf, int len);
static int selected_rm_unselected_idx(char *buf, int len);
static int selected_rm_2x1_bottom(char *buf, int len);
static int selected_rm_2x1_top(char *buf, int len);
static int selected_rm_1x2_left(char *buf, int len);
static int selected_rm_1x2_right(char *buf, int len);
static int selected_rm_from_range_top_left_point(char *buf, int len);
static int selected_rm_from_range_bottom_right_point(char *buf, int len);
static int selected_rm_from_range_middle_point(char *buf, int len);
static int selected_rm_from_range_left_edge_point(char *buf, int len);
static int selected_rm_from_range_right_edge_point(char *buf, int len);
static int selected_rm_from_range_top_edge_point(char *buf, int len);
static int selected_rm_from_range_bottom_edge_point(char *buf, int len);

static int insert_range_no_intersection(char *buf, int len);
static int insert_range_top_intersection(char *buf, int len);
static int insert_range_bottom_intersection(char *buf, int len);
static int insert_range_left_intersection(char *buf, int len);
static int insert_range_right_intersection(char *buf, int len);
static int insert_range_top_left_intersection(char *buf, int len);
static int insert_range_top_right_intersection(char *buf, int len);
static int insert_range_bottom_left_intersection(char *buf, int len);
static int insert_range_bottom_right_intersection(char *buf, int len);
static int insert_range_overlap_horizontal(char *buf, int len);
static int insert_range_overlap_vertical(char *buf, int len);
static int insert_range_overlap_new_covers_old(char *buf, int len);
static int insert_range_overlap_old_covers_new(char *buf, int len);

static int ewl_mvc_is_index(char *buf, int len, Ewl_Selection *sel,
                                                int row, int column);
static int ewl_mvc_is_range(char *buf, int len, Ewl_Selection *sel,
                                        int srow, int scolumn,
                                        int erow, int ecolumn);

static Ewl_Unit_Test mvc_unit_tests[] = {
                {"Set selected", selected_set, NULL, -1, 0},
                {"Get selected", selected_get, NULL, -1, 0},
                {"Range (single select) add", selected_range_single_add, NULL, -1, 0},
                {"Range (multi select) add", selected_range_multi_add, NULL, -1, 0},
                {"Count get", selected_count_get, NULL, -1, 0},
                {"Is selected (index)", selected_is_index, NULL, -1, 0},
                {"Is not selected (index)", selected_is_not_index, NULL, -1, 0},
                {"Is selected (range)", selected_is_range, NULL, -1, 0},
                {"Is not selected (range)", selected_is_not_range, NULL, -1, 0},
                {"Selected clear", selected_clear, NULL, -1, 0},

                {"Remove index", selected_rm_idx, NULL, -1, 0},
                {"Remove non-selected index", selected_rm_unselected_idx, NULL, -1, 0},
                {"Remove bottom from 2x1", selected_rm_2x1_bottom, NULL, -1, 0},
                {"Remove top from 2x1", selected_rm_2x1_top, NULL, -1, 0},
                {"Remove left from 1x2", selected_rm_1x2_left, NULL, -1, 0},
                {"Remove right from 1x2", selected_rm_1x2_right, NULL, -1, 0},
                {"Remove from range, top left point",
                                selected_rm_from_range_top_left_point, NULL, -1, 0},
                {"Remove from range, bottom right point",
                                selected_rm_from_range_bottom_right_point, NULL, -1, 0},
                {"Remove from range, middle point",
                                selected_rm_from_range_middle_point, NULL, -1, 0},
                {"Remove from range, left edge",
                                selected_rm_from_range_left_edge_point, NULL, -1, 0},
                {"Remove from range, right edge",
                                selected_rm_from_range_right_edge_point, NULL, -1, 0},
                {"Remove from range, top edge",
                                selected_rm_from_range_top_edge_point, NULL, -1, 0},
                {"Remove from range, bottom edge",
                                selected_rm_from_range_bottom_edge_point, NULL, -1, 0},

                {"Insert range no intersection",
                                insert_range_no_intersection, NULL, -1, 0},
                {"Insert range top intersection",
                                insert_range_top_intersection, NULL, -1, 0},
                {"Insert range bottom intersection",
                                insert_range_bottom_intersection, NULL, -1, 0},
                {"Insert range left intersection",
                                insert_range_left_intersection, NULL, -1, 0},
                {"Insert range right intersection",
                                insert_range_right_intersection, NULL, -1, 0},
                {"Insert range intersect, top left corner",
                                insert_range_top_left_intersection, NULL, -1, 0},
                {"Insert range intersect, top right corner",
                                insert_range_top_right_intersection, NULL, -1, 0},
                {"Insert range intersect, bottom left corner",
                                insert_range_bottom_left_intersection, NULL, -1, 0},
                {"Insert range intersect, bottom right corner",
                                insert_range_bottom_right_intersection, NULL, -1, 0},
                {"Insert range intersect, overlap horizontal",
                                insert_range_overlap_horizontal, NULL, -1, 0},
                {"Insert range intersect, overlap vertical",
                                insert_range_overlap_vertical, NULL, -1, 0},
                {"Insert range intersect, new covers old",
                                insert_range_overlap_new_covers_old, NULL, -1, 0},
                {"Insert range intersect, old covers new",
                                insert_range_overlap_old_covers_new, NULL, -1, 0},

                {NULL, NULL, NULL, -1, 0}
        };

void
test_info(Ewl_Test *test)
{
        test->name = "MVC";
        test->tip = "The base mvc type.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_UNIT;
        test->unit_tests = mvc_unit_tests;
}

static int
selected_set(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection_Idx *idx;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selected_set(m, NULL, ewl_mvc_data_get(m), 1, 2);
        idx = ecore_list_first_goto(m->selected);

        if (idx->sel.type != EWL_SELECTION_TYPE_INDEX)
        {
                LOG_FAILURE(buf, len, "Incorrect selection type");
                return FALSE;
        }

        if ((idx->row != 1) || (idx->column != 2))
        {
                LOG_FAILURE(buf, len, "Incorrect row/column setting: "
                                "(%d %d) vs (1, 2)", idx->row, idx->column);
                return FALSE;
        }

        return TRUE;
}

static int
selected_get(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection_Idx *idx;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selected_set(m, NULL, ewl_mvc_data_get(m), 6, 8);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 3, 0);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 9, 1);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 1, 2);
        idx = ewl_mvc_selected_get(m);

        if (idx->sel.type != EWL_SELECTION_TYPE_INDEX)
        {
                LOG_FAILURE(buf, len, "Incorrect selection type");
                return FALSE;
        }

        if ((idx->row != 1) || (idx->column != 2))
        {
                LOG_FAILURE(buf, len, "Incorrect row/column setting "
                        "(%d %d) instead of (1, 2)", idx->row, idx->column);
                return FALSE;
        }

        return TRUE;
}

static int
selected_range_single_add(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection_Idx *idx;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);
        idx = ecore_list_first_goto(m->selected);

        if (idx->sel.type != EWL_SELECTION_TYPE_INDEX)
        {
                LOG_FAILURE(buf, len, "Incorrect selection type");
                return FALSE;
        }

        if ((idx->row != 1) || (idx->column != 2))
        {
                LOG_FAILURE(buf, len, "Incorrect start row/column setting");
                return FALSE;
        }

        return TRUE;
}

static int
selected_range_multi_add(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection_Range *idx;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, NULL, 6, 8, 1, 2);
        idx = ecore_list_first_goto(m->selected);

        if (!idx)
        {
                LOG_FAILURE(buf, len, "Returned idx is NULL");
                return FALSE;
        }

        if (idx->sel.type != EWL_SELECTION_TYPE_RANGE)
        {
                LOG_FAILURE(buf, len, "Incorrect selection type");
                return FALSE;
        }

        if ((idx->start.row != 1) || (idx->start.column != 2))
        {
                LOG_FAILURE(buf, len, "Incorrect start row/column setting");
                return FALSE;
        }

        if ((idx->end.row != 6) || (idx->end.column != 8))
        {
                LOG_FAILURE(buf, len, "Incorrect end row/column setting");
                return FALSE;
        }

        return TRUE;
}

static int
selected_count_get(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        int count, expected = 44;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_set(m, NULL, ewl_mvc_data_get(m), 6, 8);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 3, 0);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 9, 1);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 1, 2);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);

        count = ewl_mvc_selected_count_get(m);
        if (count != expected)
        {
                LOG_FAILURE(buf, len, "Incorrect count (%d instead of %d)",
                                                        count, expected);
                return FALSE;
        }

        return TRUE;
}

static int
selected_is_index(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selected_set(m, NULL, ewl_mvc_data_get(m), 6, 8);

        if (!ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 6, 8))
        {
                LOG_FAILURE(buf, len, "Not selected");
                return FALSE;
        }

        return TRUE;
}

static int
selected_is_not_index(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selected_set(m, NULL, ewl_mvc_data_get(m), 6, 8);

        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 7, 7))
        {
                LOG_FAILURE(buf, len, "Item selected");
                return FALSE;
        }

        return TRUE;
}

static int
selected_is_range(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);

        if (!ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 4, 5))
        {
                LOG_FAILURE(buf, len, "Not selected");
                return FALSE;
        }

        return TRUE;
}

static int
selected_is_not_range(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);

        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 1, 1))
        {
                LOG_FAILURE(buf, len, "Item selected selected");
                return FALSE;
        }

        return TRUE;
}

static int
selected_clear(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_set(m, NULL, ewl_mvc_data_get(m), 6, 8);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 3, 0);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 9, 1);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 1, 2);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);
        ewl_mvc_selected_clear(m);

        if (ewl_mvc_selected_count_get(m) != 0)
        {
                LOG_FAILURE(buf, len, "Selected list not empty.");
                return FALSE;
        }

        return TRUE;
}

static int
selected_rm_idx(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_set(m, NULL, ewl_mvc_data_get(m), 6, 8);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 3, 0);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 9, 1);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 1, 2);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 9, 1);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 9, 1))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        return TRUE;
}

static int
selected_rm_unselected_idx(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_set(m, NULL, ewl_mvc_data_get(m), 6, 8);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 3, 0);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 9, 1);
        ewl_mvc_selected_add(m, NULL, ewl_mvc_data_get(m), 1, 2);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 10, 2);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 10, 2))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        return TRUE;
}

/*
 * The X shows either the box to remove or the position the point is being
 * removed from
 */

/*
 * ------
 * |    |
 * ------
 * |  X |
 * ------
 */
static int
selected_rm_2x1_bottom(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 2, 1);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 2, 1);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 2, 1))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        count = ewl_mvc_selected_count_get(m);
        if (count != 1)
        {
                LOG_FAILURE(buf, len, "Selected items is %d instead of 1", count);
                return FALSE;
        }

        sel = ecore_list_first_goto(m->selected);

        if (!ewl_mvc_is_index(buf, len, sel, 1, 1))
                return FALSE;

        return TRUE;
}

/*
 * ------
 * |  X |
 * ------
 * |    |
 * ------
 */
static int
selected_rm_2x1_top(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 2, 1);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 1, 1);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 1, 1))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        count = ewl_mvc_selected_count_get(m);
        if (count != 1)
        {
                LOG_FAILURE(buf, len, "Selected items is %d instead of 1", count);
                return FALSE;
        }

        sel = ecore_list_first_goto(m->selected);

        if (!ewl_mvc_is_index(buf, len, sel, 2, 1))
                return FALSE;

        return TRUE;
}

/*
 * ------------
 * | X  |     |
 * ------------
 */
static int
selected_rm_1x2_left(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 1, 2);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 1, 1);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 1, 1))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        if (ewl_mvc_selected_count_get(m) != 1)
        {
                LOG_FAILURE(buf, len, "Incorrect number of selected items");
                return FALSE;
        }

        sel = ecore_list_first_goto(m->selected);

        if (!ewl_mvc_is_index(buf, len, sel, 1, 2))
                return FALSE;

        return TRUE;
}

/*
 * ------------
 * |    |  X  |
 * ------------
 */
static int
selected_rm_1x2_right(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 1, 2);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 1, 2);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 1, 2))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        if (ewl_mvc_selected_count_get(m) != 1)
        {
                LOG_FAILURE(buf, len, "Incorrect number of selected items");
                return FALSE;
        }

        sel = ecore_list_first_goto(m->selected);

        if (!ewl_mvc_is_index(buf, len, sel, 1, 1))
                return FALSE;

        return TRUE;
}

/*
 *  -------
 *  |X    |
 *  |     |
 *  -------
 */
static int
selected_rm_from_range_top_left_point(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 1, 2);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 1, 2))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        if (ewl_mvc_selected_count_get(m) != 41)
        {
                LOG_FAILURE(buf, len, "Incorrect number of selected items: %d",
                                        ewl_mvc_selected_count_get(m));
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 2, 2, 6, 8))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 3, 1, 8))
                return FALSE;

        return TRUE;
}

/*
 *  -------
 *  |     |
 *  |    X|
 *  -------
 */
static int
selected_rm_from_range_bottom_right_point(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 6, 8);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 6, 8))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        if (ewl_mvc_selected_count_get(m) != 41)
        {
                LOG_FAILURE(buf, len, "Incorrect number of selected items: %d",
                                        ewl_mvc_selected_count_get(m));
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 5, 8))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 6, 2, 6, 7))
                return FALSE;

        return TRUE;
}

/*
 *  -------
 *  |     |
 *  |  X  |
 *  |     |
 *  -------
 */
static int
selected_rm_from_range_middle_point(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 4, 5);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 4, 5))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        if (ewl_mvc_selected_count_get(m) != 41)
        {
                LOG_FAILURE(buf, len, "Incorrect number of selected items: %d",
                                        ewl_mvc_selected_count_get(m));
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 3, 8))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 4, 2, 6, 4))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 2);
        if (!ewl_mvc_is_range(buf, len, sel, 5, 5, 6, 8))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 3);
        if (!ewl_mvc_is_range(buf, len, sel, 4, 6, 4, 8))
                return FALSE;

        return TRUE;
}

/*
 *  -------
 *  |     |
 *  |X    |
 *  |     |
 *  -------
 */
static int
selected_rm_from_range_left_edge_point(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 4, 2);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 4, 2))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        if (ewl_mvc_selected_count_get(m) != 41)
        {
                LOG_FAILURE(buf, len, "Incorrect number of selected items: %d",
                                        ewl_mvc_selected_count_get(m));
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 3, 8))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 5, 2, 6, 8))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 2);
        if (!ewl_mvc_is_range(buf, len, sel, 4, 3, 4, 8))
                return FALSE;

        return TRUE;
}

/*
 *  -------
 *  |     |
 *  |    X|
 *  |     |
 *  -------
 */
static int
selected_rm_from_range_right_edge_point(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 3, 8);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 3, 8))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        if (ewl_mvc_selected_count_get(m) != 41)
        {
                LOG_FAILURE(buf, len, "Incorrect number of selected items: %d",
                                        ewl_mvc_selected_count_get(m));
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 2, 8))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 3, 2, 6, 7))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 2);
        if (!ewl_mvc_is_range(buf, len, sel, 4, 8, 6, 8))
                return FALSE;

        return TRUE;
}

/*
 *  -------
 *  |  X  |
 *  |     |
 *  |     |
 *  -------
 */
static int
selected_rm_from_range_top_edge_point(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 1, 5);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 1, 5))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        if (ewl_mvc_selected_count_get(m) != 41)
        {
                LOG_FAILURE(buf, len, "Incorrect number of selected items: %d",
                                        ewl_mvc_selected_count_get(m));
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 6, 4))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 2, 5, 6, 8))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 2);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 6, 1, 8))
                return FALSE;

        return TRUE;
}

/*
 *  -------
 *  |     |
 *  |     |
 *  |  X  |
 *  -------
 */
static int
selected_rm_from_range_bottom_edge_point(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 6, 8, 1, 2);

        ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 6, 3);
        if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 6, 3))
        {
                LOG_FAILURE(buf, len, "Index still selected");
                return FALSE;
        }

        if (ewl_mvc_selected_count_get(m) != 41)
        {
                LOG_FAILURE(buf, len, "Incorrect number of selected items: %d",
                                        ewl_mvc_selected_count_get(m));
                return FALSE;
        }

        /* should have 3 ranges: [(1,2)(5,8)]
                                 [(2,6)]
                                 [(6,4)(6,8)] */
        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 5, 8))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_index(buf, len, sel, 6, 2))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 2);
        if (!ewl_mvc_is_range(buf, len, sel, 6, 4, 6, 8))
                return FALSE;

        return TRUE;
}

/*
 * c == currently in the list
 * r == range to be inserted
 */
/*
 * ----
 * | c|
 * ----  ----
 *       |r |
 *       ----
 */
static int
insert_range_no_intersection(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 3, 3);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 4, 4, 6, 6);

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 1, 3, 3))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 4, 4, 6, 6))
                return FALSE;

        return TRUE;
}

/*
 * ----
 * | r|
 * |--|
 * | c|
 * ----
 */
static int
insert_range_top_intersection(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 3, 1, 7, 3);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 3, 3);

        count = ecore_list_count(m->selected);
        if (count != 1)
        {
                LOG_FAILURE(buf, len, "%d items in list instead of 1", count);
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 1, 7, 3))
                return FALSE;

        return TRUE;
}

/*
 * ----
 * | c|
 * |--|
 * | r|
 * ----
 */
static int
insert_range_bottom_intersection(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 3, 3);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 3, 1, 7, 3);

        count = ecore_list_count(m->selected);
        if (count != 1)
        {
                LOG_FAILURE(buf, len, "%d items in list instead of 1", count);
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 1, 7, 3))
                return FALSE;

        return TRUE;
}

/*
 * --------
 * |  r| c|
 * --------
 */
static int
insert_range_left_intersection(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 3, 3, 5);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 3, 3);

        count = ecore_list_count(m->selected);
        if (count != 1)
        {
                LOG_FAILURE(buf, len, "%d items in list instead of 1", count);
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 1, 3, 5))
                return FALSE;

        return TRUE;
}

/*
 * --------
 * |  c| r|
 * --------
 */
static int
insert_range_right_intersection(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 3, 3);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 3, 3, 5);

        count = ecore_list_count(m->selected);
        if (count != 1)
        {
                LOG_FAILURE(buf, len, "%d items in list instead of 1", count);
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 1, 3, 5))
                return FALSE;

        return TRUE;
}

/*
 * ------
 * | r  |
 * |  ------|
 * ---|-|   |
 *    |   c |
 *    -------
 */
static int
insert_range_top_left_intersection(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 3, 3, 7, 7);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 5, 5);

        count = ecore_list_count(m->selected);
        if (count != 3)
        {
                LOG_FAILURE(buf, len, "%d items in list instead of 3", count);
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 6, 3, 7, 7))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 3, 6, 5, 7))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 2);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 1, 5, 5))
                return FALSE;

        return TRUE;
}

/*
 *      -------
 *      | r   |
 *  ----|--   |
 *  |   --|----
 *  | c   |
 *  -------
 */
static int
insert_range_top_right_intersection(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 3, 1, 5, 3);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 2, 4, 5);

        count = ecore_list_count(m->selected);
        if (count != 3)
        {
                LOG_FAILURE(buf, len, "%d items in list instead of 3", count);
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 3, 1, 5, 1))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 5, 2, 5, 3))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 2);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 4, 5))
                return FALSE;

        return TRUE;
}

/*
 *      -------
 *      | c   |
 *  ----|--   |
 *  |   --|----
 *  | r   |
 *  -------
 */
static int
insert_range_bottom_left_intersection(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 2, 4, 5);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 3, 1, 5, 3);

        count = ecore_list_count(m->selected);
        if (count != 3)
        {
                LOG_FAILURE(buf, len, "%d items in list instead of 3", count);
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 2, 5))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 3, 4, 4, 5))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 2);
        if (!ewl_mvc_is_range(buf, len, sel, 3, 1, 5, 3))
                return FALSE;

        return TRUE;
}

/*
 * ------
 * | c  |
 * |  ------|
 * ---|-|   |
 *    |   r |
 *    -------
 */
static int
insert_range_bottom_right_intersection(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 5, 5);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 3, 3, 7, 7);

        count = ecore_list_count(m->selected);
        if (count != 3)
        {
                LOG_FAILURE(buf, len, "%d items in list instead of 3", count);
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 1, 2, 5))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 3, 1, 5, 2))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 2);
        if (!ewl_mvc_is_range(buf, len, sel, 3, 3, 7, 7))
                return FALSE;

        return TRUE;
}

/*
 *    ----
 *    |r |
 * ----------
 * |  |  |  |
 * |c |  |  |
 * ----------
 *    |  |
 *    ----
 */
static int
insert_range_overlap_horizontal(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 3, 1, 5, 9);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 3, 7, 5);

        count = ecore_list_count(m->selected);
        if (count != 3)
        {
                LOG_FAILURE(buf, len, "%d items in list instead of 3", count);
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 3, 1, 5, 2))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 3, 6, 5, 9))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 2);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 3, 7, 5))
                return FALSE;

        return TRUE;
}

/*
 *    ----
 *    |c |
 * ----------
 * |  |  |  |
 * |r |  |  |
 * ----------
 *    |  |
 *    ----
 */
static int
insert_range_overlap_vertical(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 3, 7, 5);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 3, 1, 5, 9);

        count = ecore_list_count(m->selected);
        if (count != 3)
        {
                LOG_FAILURE(buf, len, "%d items in list instead of 3", count);
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 3, 2, 5))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 1);
        if (!ewl_mvc_is_range(buf, len, sel, 6, 3, 7, 5))
                return FALSE;

        sel = ecore_list_index_goto(m->selected, 2);
        if (!ewl_mvc_is_range(buf, len, sel, 3, 1, 5, 9))
                return FALSE;

        return TRUE;
}

/*
 * -----------
 * |     r   |
 * |  -----  |
 * |  | c |  |
 * |  -----  |
 * -----------
 */
static int
insert_range_overlap_new_covers_old(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 3, 3, 5, 5);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 9, 9);

        count = ecore_list_count(m->selected);
        if (count != 1)
        {
                LOG_FAILURE(buf, len, "%d items in list instead of 1", count);
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 1, 9, 9))
                return FALSE;

        return TRUE;
}

/*
 * -----------
 * |     c   |
 * |  -----  |
 * |  | r |  |
 * |  -----  |
 * -----------
 */
static int
insert_range_overlap_old_covers_new(char *buf, int len)
{
        Ewl_Widget *t;
        Ewl_MVC *m;
        Ewl_Selection *sel;
        int count;

        t = ewl_tree_new();
        m = EWL_MVC(t);

        ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 1, 1, 9, 9);
        ewl_mvc_selected_range_add(m, NULL, ewl_mvc_data_get(m), 3, 3, 5, 5);

        count = ecore_list_count(m->selected);
        if (count != 1)
        {
                LOG_FAILURE(buf, len, "%d items in list instead of 1", count);
                return FALSE;
        }

        sel = ecore_list_index_goto(m->selected, 0);
        if (!ewl_mvc_is_range(buf, len, sel, 1, 1, 9, 9))
                return FALSE;

        return TRUE;
}

static int
ewl_mvc_is_index(char *buf, int len, Ewl_Selection *sel,
                                        int row, int column)
{
        Ewl_Selection_Idx *idx;

        if (sel->type != EWL_SELECTION_TYPE_INDEX)
        {
                LOG_FAILURE(buf, len, "Incorrect selection type");
                return FALSE;
        }

        idx = EWL_SELECTION_IDX(sel);
        if (((int)idx->row != row) || ((int)idx->column != column))
        {
                LOG_FAILURE(buf, len, "Incorrect indices in index "
                                        "([%d %d] vs [%d %d])",
                                        idx->row, idx->column,
                                        row, column);
                return FALSE;
        }

        return TRUE;
}

static int
ewl_mvc_is_range(char *buf, int len, Ewl_Selection *sel,
                                        int srow, int scolumn,
                                        int erow, int ecolumn)
{
        Ewl_Selection_Range *rng;

        if (sel->type != EWL_SELECTION_TYPE_RANGE)
        {
                LOG_FAILURE(buf, len, "Incorrect selection type");
                return FALSE;
        }

        rng = EWL_SELECTION_RANGE(sel);
        if (((int)rng->start.row != srow) || ((int)rng->start.column != scolumn))
        {
                LOG_FAILURE(buf, len, "Start row/column incorrect "
                                        "([%d %d] vs [%d %d])",
                                        rng->start.row, rng->start.column,
                                        srow, scolumn);
                return FALSE;
        }

        if (((int)rng->end.row != erow) || ((int)rng->end.column != ecolumn))
        {
                LOG_FAILURE(buf, len, "End row/column incorrect "
                                        "([%d %d] vs [%d %d])",
                                        rng->end.row, rng->end.column,
                                        erow, ecolumn);
                return FALSE;
        }

        return TRUE;
}


