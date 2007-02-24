#include "ephoto.h"

static Ewl_Widget *list_view_new(void);
static void list_view_assign(Ewl_Widget *w, void *data);
static Ewl_Widget *list_header_fetch(void *data, int column);
static void *list_data_fetch(void *data, unsigned int row, unsigned int column);
static int list_data_count(void *data);
static void set_active_list_view(Ewl_Widget *w, void *event, void *data);

/*Add the list view*/
Ewl_Widget *add_list_view(Ewl_Widget *c)
{
        em->list_vbox = ewl_vbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(em->list_vbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(c), em->list_vbox);
        ewl_widget_show(em->list_vbox);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), em->list_vbox, "List");

	em->ltree = add_ltree(em->list_vbox);

	return em->list_vbox;
}

/*Show the list view*/
void show_list_view(Ewl_Widget *w, void *event, void *data)
{
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->view_box), em->list_vbox);
        ewl_mvc_dirty_set(EWL_MVC(em->ltree), 1);
	ewl_widget_disable(em->edit_tools);
        ewl_widget_show(em->atree);
        ewl_widget_show(em->tbar);
	ewl_widget_hide(em->ilabel);
}

static void set_active_list_view(Ewl_Widget *w, void *event, void *data)
{
	if (em->currentl)
	{
		ewl_widget_state_set(em->currentl, "unselected", EWL_STATE_PERSISTENT);
	}
	em->currentl = w;
	ewl_widget_state_set(em->currentl, "selected", EWL_STATE_PERSISTENT);

	return;
}

/*Create and Add a Tree to the Container c*/
Ewl_Widget *add_ltree(Ewl_Widget *c)
{
	Ewl_Widget *tree;
	Ewl_Model *model;
	Ewl_View *view;

	model = ewl_model_new();
	ewl_model_fetch_set(model, list_data_fetch);
	ewl_model_count_set(model, list_data_count);

	tree = ewl_tree2_new();
	ewl_tree2_headers_visible_set(EWL_TREE2(tree), 0);
	ewl_tree2_fixed_rows_set(EWL_TREE2(tree), 1);
	ewl_mvc_model_set(EWL_MVC(tree), model);
	ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(c), tree);
	ewl_widget_show(tree);

	view = ewl_view_new();
	ewl_view_constructor_set(view, list_view_new);
	ewl_view_assign_set(view, list_view_assign);
	ewl_view_header_fetch_set(view, list_header_fetch);
	ewl_tree2_column_append(EWL_TREE2(tree), view, FALSE);

	return tree;
}


/* The view of the images */
static Ewl_Widget *list_view_new(void)
{
	Ewl_Widget *hbox;

	hbox = ewl_hbox_new();
	ewl_box_spacing_set(EWL_BOX(hbox), 10);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
	ewl_callback_append(hbox, EWL_CALLBACK_CLICKED, set_active_list_view, NULL);
	ewl_widget_show(hbox);

	return hbox;
}

/*The row that is added to the tree*/
static void list_view_assign(Ewl_Widget *w, void *data)
{
	const char *image;
	char info[PATH_MAX];
	int size;
	int width, height;
	Ewl_Widget *img, *text;

	image = data;
	image_pixels_int_get(image, &width, &height);
	
	img = add_image(w, image, 1, NULL, NULL);
	ewl_image_constrain_set(EWL_IMAGE(img), 48);
	ewl_widget_name_set(w, image);
	
	size = ecore_file_size(image);

	snprintf(info, PATH_MAX, "Name: %s\nPixels: %s\nSize: %s\n",
						 basename((char *)image), 
						 image_pixels_string_get(image),
						 file_size_get(size));
	
	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), info);
	ewl_container_child_append(EWL_CONTAINER(w), text);
	ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_SHRINK);
	ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(text);

	return;
}	

/* The header for the tree */
static Ewl_Widget *list_header_fetch(void *data, int column)
{
	Ewl_Widget *label;

	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), "Images");
	ewl_widget_show(label);

	return label;
}

/* The images that will be displayed*/ 
static void *list_data_fetch(void *data, unsigned int row, unsigned int column)
{
	const char *image;
	void *val = NULL;

	image = ecore_list_goto_index(em->images, row);
	if (image)
	{
		val = image;
	}

	return val;
}

/* The number of images the view is displaying */
static int list_data_count(void *data)
{
	int val;

	val = ecore_list_nodes(em->images);

	return val;
}

