/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_check.h"
#include "ewl_separator.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

extern Ewl_Unit_Test check_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Check";
        test->tip = "The check class is a basic check\n"
                "for an undecorated checkbutton.";
        test->filename = __FILE__;
        test->type = EWL_TEST_TYPE_UNIT;
        test->unit_tests = check_unit_tests;
}
