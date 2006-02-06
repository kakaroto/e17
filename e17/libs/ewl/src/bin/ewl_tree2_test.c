#include "ewl_test.h"

static Ewl_Widget *tree_button;

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

static void
cb_destroy_window(Ewl_Widget *w, void *ev_data __UNUSED__,
				 void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(tree_button, EWL_CALLBACK_CLICKED,
			    __create_tree_test_window, NULL);
}

static void *
tree2_test_data_setup(void)
{
	Tree2_Test_Data *data;
	Tree2_Test_Row_Data *dt;

	data = NEW(Tree2_Test_Data, 1);
	dt = NEW(Tree2_Test_Row_Data, 3);

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

void
__create_tree2_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	Ewl_Widget *tree_win, *tree, *box;
	Ewl_Model *model;
	Ewl_View *view;
	void *data;

	tree_button = w;

	tree_win = ewl_window_new();
	ewl_object_fill_policy_set(EWL_OBJECT(tree_win), EWL_FLAG_FILL_ALL);
	ewl_window_title_set(EWL_WINDOW(tree_win), "Tree2 Test");
	ewl_window_name_set(EWL_WINDOW(tree_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(tree_win), "EFL Test Application");
	ewl_object_size_request(EWL_OBJECT(tree_win), 400, 200);

	if (w) 
	{
		ewl_callback_del(w, EWL_CALLBACK_CLICKED,
		                	 __create_tree_test_window);
		ewl_callback_append(tree_win, EWL_CALLBACK_DELETE_WINDOW,
						    cb_destroy_window, NULL);
	} 
	else
		ewl_callback_append(tree_win, EWL_CALLBACK_DELETE_WINDOW,
						__close_main_window, NULL);
	ewl_widget_show(tree_win);

	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(tree_win), box);
	ewl_widget_show(box);

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
	ewl_tree2_data_set(EWL_TREE2(tree), data);
	ewl_widget_show(tree);

	/* create a view for the first column that just has an ewl label */
	view = ewl_view_new();
	ewl_view_constructor_set(view, ewl_label_new);
	ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_label_text_set));
	ewl_tree2_column_append(EWL_TREE2(tree), model, view);

	/* create a view for the second column that has a custom widget */
	view = ewl_view_new();
	ewl_view_constructor_set(view, tree2_test_custom_new);
	ewl_view_assign_set(view, tree2_test_custom_assign_set);
	ewl_tree2_column_append(EWL_TREE2(tree), model, view);

}

