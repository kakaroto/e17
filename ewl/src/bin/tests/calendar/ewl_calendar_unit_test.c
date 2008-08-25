/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_calendar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test calendar_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *c;
        int ret = 0;

        c = ewl_calendar_new();

        if (!EWL_CALENDAR_IS(c))
        {
                LOG_FAILURE(buf, len, "returned calendar is not of the type"
                                " calendar");
                goto DONE;
        }
        if (!!strcmp(ewl_widget_appearance_get(c), EWL_CALENDAR_TYPE))
        {
                LOG_FAILURE(buf, len, "calendar has wrong appearance");
                goto DONE;
        }
        if (!ewl_object_fill_policy_get(EWL_OBJECT(c)) == EWL_FLAG_FILL_FILL)
        {
                LOG_FAILURE(buf, len, "calendar's fill policy is not set to fill");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(c);

        return ret;
}

