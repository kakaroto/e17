#include "ephoto.h"

static Ewl_Widget *list_view_new(void);
static void list_view_assign(Ewl_Widget *w, void *data);
static Ewl_Widget *list_header_fetch(void *data, int column);
static void *list_data_fetch(void *data, unsigned int row, unsigned int column);
static int list_data_count(void *data);

/*Show the list view*/
void show_list_view(Ewl_Widget *w, void *event, void *data)
{
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(view_box), list_vbox);
        ewl_mvc_dirty_set(EWL_MVC(ltree), 1);
        ewl_widget_hide(edit_tools);
	ewl_widget_hide(ilabel);
        ewl_widget_show(atree);
        ewl_widget_show(tbar);
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
	Ewl_Widget *icon;

	icon = ewl_icon_new();
	ewl_icon_type_set(EWL_ICON(icon), EWL_ICON_TYPE_LONG);
	ewl_box_orientation_set(EWL_BOX(icon), EWL_ORIENTATION_HORIZONTAL);
	ewl_object_alignment_set(EWL_OBJECT(icon), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(icon), EWL_FLAG_FILL_ALL);
	ewl_callback_append(icon, EWL_CALLBACK_CLICKED, set_info, NULL);
	ewl_widget_show(icon);

	return icon;
}

/*The row that is added to the tree*/
static void list_view_assign(Ewl_Widget *w, void *data)
{
	char *image;
	char info[PATH_MAX];
	int size;
	int width, height;
	Ewl_Widget *text;

	image = data;
	image_pixels_int_get(image, &width, &height);
	if(width > 75 || height > 75)
	{
		ewl_icon_thumbnailing_set(EWL_ICON(w), 1);
	}
	else
	{
		ewl_icon_thumbnailing_set(EWL_ICON(w), 0);
	}
	ewl_icon_image_set(EWL_ICON(w), image, NULL);
	ewl_icon_constrain_set(EWL_ICON(w), 67);
	ewl_widget_name_set(w, image);
	
	size = ecore_file_size(image);
	
	snprintf(info, PATH_MAX, "Name: %s\n"
				 "Pixels: %s\n"
				 "Size: %s\n",
						 basename(image), 
						 image_pixels_string_get(image),
						 file_size_get(size));
	
	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), info);
	ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
	ewl_widget_show(text);
	ewl_icon_extended_data_set(EWL_ICON(w), text);

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
	char *image;
	void *val = NULL;

	image = ecore_list_goto_index(images, row);
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

	val = ecore_list_nodes(images);

	return val;
}

