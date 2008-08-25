/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"

extern Ewl_Unit_Test object_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Object";
        test->tip = "The base object type.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_MISC;
        test->unit_tests = object_unit_tests;
}
