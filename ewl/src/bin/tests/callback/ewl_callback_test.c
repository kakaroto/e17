/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"

extern Ewl_Unit_Test callback_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Callback";
        test->tip = "The base callback manipulation.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_MISC;
        test->unit_tests = callback_unit_tests;
}
