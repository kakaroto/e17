#include "ephoto.h"

static Ewl_Widget *add_ftree(Ewl_Widget *c);
static Ewl_Widget *fsystem_view_new(void *data, unsigned int row, 
					unsigned int column);
static Ewl_Widget *fsystem_header_fetch(void *data, 
					unsigned int column);
static void *fsystem_data_fetch(void *data, unsigned int row, 
					unsigned int column);
static unsigned int fsystem_data_count(void *data);
static void directory_clicked(Ewl_Widget *w, void *event, void *data);
static void populate_images(Ewl_Widget *w, void *event, void *data);

void 
show_fsystem(Ewl_Widget *c)
{
	em->ftree = add_ftree(c);
       	ewl_object_maximum_w_set(EWL_OBJECT(em->ftree), 180);

	populate_fsystem(NULL, NULL, NULL);
}

static unsigned int
highlight_cb(void *data, unsigned int row)
{
	return 0;
}

static Ewl_Widget *
add_ftree(Ewl_Widget *c)
{
	Ewl_Widget *tree;
	Ewl_Model *model;
	Ewl_View *view;

	model = ewl_model_new();
	ewl_model_data_fetch_set(model, fsystem_data_fetch);
	ewl_model_data_count_set(model, fsystem_data_count);
	ewl_model_data_highlight_set(model, highlight_cb);

        view = ewl_view_new();
        ewl_view_widget_fetch_set(view, fsystem_view_new);
        ewl_view_header_fetch_set(view, fsystem_header_fetch);

	tree = ewl_tree_new();
	ewl_tree_headers_visible_set(EWL_TREE(tree), TRUE);
	ewl_tree_fixed_rows_set(EWL_TREE(tree), TRUE);
	ewl_tree_column_count_set(EWL_TREE(tree), 1);
	ewl_mvc_model_set(EWL_MVC(tree), model);
	ewl_mvc_view_set(EWL_MVC(tree), view);
	ewl_mvc_selection_mode_set(EWL_MVC(tree), EWL_SELECTION_MODE_SINGLE);
	ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_FILL);
	ewl_object_minimum_w_set(EWL_OBJECT(tree), 180);
	ewl_container_child_append(EWL_CONTAINER(c), tree);
	ewl_widget_show(tree);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), tree, "File System");

	return tree;
}

static Ewl_Widget *
fsystem_view_new(void *data, unsigned int row, unsigned int column)
{
	char *directory;
	Ewl_Widget *icon;

	directory = data;

	icon = add_icon(NULL, basename(directory), 
				PACKAGE_DATA_DIR "/images/folder.png", 0, 
				directory_clicked, NULL);
        ewl_icon_constrain_set(EWL_ICON(icon), 25);
	ewl_box_orientation_set(EWL_BOX(icon), EWL_ORIENTATION_HORIZONTAL);
	ewl_object_alignment_set(EWL_OBJECT(icon), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(icon), EWL_FLAG_FILL_ALL);
	ewl_widget_name_set(icon, directory);

	return icon;
}

static Ewl_Widget *
fsystem_header_fetch(void *data, unsigned int column)
{
	Ewl_Widget *label;

	label = add_label(NULL, "File System");

	return label;
}

static void *
fsystem_data_fetch(void *data, unsigned int row, unsigned int column)
{
	const char *directory;
	void *val = NULL;

	directory = ecore_list_index_goto(em->fsystem, row);
	if (directory)
	{
		val = (void *)directory;
	}

	return val;
}

static unsigned int 
fsystem_data_count(void *data)
{
	int val;

	val = ecore_list_count(em->fsystem);

	return val;
}

static void directory_clicked(Ewl_Widget *w, void *event, void *data)
{
	const char *dir_clicked, *path;

	dir_clicked = ewl_widget_name_get(w);
	if (!strcmp(dir_clicked, ".."))
		path = dirname(em->current_directory);
	else
		path = dir_clicked;

	em->current_directory = strdup(path);
	populate_fsystem(NULL, NULL, NULL);
}

void 
populate_fsystem(Ewl_Widget *w, void *event, void *data)
{
	const char *directory;
		
	directory = NULL;

	if (w)
	{
		directory = ewl_widget_name_get(w);
		em->current_directory = strdup(directory);
	}

	if (!ecore_list_empty_is(em->fsystem))
		ecore_list_destroy(em->fsystem);

	em->fsystem = ecore_list_new();
	em->fsystem = get_directories(em->current_directory);	
	ecore_list_first_goto(em->fsystem);
	ewl_mvc_data_set(EWL_MVC(em->ftree), em->fsystem);
	populate_images(NULL, NULL, NULL);

	return;
}

static void
populate_images(Ewl_Widget *w, void *event, void *data)
{
	if (!ecore_dlist_empty_is(em->images))
		ecore_dlist_destroy(em->images);

	em->images = ecore_dlist_new();
	if (em->fbox) ewl_container_reset(EWL_CONTAINER(em->fbox));
	em->images = get_images(em->current_directory);
	ecore_dlist_first_goto(em->images);
	ewl_mvc_data_set(EWL_MVC(em->fbox), em->images);

	return;
}

