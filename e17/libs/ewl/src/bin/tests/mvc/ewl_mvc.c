#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int selected_set(char *buf, int len);
static int selected_get(char *buf, int len);
static int selected_list_set(char *buf, int len);
static int selected_list_get(char *buf, int len);
static int selected_range_single_add(char *buf, int len);
static int selected_range_multi_add(char *buf, int len);
static int selected_count_get(char *buf, int len);
static int selected_is_index(char *buf, int len);
static int selected_is_not_index(char *buf, int len);
static int selected_is_range(char *buf, int len);
static int selected_is_not_range(char *buf, int len);
static int selected_clear(char *buf, int len);

static int selected_rm_idx(char *buf, int len);
static int selected_rm_from_range_top_left_point(char *buf, int len);
static int selected_rm_from_range_bottom_right_point(char *buf, int len);
static int selected_rm_from_range_middle_point(char *buf, int len);
static int selected_rm_from_range_left_edge_point(char *buf, int len);
static int selected_rm_from_range_right_edge_point(char *buf, int len);
static int selected_rm_from_range_top_edge_point(char *buf, int len);
static int selected_rm_from_range_bottom_edge_point(char *buf, int len);

static Ewl_Unit_Test mvc_unit_tests[] = {
		{"Set selected", selected_set},
		{"Get selected", selected_get},
		{"List set selected", selected_list_set},
		{"List get selected", selected_list_get},
		{"Range (single select) add", selected_range_single_add},
		{"Range (multi select) add", selected_range_multi_add},
		{"Count get", selected_count_get},
		{"Is selected (index)", selected_is_index},
		{"Is not selected (index)", selected_is_not_index},
		{"Is selected (range)", selected_is_range},
		{"Is not selected (range)", selected_is_not_range},
		{"Selected clear", selected_clear},
		{"Remove index", selected_rm_idx},
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

	ewl_mvc_selected_set(m, 1, 2);
	idx = ecore_list_goto_first(m->selected);

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
selected_get(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection_Idx *idx;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selected_set(m, 6, 8);
	ewl_mvc_selected_add(m, 3, 0);
	ewl_mvc_selected_add(m, 9, 1);
	ewl_mvc_selected_add(m, 1, 2);
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
selected_list_set(char *buf, int len)
{
	return FALSE;
}

static int
selected_list_get(char *buf, int len)
{
	return FALSE;
}

static int
selected_range_single_add(char *buf, int len)
{
	Ewl_Widget *t;
	Ewl_MVC *m;
	Ewl_Selection_Idx *idx;

	t = ewl_tree2_new();
	m = EWL_MVC(t);

	ewl_mvc_selected_range_add(m, 6, 8, 1, 2);
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
	ewl_mvc_selected_range_add(m, 6, 8, 1, 2);
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
	ewl_mvc_selected_set(m, 6, 8);
	ewl_mvc_selected_add(m, 3, 0);
	ewl_mvc_selected_add(m, 9, 1);
	ewl_mvc_selected_add(m, 1, 2);
	ewl_mvc_selected_range_add(m, 6, 8, 1, 2);

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

	ewl_mvc_selected_set(m, 6, 8);

	if (!ewl_mvc_selected_is(m, 6, 8))
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

	ewl_mvc_selected_set(m, 6, 8);

	if (ewl_mvc_selected_is(m, 7, 7))
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
	ewl_mvc_selected_range_add(m, 6, 8, 1, 2);

	if (!ewl_mvc_selected_is(m, 4, 5))
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
	ewl_mvc_selected_range_add(m, 6, 8, 1, 2);

	if (ewl_mvc_selected_is(m, 1, 1))
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
	ewl_mvc_selected_set(m, 6, 8);
	ewl_mvc_selected_add(m, 3, 0);
	ewl_mvc_selected_add(m, 9, 1);
	ewl_mvc_selected_add(m, 1, 2);
	ewl_mvc_selected_range_add(m, 6, 8, 1, 2);
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
	return FALSE;
}

static int
selected_rm_from_range_top_left_point(char *buf, int len)
{
	return FALSE;
}

static int
selected_rm_from_range_bottom_right_point(char *buf, int len)
{
	return FALSE;
}

static int
selected_rm_from_range_middle_point(char *buf, int len)
{
	return FALSE;
}

static int
selected_rm_from_range_left_edge_point(char *buf, int len)
{
	return FALSE;
}

static int
selected_rm_from_range_right_edge_point(char *buf, int len)
{
	return FALSE;
}

static int
selected_rm_from_range_top_edge_point(char *buf, int len)
{
	return FALSE;
}

static int
selected_rm_from_range_bottom_edge_point(char *buf, int len)
{
	return FALSE;
}



