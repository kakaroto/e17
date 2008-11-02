/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_paned.h"

#include <stdio.h>

static int test_constructor(char *buf, int len);
static int test_initial_size_get(char *buf, int len);
static int test_initial_size_unset_get(char *buf, int len);
static int test_initial_size_after_remove_get(char *buf, int len);
static int test_initial_size_many_get(char *buf, int len);
static int test_fixed_size_get(char *buf, int len);
static int test_fixed_size_unset_get(char *buf, int len);

Ewl_Unit_Test paned_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {"get initial size", test_initial_size_get, NULL, -1, 0},
                {"get unset initial size", test_initial_size_unset_get, NULL, -1, 0},
                {"get initial size after remove", test_initial_size_after_remove_get, NULL, -1, 0},
                {"get initial size for many widgets", test_initial_size_many_get, NULL, -1, 0},
                {"get the fixed size flag", test_fixed_size_get, NULL, -1, 0},
                {"get the unset fixed size flag", test_fixed_size_unset_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *paned;
        int ret = 0;

        paned = ewl_paned_new();

        if (!EWL_PANED_IS(paned))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_PANED_TYPE);
        else if (ewl_paned_orientation_get(EWL_PANED(paned))
                        != EWL_ORIENTATION_HORIZONTAL)
                LOG_FAILURE(buf, len, "paned is not horizontal");
        else if (ewl_object_fill_policy_get(EWL_OBJECT(paned))
                        != EWL_FLAG_FILL_FILL)
                LOG_FAILURE(buf, len, "paned's fill policy is not FILL");
        else if (ewl_object_alignment_get(EWL_OBJECT(paned))
                        != (EWL_FLAG_ALIGN_TOP | EWL_FLAG_ALIGN_LEFT))
                LOG_FAILURE(buf, len, "alignment is not LEFT | TOP");
        else
                ret = 1;

        ewl_widget_destroy(paned);

        return ret;
}

static int 
test_initial_size_get(char *buf, int len)
{
        Ewl_Widget *paned;
        Ewl_Widget *child;
        int ret = 1;
        int val;

        paned = ewl_paned_new();
        child = ewl_cell_new();

        ewl_container_child_append(EWL_CONTAINER(paned), child);
        ewl_paned_initial_size_set(EWL_PANED(paned), child, 240);
        val = ewl_paned_initial_size_get(EWL_PANED(paned), child);

        if (val != 240) {
                LOG_FAILURE(buf, len, "get value is different from the set one");
                ret = 0;
        }

        ewl_widget_destroy(paned);

        return ret;
}

static int 
test_initial_size_unset_get(char *buf, int len)
{
        Ewl_Widget *paned;
        Ewl_Widget *child;
        int ret = 1;
        int val;

        paned = ewl_paned_new();
        child = ewl_cell_new();

        ewl_container_child_append(EWL_CONTAINER(paned), child);
        val = ewl_paned_initial_size_get(EWL_PANED(paned), child);

        if (val != 0) {
                LOG_FAILURE(buf, len, "get value is not 0");
                ret = 0;
        }

        ewl_widget_destroy(paned);

        return ret;
}

static int 
test_initial_size_after_remove_get(char *buf, int len)
{
        Ewl_Widget *paned;
        Ewl_Widget *child;
        int ret = 1;
        int val;

        /* use a hpaned here to cover this up */
        paned = ewl_hpaned_new();
        child = ewl_cell_new();

        ewl_container_child_append(EWL_CONTAINER(paned), child);
        ewl_paned_initial_size_set(EWL_PANED(paned), child, 240);
        ewl_container_child_remove(EWL_CONTAINER(paned), child);
        val = ewl_paned_initial_size_get(EWL_PANED(paned), child);

        if (val != 0) {
                LOG_FAILURE(buf, len, "get value is not zero");
                ret = 0;
        }

        ewl_widget_destroy(child);
        ewl_widget_destroy(paned);

        return ret;
}

static int 
test_initial_size_many_get(char *buf, int len)
{
        Ewl_Widget *paned;
        Ewl_Widget *w[4];
        int ret = 1, i;

        /* use a vpaned here to cover this up */
        paned = ewl_vpaned_new();

        /* build the children and set the initial size for them */
        for (i = 0; i < 4; i++) {
                w[i] = ewl_cell_new();
                ewl_container_child_append(EWL_CONTAINER(paned), w[i]);
                ewl_paned_initial_size_set(EWL_PANED(paned), w[i], 240 + i);
        }

        /* now check the set values */
        for (i = 0; i < 4; i++) {
                int val = ewl_paned_initial_size_get(EWL_PANED(paned), w[i]);

                if (val != 240 + i) {
                        LOG_FAILURE(buf, len, "get value is not zero");
                        ret = 0;
                        break;
                }
        }

        ewl_widget_destroy(paned);

        return ret;
}

static int 
test_fixed_size_get(char *buf, int len)
{
        Ewl_Widget *paned;
        Ewl_Widget *child;
        int ret = 1;
        unsigned int val;

        paned = ewl_paned_new();
        child = ewl_cell_new();

        ewl_container_child_append(EWL_CONTAINER(paned), child);
        ewl_paned_fixed_size_set(EWL_PANED(paned), child, TRUE);
        val = ewl_paned_fixed_size_get(EWL_PANED(paned), child);

        if (!val) {
                LOG_FAILURE(buf, len, "get value is different from the set one");
                ret = 0;
        }

        ewl_widget_destroy(paned);

        return ret;
}

static int 
test_fixed_size_unset_get(char *buf, int len)
{
        Ewl_Widget *paned;
        Ewl_Widget *child;
        int ret = 1;
        unsigned int val;

        paned = ewl_paned_new();
        child = ewl_cell_new();

        ewl_container_child_append(EWL_CONTAINER(paned), child);
        val = ewl_paned_fixed_size_get(EWL_PANED(paned), child);

        if (val) {
                LOG_FAILURE(buf, len, "get value is not FALSE");
                ret = 0;
        }

        ewl_widget_destroy(paned);

        return ret;
}

