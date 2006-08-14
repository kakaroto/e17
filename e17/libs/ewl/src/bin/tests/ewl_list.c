#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct List_Test_Row_Data List_Test_Row_Data;
struct List_Test_Row_Data
{
	char *image;
	char *text;
};

typedef struct List_Test_Data List_Test_Data;
struct List_Test_Data
{
	unsigned int count;
	List_Test_Row_Data **rows;
};

static int create_test(Ewl_Container *win);

static void *list_test_data_setup(void);
static void list_cb_value_changed(Ewl_Widget *w, void *ev, void *data);

static void list_test_assign_set(Ewl_Widget *w, void *data);
static void *list_test_data_fetch(void *data, unsigned int row, 
						unsigned int column);
static int list_test_data_count_get(void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "List";
	test->tip = "Defines a widget for laying out other\n"
			"widgets in a list like manner.";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
	Ecore_List *str_data;
	Ewl_Widget *list, *o;
	Ewl_Model *model;
	Ewl_View *view;
	char *strs[] = {"foo", "bar", "baz", "bobby", NULL};
	int i;
	void *data;

	/* create a list using an ecore_list of strings of labels */
	o = ewl_border_new();
	ewl_border_text_set(EWL_BORDER(o), "Label List");
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	str_data = ecore_list_new();
	ecore_list_append(str_data, "first");
	ecore_list_append(str_data, "second");
	ecore_list_append(str_data, "third");
	ecore_list_append(str_data, "fourth");

	model = ewl_model_ecore_list_get();

	view = ewl_view_new();
	ewl_view_constructor_set(view, ewl_label_new);
	ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_label_text_set));

	list = ewl_list_new();
	ewl_container_child_append(EWL_CONTAINER(o), list);
	ewl_box_orientation_set(EWL_BOX(list), EWL_ORIENTATION_HORIZONTAL);
	ewl_list_model_set(EWL_LIST(list), model);
	ewl_list_view_set(EWL_LIST(list), view);
	ewl_list_data_set(EWL_LIST(list), str_data);
	ewl_callback_append(list, EWL_CALLBACK_VALUE_CHANGED, 
					list_cb_value_changed, NULL);
	ewl_widget_show(list);


	/* Create a list from a custom array with a custom assign on the
	 * view */
	o = ewl_border_new();
	ewl_border_text_set(EWL_BORDER(o), "Custom List");
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	model = ewl_model_new();
	ewl_model_fetch_set(model, list_test_data_fetch);
	ewl_model_count_set(model, list_test_data_count_get);

	view = ewl_view_new();
	ewl_view_constructor_set(view, ewl_button_new);
	ewl_view_assign_set(view, EWL_VIEW_ASSIGN(list_test_assign_set));

	data = list_test_data_setup();

	list = ewl_list_new();
	ewl_container_child_append(EWL_CONTAINER(o), list);
	ewl_list_model_set(EWL_LIST(list), model);
	ewl_list_view_set(EWL_LIST(list), view);
	ewl_list_data_set(EWL_LIST(list), data);
	ewl_widget_show(list);


	/* create a list by appending label widgets. This dosne't use the
	 * MVC controls just allows you to use the container functions */
	o = ewl_border_new();
	ewl_border_text_set(EWL_BORDER(o), "Container Functions");
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	list = ewl_list_new();
	ewl_container_child_append(EWL_CONTAINER(o), list);
	ewl_widget_show(list);

	for (i = 0; strs[i]; i++)
	{
		o = ewl_label_new();
		ewl_label_text_set(EWL_LABEL(o), strs[i]);
		ewl_widget_show(o);

		ewl_container_child_append(EWL_CONTAINER(list), o);
	}
	
	return 1;
}

static void *
list_test_data_setup(void)
{
	List_Test_Data *data;

	data = calloc(1, sizeof(List_Test_Data));
	data->rows = calloc(3, sizeof(List_Test_Row_Data *));

	data->rows[0] = calloc(1, sizeof(List_Test_Row_Data));
	data->rows[0]->image = strdup("/usr/local/share/ewl/e-logo.png");
	data->rows[0]->text = strdup("The E logo");

	data->rows[1] = calloc(1, sizeof(List_Test_Row_Data));
	data->rows[1]->image = strdup("/usr/local/share/ewl/entice.png");
	data->rows[1]->text = strdup("The Entice image");

	data->rows[2] = calloc(1, sizeof(List_Test_Row_Data));
	data->rows[2]->image = strdup("/usr/local/share/entrance.png");
	data->rows[2]->text = strdup("The Entrance image");

	data->count = 3;

	return data;
}

static void
list_test_assign_set(Ewl_Widget *w, void *data)
{
	List_Test_Row_Data *d;

	d = data;
	ewl_button_label_set(EWL_BUTTON(w), d->text);
	ewl_button_image_set(EWL_BUTTON(w), d->image, NULL);
}

static void *
list_test_data_fetch(void *data, unsigned int row, 
					unsigned int column __UNUSED__)
{
	List_Test_Data *d;

	d = data;

	/* NOTE: this is just for testing purposes, should not be needed in a
	 * normal app */
	if (row >= d->count)
	{
		printf("Asking for too many rows\n");
		return NULL;
	}

	return d->rows[row];
}

static int
list_test_data_count_get(void *data)
{
	List_Test_Data *d;

	d = data;

	return d->count;
}

static void
list_cb_value_changed(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_List *list;
	Ecore_List *el;

	list = EWL_LIST(w);
	el = ewl_list_data_get(list);

	ecore_list_goto_index(el, ewl_list_selected_index_get(list));
	printf("Selected (%s)\n", (char *)ecore_list_current(el));

}


