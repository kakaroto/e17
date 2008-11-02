/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_freebox.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test freebox_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *freebox;
        int ret = 0;

        freebox = ewl_freebox_new();

        if (!EWL_FREEBOX_IS(freebox))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_FREEBOX_TYPE);
        else if (ewl_freebox_orientation_get(EWL_FREEBOX(freebox))
                        != EWL_ORIENTATION_HORIZONTAL)
                LOG_FAILURE(buf, len, "the freebox is not horizontal");
        else if (ewl_freebox_layout_type_get(EWL_FREEBOX(freebox))
                        != EWL_FREEBOX_LAYOUT_AUTO)
                LOG_FAILURE(buf, len, "layout is not auto");
        else if (ewl_freebox_comparator_get(EWL_FREEBOX(freebox)) != NULL)
                LOG_FAILURE(buf, len, "an comparator is set");
        else if (ewl_widget_focusable_get(freebox))
                LOG_FAILURE(buf, len, "the free box is focusable");
        else
                ret = 1;

        ewl_widget_destroy(freebox);

        return ret;
}

