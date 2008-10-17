/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_menu.h"

#include <stdio.h>
#include <string.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test menu_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *menu;
        int ret = 0;

        menu = ewl_menu_new();

        if (!EWL_MENU_IS(menu))
                LOG_FAILURE(buf, len, "widget is not of the type "
                                EWL_MENU_TYPE);
        else
                ret = 1;

        ewl_widget_destroy(menu);

        return ret;
}

