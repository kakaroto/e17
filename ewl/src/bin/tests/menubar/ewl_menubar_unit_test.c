/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_menubar.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test menubar_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *menubar;
        int ret = 0;

        menubar = ewl_menubar_new();

        if (!EWL_MENUBAR_IS(menubar))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_MENUBAR_TYPE);
        else if (ewl_menubar_orientation_get(EWL_MENUBAR(menubar))
                        != EWL_ORIENTATION_HORIZONTAL)
                LOG_FAILURE(buf, len, "the menubar is not horizontal");
        else if (ewl_object_fill_policy_get(EWL_OBJECT(menubar))
                        != EWL_FLAG_FILL_HFILL)
                LOG_FAILURE(buf, len, "the fill policy is not HFILL");
        else
                ret = 1;

        ewl_widget_destroy(menubar);

        return ret;
}

