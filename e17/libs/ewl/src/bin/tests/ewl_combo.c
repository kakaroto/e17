#include "Ewl_Test.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Combo_Test_Data Combo_Test_Data;
struct Combo_Test_Data
{
	unsigned int count;
	char ** data;
};

static void *combo_test_data_setup(void);
static int create_test(Ewl_Container *win);
static void combo_value_changed(Ewl_Widget *w, void *ev, void *data);
static Ewl_Widget *combo_test_data_header_fetch(void *data, 
							unsigned int col);
static void *combo_test_data_fetch(void *data, unsigned int row,
						unsigned int col);
static int combo_test_data_count_get(void *data);
static void combo_cb_add(Ewl_Widget *w, void *ev, void *data);

static Ewl_Widget *combo_test_data_editable_header_fetch(void *data, 
							unsigned int col);
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
	ewl_model_header_fetch_set(model, combo_test_data_header_fetch);

	/* create the view for ewl_label widgets */
	view = ewl_view_new();
	ewl_view_constructor_set(view, ewl_label_new);
	ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_label_text_set));

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
	ewl_model_header_fetch_set(model, 
			combo_test_data_editable_header_fetch);

	view = ewl_view_new();
	ewl_view_constructor_set(view, combo_test_editable_new);
	ewl_view_assign_set(view, 	
			EWL_VIEW_ASSIGN(combo_test_editable_val_set));

	combo = ewl_combo_new();
	ewl_widget_name_set(combo, "combo_custom");
	ewl_container_child_append(EWL_CONTAINER(hbox), combo);
	ewl_callback_append(combo, EWL_CALLBACK_VALUE_CHANGED,
					combo_value_changed, NULL);
	ewl_combo_model_set(EWL_COMBO(combo), model);
	ewl_combo_view_set(EWL_COMBO(combo), view);
	ewl_combo_data_set(EWL_COMBO(combo), data);
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
combo_test_data_header_fetch(void *data __UNUSED__, 
				unsigned int col __UNUSED__)
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
combo_test_data_editable_header_fetch(void *data, 
				unsigned int col __UNUSED__)
{
	Combo_Test_Data *d;
	Ewl_Widget *w, *o, *combo;
	int idx;
	char *val;

	d = data;
	combo = ewl_widget_name_find("combo_custom");
	idx = ewl_combo_selected_get(EWL_COMBO(combo));

	w = ewl_hbox_new();
	if (idx > -1)
	{
		o = ewl_image_new();
		ewl_image_file_path_set(EWL_IMAGE(o), d->data[idx]);
		ewl_container_child_append(EWL_CONTAINER(w), o);
		ewl_widget_show(o);

		val = d->data[idx];
	}
	else
		val = "Please select an option.";

	o = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(o), d->data[idx]);
	ewl_container_child_append(EWL_CONTAINER(w), o);
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

printf("%s\n", (char *)data);

	o = ewl_image_new();
	ewl_image_file_path_set(EWL_IMAGE(o), (char *)data);
	ewl_container_child_append(EWL_CONTAINER(w), o);
	ewl_widget_show(o);

	o = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(o), (char *)data);
	ewl_container_child_append(EWL_CONTAINER(w), o);
	ewl_widget_show(o);
}



