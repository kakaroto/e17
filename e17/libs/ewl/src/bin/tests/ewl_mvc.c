#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int selected_set(char *buf, int len);
static int selected_get(char *buf, int len);
static int selected_list_set(char *buf, int len);
static int selected_list_get(char *buf, int len);
static int selected_range_add(char *buf, int len);
static int selected_count_get(char *buf, int len);
static int selected_is(char *buf, int len);
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
		{"Range add", selected_range_add},
		{"Count get", selected_count_get},
		{"Is selected", selected_is},
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
	return FALSE;
}

static int
selected_get(char *buf, int len)
{
	return FALSE;
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
selected_range_add(char *buf, int len)
{
	return FALSE;
}

static int
selected_count_get(char *buf, int len)
{
	return FALSE;
}

static int
selected_is(char *buf, int len)
{
	return FALSE;
}

static int
selected_clear(char *buf, int len)
{
	return FALSE;
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



