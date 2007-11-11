/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include <limits.h>
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * While this set of tests is targeted at objects, it requires setting up full
 * widgets since objects assume they are widgets.
 */

#define MATCH_SIZE 32
#define DIFFER_WIDTH 30
#define DIFFER_HEIGHT 20

static int preferred_inner_size_test_set_get(char *buf, int len);
static int preferred_size_test_set_get(char *buf, int len);
static int minimum_size_test_set_get(char *buf, int len);
static int maximum_size_test_set_get(char *buf, int len);
static int minimum_size_test_set_request(char *buf, int len);
static int maximum_size_test_set_request(char *buf, int len);
static int fill_policy_test_set_get(char *buf, int len);
static int alignment_test_set_get(char *buf, int len);

static Ewl_Unit_Test object_unit_tests[] = {
		{"preferred inner size set/get", preferred_inner_size_test_set_get, NULL, -1, 0},
		{"preferred size set/get", preferred_size_test_set_get, NULL, -1, 0},
		{"minimum size set/get", minimum_size_test_set_get, NULL, -1, 0},
		{"maximum size set/get", maximum_size_test_set_get, NULL, -1, 0},
		{"minimum size set/request", minimum_size_test_set_request, NULL, -1, 0},
		{"maximum size set/request", maximum_size_test_set_request, NULL, -1, 0},
		{"fill policy set/get", fill_policy_test_set_get, NULL, -1, 0},
		{"alignment set/get", alignment_test_set_get, NULL, -1, 0},
		{NULL, NULL, NULL, -1, 0}
	};

void
test_info(Ewl_Test *test)
{
	test->name = "Object";
	test->tip = "The base object type.";
	test->filename = __FILE__;
	test->type = EWL_TEST_TYPE_MISC;
	test->unit_tests = object_unit_tests;
}

/*
 * Set the preferred inner size and verify that it gets the same size back under
 * various combinations of sizes.
 */
static int
preferred_inner_size_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int width = 0, height = 0;
	int ret = 0;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	ewl_object_preferred_inner_size_get(EWL_OBJECT(w), &width, &height);
	if (width == 0 && height == 0) {
		ewl_object_preferred_inner_size_set(EWL_OBJECT(w), MATCH_SIZE,
							MATCH_SIZE);
		ewl_object_preferred_inner_size_get(EWL_OBJECT(w), &width,
							&height);
		if (width == MATCH_SIZE && height == MATCH_SIZE) {
			ewl_object_preferred_inner_size_set(EWL_OBJECT(w),
								DIFFER_WIDTH,
								DIFFER_HEIGHT);
			ewl_object_preferred_inner_size_get(EWL_OBJECT(w),
								&width,
								&height);
			if (width == DIFFER_WIDTH && height == DIFFER_HEIGHT)
				ret = 1;
			else
				snprintf(buf, len, "preferred inner sizes match");
		}
		else
			snprintf(buf, len, "preferred inner sizes differ");
	}
	else
		snprintf(buf, len, "default preferred inner size %dx%d",
			width, height);

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Set the preferred size under a variety of cases and verify that it gets the
 * same sizes back.
 */
static int
preferred_size_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int width = 0, height = 0;
	int ret = 0;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	ewl_object_preferred_size_get(EWL_OBJECT(w), &width, &height);
	if (width == EWL_OBJECT_MIN_SIZE && height == EWL_OBJECT_MIN_SIZE) {
		ewl_object_preferred_inner_size_set(EWL_OBJECT(w), MATCH_SIZE,
							MATCH_SIZE);
		ewl_object_preferred_inner_size_get(EWL_OBJECT(w), &width,
							&height);
		if (width == MATCH_SIZE && height == MATCH_SIZE) {
			ewl_object_preferred_inner_size_set(EWL_OBJECT(w),
					DIFFER_WIDTH,
					DIFFER_HEIGHT);
			ewl_object_preferred_inner_size_get(EWL_OBJECT(w),
								&width,
								&height);
			if (width == DIFFER_WIDTH && height == DIFFER_HEIGHT)
				ret = 1;
			else
				snprintf(buf, len, "preferred sizes match");
		}
		else
			snprintf(buf, len, "preferred sizes differ");
	}
	else
		snprintf(buf, len, "default preferred size %dx%d",
			width, height);

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Set the minimum size and verify that it returns the same minimum size when
 * get is called.
 */
static int
minimum_size_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int width = 0, height = 0;
	int ret = 0;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	ewl_object_minimum_size_get(EWL_OBJECT(w), &width, &height);
	if (width == EWL_OBJECT_MIN_SIZE && height == EWL_OBJECT_MIN_SIZE) {
		ewl_object_minimum_size_set(EWL_OBJECT(w), MATCH_SIZE,
							MATCH_SIZE);
		ewl_object_minimum_size_get(EWL_OBJECT(w), &width, &height);
		if (width == MATCH_SIZE && height == MATCH_SIZE) {
			ewl_object_minimum_size_set(EWL_OBJECT(w),
					DIFFER_WIDTH,
					DIFFER_HEIGHT);
			ewl_object_minimum_size_get(EWL_OBJECT(w), &width,
					&height);
			if (width == DIFFER_WIDTH && height == DIFFER_HEIGHT)
				ret = 1;
			else
				snprintf(buf, len, "minimum sizes match");
		}
		else
			snprintf(buf, len, "minimum sizes differ");
	}
	else
		snprintf(buf, len, "default minimum size %dx%d",
			width, height);

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Set the maximum size with a variety of aspects and test that get returns the
 * same sizes set.
 */
static int
maximum_size_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int width = 0, height = 0;
	int ret = 0;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	ewl_object_maximum_size_get(EWL_OBJECT(w), &width, &height);
	if (width == EWL_OBJECT_MAX_SIZE && height == EWL_OBJECT_MAX_SIZE) {
		ewl_object_maximum_size_set(EWL_OBJECT(w), MATCH_SIZE,
							MATCH_SIZE);
		ewl_object_maximum_size_get(EWL_OBJECT(w), &width, &height);
		if (width == MATCH_SIZE && height == MATCH_SIZE) {
			ewl_object_maximum_size_set(EWL_OBJECT(w),
					DIFFER_WIDTH,
					DIFFER_HEIGHT);
			ewl_object_maximum_size_get(EWL_OBJECT(w), &width,
					&height);
			if (width == DIFFER_WIDTH && height == DIFFER_HEIGHT)
				ret = 1;
			else
				snprintf(buf, len, "maximum sizes match");
		}
		else
			snprintf(buf, len, "maximum sizes differ");
	}
	else
		snprintf(buf, len, "default maximum size %dx%d",
			width, height);

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Set the minimum size to different aspects and verify that requesting the size
 * for the object respects the minimum sizes properly.
 */
static int
minimum_size_test_set_request(char *buf, int len)
{
	Ewl_Widget *w;
	int width = 0, height = 0;
	int ret = 0;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	/*
	 * Test the base case of the requested size equal to the minimum.
	 */
	ewl_object_minimum_size_set(EWL_OBJECT(w), DIFFER_WIDTH, DIFFER_HEIGHT);
	ewl_object_size_request(EWL_OBJECT(w), DIFFER_WIDTH, DIFFER_HEIGHT);
	ewl_object_current_size_get(EWL_OBJECT(w), &width, &height);

	if (width == DIFFER_WIDTH && height == DIFFER_HEIGHT) {

		/*
		 * Verify a valid size not equal to the boundary.
		 */
		ewl_object_size_request(EWL_OBJECT(w), DIFFER_WIDTH + 1,
				DIFFER_HEIGHT + 1);
		ewl_object_current_size_get(EWL_OBJECT(w), &width, &height);

		if (width == DIFFER_WIDTH + 1 && height == DIFFER_HEIGHT + 1) {

			/*
			 * Verify an invalid size is forced to the boundary.
			 */
			ewl_object_size_request(EWL_OBJECT(w),
					DIFFER_WIDTH - 1,
					DIFFER_HEIGHT - 1);
			ewl_object_current_size_get(EWL_OBJECT(w), &width,
					&height);
			if (width == DIFFER_WIDTH && height == DIFFER_HEIGHT)
				ret = 1;
			else
				snprintf(buf, len, "min size ignored %dx%d",
						width, height);
		}
		else
			snprintf(buf, len, "differing sizes wrong %dx%d",
					width, height);
	}
	else
		snprintf(buf, len, "same sizes wrong %dx%d", width, height);

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Test that size requests respect the maximum size settings under a variety of
 * cases.
 */
static int
maximum_size_test_set_request(char *buf, int len)
{
	Ewl_Widget *w;
	int width = 0, height = 0;
	int ret = 0;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	/*
	 * Test the base case of the requested size equal to the maximum.
	 */
	ewl_object_maximum_size_set(EWL_OBJECT(w), DIFFER_WIDTH, DIFFER_HEIGHT);
	ewl_object_size_request(EWL_OBJECT(w), DIFFER_WIDTH, DIFFER_HEIGHT);
	ewl_object_current_size_get(EWL_OBJECT(w), &width, &height);

	if (width == DIFFER_WIDTH && height == DIFFER_HEIGHT) {

		/*
		 * Verify a valid size not equal to the boundary.
		 */
		ewl_object_size_request(EWL_OBJECT(w), DIFFER_WIDTH - 1,
				DIFFER_HEIGHT - 1);
		ewl_object_current_size_get(EWL_OBJECT(w), &width, &height);

		if (width == DIFFER_WIDTH - 1 && height == DIFFER_HEIGHT - 1) {

			/*
			 * Verify an invalid size is forced to the boundary.
			 */
			ewl_object_size_request(EWL_OBJECT(w),
					DIFFER_WIDTH + 1,
					DIFFER_HEIGHT + 1);
			ewl_object_current_size_get(EWL_OBJECT(w), &width,
					&height);
			if (width == DIFFER_WIDTH && height == DIFFER_HEIGHT)
				ret = 1;
			else
				snprintf(buf, len, "max size ignored %dx%d",
						width, height);
		}
		else
			snprintf(buf, len, "differing sizes wrong %dx%d",
					width, height);
	}
	else
		snprintf(buf, len, "same sizes wrong %dx%d", width, height);

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Verify that setting a fill policy on an object returns the proper value when
 * get is called.
 */
static int
fill_policy_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	unsigned int fill;
	int ret = 0;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	/*
	 * Since fill all should simply be a mask of all other values, this
	 * tests each bit being set.
	 */
	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_ALL);
	fill = ewl_object_fill_policy_get(EWL_OBJECT(w));

	if ((fill & EWL_FLAG_FILL_HSHRINK) && (fill & EWL_FLAG_FILL_VSHRINK) &&
	    (fill & EWL_FLAG_FILL_HFILL) && (fill & EWL_FLAG_FILL_VFILL)) {
		ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
		fill = ewl_object_fill_policy_get(EWL_OBJECT(w));
		if (!fill)
			ret = 1;
		else
			snprintf(buf, len, "fill none incorrect");
	}
	else
		snprintf(buf, len, "fill all missing flags");

	ewl_widget_destroy(w);

	return ret;
}

/*
 * Set the alignment of an object to a variety of options and check that get
 * returns the correctly set alignment.
 */
static int
alignment_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	unsigned int align;
	int ret = 0;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_LEFT |
			EWL_FLAG_ALIGN_RIGHT | EWL_FLAG_ALIGN_TOP |
			EWL_FLAG_ALIGN_BOTTOM);
	align = ewl_object_alignment_get(EWL_OBJECT(w));

	if ((align & EWL_FLAG_ALIGN_LEFT) && (align & EWL_FLAG_ALIGN_RIGHT) &&
	    (align & EWL_FLAG_ALIGN_TOP) && (align & EWL_FLAG_ALIGN_BOTTOM)) {
		ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_CENTER);
		align = ewl_object_alignment_get(EWL_OBJECT(w));
		if (!align)
			ret = 1;
		else
			snprintf(buf, len, "align none incorrect");
	}
	else
		snprintf(buf, len, "alignment missing flags");

	ewl_widget_destroy(w);

	return ret;
}
