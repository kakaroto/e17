/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"

extern Ewl_Unit_Test container_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Container";
        test->tip = "The base container type.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_UNIT;
        test->unit_tests = container_unit_tests;
}
