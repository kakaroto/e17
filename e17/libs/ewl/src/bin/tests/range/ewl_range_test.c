/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_range.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * This set of tests is targeted at the range widget
 */


static int range_new(char *buf, int len);

static Ewl_Unit_Test range_unit_tests[] = {
                {"range new", range_new, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

void
test_info(Ewl_Test *test)
{
        test->name = "Range";
        test->tip = "The abstract range class";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_SIMPLE;
        test->unit_tests = range_unit_tests;
}

/*
 * Get a new range and test it default values
 */
static int
range_new(char *buf, int len)
{
        Ewl_Widget *range;
        int ret = 0;

        range = calloc(1, sizeof(Ewl_Range));
        /* if we don't get a valid pointer we are out of space or something
         * went wrong */
        if (!ewl_range_init(EWL_RANGE(range))) 
                LOG_FAILURE(buf, len, "Fail to init a new range widget");
        else if (ewl_range_value_get(EWL_RANGE(range)) != 0.0)
                LOG_FAILURE(buf, len, "Range value != 0.0");
        else if (ewl_range_minimum_value_get(EWL_RANGE(range)) != 0.0)
                LOG_FAILURE(buf, len, "Range minimum != 0.0");
        else if (ewl_range_maximum_value_get(EWL_RANGE(range)) != 100.0)
                LOG_FAILURE(buf, len, "Range maximum != 100.0");
        else if (ewl_range_step_get(EWL_RANGE(range)) != 10.0)
                LOG_FAILURE(buf, len, "Range step != 10.0");
        else if (ewl_range_invert_get(EWL_RANGE(range)))
                LOG_FAILURE(buf, len, "Range is inverted by default");
        else if (ewl_range_unknown_get(EWL_RANGE(range)))
                LOG_FAILURE(buf, len, "Range's range is unknown by default");
        else
                ret = 1;

        ewl_widget_destroy(range);

        return ret;
}

