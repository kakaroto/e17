/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_tree.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test tree_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *tree;
        int ret = 0;

        tree = ewl_tree_new();

        if (!EWL_TREE_IS(tree))
                LOG_FAILURE(buf, len, "returned widget is not of the type "
                                EWL_TREE_TYPE);
        else if (ewl_tree_column_count_get(EWL_TREE(tree)) != 1)
                LOG_FAILURE(buf, len, "tree has column count of %i",
                                ewl_tree_column_count_get(EWL_TREE(tree)));
        else if (!ewl_tree_headers_visible_get(EWL_TREE(tree)))
                LOG_FAILURE(buf, len, "headers are not visible");
        else if (ewl_tree_selection_type_get(EWL_TREE(tree))
                        != EWL_TREE_SELECTION_TYPE_CELL)
                LOG_FAILURE(buf, len, "selection type is not cell");
        else if (ewl_tree_fixed_rows_get(EWL_TREE(tree)))
                LOG_FAILURE(buf, len, "rows are fixed sized");
        else if (!ewl_tree_alternate_row_colors_get(EWL_TREE(tree)))
                LOG_FAILURE(buf, len, "rows use only one color");
        else if (ewl_object_fill_policy_get(EWL_OBJECT(tree))
                                != EWL_FLAG_FILL_ALL)
                LOG_FAILURE(buf, len, "fill policy is not ALL");
        else if (ewl_widget_focusable_get(EWL_WIDGET(tree)))
                LOG_FAILURE(buf, len, "tree is focusable");
        else
                ret = 1;

        ewl_widget_destroy(tree);

        return ret;
}

