#include "ephoto_fsystem.h"

static Ewl_Widget *add_ftree(Ewl_Widget *c);
static unsigned int highlight_cb(void *data, unsigned int row);
static Ewl_Widget *fsystem_view_constructor(unsigned int column, void *pr_data);
static void fsystem_view_assign(Ewl_Widget *w, void *data, unsigned int row,
                                        unsigned int column, void *pr_data);
static Ewl_Widget *fsystem_header_fetch(void *data, unsigned int row, 
						void *dat);
static void *fsystem_data_fetch(void *data, unsigned int row,
					unsigned int column);
static unsigned int fsystem_data_count(void *data);
static void directory_clicked(Ewl_Widget *w, void *event, void *data);
static void populate_images(Ewl_Widget *w, void *event, void *data);

void add_fsystem(Ewl_Widget *c)
{
	Ewl_Widget *ftree;

	ftree = add_ftree(c);
	ephoto_set_ftree(ftree);

	populate_fsystem(NULL, NULL, NULL);
}

static Ewl_Widget *add_ftree(Ewl_Widget *c) {
	Ewl_Widget *tree;
	Ewl_Model *model;
	Ewl_View *view;

	model = ewl_model_new();
	ewl_model_data_fetch_set(model, fsystem_data_fetch);
	ewl_model_data_count_set(model, fsystem_data_count);
	ewl_model_data_highlight_set(model, highlight_cb);

	view = ewl_view_new();
	ewl_view_widget_constructor_set(view, fsystem_view_constructor);
	ewl_view_widget_assign_set(view, fsystem_view_assign);
	ewl_view_header_fetch_set(view, fsystem_header_fetch);

	tree = ewl_tree_new();
	ewl_tree_headers_visible_set(EWL_TREE(tree), TRUE);
	ewl_tree_fixed_rows_set(EWL_TREE(tree), TRUE);
	ewl_tree_column_count_set(EWL_TREE(tree), 1);
	ewl_mvc_model_set(EWL_MVC(tree), model);
	ewl_mvc_view_set(EWL_MVC(tree), view);
	ewl_mvc_selection_mode_set(EWL_MVC(tree), EWL_SELECTION_MODE_SINGLE);
	ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_FILL);
	ewl_container_child_prepend(EWL_CONTAINER(c), tree);
	ewl_widget_show(tree);
	ewl_paned_initial_size_set(EWL_PANED(c), tree, 190);
	ewl_paned_fixed_size_set(EWL_PANED(c), tree, 190);

	return tree;
}

static unsigned int highlight_cb(void *data, unsigned int row) {
	return 0;
}

static Ewl_Widget *fsystem_view_constructor(unsigned int column, void *pr_data) {
	Ewl_Widget *icon;

	icon = ewl_icon_simple_new();

	return icon;
}

static void fsystem_view_assign(Ewl_Widget *w, void *data, unsigned int row,
					unsigned int column, void *pr_data) {
	Ewl_Widget *icon;
	char *directory;

	icon = w;
	directory = (char *)data;

	ewl_icon_label_set(EWL_ICON(icon), basename(directory));
	ewl_icon_constrain_set(EWL_ICON(icon), 25);
	ewl_box_orientation_set(EWL_BOX(icon), EWL_ORIENTATION_HORIZONTAL);
	ewl_object_alignment_set(EWL_OBJECT(icon), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(icon), EWL_FLAG_FILL_ALL);
	ewl_callback_append(icon, EWL_CALLBACK_CLICKED, 
					directory_clicked, NULL);
	if (!strncmp(directory, "..", 2)) {
		ewl_widget_name_set(icon, 
				dirname(ephoto_get_current_directory()));
		ewl_icon_image_set(EWL_ICON(icon),
                                	PACKAGE_DATA_DIR "/images/go-up.png", 
					NULL);
	}
	else {
		ewl_widget_name_set(icon, directory);
		ewl_icon_image_set(EWL_ICON(icon),
                                        PACKAGE_DATA_DIR "/images/folder.png",  
                                        NULL);
	}
	ewl_widget_show(icon);
}

static Ewl_Widget *fsystem_header_fetch(void *data, unsigned int column, 
							void *dat) {
	Ewl_Widget *label;

	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), "File System");
	ewl_widget_show(label);

	return label;
}

static void *fsystem_data_fetch(void *data, unsigned int row, 
						unsigned int column) {
	const char *directory;
	void *val = NULL;

	directory = (char *)ecore_list_index_goto(ephoto_get_fsystem(), row);
	if (directory)
		val = (void *)directory;
	return val;
}

static unsigned int fsystem_data_count(void *data) {
	int val;

	val = ecore_list_count(ephoto_get_fsystem());

	return val;
}

static void directory_clicked(Ewl_Widget *w, void *event, void *data) {
	char *dir_clicked, *path;

	dir_clicked = (char *)ewl_widget_name_get(w);
	if (!strcmp(dir_clicked, ".."))
		path = dirname(ephoto_get_current_directory());
	else
		path = dir_clicked;
	ephoto_set_current_directory(strdup(path));
	populate_fsystem(NULL, NULL, NULL);
}

void populate_fsystem(Ewl_Widget *w, void *event, void *data) {
	char *directory;
	Ecore_List *fsystem;

	directory = NULL;
	
	if (w) {
		directory = (char *)ewl_widget_name_get(w);
		ephoto_set_current_directory(strdup(directory));
	}
	if (!ecore_list_empty_is(ephoto_get_fsystem()))
		ecore_list_destroy(ephoto_get_fsystem());
	fsystem = ecore_list_new();
	fsystem = get_directories(ephoto_get_current_directory());
	ecore_list_first_goto(fsystem);
	ewl_mvc_data_set(EWL_MVC(ephoto_get_ftree()), fsystem);
	ephoto_set_fsystem(fsystem);

	populate_images(NULL, NULL, NULL);

	return;
}

static void populate_images(Ewl_Widget *w, void *event, void *data) {
	Ecore_List *images;

	if (!ecore_dlist_empty_is(ephoto_get_images()))
		ecore_dlist_destroy(ephoto_get_images());

	images = ecore_dlist_new();
	images = get_images(ephoto_get_current_directory());
	ecore_dlist_first_goto(images);
	ewl_mvc_data_set(EWL_MVC(ephoto_get_fbox()), images);
	ephoto_set_images(images);

	return;
}
