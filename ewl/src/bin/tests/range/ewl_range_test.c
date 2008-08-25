/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_range.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern Ewl_Unit_Test range_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Range";
        test->tip = "The abstract range class";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_SIMPLE;
        test->unit_tests = range_unit_tests;
}
