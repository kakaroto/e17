/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_shadow.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test shadow_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *s;
        int ret = 0;

        s = ewl_shadow_new();

        if (!EWL_SHADOW_IS(s))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_SHADOW_TYPE);
        else if (ewl_object_fill_policy_get(EWL_OBJECT(s))
                        != EWL_FLAG_FILL_SHRINKABLE)
                LOG_FAILURE(buf, len, "fill policy is not SHRINKABLE");
        else
                ret = 1;

        ewl_widget_destroy(s);

        return ret;
}

