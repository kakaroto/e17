/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_freebox_mvc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test freebox_mvc_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *fmvc;
        int ret = 0;

        fmvc = ewl_freebox_mvc_new();

        if (!EWL_FREEBOX_MVC_IS(fmvc))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_FREEBOX_MVC_TYPE);
        else if (ewl_freebox_mvc_orientation_get(EWL_FREEBOX_MVC(fmvc))
                        != EWL_ORIENTATION_HORIZONTAL)
                LOG_FAILURE(buf, len, "the freebox mvc is not horizontal");
        else
                ret = 1;

        ewl_widget_destroy(fmvc);

        return ret;
}

