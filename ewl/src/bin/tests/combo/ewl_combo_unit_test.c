/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_combo.h"

#include <stdio.h>
#include <string.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test combo_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *c;
        int ret = 0;

        c = ewl_combo_new();

        if (!EWL_COMBO_IS(c))
        {
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_COMBO_TYPE);
                goto DONE;
        }
        if (!!strcmp(ewl_widget_appearance_get(c), EWL_COMBO_TYPE))
        {
                LOG_FAILURE(buf, len, "combo has a wrong appearance");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(c);

        return ret;
}

