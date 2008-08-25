/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_tree.h"
#include "ewl_label.h"
#include "ewl_border.h"
#include "ewl_paned.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_type_get(char *buf, int len);
static int test_dampen_get(char *buf, int len);
static int test_vmax_get(char *buf, int len);
static int test_vmin_get(char *buf, int len);
static int test_fps_get(char *buf, int len);

Ewl_Unit_Test kinetic_tree_unit_tests[] = {
        {"get type of scrolling", test_type_get, NULL, -1, 0},
        {"get kinetic dampning", test_dampen_get, NULL, -1, 0},
        {"get maximum kinetic velocity", test_vmax_get, NULL, -1, 0},
        {"get minimum kinetic velocity", test_vmin_get, NULL, -1, 0},
        {"get kinetic fps", test_fps_get, NULL, -1, 0},
        {NULL, NULL, NULL, -1, 0}
};

static int
test_type_get(char *buf, int len)
{
        Ewl_Widget *tree;
        Ewl_Kinetic_Scroll type;
        int ret = 1;

        tree = ewl_tree_new();
        ewl_tree_kinetic_scrolling_set(EWL_TREE(tree),
                                                EWL_KINETIC_SCROLL_NORMAL);
        type = ewl_tree_kinetic_scrolling_get(EWL_TREE(tree));

        if (type != EWL_KINETIC_SCROLL_NORMAL)
        {
                LOG_FAILURE(buf, len, "get type is different from the set one");
                ret = 0;
        }

        ewl_widget_destroy(tree);

        return ret;
}

static int
test_dampen_get(char *buf, int len)
{
        Ewl_Widget *tree;
        double val, set;
        int ret = 1;

        set = 0.06;
        tree = ewl_tree_new();
        ewl_tree_kinetic_dampen_set(EWL_TREE(tree), set);
        val = ewl_tree_kinetic_dampen_get(EWL_TREE(tree));

        if (val != set)
        {
                LOG_FAILURE(buf, len, "get dampen is different from the set dampen");
                ret = 0;
        }
        
        ewl_widget_destroy(tree);

        return ret;
}

static int
test_vmax_get(char *buf, int len)
{
        Ewl_Widget *tree;
        double val, set;
        int ret = 1;

        set = 27;
        tree = ewl_tree_new();
        ewl_tree_kinetic_max_velocity_set(EWL_TREE(tree), set);
        val = ewl_tree_kinetic_max_velocity_get(EWL_TREE(tree));

        if (val != set)
        {
                LOG_FAILURE(buf, len, "get vmax is different from the set vmax");
                ret = 0;
        }

        ewl_widget_destroy(tree);

        return ret;
}

static int
test_vmin_get(char *buf, int len)
{
        Ewl_Widget *tree;
        double val, set;
        int ret = 1;

        set = 13;
        tree = ewl_tree_new();
        ewl_tree_kinetic_min_velocity_set(EWL_TREE(tree), set);
        val = ewl_tree_kinetic_min_velocity_get(EWL_TREE(tree));

        if (val != set)
        {
                LOG_FAILURE(buf, len, "get vmin is different from the set vmin");
                ret = 0;
        }

        ewl_widget_destroy(tree);

        return ret;
}

static int
test_fps_get(char *buf, int len)
{
        Ewl_Widget *tree;
        int val;
        int ret = 1;

        tree = ewl_tree_new();
        ewl_tree_kinetic_fps_set(EWL_TREE(tree), 3);
        val = ewl_tree_kinetic_fps_get(EWL_TREE(tree));

        if (val != 3)
        {
                LOG_FAILURE(buf, len, "get fps is different from the set fps");
                ret = 0;
        }

        ewl_widget_destroy(tree);

        return ret;
}

