#include "Ewl_Test2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tree2_Test_Row_Data Tree2_Test_Row_Data;
struct Tree2_Test_Row_Data
{
        char *image;
        char *text;
};

typedef struct Tree2_Test_Data Tree2_Test_Data;
struct Tree2_Test_Data
{
        unsigned int count;
        Tree2_Test_Row_Data **rows;
};

static int create_test(Ewl_Container *win);
static void *tree2_test_data_setup(void);
static Ewl_Widget *tree2_test_custom_new(void);
static void tree2_test_custom_assign_set(Ewl_Widget *w, void *data);
static Ewl_Widget *tree2_test_data_header_fetch(void *data, 
						unsigned int column);
static void *tree2_test_data_fetch(void *data, unsigned int row, 
						unsigned int column);
static void tree2_test_data_sort(void *data, unsigned int column);
static int tree2_test_data_count_get(void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Tree2";
	test->tip = "Defines a widget for laying out other\n"
			"widgets in a tree or list like manner.";
	test->filename = "ewl_tree2.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *tree;
        Ewl_Model *model;
        Ewl_View *view;
        void *data;

        /* create our data */
        data = tree2_test_data_setup();

        /* the tree will only use one model. We could use a model per
         * column, but a single model will work fine for this test */
        model = ewl_model_new();
        ewl_model_fetch_set(model, tree2_test_data_fetch);
        ewl_model_sort_set(model, tree2_test_data_sort);
        ewl_model_count_set(model, tree2_test_data_count_get);
        ewl_model_header_fetch_set(model, tree2_test_data_header_fetch);

        tree = ewl_tree2_new();
        ewl_container_child_append(EWL_CONTAINER(box), tree);
        ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_ALL);
        ewl_tree2_data_set(EWL_TREE2(tree), data);
        ewl_widget_show(tree);

        /* create a view for the first column that just has an ewl label */
        view = ewl_view_new();
        ewl_view_constructor_set(view, ewl_label_new);
        ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_label_text_set));
        ewl_tree2_column_append(EWL_TREE2(tree), model, view);

        /* create a view for the second column that just has an ewl image */
        view = ewl_view_new();
        ewl_view_constructor_set(view, ewl_image_new);
        ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_image_file_path_set));
        ewl_tree2_column_append(EWL_TREE2(tree), model, view);

        /* create a view for the third column that has a custom widget */
        view = ewl_view_new();
        ewl_view_constructor_set(view, tree2_test_custom_new);
        ewl_view_assign_set(view, tree2_test_custom_assign_set);
        ewl_tree2_column_append(EWL_TREE2(tree), model, view);

	return 1;
}

static void *
tree2_test_data_setup(void)
{
        Tree2_Test_Data *data;
        Tree2_Test_Row_Data *dt;

        data = calloc(1, sizeof(Tree2_Test_Data));
        dt = calloc(3, sizeof(Tree2_Test_Row_Data));

        dt[0].image = strdup("/usr/local/share/ewl/e-logo.png");
        dt[0].text = strdup("The E logo");

        dt[1].image = strdup("/usr/local/share/ewl/entice.png");
        dt[1].text = strdup("The Entice image");

        dt[2].image = strdup("/usr/local/share/entrance.png");
        dt[2].text = strdup("The Entrance image");

        data->rows = &dt;
        data->count = 3;

        return data;
}

static Ewl_Widget *
tree2_test_custom_new(void)
{
        Ewl_Widget *button;

        button = ewl_button_new();
        ewl_widget_show(button);

        return button;
}

static void
tree2_test_custom_assign_set(Ewl_Widget *w, void *data)
{
        Tree2_Test_Row_Data *d;

        d = data;
        ewl_button_label_set(EWL_BUTTON(w), d->text);
        ewl_button_image_set(EWL_BUTTON(w), d->image, NULL);
}

static Ewl_Widget *
tree2_test_data_header_fetch(void *data __UNUSED__, unsigned int column)
{
        Ewl_Widget *l;

        l = ewl_label_new();
        if (column == 0)
                ewl_label_text_set(EWL_LABEL(l), "Title");
        else if (column == 1)
                ewl_label_text_set(EWL_LABEL(l), "Image");
        else
                ewl_label_text_set(EWL_LABEL(l), "Button");
        ewl_widget_show(l);

        return l;
}

static void *
tree2_test_data_fetch(void *data, unsigned int row, unsigned int column)
{
        Tree2_Test_Data *d;
        void *val = NULL;

        d = data;

        /* NOTE: this is just for testing purposes, should not be needed in a
         * normal app */
        if (row >= d->count)
        {
                printf("Asking for too many rows\n");
                return NULL;
        }

        if (column == 0)
                val = d->rows[row]->text;

        else if (column == 1)
                val = d->rows[row]->image;

        else if (column == 1)
                val = d->rows[row];

        else
        {
                /* NOTE: this is just for testing purposes, should not be
                 * needed in a normal app */
                printf("Unknown column %d\n", column);
        }

        return val;
}

static void
tree2_test_data_sort(void *data __UNUSED__, unsigned int column __UNUSED__)
{
        printf("No sort yet.\n");
}

static int
tree2_test_data_count_get(void *data)
{
        Tree2_Test_Data *d;

        d = data;

        return d->count;
}

