/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"

extern  Ewl_Unit_Test config_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Config";
        test->tip = "The config system.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_MISC;
        test->unit_tests = config_unit_tests;
}
