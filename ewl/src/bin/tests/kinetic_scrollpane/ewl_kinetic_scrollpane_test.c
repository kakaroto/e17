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

static int create_test(Ewl_Container *box);
static void destroy_test(Ewl_Widget *w, void *ev, void *data);

/* unit tests */
static int scroll_type_get(char *buf, int len);
static int kinetic_dampen_get(char *buf, int len);
static int kinetic_vmax_get(char *buf, int len);
static int kinetic_vmin_get(char *buf, int len);
static int kinetic_fps_get(char *buf, int len);

static Ewl_Unit_Test kinetic_tree_unit_tests[] = {
        {"get type of scrolling", scroll_type_get, NULL, -1, 0},
        {"get kinetic dampning", kinetic_dampen_get, NULL, -1, 0},
        {"get maximum kinetic velocity", kinetic_vmax_get, NULL, -1, 0},
        {"get minimum kinetic velocity", kinetic_vmin_get, NULL, -1, 0},
        {"get kinetic fps", kinetic_fps_get, NULL, -1, 0},
        {NULL, NULL, NULL, -1, 0}
};

void
test_info(Ewl_Test *test)
{
        test->name = "Kinetic Scrollpane";
        test->tip = "A kinetic scrollpane";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
        test->unit_tests = kinetic_tree_unit_tests;
}

static void destroy_test(Ewl_Widget *w, void *ev, void *data)
{
        ecore_list_destroy(data);
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *tree, *o, *c;
        Ewl_Model *model;
        Ewl_View *view;
        Ecore_List *data;
        int i;

        data = ecore_list_new();
        ecore_list_free_cb_set(data, ECORE_FREE_CB(free));
        ewl_callback_append(EWL_WIDGET(box), EWL_CALLBACK_DESTROY, destroy_test, data);
        for (i = 0; i < 500; i++)
        {
                char text[10];

                snprintf(text, sizeof(text), "Row: %d", i);
                ecore_list_append(data, strdup(text));
        }

        c = ewl_vpaned_new();
        ewl_container_child_append(box, c);
        ewl_widget_show(c);

        model = ewl_model_ecore_list_instance();
        view = ewl_label_view_get();

        o = ewl_border_new();
        ewl_border_label_set(EWL_BORDER(o), "Normal Scrolling");
        ewl_container_child_append(EWL_CONTAINER(c), o);
        ewl_widget_show(o);

        tree = ewl_tree_new();
        ewl_tree_headers_visible_set(EWL_TREE(tree), FALSE);
        ewl_tree_fixed_rows_set(EWL_TREE(tree), TRUE);
        ewl_tree_kinetic_scrolling_set(EWL_TREE(tree), EWL_KINETIC_SCROLL_NORMAL);
        ewl_tree_kinetic_fps_set(EWL_TREE(tree), 30);
        ewl_tree_column_count_set(EWL_TREE(tree), 1);
        ewl_mvc_model_set(EWL_MVC(tree), model);
        ewl_mvc_view_set(EWL_MVC(tree), view);
        ewl_mvc_data_set(EWL_MVC(tree), data);
        ewl_container_child_append(EWL_CONTAINER(o), tree);
        ewl_widget_show(tree);

        o = ewl_border_new();
        ewl_border_label_set(EWL_BORDER(o), "Embeded Type Scrolling");
        ewl_container_child_append(EWL_CONTAINER(c), o);
        ewl_widget_show(o);

        tree = ewl_tree_new();
        ewl_tree_headers_visible_set(EWL_TREE(tree), FALSE);
        ewl_tree_fixed_rows_set(EWL_TREE(tree), TRUE);
        ewl_tree_kinetic_scrolling_set(EWL_TREE(tree), EWL_KINETIC_SCROLL_EMBEDDED);
        ewl_tree_kinetic_fps_set(EWL_TREE(tree), 30);
        ewl_tree_kinetic_dampen_set(EWL_TREE(tree), 0.99);
        ewl_tree_column_count_set(EWL_TREE(tree), 1);
        ewl_mvc_model_set(EWL_MVC(tree), model);
        ewl_mvc_view_set(EWL_MVC(tree), view);
        ewl_mvc_data_set(EWL_MVC(tree), data);
        ewl_container_child_append(EWL_CONTAINER(o), tree);
        ewl_widget_show(tree);

        return 1;
}

static int
scroll_type_get(char *buf, int len)
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
kinetic_dampen_get(char *buf, int len)
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
kinetic_vmax_get(char *buf, int len)
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
kinetic_vmin_get(char *buf, int len)
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
kinetic_fps_get(char *buf, int len)
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

