/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

typedef struct Ewl_Filelist_Tree_Data Ewl_Filelist_Tree_Data;
#define EWL_FILELIST_TREE_DATA(data) ((Ewl_Filelist_Tree_Data *)(data))

struct Ewl_Filelist_Tree_Data
{
	Ewl_Filelist_Tree *list;
	Ecore_List *files;
};

static Ewl_View *ewl_filelist_tree_view = NULL;

static void ewl_filelist_tree_add(Ewl_Filelist *fl, const char *dir, 
						char *file, void *data);

/* Model callbacks */
static void * ewl_filelist_tree_data_fetch(void *data, unsigned int row,
						unsigned int column);
static void ewl_filelist_tree_data_sort(void *data, unsigned int column,
						Ewl_Sort_Direction sort);
static int ewl_filelist_tree_data_count(void *data);
static int ewl_filelist_tree_data_expandable_get(void *data, unsigned int row);
static void * ewl_filelist_tree_data_expansion_data_fetch(void *data,
						unsigned int parent);

/**
 * @return Returns the view for the filelist tree
 * @brief Retrieves the Ewl_View needed to use the filelist tree view
 */
Ewl_View *
ewl_filelist_tree_view_get(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	
	if (!ewl_filelist_tree_view)
	{
		ewl_filelist_tree_view = ewl_view_new();
		ewl_view_constructor_set(ewl_filelist_tree_view,
						ewl_filelist_tree_new);
	}

	DRETURN_PTR(ewl_filelist_tree_view, DLEVEL_STABLE);
}

/**
 * @return Returns a new Ewl_Filelist_Tree widget or NULL on failure
 * @brief Creates a new Ewl_Filelist_Tree widget
 */
Ewl_Widget *
ewl_filelist_tree_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Filelist_Tree, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_filelist_tree_init(EWL_FILELIST_TREE(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param fl: The Ewl_Filelist_Tree to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Filelist_Tree widget to default values
 */
int
ewl_filelist_tree_init(Ewl_Filelist_Tree *fl)
{
	Ewl_View *view;
	Ewl_Model *model;
	Ewl_Filelist *tree;
	Ewl_Filelist_Tree_Data *data;
	/* char *headers[] = {"filename", "size", "modifed", 
			"permissions", "owner", "group"}; */

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, FALSE);

	if (!ewl_filelist_init(EWL_FILELIST(fl)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(fl), EWL_FILELIST_TREE_TYPE);
	ewl_widget_inherit(EWL_WIDGET(fl), EWL_FILELIST_TREE_TYPE);

	tree = EWL_FILELIST(fl);
	tree->dir_change = ewl_filelist_tree_dir_change;
	tree->filter_change = ewl_filelist_tree_dir_change;
	tree->show_dot_change = ewl_filelist_tree_dir_change;
	tree->selected_file_add = ewl_filelist_tree_selected_file_add;
	tree->file_name_get = ewl_filelist_tree_filename_get;
	tree->selected_unselect = ewl_filelist_tree_selected_unselect;
	tree->shift_handle = ewl_filelist_tree_shift_handle;

	/* Wrapper struct to keep a pointer to our filelist */
	data = NEW(Ewl_Filelist_Tree_Data, 1);
	data->list = fl;

	fl->tree = ewl_tree2_new();
	ewl_container_child_append(EWL_CONTAINER(fl), fl->tree);
	ewl_mvc_data_set(EWL_MVC(fl->tree), data);
	ewl_widget_show(fl->tree);

	/* Setup the columns model, by default only one column */
	model = ewl_model_new();
	ewl_model_count_set(model, ewl_filelist_tree_data_count);
	ewl_model_fetch_set(model, ewl_filelist_tree_data_fetch);
	ewl_model_sort_set(model, ewl_filelist_tree_data_sort);
	ewl_model_expandable_set(model, ewl_filelist_tree_data_expandable_get);
	ewl_model_expansion_data_fetch_set(model,
			ewl_filelist_tree_data_expansion_data_fetch);

	view = ewl_label_view_get();
	ewl_tree2_column_append(EWL_TREE2(fl->tree), model, view);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns no value
 * @brief The callback to notify of a directory change
 */
void
ewl_filelist_tree_dir_change(Ewl_Filelist *fl)
{
	Ewl_Filelist_Tree *tree;;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);

	tree = EWL_FILELIST_TREE(fl);
	ewl_filelist_directory_read(fl, ewl_filelist_directory_get(fl),
				FALSE, ewl_filelist_tree_add, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @param file: The file to set selected
 * @return Returns no value
 * @brief Callback when the selected files are changed
 */
void
ewl_filelist_tree_selected_file_add(Ewl_Filelist *fl, 
				const char *file __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);

	/* XXX Write me ... */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @param item: The item to get the name from
 * @return Returns the filename for the given item
 * @brief Retrieves the filename for the given item
 */
const char *
ewl_filelist_tree_filename_get(Ewl_Filelist *fl, void *item)
{
	Ewl_Filelist_Tree_Data *data;
	Ewl_Selection_Idx *selected;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, NULL);
	DCHECK_PARAM_PTR_RET("item", item, NULL);
	DCHECK_TYPE_RET("fl", fl, EWL_FILELIST_TYPE, NULL);

	selected = ewl_mvc_selected_get(EWL_MVC(fl));
	data = selected->sel.data;

	DRETURN_PTR(ecore_list_goto_index(data->files, selected->row), DLEVEL_STABLE);
}

/**
 * @internal
 * @param fl: The filelist to work with
 * @return Returns no value.
 * @brief This will set all of the rows back to their unselected state
 */
void
ewl_filelist_tree_selected_unselect(Ewl_Filelist *fl)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	ewl_filelist_selected_signal_all(fl, "row,unselect");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param fl: The filelist to deal with
 * @param clicked: The currently clicked item
 * @return Returns no value
 * @brief Select the appropriate widgets to deal with a shift click
 */
void
ewl_filelist_tree_shift_handle(Ewl_Filelist *fl, Ewl_Widget *clicked)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("clicked", clicked);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);
	DCHECK_TYPE("clicked", clicked, EWL_WIDGET_TYPE);

	/* XXX fix me */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_tree_add(Ewl_Filelist *fl, const char *dir, char *file, 
						void *data __UNUSED__)
{
	char path[PATH_MAX];
	struct stat buf;
	Ewl_Filelist_Tree *tree;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("dir", dir);
	DCHECK_PARAM_PTR("file", file);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	tree = EWL_FILELIST_TREE(fl);

	snprintf(path, PATH_MAX, "%s/%s", 
			ewl_filelist_directory_get(fl), file);
	if (stat(path, &buf) == 0)
	{
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 */
static int
ewl_filelist_tree_data_count(void *data)
{
	Ewl_Filelist_Tree_Data *td = data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, 0);

	DRETURN_INT(ecore_list_nodes(td->files), DLEVEL_STABLE);
}

/**
 * @internal
 */
static void *
ewl_filelist_tree_data_fetch(void *data, unsigned int row, unsigned int col)
{
	Ewl_Filelist_Tree_Data *td = data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, 0);

	DRETURN_INT(ecore_list_goto_index(td->files, row), DLEVEL_STABLE);
}

/**
 * @internal
 */
static void
ewl_filelist_tree_data_sort(void *data, unsigned int column, Ewl_Sort_Direction sort)
{
	char *file;
	unsigned int count;
	Ecore_Sheap *sorttmp;
	int (*list_add)(Ecore_List *list, void *data);
	Ewl_Filelist_Tree_Data *td = data;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (sort == EWL_SORT_DIRECTION_NONE)
		DRETURN(DLEVEL_STABLE);

	/* Similar to ecore_file_ls, sort using a temporary heap. */
	count = ecore_list_nodes(td->files);
	sorttmp = ecore_sheap_new(ECORE_COMPARE_CB(strcmp), count);

	/* Remove from list and push into sorted heap */
	while ((file = ecore_list_remove_first(td->files)))
		ecore_sheap_insert(sorttmp, file);

	/* Handle sort order by switching between append and prepend */
	if (sort == EWL_SORT_DIRECTION_ASCENDING)
		list_add = ecore_list_append;
	else
		list_add = ecore_list_prepend;

	/* Remove from heap in sorted order and add to list */
	while ((file = ecore_sheap_extract(sorttmp)))
		list_add(td->files, file);

	ecore_sheap_destroy(sorttmp);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 */
static int
ewl_filelist_tree_data_expandable_get(void *data, unsigned int row)
{
	int result;
	char *path;
	const char *file;
	Ewl_Filelist_Tree_Data *td = data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, 0);

	file = ecore_list_goto_index(td->files, row);
	path = ewl_filelist_expand_path(EWL_FILELIST(td->list), file);
	result = ecore_file_is_dir(path);
	FREE(path);

	DRETURN_INT(result, DLEVEL_STABLE);
}

/**
 * @internal
 */
static void *
ewl_filelist_tree_data_expansion_data_fetch(void *data, unsigned int parent)
{
	char *path;
	const char *file;
	Ecore_List *subdir;
	Ewl_Filelist_Tree_Data *td = data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, NULL);

	file = ecore_list_goto_index(td->files, parent);
	path = ewl_filelist_expand_path(EWL_FILELIST(td->list), file);

	subdir = ecore_file_ls(path);
	FREE(path);

	DRETURN_PTR(subdir, DLEVEL_STABLE);
}

