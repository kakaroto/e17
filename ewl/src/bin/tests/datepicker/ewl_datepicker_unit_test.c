/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_datepicker.h"
#include <stdio.h>
#include <string.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test datepicker_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *c;
        int ret = 0;

        c = ewl_datepicker_new();

        if (!EWL_DATEPICKER_IS(c))
        {
                LOG_FAILURE(buf, len, "returned widget is not of the type"
                                " " EWL_DATEPICKER_TYPE);
                goto DONE;
        }
        if (!!strcmp(ewl_widget_appearance_get(c), EWL_DATEPICKER_TYPE))
        {
                LOG_FAILURE(buf, len, "datepicker has not the appearance"
                                " " EWL_DATEPICKER_TYPE);
                goto DONE;
        }
        if (ewl_object_fill_policy_get(EWL_OBJECT(c)) != EWL_FLAG_FILL_NONE)
        {
                LOG_FAILURE(buf, len, "returned datepicker has wrong fill "
                                "policy");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(c);

        return ret;
}

