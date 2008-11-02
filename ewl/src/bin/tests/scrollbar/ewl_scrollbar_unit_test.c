/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_scrollbar.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);
static int test_hconstructor(char *buf, int len);
static int test_vconstructor(char *buf, int len);

Ewl_Unit_Test scrollbar_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {"constructor horizontal", test_hconstructor, NULL, -1, 0},
                {"constructor vertical", test_vconstructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *s;
        int ret = 0;

        s = ewl_scrollbar_new();

        if (!EWL_SCROLLBAR_IS(s))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_SCROLLBAR_TYPE);
        else if (ewl_scrollbar_orientation_get(EWL_SCROLLBAR(s))
                        != EWL_ORIENTATION_HORIZONTAL)
                LOG_FAILURE(buf, len, "scrollbar is not horizontal");
        else if (!ewl_scrollbar_inverse_scroll_get(EWL_SCROLLBAR(s))) 
                LOG_FAILURE(buf, len, "scrollbar is not inverted");
        else
                ret = 1;

        return ret;
}

static int
test_vconstructor(char *buf, int len)
{
        Ewl_Widget *s;
        int ret = 0;

        s = ewl_vscrollbar_new();

        if (!EWL_SCROLLBAR_IS(s))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_SCROLLBAR_TYPE);
        else if (ewl_scrollbar_orientation_get(EWL_SCROLLBAR(s))
                        != EWL_ORIENTATION_VERTICAL)
                LOG_FAILURE(buf, len, "scrollbar is not vertical");
        else if (!ewl_scrollbar_inverse_scroll_get(EWL_SCROLLBAR(s))) 
                LOG_FAILURE(buf, len, "scrollbar is not inverted");
        else
                ret = 1;

        return ret;
}

static int
test_hconstructor(char *buf, int len)
{
        Ewl_Widget *s;
        int ret = 0;

        s = ewl_hscrollbar_new();

        if (!EWL_SCROLLBAR_IS(s))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_SCROLLBAR_TYPE);
        else if (ewl_scrollbar_orientation_get(EWL_SCROLLBAR(s))
                        != EWL_ORIENTATION_HORIZONTAL)
                LOG_FAILURE(buf, len, "scrollbar is not horizontal");
        else if (!ewl_scrollbar_inverse_scroll_get(EWL_SCROLLBAR(s))) 
                LOG_FAILURE(buf, len, "scrollbar is not inverted");
        else
                ret = 1;

        return ret;
}

