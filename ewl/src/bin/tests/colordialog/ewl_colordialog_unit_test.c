/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_colordialog.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test colordialog_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *c;
        int ret = 0;

        c = ewl_colordialog_new();

        if (!EWL_COLORDIALOG_IS(c))
        {
                LOG_FAILURE(buf, len, "returned color dialog is not of the type"
                                " " EWL_COLORDIALOG_TYPE);
                goto DONE;
        }
        if (!!strcmp(ewl_window_title_get(EWL_WINDOW(c)), "Ewl Colordialog"))
        {
                LOG_FAILURE(buf, len, "window title is not Ewl Colordialog");
                goto DONE;
        }
        if (!!strcmp(ewl_window_name_get(EWL_WINDOW(c)), "Ewl Colordialog"))
        {
                LOG_FAILURE(buf, len, "window name is not Ewl Colordialog");
                goto DONE;
        }
        if (!!strcmp(ewl_window_title_get(EWL_WINDOW(c)), "Ewl Colordialog"))
        {
                LOG_FAILURE(buf, len, "window group is not Ewl Colordialog");
                goto DONE;
        }
        if (!ewl_colordialog_has_alpha_get(EWL_COLORDIALOG(c)))
        {
                LOG_FAILURE(buf, len, "colordialog has no alpha channel");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(c);

        return ret;
}

