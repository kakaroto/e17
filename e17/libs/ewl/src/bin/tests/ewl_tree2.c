#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TREE2_DATA_ELEMENTS 5

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
						int column);
static void *tree2_test_data_fetch(void *data, unsigned int row, 
						unsigned int column);
static void tree2_test_data_sort(void *data, unsigned int column, 
						Ewl_Sort_Direction sort);
static int tree2_test_data_count_get(void *data);

static void ewl_tree2_cb_scroll_headers(Ewl_Widget *w, void *ev, void *data);
static void ewl_tree2_cb_scroll_visible(Ewl_Widget *w, void *ev, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Tree2";
	test->tip = "Defines a widget for laying out other\n"
			"widgets in a tree or list like manner.";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *tree, *o, *o2;
        Ewl_Model *model;
        Ewl_View *view;
        void *data;

	o2 = ewl_hbox_new();
	ewl_container_child_append(box, o2);
	ewl_object_fill_policy_set(EWL_OBJECT(o2), 
				EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
	ewl_widget_show(o2);

        /* create our data */
        data = tree2_test_data_setup();

        /* the tree will only use one model. We could use a model per
         * column, but a single model will work fine for this test */
        model = ewl_model_new();
        ewl_model_fetch_set(model, tree2_test_data_fetch);
        ewl_model_sort_set(model, tree2_test_data_sort);
        ewl_model_count_set(model, tree2_test_data_count_get);

        tree = ewl_tree2_new();
        ewl_container_child_append(EWL_CONTAINER(box), tree);
        ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_ALL);
        ewl_tree2_data_set(EWL_TREE2(tree), data);
        ewl_widget_show(tree);

        /* create a view for the first column that just has an ewl label */
        view = ewl_view_new();
        ewl_view_constructor_set(view, ewl_label_new);
        ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_label_text_set));
        ewl_view_header_fetch_set(view, tree2_test_data_header_fetch);
        ewl_tree2_column_append(EWL_TREE2(tree), model, view);

        /* create a view for the second column that just has an ewl image */
        view = ewl_view_new();
        ewl_view_constructor_set(view, ewl_image_new);
        ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_image_file_path_set));
        ewl_view_header_fetch_set(view, tree2_test_data_header_fetch);
        ewl_tree2_column_append(EWL_TREE2(tree), model, view);

	/* we don't want this one sortable */
	model = ewl_model_new();
        ewl_model_fetch_set(model, tree2_test_data_fetch);
        ewl_model_count_set(model, tree2_test_data_count_get);

        /* create a view for the third column that has a custom widget */
        view = ewl_view_new();
        ewl_view_constructor_set(view, tree2_test_custom_new);
        ewl_view_assign_set(view, tree2_test_custom_assign_set);
        ewl_view_header_fetch_set(view, tree2_test_data_header_fetch);
        ewl_tree2_column_append(EWL_TREE2(tree), model, view);

	/* create the checkbuttons for the top box */
	o = ewl_checkbutton_new();
	ewl_button_label_set(EWL_BUTTON(o), "Scroll headers");
	ewl_container_child_append(EWL_CONTAINER(o2), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
				ewl_tree2_cb_scroll_headers, tree);
	ewl_widget_show(o);

	o = ewl_checkbutton_new();
	ewl_button_label_set(EWL_BUTTON(o), "Scroll visible");
	ewl_container_child_append(EWL_CONTAINER(o2), o);
	ewl_checkbutton_checked_set(EWL_CHECKBUTTON(o), TRUE);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
				ewl_tree2_cb_scroll_visible, tree);
	ewl_widget_show(o);

	return 1;
}

static void *
tree2_test_data_setup(void)
{
        Tree2_Test_Data *data;
        Tree2_Test_Row_Data **dt;

        data = calloc(1, sizeof(Tree2_Test_Data));
        dt = calloc(TREE2_DATA_ELEMENTS, sizeof(Tree2_Test_Row_Data *));

	dt[0] = calloc(1, sizeof(Tree2_Test_Row_Data));
        dt[0]->image = strdup(PACKAGE_DATA_DIR"/images/e-logo.png");
        dt[0]->text = strdup("The E logo");

	dt[1] = calloc(1, sizeof(Tree2_Test_Row_Data));
        dt[1]->image = strdup(PACKAGE_DATA_DIR"/images/elicit.png");
        dt[1]->text = strdup("The Elicit image");

	dt[2] = calloc(1, sizeof(Tree2_Test_Row_Data));
        dt[2]->image = strdup(PACKAGE_DATA_DIR"/images/entrance.png");
        dt[2]->text = strdup("The Entrance image");

	dt[3] = calloc(1, sizeof(Tree2_Test_Row_Data));
        dt[3]->image = strdup(PACKAGE_DATA_DIR"/images/End.png");
        dt[3]->text = strdup("Zebra");
	
	dt[4] = calloc(1, sizeof(Tree2_Test_Row_Data));
        dt[4]->image = strdup(PACKAGE_DATA_DIR"/images/banner-top.png");
        dt[4]->text = strdup("Ant");

        data->rows = dt;
        data->count = TREE2_DATA_ELEMENTS;

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
tree2_test_data_header_fetch(void *data __UNUSED__, int column)
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

        else if (column == 2)
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
tree2_test_data_sort(void *data, unsigned int column, Ewl_Sort_Direction sort)
{
	int i;
        Tree2_Test_Data *d;

	/* just leave it if we're in sort none. */
	if (sort == EWL_SORT_DIRECTION_NONE)
		return;

	d = data;

	for (i = (TREE2_DATA_ELEMENTS - 1); i >= 0; i--)
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

			if (((sort == EWL_SORT_DIRECTION_ASCENDING) && strcmp(a, b) > 0)
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
tree2_test_data_count_get(void *data)
{
        Tree2_Test_Data *d;

        d = data;

        return d->count;
}

static void
ewl_tree2_cb_scroll_headers(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	Ewl_Tree2 *tree;

	tree = data;
	ewl_tree2_scroll_headers_set(tree, 
			ewl_checkbutton_is_checked(EWL_CHECKBUTTON(w)));
}

static void
ewl_tree2_cb_scroll_visible(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	Ewl_Tree2 *tree;

	tree = data;
	ewl_tree2_scroll_visible_set(tree, 
			ewl_checkbutton_is_checked(EWL_CHECKBUTTON(w)));
}


