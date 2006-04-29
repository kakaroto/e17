#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static Ewl_View *ewl_filelist_list_view = NULL;

static void ewl_filelist_list_add(Ewl_Filelist *fl, const char *dir, 
						char *file, void *data);
static void ewl_filelist_list_cb_dir_clicked(Ewl_Widget *w, void *ev, 
							void *data);
static void ewl_filelist_list_cb_icon_clicked(Ewl_Widget *w, void *ev, 
							void *data);

/**
 * @return Returns the view for the filelist list
 * @brief Retrieves the Ewl_View needed to use the filelist list view
 */
Ewl_View *
ewl_filelist_list_view_get(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	
	if (!ewl_filelist_list_view)
	{
		ewl_filelist_list_view = ewl_view_new();
		ewl_view_constructor_set(ewl_filelist_list_view,
						ewl_filelist_list_new);
	}

	DRETURN_PTR(ewl_filelist_list_view, DLEVEL_STABLE);
}

/**
 * @return Returns a new Ewl_Filelist_List widget or NULL on failure
 * @brief Creates a new Ewl_Filelist_List widget
 */
Ewl_Widget *
ewl_filelist_list_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Filelist_List, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_filelist_list_init(EWL_FILELIST_LIST(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param fl: The Ewl_Filelist_List to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Filelist_List widget to default values
 */
int
ewl_filelist_list_init(Ewl_Filelist_List *fl)
{
	Ewl_Filelist *list;
	char *headers[] = {"filename", "size", "modifed", 
			"permissions", "owner", "group"};

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, FALSE);

	if (!ewl_filelist_init(EWL_FILELIST(fl)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(fl), EWL_FILELIST_LIST_TYPE);
	ewl_widget_inherit(EWL_WIDGET(fl), EWL_FILELIST_LIST_TYPE);

	list = EWL_FILELIST(fl);
	list->dir_change = ewl_filelist_list_dir_change;
	list->filter_change = ewl_filelist_list_dir_change;
	list->show_dot_change = ewl_filelist_list_dir_change;
	list->selected_file_add = ewl_filelist_list_selected_file_add;
	list->file_name_get = ewl_filelist_list_filename_get;
	list->selected_unselect = ewl_filelist_list_selected_unselect;
	list->shift_handle = ewl_filelist_list_shift_handle;

	fl->tree = ewl_tree_new(6);
	ewl_tree_headers_set(EWL_TREE(fl->tree), headers);
	ewl_container_child_append(EWL_CONTAINER(fl), fl->tree);
	ewl_widget_show(fl->tree);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns no value
 * @brief The callback to notify of a directory change
 */
void
ewl_filelist_list_dir_change(Ewl_Filelist *fl)
{
	Ewl_Filelist_List *list;;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);

	list = EWL_FILELIST_LIST(fl);
	ewl_container_reset(EWL_CONTAINER(list->tree));
	ewl_filelist_directory_read(fl, ewl_filelist_directory_get(fl),
				FALSE, ewl_filelist_list_add, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns no value
 * @brief Callback when the selected files are changed
 */
void
ewl_filelist_list_selected_file_add(Ewl_Filelist *fl, 
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
ewl_filelist_list_filename_get(Ewl_Filelist *fl, void *item)
{
	Ewl_Widget *o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, NULL);
	DCHECK_PARAM_PTR_RET("item", item, NULL);
	DCHECK_TYPE_RET("fl", fl, EWL_FILELIST_TYPE, NULL);

	o = ewl_tree_row_column_get(EWL_ROW(item), 0);

	DRETURN_PTR(ewl_label_text_get(EWL_LABEL(o)), DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns no value.
 * @brief This will set all of the rows back to their unselected state
 */
void
ewl_filelist_list_selected_unselect(Ewl_Filelist *fl)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	ewl_filelist_selected_signal_all(fl, "row,unselect");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to deal with
 * @param clicked: The currently clicked item
 * @return Returns no value
 * @brief Select the appropriate widgets to deal with a shift click
 */
void
ewl_filelist_list_shift_handle(Ewl_Filelist *fl, Ewl_Widget *clicked)
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
ewl_filelist_list_add(Ewl_Filelist *fl, const char *dir, char *file, 
						void *data __UNUSED__)
{
	char path[PATH_MAX];
	char *vals[6];
	struct stat buf;
	struct passwd *pwd;
	struct group *grp;
	struct tm *tm;
	char date[256];
	Ewl_Filelist_List *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("dir", dir);
	DCHECK_PARAM_PTR("file", file);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	list = EWL_FILELIST_LIST(fl);

	snprintf(path, PATH_MAX, "%s/%s", 
			ewl_filelist_directory_get(fl), file);
	if (stat(path, &buf) == 0)
	{
		Ewl_Widget *row;

		vals[0] = file;
		vals[1] = ewl_filelist_size_get(buf.st_size);

		tm = localtime(&buf.st_mtime);
		strftime(date, sizeof(date), nl_langinfo(D_T_FMT), tm);
		vals[2] = strdup(date);

		vals[3] = ewl_filelist_perms_get(buf.st_mode);

		vals[4] = ewl_filelist_username_get(buf.st_uid);

		vals[5] = ewl_filelist_groupname_get(buf.st_gid);

		row = ewl_tree_text_row_add(EWL_TREE(list->tree), NULL, vals);

		if (ecore_file_is_dir(path))
			ewl_callback_append(row, EWL_CALLBACK_CLICKED,
					ewl_filelist_list_cb_dir_clicked, fl);
		else
			ewl_callback_append(row, EWL_CALLBACK_CLICKED,
					ewl_filelist_list_cb_icon_clicked, fl);

		FREE(vals[1]);
		FREE(vals[2]);
		FREE(vals[3]);
		FREE(vals[4]);
		FREE(vals[5]);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_list_cb_dir_clicked(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Widget *o;
	Ewl_Filelist_List *fl;
	const char *dir;
	char *path;
	Ewl_Event_Mouse_Up *event;

	DENTER_FUNCTION(DLEVEL_STABLE);

	o = ewl_tree_row_column_get(EWL_ROW(w), 0);
	fl = data;
	event = ev;

	/* only trigger on lmb */
	if (event->button != 1) 
		DRETURN(DLEVEL_STABLE);

	dir = ewl_label_text_get(EWL_LABEL(o));
	path = ewl_filelist_expand_path(EWL_FILELIST(fl), dir);
	ewl_filelist_directory_set(EWL_FILELIST(fl), path);

	FREE(path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_list_cb_icon_clicked(Ewl_Widget *w, void *ev, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev", ev);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_filelist_handle_click(EWL_FILELIST(data), w, ev, 
					"row,select", "row,unselect");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

