/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_progressbar.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test progressbar_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *p;
        int ret = 0;

        p = ewl_progressbar_new();

        if (!EWL_PROGRESSBAR_IS(p))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_PROGRESSBAR_TYPE);
        else if (ewl_object_fill_policy_get(EWL_OBJECT(p)) 
                        != (EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINKABLE))
                LOG_FAILURE(buf, len, "fill policy is HFILL | VSHRINKABLE");
        else
                ret = 1;

        return ret;
}

