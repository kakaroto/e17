/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_box.h"
#include "ewl_button.h"
#include <stdio.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test box_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *b;
        int ret = 0;

        b = ewl_box_new();

        if (!EWL_BOX_IS(b))
        {
                LOG_FAILURE(buf, len, "returned box is not of the type box");
                goto DONE;
        }
        if (!ewl_box_orientation_get(EWL_BOX(b)) == EWL_ORIENTATION_HORIZONTAL)
        {
                LOG_FAILURE(buf, len, "button has a stock type set");
                goto DONE;
        }
        if (ewl_widget_focusable_get(b))
        {
                LOG_FAILURE(buf, len, "box is not focusable");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(b);

        return ret;
}

