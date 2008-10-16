/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_spinner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test spinner_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *s;
        int ret = 0;

        s = ewl_spinner_new();

        if (!EWL_SPINNER_IS(s))
                LOG_FAILURE(buf, len, "Returned widget is not of the type"
                                " " EWL_SPINNER_TYPE);
        else if (ewl_spinner_digits_get(EWL_SPINNER(s)) != 2)
                LOG_FAILURE(buf, len, "digits are not set to 2");
        else if (ewl_object_fill_policy_get(EWL_OBJECT(s))
                        != (EWL_FLAG_FILL_HSHRINKABLE | EWL_FLAG_FILL_HFILL))
                LOG_FAILURE(buf, len, "fill policy is not HSHRINK | HFILL");
        else
                ret = 1;

        ewl_widget_destroy(s);

        return ret;
}

