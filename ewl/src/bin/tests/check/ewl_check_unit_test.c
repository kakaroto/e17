/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_check.h"
#include "ewl_separator.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test check_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *check;
        int ret = 0;

        check = ewl_check_new();

        if (ewl_check_is_checked(EWL_CHECK(check)))
                LOG_FAILURE(buf, len, "default check state TRUE");
        else {
                ewl_check_checked_set(EWL_CHECK(check), TRUE);
                if (ewl_check_is_checked(EWL_CHECK(check))) {
                        ewl_check_checked_set(EWL_CHECK(check), FALSE);
                        if (!ewl_check_is_checked(EWL_CHECK(check)))
                                ret = 1;
                        else
                                LOG_FAILURE(buf, len, "check state TRUE");
                }
                else
                        LOG_FAILURE(buf, len, "check state FALSE");
        }

        ewl_widget_destroy(check);

        return ret;
}
