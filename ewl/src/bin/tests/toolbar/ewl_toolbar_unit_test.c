/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_toolbar.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test toolbar_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *toolbar;
        int ret = 0;

        toolbar = ewl_toolbar_new();

        if (!EWL_TOOLBAR_IS(toolbar))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_TOOLBAR_TYPE);
        else if (ewl_toolbar_icon_part_hidden_get(EWL_TOOLBAR(toolbar)) !=
                        EWL_ICON_PART_NONE)
                LOG_FAILURE(buf, len, "toolbar has hidden icon part");
        else if (ewl_toolbar_orientation_get(EWL_TOOLBAR(toolbar))
                        != EWL_ORIENTATION_HORIZONTAL)
                LOG_FAILURE(buf, len, "the toolbar is not horizontal");
        else
                ret = 1;

        ewl_widget_destroy(toolbar);

        return ret;
}

