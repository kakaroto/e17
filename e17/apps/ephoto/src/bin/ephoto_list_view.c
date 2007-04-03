#include "ephoto.h"

static Ewl_Widget *list_view_new(void *data, unsigned int row, unsigned int column);
static Ewl_Widget *list_header_fetch(void *data, unsigned int column);
static void *list_data_fetch(void *data, unsigned int row, unsigned int column);
static unsigned int list_data_count(void *data);

/*Add the list view*/
Ewl_Widget *add_list_view(Ewl_Widget *c)
{
        em->list_vbox = add_box(c, EWL_ORIENTATION_VERTICAL, 5);
        ewl_object_fill_policy_set(EWL_OBJECT(em->list_vbox), EWL_FLAG_FILL_ALL);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), em->list_vbox, "List");

	em->ltree = add_ltree(em->list_vbox);

	return em->list_vbox;
}

/*Show the list view*/
void show_list_view(Ewl_Widget *w, void *event, void *data)
{
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->view_box), em->list_vbox);
}

/*Create and Add a Tree to the Container c*/
Ewl_Widget *add_ltree(Ewl_Widget *c)
{
	Ewl_Widget *tree;
	Ewl_Model *model;
	Ewl_View *view;

	model = ewl_model_new();
	ewl_model_data_fetch_set(model, list_data_fetch);
	ewl_model_data_count_set(model, list_data_count);

        view = ewl_view_new();
        ewl_view_widget_fetch_set(view, list_view_new);
        ewl_view_header_fetch_set(view, list_header_fetch);

	tree = ewl_tree2_new();
	ewl_tree2_headers_visible_set(EWL_TREE2(tree), 0);
	ewl_tree2_fixed_rows_set(EWL_TREE2(tree), 1);
	ewl_tree2_column_count_set(EWL_TREE2(tree), 1);
	ewl_mvc_model_set(EWL_MVC(tree), model);
	ewl_mvc_view_set(EWL_MVC(tree), view);
	ewl_mvc_selection_mode_set(EWL_MVC(tree), EWL_SELECTION_MODE_SINGLE);
	ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(c), tree);
	ewl_widget_show(tree);

	return tree;
}


/* The view of the images */
static Ewl_Widget *list_view_new(void *data, unsigned int row, unsigned int column)
{
	const char *image;
	char info[PATH_MAX];
	int size, width, height;
	Ewl_Widget *hbox, *img, *text;

        image = data;
        image_pixels_int_get(image, &width, &height);
        size = ecore_file_size(image);
        snprintf(info, PATH_MAX, "Name: %s\nPixels: %s\nSize: %s\n",
                                                 basename((char *)image),
                                                 image_pixels_string_get(image),
                                                 file_size_get(size));

	hbox = add_box(NULL, EWL_ORIENTATION_HORIZONTAL, 10);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
//	ewl_callback_append(hbox, EWL_CALLBACK_CLICKED, show_edit_view, strdup(image));
	ewl_widget_name_set(hbox, image);

        img = add_image(hbox, image, 1, NULL, NULL);
        ewl_image_constrain_set(EWL_IMAGE(img), 64);

        text = add_text(hbox, info);
        ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_SHRINK);
        ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_LEFT);

	return hbox;
}

/* The header for the tree */
static Ewl_Widget *list_header_fetch(void *data, unsigned int column)
{
	Ewl_Widget *label;

	label = add_label(NULL, "Images");

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
		val = (void *)image;
	}

	return val;
}

/* The number of images the view is displaying */
static unsigned int list_data_count(void *data)
{
	int val;

	val = ecore_list_nodes(em->images);

	return val;
}

