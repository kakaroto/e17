/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_notebook.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);
static int test_visible_page(char *buf, int len);

Ewl_Unit_Test notebook_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {"visible page", test_visible_page, NULL, -1, 0},
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

static int
test_visible_page(char *buf, int len)
{
        Ewl_Widget *notebook;
        Ewl_Widget *w1, *w2, *w3;
        int ret = 0;

        notebook = ewl_notebook_new();

        /* before every thing the visible page has to be NULL */
        if (ewl_notebook_visible_page_get(EWL_NOTEBOOK(notebook)))
        {
                LOG_FAILURE(buf, len, "there is a visible page, although"
                               " there is no page");
                goto CLEANUP;
        }

        /* append the first widget */
        w1 = ewl_cell_new();
        ewl_container_child_append(EWL_CONTAINER(notebook), w1);
        
        if (ewl_notebook_visible_page_get(EWL_NOTEBOOK(notebook)) != w1)
        {
                LOG_FAILURE(buf, len, "the visible page is not the newly "
                                "added widget");
                goto CLEANUP;
        }
        
        /* append the second widget */
        w2 = ewl_cell_new();
        ewl_container_child_append(EWL_CONTAINER(notebook), w2);
        
        if (ewl_notebook_visible_page_get(EWL_NOTEBOOK(notebook)) != w1)
        {
                LOG_FAILURE(buf, len, "the visible page is not the first "
                                "added widget");
                goto CLEANUP;
        }
        
        /* prepend the third widget */
        w3 = ewl_cell_new();
        ewl_container_child_append(EWL_CONTAINER(notebook), w3);
        
        if (ewl_notebook_visible_page_get(EWL_NOTEBOOK(notebook)) != w1)
        {
                LOG_FAILURE(buf, len, "the visible page is not the first "
                                "added widget");
                goto CLEANUP;
        }

        ewl_widget_destroy(w2);
        if (ewl_notebook_visible_page_get(EWL_NOTEBOOK(notebook)) != w1)
        {
                LOG_FAILURE(buf, len, "the visible page is not the first "
                                "added widget");
                goto CLEANUP;
        }

        ewl_widget_destroy(w3);
        if (ewl_notebook_visible_page_get(EWL_NOTEBOOK(notebook)) != w1)
        {
                LOG_FAILURE(buf, len, "the visible page is not the first "
                                "added widget");
                goto CLEANUP;
        }
        
        ewl_widget_destroy(w1);
        if (ewl_notebook_visible_page_get(EWL_NOTEBOOK(notebook)))
        {
                LOG_FAILURE(buf, len, "there is a visible page, although "
                                "the notebook is empty");
                goto CLEANUP;
        }
        
        /* and now test the set function */
        w1 = ewl_cell_new();
        ewl_container_child_append(EWL_CONTAINER(notebook), w1);
        w2 = ewl_cell_new();
        ewl_container_child_prepend(EWL_CONTAINER(notebook), w2);
        w3 = ewl_cell_new();
        ewl_container_child_append(EWL_CONTAINER(notebook), w3);

        ewl_notebook_visible_page_set(EWL_NOTEBOOK(notebook), w2);
        if (ewl_notebook_visible_page_get(EWL_NOTEBOOK(notebook)) != w2)
        {
                LOG_FAILURE(buf, len, "the visible page is not the second "
                                "added widget");
                goto CLEANUP;
        }
        
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(notebook), w3);
        if (ewl_notebook_visible_page_get(EWL_NOTEBOOK(notebook)) != w3)
        {
                LOG_FAILURE(buf, len, "the visible page is not the third "
                                "added widget");
                goto CLEANUP;
        }

        ret = 1;

CLEANUP:
        ewl_widget_destroy(notebook);

        return ret;
}
