/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_text_fmt.h"
#include "ewl_text_context.h"

#include <stdio.h>

extern Ewl_Unit_Test text_fmt_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Text Format";
        test->tip = "Defines unit tests for the text formatting nodes.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_UNIT;
        test->unit_tests = text_fmt_unit_tests;
}
