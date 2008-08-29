/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"

extern Ewl_Unit_Test mvc_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "MVC";
        test->tip = "The base mvc type.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_UNIT;
        test->unit_tests = mvc_unit_tests;
}

