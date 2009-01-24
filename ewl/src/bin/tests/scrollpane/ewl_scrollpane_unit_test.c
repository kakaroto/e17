/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_scrollpane.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test scrollpane_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *sp;
        int ret = 0;

        sp = ewl_scrollpane_new();

        if (!EWL_SCROLLPANE_IS(sp))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_SCROLLPANE_TYPE);
        else if (ewl_scrollport_hscrollbar_flag_get(EWL_SCROLLPORT(sp))
                        != EWL_SCROLLPORT_FLAG_AUTO_VISIBLE)
                LOG_FAILURE(buf, len, "horizontal scrollbar doesn't auto hide");
        else if (ewl_scrollport_vscrollbar_flag_get(EWL_SCROLLPORT(sp))
                        != EWL_SCROLLPORT_FLAG_AUTO_VISIBLE)
                LOG_FAILURE(buf, len, "vertical scrollbar doesn't auto hide");
        else if (ewl_object_fill_policy_get(EWL_OBJECT(sp))
                        != (EWL_FLAG_FILL_FILL | EWL_FLAG_FILL_SHRINK))
                LOG_FAILURE(buf, len, "fill policy is not FILL | SHRINK");
        else
                ret = 1;

        ewl_widget_destroy(sp);

        return ret;
}

