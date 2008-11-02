/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_notebook.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test notebook_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *notebook;
        int ret = 0;

        notebook = ewl_notebook_new();

        if (!EWL_NOTEBOOK_IS(notebook))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_NOTEBOOK_TYPE);
        else if (ewl_notebook_tabbar_alignment_get(EWL_NOTEBOOK(notebook))
                        != EWL_FLAG_ALIGN_CENTER)
                LOG_FAILURE(buf, len, "tabbar is not center aligned");
        else if (ewl_notebook_tabbar_position_get(EWL_NOTEBOOK(notebook))
                        != EWL_POSITION_TOP)
                LOG_FAILURE(buf, len, "tabbar is not on the top");
        else if (!ewl_notebook_tabbar_visible_get(EWL_NOTEBOOK(notebook)))
                LOG_FAILURE(buf, len, "tabbar is not visible");
        else if (ewl_notebook_tabbar_homogeneous_get(EWL_NOTEBOOK(notebook)))
                LOG_FAILURE(buf, len, "tabbar is homogeneous");
        else
                ret = 1;

        ewl_widget_destroy(notebook);

        return ret;
}

