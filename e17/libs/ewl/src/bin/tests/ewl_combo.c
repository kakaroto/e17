#include "Ewl_Test.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @addtogroup Ewl_Combo
 * @section combo_tut Tutorial
 *
 * The Ewl_Combo widget is based on a Model/View/Controller design. Before
 * you can use the combo you need to setup your startting data structure,
 * your model and your view. Once everything is created if you want to
 * change your data all you have to do is tell the combo that it's data is
 * dirty and it will redisplay the combo box.
 *
 * @code
 * model = ewl_model_new();
 * ewl_model_fetch_set(model, combo_test_data_fetch);
 * ewl_model_count_set(model, combo_test_data_count_get);

 * view = ewl_view_new();
 * ewl_view_constructor_set(view, ewl_label_new);
 * ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_label_text_set));
 * ewl_view_header_fetch_set(view, combo_test_data_header_fetch);

 * combo = ewl_combo_new();
 * ewl_callback_append(combo, EWL_CALLBACK_VALUE_CHANGED,
 * 				combo_value_changed, NULL);
 * ewl_combo_model_set(EWL_COMBO(combo), model);
 * ewl_combo_view_set(EWL_COMBO(combo), view);
 * ewl_combo_data_set(EWL_COMBO(combo), data);
 * ewl_widget_show(combo);
 * @endcode
 * 
 * If you have a custom widget you wish to display you can set your own
 * functions into the view to draw your widget. In this case we just want a
 * simple label displayed.
 *
 * The data header is optional and will be displayed at the top of your
 * combo. In the case where the combo is editable it will use the header as
 * the normal display. In this case you are responsible for creating and
 * populating the widget.
 *
 * @code
 * static Ewl_Widget *combo_test_data_header_fetch(void *data, int col);
 * static void *combo_test_data_fetch(void *data, unsigned int row,
 *						unsigned int col);
 * static int combo_test_data_count_get(void *data);
 * @endcode
 *
 * The three model functions are responsible for getting the information
 * from your model as needed. Each time the combo needs another row of data
 * it will call the data_fetch function. The col parameter is unused by the
 * combo box. The count_get function is responsible for returning a count of
 * the number of items in your data structure. Each of these three functions
 * receive a void *data param. This is your data as set into the combo box
 * so you shouldn't need to create a global pointer to the data.
 * 
 * @note If you set the combo to editable, with ewl_combo_editable set then
 * instead of using the model/view to get the data we will query the view
 * for the header. It is then up to the app to do what they will with the
 * header to make it 'editable'. In most cases, this will mean packing an
 * entry into there with the value from the data. In this case you will need
 * to attach a EWL_CALLBACK_VALUE_CHANGED callback to the entry and handle
 * its value change yourself. The combo won't know about any changed values
 * in the entry and will always have the value from the data.
 */

typedef struct Combo_Test_Data Combo_Test_Data;
struct Combo_Test_Data
{
	unsigned int count;
	char ** data;
};

static void *combo_test_data_setup(void);
static int create_test(Ewl_Container *win);
static void combo_value_changed(Ewl_Widget *w, void *ev, void *data);
static Ewl_Widget *combo_test_data_header_fetch(void *data, int col);
static void *combo_test_data_fetch(void *data, unsigned int row,
						unsigned int col);
static int combo_test_data_count_get(void *data);
static void combo_cb_add(Ewl_Widget *w, void *ev, void *data);
static void combo_cb_entry_changed(Ewl_Widget *w, void *ev, void *data);

static Ewl_Widget *combo_test_data_editable_header_fetch(void *data, int col);
static Ewl_Widget *combo_test_editable_new(void);
static void combo_test_editable_val_set(Ewl_Widget *w, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Combo";
	test->tip = "Defines a combo box used internally.\n"
		"The contents of the box are not drawn\n"
		"outside of the evas.";
	test->filename = "ewl_combo.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_MISC;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *combo, *hbox, *o;
	Ewl_Model *model;
	Ewl_View *view;
	void *data;

	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), hbox);
	ewl_widget_show(hbox);

	data = combo_test_data_setup();

	/* create the model */
	model = ewl_model_new();
	ewl_model_fetch_set(model, combo_test_data_fetch);
	ewl_model_count_set(model, combo_test_data_count_get);

	/* create the view for ewl_label widgets */
	view = ewl_view_new();
	ewl_view_constructor_set(view, ewl_label_new);
	ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_label_text_set));
	ewl_view_header_fetch_set(view, combo_test_data_header_fetch);

	combo = ewl_combo_new();
	ewl_widget_name_set(combo, "combo_label");
	ewl_container_child_append(EWL_CONTAINER(hbox), combo);
	ewl_callback_append(combo, EWL_CALLBACK_VALUE_CHANGED,
					combo_value_changed, NULL);
	ewl_combo_model_set(EWL_COMBO(combo), model);
	ewl_combo_view_set(EWL_COMBO(combo), view);
	ewl_combo_data_set(EWL_COMBO(combo), data);
	ewl_widget_show(combo);

	/* create the view for ewl_image widgets */
	view = ewl_view_new();
	ewl_view_constructor_set(view, ewl_image_new);
	ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_image_file_path_set));
	ewl_view_header_fetch_set(view, combo_test_data_header_fetch);

	combo = ewl_combo_new();
	ewl_widget_name_set(combo, "combo_image");
	ewl_container_child_append(EWL_CONTAINER(hbox), combo);
	ewl_callback_append(combo, EWL_CALLBACK_VALUE_CHANGED,
					combo_value_changed, NULL);
	ewl_combo_model_set(EWL_COMBO(combo), model);
	ewl_combo_view_set(EWL_COMBO(combo), view);
	ewl_combo_data_set(EWL_COMBO(combo), data);
	ewl_widget_show(combo);

	/* create the editable model/view */
	model = ewl_model_new();
	ewl_model_count_set(model, combo_test_data_count_get);
	ewl_model_fetch_set(model, combo_test_data_fetch);

	view = ewl_view_new();
	ewl_view_constructor_set(view, combo_test_editable_new);
	ewl_view_assign_set(view, 	
			EWL_VIEW_ASSIGN(combo_test_editable_val_set));
	ewl_view_header_fetch_set(view, 
			combo_test_data_editable_header_fetch);

	combo = ewl_combo_new();
	ewl_widget_name_set(combo, "combo_custom");
	ewl_container_child_append(EWL_CONTAINER(hbox), combo);
	ewl_callback_append(combo, EWL_CALLBACK_VALUE_CHANGED,
					combo_value_changed, NULL);
	ewl_combo_model_set(EWL_COMBO(combo), model);
	ewl_combo_view_set(EWL_COMBO(combo), view);
	ewl_combo_data_set(EWL_COMBO(combo), data);
	ewl_combo_editable_set(EWL_COMBO(combo), TRUE);
	ewl_widget_show(combo);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Add items");
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, combo_cb_add, NULL);
	ewl_widget_show(o);

	return 1;
}

static void *
combo_test_data_setup(void)
{
	Combo_Test_Data *data;

	data = calloc(1, sizeof(Combo_Test_Data));
	data->count = 3;

	data->data = calloc(3, sizeof(char *));
	data->data[0] = strdup(PACKAGE_DATA_DIR "/images/Draw.png");
	data->data[1] = strdup(PACKAGE_DATA_DIR "/images/End.png");
	data->data[2] = strdup(PACKAGE_DATA_DIR "/images/World.png");

	return data;
}

static Ewl_Widget *
combo_test_data_header_fetch(void *data __UNUSED__, int col __UNUSED__)
{
	Ewl_Widget *header;

	header = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(header), "Select Image");
	ewl_widget_show(header);

	return header;
}

static void *
combo_test_data_fetch(void *data, unsigned int row, 
				unsigned int col __UNUSED__)
{
	Combo_Test_Data *d;

	d = data;
	if (row < d->count)
		return d->data[row];
	else
		return NULL;
}

static int
combo_test_data_count_get(void *data)
{
	Combo_Test_Data *d;

	d = data;
	return d->count;
}

static void
combo_value_changed(Ewl_Widget *w, void *ev __UNUSED__, 
				void *data __UNUSED__)
{
	Combo_Test_Data *d;
	int idx;

	d = ewl_combo_data_get(EWL_COMBO(w));
	idx = ewl_combo_selected_get(EWL_COMBO(w));
	if (idx > -1)
		printf("value changed to %d (%s)\n", idx, d->data[idx]);
	else
		printf("Nothing selected.\n");
}

static void
combo_cb_add(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Widget *c;
	Combo_Test_Data *d;
	int s;

	c = ewl_widget_name_find("combo_label");
	d = ewl_combo_data_get(EWL_COMBO(c));

	s = d->count;
	d->count += 2;
	d->data = realloc(d->data, sizeof(char *) * d->count);

	d->data[s] = strdup(PACKAGE_DATA_DIR "/images/Package.png");
	d->data[s + 1] = strdup(PACKAGE_DATA_DIR "/images/Open.png");

	ewl_combo_dirty_set(EWL_COMBO(c), 1);

	c = ewl_widget_name_find("combo_image");
	ewl_combo_dirty_set(EWL_COMBO(c), 1);
}

static Ewl_Widget *
combo_test_data_editable_header_fetch(void *data, int col)
{
	Combo_Test_Data *d;
	Ewl_Widget *w, *o;
	char *val;

	d = data;
	w = ewl_hbox_new();
	if (col > -1)
	{
		o = ewl_image_new();
		ewl_image_file_path_set(EWL_IMAGE(o), d->data[col]);
		ewl_container_child_append(EWL_CONTAINER(w), o);
		ewl_widget_show(o);

		val = d->data[col];
	}
	else
		val = "Please select an option.";

	o = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(o), val);
	ewl_container_child_append(EWL_CONTAINER(w), o);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED, 
				combo_cb_entry_changed, NULL);
	ewl_widget_show(o);

	return w;
}

static Ewl_Widget *
combo_test_editable_new(void)
{
	Ewl_Widget *w;

	w = ewl_hbox_new();

	return w;
}

static void
combo_test_editable_val_set(Ewl_Widget *w, void *data)
{
	Ewl_Widget *o;

	o = ewl_image_new();
	ewl_image_file_path_set(EWL_IMAGE(o), (char *)data);
	ewl_container_child_append(EWL_CONTAINER(w), o);
	ewl_widget_show(o);

	o = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(o), (char *)data);
	ewl_container_child_append(EWL_CONTAINER(w), o);
	ewl_widget_show(o);
}

static void
combo_cb_entry_changed(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	printf("value changed to (%s)\n", ewl_text_text_get(EWL_TEXT(w)));
}



