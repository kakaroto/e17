/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
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

static int ewl_mvc_is_index(char *buf, int len, Ewl_Selection *sel, 
						int row, int column);
static int ewl_mvc_is_range(char *buf, int len, Ewl_Selection *sel, 
					int srow, int scolumn,
					int erow, int ecolumn);

static Ewl_Unit_Test mvc_unit_tests[] = {
		{"Set selected", selected_set},
		{"Get selected", selected_get},
		{"Range (single select) add", selected_range_single_add},
		{"Range (multi select) add", selected_range_multi_add},
		{"Count get", selected_count_get},
		{"Is selected (index)", selected_is_index},
		{"Is not selected (index)", selected_is_not_index},
		{"Is selected (range)", selected_is_range},
		{"Is not selected (range)", selected_is_not_range},
		{"Selected clear", selected_clear},
		{"Remove index", selected_rm_idx},
		{"Remove non-selected index", selected_rm_unselected_idx},
		{"Remove bottom from 2x1", selected_rm_2x1_bottom},
		{"Remove top from 2x1", selected_rm_2x1_top},
		{"Remove left from 1x2", selected_rm_1x2_left},
		{"Remove right from 1x2", selected_rm_1x2_right},
		{"Remove from range, top left point",
				selected_rm_from_range_top_left_point},
		{"Remove from range, bottom right point",
				selected_rm_from_range_bottom_right_point},
		{"Remove from range, middle point",
				selected_rm_from_range_middle_point},
		{"Remove from range, left edge",
				selected_rm_from_range_left_edge_point},
		{"Remove from range, right edge",
				selected_rm_from_range_right_edge_point},
		{"Remove from range, top edge",
				selected_rm_from_range_top_edge_point},
		{"Remove from range, bottom edge",
				selected_rm_from_range_bottom_edge_point},
		{NULL, NULL}
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

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selected_set(m, ewl_mvc_data_get(m), 1, 2);
	idx = ecore_list_goto_first(m->selected);

	if (idx->sel.type != EWL_SELECTION_TYPE_INDEX)
	{
		snprintf(buf, len, "Incorrect selection type");
		return FALSE;
	}

	if ((idx->row != 1) || (idx->column != 2))
	{
		snprintf(buf, len, "Incorrect row/column setting: "
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

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selected_set(m, ewl_mvc_data_get(m), 6, 8);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 3, 0);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 9, 1);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 1, 2);
	idx = ewl_mvc_selected_get(m);

	if (idx->sel.type != EWL_SELECTION_TYPE_INDEX)
	{
		snprintf(buf, len, "Incorrect selection type");
		return FALSE;
	}

	if ((idx->row != 1) || (idx->column != 2))
	{
		snprintf(buf, len, "Incorrect row/column setting");
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

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);
	idx = ecore_list_goto_first(m->selected);

	if (idx->sel.type != EWL_SELECTION_TYPE_INDEX)
	{
		snprintf(buf, len, "Incorrect selection type");
		return FALSE;
	}

	if ((idx->row != 1) || (idx->column != 2))
	{
		snprintf(buf, len, "Incorrect start row/column setting");
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

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);
	idx = ecore_list_goto_first(m->selected);

	if (idx->sel.type != EWL_SELECTION_TYPE_RANGE)
	{
		snprintf(buf, len, "Incorrect selection type");
		return FALSE;
	}

	if ((idx->start.row != 1) || (idx->start.column != 2))
	{
		snprintf(buf, len, "Incorrect start row/column setting");
		return FALSE;
	}

	if ((idx->end.row != 6) || (idx->end.column != 8))
	{
		snprintf(buf, len, "Incorrect end row/column setting");
		return FALSE;
	}

	return TRUE;
}

static int
selected_count_get(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	int count, expected = 46;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_set(m, ewl_mvc_data_get(m), 6, 8);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 3, 0);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 9, 1);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 1, 2);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);

	count = ewl_mvc_selected_count_get(m);
	if (count != expected)
	{
		snprintf(buf, len, "Incorrect count (%d instead of %d)", 
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

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selected_set(m, ewl_mvc_data_get(m), 6, 8);

	if (!ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 6, 8))
	{
		snprintf(buf, len, "Not selected");
		return FALSE;
	}

	return TRUE;
}

static int
selected_is_not_index(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selected_set(m, ewl_mvc_data_get(m), 6, 8);

	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 7, 7))
	{
		snprintf(buf, len, "Item selected");
		return FALSE;
	}

	return TRUE;
}

static int
selected_is_range(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);

	if (!ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 4, 5))
	{
		snprintf(buf, len, "Not selected");
		return FALSE;
	}

	return TRUE;
}

static int
selected_is_not_range(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);

	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 1, 1))
	{
		snprintf(buf, len, "Item selected selected");
		return FALSE;
	}

	return TRUE;
}

static int
selected_clear(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_set(m, ewl_mvc_data_get(m), 6, 8);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 3, 0);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 9, 1);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 1, 2);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);
	ewl_mvc_selected_clear(m);

	if (ewl_mvc_selected_count_get(m) != 0)
	{
		snprintf(buf, len, "Selected list not empty.");
		return FALSE;
	}

	return TRUE;
}

static int
selected_rm_idx(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_set(m, ewl_mvc_data_get(m), 6, 8);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 3, 0);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 9, 1);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 1, 2);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 9, 1);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 9, 1))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	return TRUE;
}

static int
selected_rm_unselected_idx(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_set(m, ewl_mvc_data_get(m), 6, 8);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 3, 0);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 9, 1);
	ewl_mvc_selected_add(m, ewl_mvc_data_get(m), 1, 2);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 10, 2);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 10, 2))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	return TRUE;
}

static int
selected_rm_2x1_bottom(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection *sel;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 1, 1, 2, 1);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 2, 1);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 2, 1))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	if (ewl_mvc_selected_count_get(m) != 1)
	{
		snprintf(buf, len, "Incorrect number of selected items");
		return FALSE;
	}

	sel = ecore_list_goto_first(m->selected);

	/* should have 1 index [(1,1)] */
	if (!ewl_mvc_is_index(buf, len, sel, 1, 1))
		return FALSE;

	return TRUE;
}

static int
selected_rm_2x1_top(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection *sel;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 1, 1, 2, 1);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 1, 1);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 1, 1))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	if (ewl_mvc_selected_count_get(m) != 1)
	{
		snprintf(buf, len, "Incorrect number of selected items");
		return FALSE;
	}

	sel = ecore_list_goto_first(m->selected);

	/* should have 1 index [(2,1)] */
	if (!ewl_mvc_is_index(buf, len, sel, 2, 1))
		return FALSE;

	return TRUE;
}

static int
selected_rm_1x2_left(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection *sel;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 1, 1, 1, 2);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 1, 1);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 1, 1))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	if (ewl_mvc_selected_count_get(m) != 1)
	{
		snprintf(buf, len, "Incorrect number of selected items");
		return FALSE;
	}

	sel = ecore_list_goto_first(m->selected);

	/* should have 1 index [(1,2)] */
	if (!ewl_mvc_is_index(buf, len, sel, 1, 2))
		return FALSE;

	return TRUE;
}

static int
selected_rm_1x2_right(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection *sel;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 1, 1, 1, 2);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 1, 2);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 1, 2))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	if (ewl_mvc_selected_count_get(m) != 1)
	{
		snprintf(buf, len, "Incorrect number of selected items");
		return FALSE;
	}

	sel = ecore_list_goto_first(m->selected);

	/* should have 1 index [(1,1)] */
	if (!ewl_mvc_is_index(buf, len, sel, 1, 1))
		return FALSE;

	return TRUE;
}

static int
selected_rm_from_range_top_left_point(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection *sel;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 1, 2);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 1, 2))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	if (ewl_mvc_selected_count_get(m) != 41)
	{
		snprintf(buf, len, "Incorrect number of selected items: %d",
					ewl_mvc_selected_count_get(m));
		return FALSE;
	}

	/* should have 2 ranges [(1,3)(6,8)] [(2,2)(6,2)] */
	sel = ecore_list_goto_index(m->selected, 0);
	if (!ewl_mvc_is_range(buf, len, sel, 1, 3, 6, 8))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 1);
	if (!ewl_mvc_is_range(buf, len, sel, 2, 2, 6, 2))
		return FALSE;

	return TRUE;
}

static int
selected_rm_from_range_bottom_right_point(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection *sel;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 6, 8);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 6, 8))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	if (ewl_mvc_selected_count_get(m) != 41)
	{
		snprintf(buf, len, "Incorrect number of selected items: %d",
					ewl_mvc_selected_count_get(m));
		return FALSE;
	}

	/* should have 2 ranges [(1,2)(5,8)] [(6,2)(6,7)] */
	sel = ecore_list_goto_index(m->selected, 0);
	if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 5, 8))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 1);
	if (!ewl_mvc_is_range(buf, len, sel, 6, 2, 6, 7))
		return FALSE;

	return TRUE;
}

static int
selected_rm_from_range_middle_point(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection *sel;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 4, 5);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 4, 5))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	if (ewl_mvc_selected_count_get(m) != 41)
	{
		snprintf(buf, len, "Incorrect number of selected items: %d",
					ewl_mvc_selected_count_get(m));
		return FALSE;
	}

	/* should have 4 ranges: [(1,2)(3,8)] 
				 [(4,2)(6,4)] 
				 [(4,6)(6,8)]
				 [(5,5)(6,5)] */
	sel = ecore_list_goto_index(m->selected, 0);
	if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 3, 8))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 1);
	if (!ewl_mvc_is_range(buf, len, sel, 4, 2, 6, 4))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 2);
	if (!ewl_mvc_is_range(buf, len, sel, 4, 6, 6, 8))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 3);
	if (!ewl_mvc_is_range(buf, len, sel, 5, 5, 6, 5))
		return FALSE;

	return TRUE;
}

static int
selected_rm_from_range_left_edge_point(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection *sel;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 4, 2);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 4, 2))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	if (ewl_mvc_selected_count_get(m) != 41)
	{
		snprintf(buf, len, "Incorrect number of selected items: %d",
					ewl_mvc_selected_count_get(m));
		return FALSE;
	}

	/* should have 3 ranges: [(1,2)(3,8)] 
				 [(4,3)(6,8)]
				 [(5,2)(6,2)] */
	sel = ecore_list_goto_index(m->selected, 0);
	if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 3, 8))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 1);
	if (!ewl_mvc_is_range(buf, len, sel, 4, 3, 6, 8))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 2);
	if (!ewl_mvc_is_range(buf, len, sel, 5, 2, 6, 2))
		return FALSE;

	return TRUE;
}

static int
selected_rm_from_range_right_edge_point(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection *sel;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 3, 8);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 3, 8))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	if (ewl_mvc_selected_count_get(m) != 41)
	{
		snprintf(buf, len, "Incorrect number of selected items: %d",
					ewl_mvc_selected_count_get(m));
		return FALSE;
	}

	/* should have 3 ranges: [(1,2)(2,8)] 
				 [(3,2)(6,7)] 
				 [(4,8)(6,8)] */
	sel = ecore_list_goto_index(m->selected, 0);
	if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 2, 8))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 1);
	if (!ewl_mvc_is_range(buf, len, sel, 3, 2, 6, 7))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 2);
	if (!ewl_mvc_is_range(buf, len, sel, 4, 8, 6, 8))
		return FALSE;

	return TRUE;
}

static int
selected_rm_from_range_top_edge_point(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection *sel;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 1, 5);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 1, 5))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	if (ewl_mvc_selected_count_get(m) != 41)
	{
		snprintf(buf, len, "Incorrect number of selected items: %d",
					ewl_mvc_selected_count_get(m));
		return FALSE;
	}

	/* should have 3 ranges: [(1,2)(6,4)] 
				 [(1,6)(6,8)]
				 [(2,5)(6,5)] */
	sel = ecore_list_goto_index(m->selected, 0);
	if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 6, 4))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 1);
	if (!ewl_mvc_is_range(buf, len, sel, 1, 6, 6, 8))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 2);
	if (!ewl_mvc_is_range(buf, len, sel, 2, 5, 6, 5))
		return FALSE;

	return TRUE;
}

static int
selected_rm_from_range_bottom_edge_point(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection *sel;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selection_mode_set(m, EWL_SELECTION_MODE_MULTI);
	ewl_mvc_selected_range_add(m, ewl_mvc_data_get(m), 6, 8, 1, 2);

	ewl_mvc_selected_rm(m, ewl_mvc_data_get(m), 6, 3);
	if (ewl_mvc_selected_is(m, ewl_mvc_data_get(m), 6, 3))
	{
		snprintf(buf, len, "Index still selected");
		return FALSE;
	}

	if (ewl_mvc_selected_count_get(m) != 41)
	{
		snprintf(buf, len, "Incorrect number of selected items: %d",
					ewl_mvc_selected_count_get(m));
		return FALSE;
	}

	/* should have 3 ranges: [(1,2)(5,8)] 
				 [(2,6)]
				 [(6,4)(6,8)] */
	sel = ecore_list_goto_index(m->selected, 0);
	if (!ewl_mvc_is_range(buf, len, sel, 1, 2, 5, 8))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 1);
	if (!ewl_mvc_is_index(buf, len, sel, 6, 2))
		return FALSE;

	sel = ecore_list_goto_index(m->selected, 2);
	if (!ewl_mvc_is_range(buf, len, sel, 6, 4, 6, 8))
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
		snprintf(buf, len, "Incorrect selection type");
		return FALSE;
	}

	idx = EWL_SELECTION_IDX(sel);
	if ((idx->row != row) || (idx->column != column))
	{
		snprintf(buf, len, "Incorrect indices in index "
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
		snprintf(buf, len, "Incorrect selection type");
		return FALSE;
	}

	rng = EWL_SELECTION_RANGE(sel);
	if ((rng->start.row != srow) || (rng->start.column != scolumn))
	{
		snprintf(buf, len, "Start row/column incorrect "
					"([%d %d] vs [%d %d])",
					rng->start.row, rng->start.column,
					srow, scolumn);
		return FALSE;
	}

	if ((rng->end.row != erow) || (rng->end.column != ecolumn))
	{
		snprintf(buf, len, "End row/column incorrect "
					"([%d %d] vs [%d %d])",
					rng->end.row, rng->end.column,
					erow, ecolumn);
		return FALSE;
	}

	return TRUE;
}


