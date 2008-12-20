/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_checkbutton.h"
#include "ewl_image.h"
#include "ewl_label.h"
#include "ewl_spinner.h"
#include "ewl_tree.h"
#include "ewl_tree_view_plain.h"
#include "ewl_tree_view_scrolled.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TREE_DATA_ELEMENTS 5

typedef struct Tree_Test_Data Tree_Test_Data;
typedef struct Tree_Test_Row_Data Tree_Test_Row_Data;
struct Tree_Test_Row_Data
{
        char *image;
        char *text;
        Tree_Test_Data *subdata;

        int expandable;
};

struct Tree_Test_Data
{
        unsigned int count;
        unsigned int row_count;
        Tree_Test_Row_Data **rows;
};

static int create_test(Ewl_Container *win);
static void *tree_test_data_setup(void);
static Ewl_Widget *tree_test_cb_widget_fetch(void *data, unsigned int row,
                                                unsigned int column,
                                                void *pr_data);
static void *tree_test_cb_header_data_fetch(void *data, unsigned int column);
static Ewl_Widget *tree_test_cb_header_fetch(void *data, unsigned int column,
                                                void *pr_data);
static void *tree_test_data_fetch(void *data, unsigned int row,
                                                unsigned int column);
static int tree_test_column_sortable(void *data, unsigned int column);
static void tree_test_data_sort(void *data, unsigned int column,
                                                Ewl_Sort_Direction sort);
static unsigned int tree_test_data_count_get(void *data);
static int tree_test_data_expandable_get(void *data, unsigned int row);
static void *tree_test_data_expansion_fetch(void *data, unsigned int row);

static void ewl_tree_cb_scroll_headers(Ewl_Widget *w, void *ev, void *data);
static void ewl_tree_cb_hide_headers(Ewl_Widget *w, void *ev, void *data);
static void ewl_tree_cb_plain_view(Ewl_Widget *w, void *ev, void *data);
static void ewl_tree_cb_set_rows_clicked(Ewl_Widget *w, void *ev, void *data);
static void tree_cb_value_changed(Ewl_Widget *w, void *ev, void *data);
static void tree_cb_select_mode_change(Ewl_Widget *w, void *ev, void *data);
static void tree_cb_ensure_visible(Ewl_Widget *w, void *ev, void *data);

extern Ewl_Unit_Test tree_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Tree";
        test->tip = "Defines a widget for laying out other\n"
                        "widgets in a tree or list like manner.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
        test->unit_tests = tree_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *tree, *o, *o2, *o3;
        Ewl_Model *model;
        Ewl_View *view;
        void *data;

        o2 = ewl_hbox_new();
        ewl_container_child_append(box, o2);
        ewl_object_fill_policy_set(EWL_OBJECT(o2), EWL_FLAG_FILL_HFILL);
        ewl_widget_show(o2);

        /* create our data */
        data = tree_test_data_setup();

        /* the tree will only use one model. We could use a model per
         * column, but a single model will work fine for this test */
        model = ewl_model_new();
        ewl_model_data_fetch_set(model, tree_test_data_fetch);
        ewl_model_data_header_fetch_set(model,
                                tree_test_cb_header_data_fetch);
        ewl_model_data_sort_set(model, tree_test_data_sort);
        ewl_model_column_sortable_set(model, tree_test_column_sortable);
        ewl_model_data_count_set(model, tree_test_data_count_get);
        ewl_model_data_expandable_set(model, tree_test_data_expandable_get);
        ewl_model_expansion_data_fetch_set(model,
                                tree_test_data_expansion_fetch);

        view = ewl_view_new();
        ewl_view_widget_fetch_set(view, tree_test_cb_widget_fetch);
        ewl_view_header_fetch_set(view, tree_test_cb_header_fetch);

        tree = ewl_tree_new();
        ewl_container_child_append(EWL_CONTAINER(box), tree);
        ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_ALL);
        ewl_callback_append(tree, EWL_CALLBACK_VALUE_CHANGED,
                                        tree_cb_value_changed, NULL);
        ewl_mvc_data_set(EWL_MVC(tree), data);
        ewl_mvc_model_set(EWL_MVC(tree), model);
        ewl_mvc_view_set(EWL_MVC(tree), view);
        ewl_mvc_selection_mode_set(EWL_MVC(tree), EWL_SELECTION_MODE_MULTI);
        ewl_tree_column_count_set(EWL_TREE(tree), 3);
        ewl_tree_row_expand(EWL_TREE(tree), data, 2);
        ewl_widget_name_set(tree, "tree");
        ewl_widget_show(tree);

        o3 = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(o2), o3);
        ewl_widget_show(o3);

        /* create the checkbuttons for the top box */
        o = ewl_checkbutton_new();
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
        ewl_button_label_set(EWL_BUTTON(o), "Scroll headers");
        ewl_container_child_append(EWL_CONTAINER(o3), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                ewl_tree_cb_scroll_headers, tree);
        ewl_widget_show(o);

        o = ewl_checkbutton_new();
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
        ewl_button_label_set(EWL_BUTTON(o), "Hide headers");
        ewl_container_child_append(EWL_CONTAINER(o3), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                ewl_tree_cb_hide_headers, tree);
        ewl_widget_show(o);

        o = ewl_checkbutton_new();
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_button_label_set(EWL_BUTTON(o), "Plain view");
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                ewl_tree_cb_plain_view, tree);
        ewl_widget_show(o);

        o = ewl_spinner_new();
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_spinner_digits_set(EWL_SPINNER(o), 0);
        ewl_range_minimum_value_set(EWL_RANGE(o), 0);
        ewl_range_maximum_value_set(EWL_RANGE(o), 10000);
        ewl_range_value_set(EWL_RANGE(o), 5);
        ewl_range_step_set(EWL_RANGE(o), 1);
        ewl_widget_name_set(o, "rows_spinner");
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_button_label_set(EWL_BUTTON(o), "Set number of rows");
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                ewl_tree_cb_set_rows_clicked, NULL);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_button_label_set(EWL_BUTTON(o), "Row select");
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                tree_cb_select_mode_change, NULL);
        ewl_widget_show(o);

	o = ewl_button_new();
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_button_label_set(EWL_BUTTON(o), "Ensure visible");
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                tree_cb_ensure_visible, NULL);
        ewl_widget_show(o);

        return 1;
}

static void *
tree_test_data_setup(void)
{
        Tree_Test_Data *data;
        Tree_Test_Row_Data **dt;

        data = calloc(1, sizeof(Tree_Test_Data));
        dt = calloc(TREE_DATA_ELEMENTS, sizeof(Tree_Test_Row_Data *));

        dt[0] = calloc(1, sizeof(Tree_Test_Row_Data));
        dt[0]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/e-logo.png");
        dt[0]->text = strdup("The E logo");
        dt[0]->expandable = 0;

        dt[1] = calloc(1, sizeof(Tree_Test_Row_Data));
        dt[1]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/elicit.png");
        dt[1]->text = strdup("The Elicit image");
        dt[1]->expandable = 1;

        dt[1]->subdata = calloc(1, sizeof(Tree_Test_Data));
        dt[1]->subdata->count = 1;
        dt[1]->subdata->row_count = 1;
        dt[1]->subdata->rows = calloc(dt[1]->subdata->count, sizeof(Tree_Test_Row_Data *));
        dt[1]->subdata->rows[0] = calloc(1, sizeof(Tree_Test_Row_Data));
        dt[1]->subdata->rows[0]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/e-logo.png");
        dt[1]->subdata->rows[0]->text = strdup("The First Subrow");

        dt[2] = calloc(1, sizeof(Tree_Test_Row_Data));
        dt[2]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/entrance.png");
        dt[2]->text = strdup("The Entrance image");
        dt[2]->expandable = 1;

        dt[2]->subdata = calloc(1, sizeof(Tree_Test_Data));
        dt[2]->subdata->count = 6;
        dt[2]->subdata->row_count = 6;
        dt[2]->subdata->rows = calloc(dt[2]->subdata->count, sizeof(Tree_Test_Row_Data *));
        dt[2]->subdata->rows[0] = calloc(1, sizeof(Tree_Test_Row_Data));
        dt[2]->subdata->rows[0]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/e-logo.png");
        dt[2]->subdata->rows[0]->text = strdup("Squee. 1.");
        dt[2]->subdata->rows[1] = calloc(1, sizeof(Tree_Test_Row_Data));
        dt[2]->subdata->rows[1]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/e-logo.png");
        dt[2]->subdata->rows[1]->text = strdup("Splat. 2.");
        dt[2]->subdata->rows[2] = calloc(1, sizeof(Tree_Test_Row_Data));
        dt[2]->subdata->rows[2]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/e-logo.png");
        dt[2]->subdata->rows[2]->text = strdup("Squee. 3.");
        dt[2]->subdata->rows[3] = calloc(1, sizeof(Tree_Test_Row_Data));
        dt[2]->subdata->rows[3]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/e-logo.png");
        dt[2]->subdata->rows[3]->text = strdup("Splat. 4.");
        dt[2]->subdata->rows[4] = calloc(1, sizeof(Tree_Test_Row_Data));
        dt[2]->subdata->rows[4]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/e-logo.png");
        dt[2]->subdata->rows[4]->text = strdup("Squee. 5.");
        dt[2]->subdata->rows[5] = calloc(1, sizeof(Tree_Test_Row_Data));
        dt[2]->subdata->rows[5]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/e-logo.png");
        dt[2]->subdata->rows[5]->text = strdup("Splat. 6.");

        dt[3] = calloc(1, sizeof(Tree_Test_Row_Data));
        dt[3]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/End.png");
        dt[3]->text = strdup("Zebra");
        dt[3]->expandable = 0;

        dt[4] = calloc(1, sizeof(Tree_Test_Row_Data));
        dt[4]->image = strdup(PACKAGE_DATA_DIR"/ewl/images/banner-top.png");
        dt[4]->text = strdup("Ant");
        dt[4]->expandable = 0;

        data->rows = dt;
        data->count = TREE_DATA_ELEMENTS;
        data->row_count = TREE_DATA_ELEMENTS;

        return data;
}

static Ewl_Widget *
tree_test_cb_widget_fetch(void *data, unsigned int row __UNUSED__,
                                        unsigned int column,
                                        void *pr_data __UNUSED__)
{
        Ewl_Widget *w = NULL;

        switch (column) {
                case 0:
                        w = ewl_label_new();
                        ewl_label_text_set(EWL_LABEL(w), data);
                        break;
                case 1:
                        w = ewl_image_new();
                        ewl_image_file_path_set(EWL_IMAGE(w), data);
                        break;
                case 2:
                        {
                                Tree_Test_Row_Data *d;
                                d = data;

                                w = ewl_button_new();
                                ewl_button_label_set(EWL_BUTTON(w), d->text);
                                ewl_button_image_set(EWL_BUTTON(w), d->image, NULL);
                        }
                        break;
        }
        ewl_widget_show(w);

        return w;
}

static void *
tree_test_cb_header_data_fetch(void *data __UNUSED__, unsigned int column)
{
        if (column == 0)
                return "Title";

        if (column == 1)
                return "Image";

        return "Button";
}

static Ewl_Widget *
tree_test_cb_header_fetch(void *data, unsigned int column __UNUSED__,
                                void *pr_data __UNUSED__)
{
        Ewl_Widget *l;

        l = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(l), data);
        ewl_widget_show(l);

        return l;
}

static void *
tree_test_data_fetch(void *data, unsigned int row, unsigned int column)
{
        Tree_Test_Data *d;
        void *val = NULL;

        d = data;

        /* NOTE: this is just for testing purposes, should not be needed in a
         * normal app */
        if (row >= d->row_count)
        {
                printf("Asking for too many rows %d (count == %d)\n",
                                                        row, d->row_count);
                return NULL;
        }

        if (column == 0)
                val = d->rows[row % d->count]->text;

        else if (column == 1)
                val = d->rows[row % d->count]->image;

        else if (column == 2)
                val = d->rows[row % d->count];

        else
        {
                /* NOTE: this is just for testing purposes, should not be
                 * needed in a normal app */
                printf("Unknown column %d\n", column);
        }

        return val;
}

static void
tree_test_data_sort(void *data, unsigned int column, Ewl_Sort_Direction sort)
{
        int i;
        Tree_Test_Data *d;

        /* just leave it if we're in sort none. */
        if (sort == EWL_SORT_DIRECTION_NONE)
                return;

        d = data;

        for (i = (d->count - 1); i >= 0; i--)
        {
                int j;

                for (j = 1; j <= i; j++)
                {
                        char *a, *b;

                        if (column == 0)
                        {
                                a = d->rows[j - 1]->text;
                                b = d->rows[j]->text;
                        }
                        else
                        {
                                a = d->rows[j - 1]->image;
                                b = d->rows[j]->image;
                        }

                        if (((sort == EWL_SORT_DIRECTION_ASCENDING)
                                                && strcmp(a, b) > 0)
                                        || ((sort == EWL_SORT_DIRECTION_DESCENDING)
                                                && strcmp(a, b) < 0))
                        {
                                char *temp;

                                temp = d->rows[j - 1]->text;
                                d->rows[j - 1]->text = d->rows[j]->text;
                                d->rows[j]->text = temp;

                                temp = d->rows[j - 1]->image;
                                d->rows[j - 1]->image = d->rows[j]->image;
                                d->rows[j]->image = temp;
                        }
                }
        }
}

static int
tree_test_column_sortable(void *data __UNUSED__, unsigned int column)
{
        return ((column == 0) || (column == 1));
}

static unsigned int
tree_test_data_count_get(void *data)
{
        Tree_Test_Data *d;

        d = data;

        return d->row_count;
}

static int
tree_test_data_expandable_get(void *data, unsigned int row)
{
        Tree_Test_Data *d;
        int ret = FALSE;

        d = data;

        if (d && d->rows[row % d->count])
                ret = d->rows[row % d->count]->expandable;

        return ret;
}

static void *
tree_test_data_expansion_fetch(void *data, unsigned int parent)
{
        Tree_Test_Data *d;

        d = data;

        return d->rows[parent]->subdata;
}


static void
ewl_tree_cb_scroll_headers(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Tree *tree;
        Ewl_Widget *view;

        tree = data;
        view = ewl_tree_content_widget_get(tree);

        if (EWL_TREE_VIEW_SCROLLED_IS(view))
                ewl_tree_view_scrolled_scroll_headers_set(EWL_TREE_VIEW(view),
                        ewl_checkbutton_is_checked(EWL_CHECKBUTTON(w)));
}

static void
ewl_tree_cb_hide_headers(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data)
{
        Ewl_Tree *tree;
        int vis = TRUE;

        tree = data;

        if (ewl_tree_headers_visible_get(tree))
                vis = FALSE;

        ewl_tree_headers_visible_set(tree, vis);
}

static void
ewl_tree_cb_plain_view(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Tree *tree;
        const Ewl_View *view;

        tree = data;
        if (ewl_checkbutton_is_checked(EWL_CHECKBUTTON(w)))
                view = ewl_tree_view_plain_get();
        else
                view = ewl_tree_view_scrolled_get();

        ewl_tree_content_view_set(EWL_TREE(tree), view);
}

static void
ewl_tree_cb_set_rows_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Widget *spinner, *tree;
        Tree_Test_Data *d;

        tree = ewl_widget_name_find("tree");
        spinner = ewl_widget_name_find("rows_spinner");

        d = ewl_mvc_data_get(EWL_MVC(tree));
        d->row_count = ewl_range_value_get(EWL_RANGE(spinner));

        ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);
}

static void
tree_cb_value_changed(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ecore_List *selected;
        Ewl_Selection *sel;

        printf("Selected:\n");
        selected = ewl_mvc_selected_list_get(EWL_MVC(w));
        ecore_list_first_goto(selected);
        while ((sel = ecore_list_next(selected)))
        {
                if (sel->type == EWL_SELECTION_TYPE_INDEX)
                {
                        char *val;
                        unsigned int col;
                        Ewl_Selection_Idx *idx;

                        idx = EWL_SELECTION_IDX(sel);
                        col = idx->column;
                        if (col != 2)
                                val = sel->model->fetch(sel->data, idx->row, col);
                        else
                        {
                                Tree_Test_Row_Data *d;
                                d = sel->model->fetch(sel->data, idx->row, col);
                                val = d->text;
                        }

                        printf("    [%d,%d] %s\n", idx->row, idx->column, val);
                }
                else
                {
                        Ewl_Selection_Range *idx;
                        unsigned int i, k;

                        idx = EWL_SELECTION_RANGE(sel);
                        for (i = idx->start.row; i <= idx->end.row; i++)
                        {
                                for (k = idx->start.column; k <=
                                                        idx->end.column; k++)
                                {
                                        char *val;

                                        if (k != 3)
                                                val = sel->model->fetch(sel->data,
                                                                        i, k);
                                        else
                                        {
                                                Tree_Test_Row_Data *d;
                                                d = sel->model->fetch(sel->data,
                                                                        i, k);
                                                val = d->text;
                                        }
                                        printf("    [%d,%d] %s\n", i, k, val);
                                }
                        }
                }
        }
}

static void
tree_cb_select_mode_change(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        Ewl_Widget *tree;

        tree = ewl_widget_name_find("tree");
        if (!strcmp(ewl_button_label_get(EWL_BUTTON(w)), "Row select"))
        {
                ewl_button_label_set(EWL_BUTTON(w), "Cell select");
                ewl_tree_selection_type_set(EWL_TREE(tree),
                                        EWL_TREE_SELECTION_TYPE_ROW);
        }
        else
        {
                ewl_button_label_set(EWL_BUTTON(w), "Row select");
                ewl_tree_selection_type_set(EWL_TREE(tree),
                                        EWL_TREE_SELECTION_TYPE_CELL);
        }
}

static void
tree_cb_ensure_visible(Ewl_Widget *w, void *ev, void *data)
{
        Ewl_Widget *tree, *spinner;
	int row;

        tree = ewl_widget_name_find("tree");
	spinner = ewl_widget_name_find("rows_spinner");
	
	row = ewl_range_value_get(EWL_RANGE(spinner));

	ewl_tree_row_visible_ensure(EWL_TREE(tree), row);
}
