#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @param fl: The filelist to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialzie a filelist to default values
 */
int
ewl_filelist_init(Ewl_Filelist *fl)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, FALSE);

	if (!ewl_box_init(EWL_BOX(fl)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(EWL_WIDGET(fl), EWL_FILELIST_TYPE);
	ewl_object_fill_policy_set(EWL_OBJECT(fl), EWL_FLAG_FILL_FILL);

	fl->selected = ecore_list_new();
	ewl_callback_prepend(EWL_WIDGET(fl), EWL_CALLBACK_DESTROY,
				ewl_filelist_cb_destroy, NULL);

	ewl_filelist_filter_set(fl, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to set the directory into
 * @param dir: The directory to set
 * @return Returns no value
 * @brief Sets the given directory @a dir as the current directory in the
 * filelist
 */
void
ewl_filelist_directory_set(Ewl_Filelist *fl, const char *dir)
{
	Ewl_Filelist_Event ev_data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	IF_FREE(fl->directory);
	fl->directory = strdup(dir);
	if (fl->dir_change) fl->dir_change(fl);

	ev_data.type = EWL_FILELIST_EVENT_TYPE_DIR_CHANGE;

	ewl_callback_call_with_event_data(EWL_WIDGET(fl), 
			EWL_CALLBACK_VALUE_CHANGED, &ev_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the current directory from
 * @return Returns the current directory 
 * @brief Retrieves the current directory set on the filelist
 */
const char *
ewl_filelist_directory_get(Ewl_Filelist *fl)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, NULL);
	DCHECK_TYPE_RET("fl", fl, EWL_FILELIST_TYPE, NULL);

	DRETURN_INT(fl->directory, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to set the filter into
 * @param filter: The filter to set 
 * @return Returns no value.
 * @brief Sets the given filter into the filelist
 */
void	
ewl_filelist_filter_set(Ewl_Filelist *fl, const char *filter)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	IF_FREE(fl->filter);

	fl->filter = (filter ? strdup(filter) : NULL);
	if (fl->filter_change) fl->filter_change(fl);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the filter from
 * @return Returns the current filter
 * @brief Retrieves the current filter set on the filelist 
 */
const char *
ewl_filelist_filter_get(Ewl_Filelist *fl)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, NULL);
	DCHECK_TYPE_RET("fl", fl, EWL_FILELIST_TYPE, NULL);

	DRETURN_INT(fl->filter, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to set the multiselect value into
 * @param ms: The multiselect value to set
 * @return Returns no value
 * @brief Sets the given multiselect value into the filelist
 */
void	
ewl_filelist_multiselect_set(Ewl_Filelist *fl, unsigned int ms)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	fl->multiselect = !!ms;
	if (fl->multiselect_change) fl->multiselect_change(fl);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the multiselect value from
 * @return Returns the current multiselect state of the filelist
 * @brief Retrieves the current multiselect state of the filelist
 */
unsigned int
ewl_filelist_multiselect_get(Ewl_Filelist *fl)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, 0);
	DCHECK_TYPE_RET("fl", fl, EWL_FILELIST_TYPE, 0);

	DRETURN_INT((unsigned int)fl->multiselect, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to set the show dot files setting into
 * @param dot: The value to set into the show dot files field
 * @return Returns no value.
 * @brief Sets the show dot files setting to the given value.
 */
void
ewl_filelist_show_dot_files_set(Ewl_Filelist *fl, unsigned int dot)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);
	
	fl->show_dot_files = !!dot;
	if (fl->show_dot_change) fl->show_dot_change(fl);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the value from
 * @return Returns the current show dot files setting of the filelist
 * @brief Retrieves the current show dot files setting for the filelist
 */
unsigned int
ewl_filelist_show_dot_files_get(Ewl_Filelist *fl)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, 0);
	DCHECK_TYPE_RET("fl", fl, EWL_FILELIST_TYPE, 0);

	DRETURN_INT((unsigned int)fl->show_dot_files, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to set the selected file into
 * @param file: The file to set selected
 * @return Returns no value
 * @brief Sets the given file as selected in the filelist 
 */
void
ewl_filelist_selected_file_set(Ewl_Filelist *fl, const char *file)
{
	Ewl_Filelist_Event ev_data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	/* clean out the old set of selected files */
	ecore_list_clear(fl->selected);

	if (file) ecore_list_append(fl->selected, strdup(file));
	if (fl->selected_files_change) fl->selected_files_change(fl);

	ev_data.type = EWL_FILELIST_EVENT_TYPE_SELECTION_CHANGE;

	ewl_callback_call_with_event_data(EWL_WIDGET(fl), 
			EWL_CALLBACK_VALUE_CHANGED, &ev_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the selected file from
 * @return Returns the file currently selected in the filelist
 * @brief Returns the currently selected file from the filelist
 */
char *
ewl_filelist_selected_file_get(Ewl_Filelist *fl)
{
	char *file;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, NULL);
	DCHECK_TYPE_RET("fl", fl, EWL_FILELIST_TYPE, 0);

	ecore_list_goto_first(fl->selected);
	file = ecore_list_current(fl->selected);

	DRETURN_PTR((file ? strdup(file) : NULL), DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to set the selected files into
 * @param files: The Ecore_List of files to set
 * @return Returns no value.
 * @brief Sets the given files as selected in the filelist
 */
void
ewl_filelist_selected_files_set(Ewl_Filelist *fl, Ecore_List *files)
{
	char *file;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	/* clean out the old set of selected files */
	ecore_list_clear(fl->selected);

	ecore_list_goto_first(files);
	while ((file = ecore_list_next(files)))
		ecore_list_append(fl->selected, file);

	if (fl->selected_files_change) fl->selected_files_change(fl);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the selected files from
 * @return Returns the Ecore_List of selected files in the filelist
 * @brief Retrieves the list of selected files in the filelist
 */
Ecore_List *
ewl_filelist_selected_files_get(Ewl_Filelist *fl)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, NULL);
	DCHECK_TYPE_RET("fl", fl, EWL_FILELIST_TYPE, NULL);

	DRETURN_INT(fl->selected, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the path from
 * @param dir: The dir name to append to the path
 * @return Returns the full path to the given directory
 * @brief This will attempt to return the full path to the given directory.
 * It should handle things like .. as well.
 */
char *
ewl_filelist_expand_path(Ewl_Filelist *fl, const char *dir)
{
	char path[PATH_MAX];
	const char *cur_dir;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, NULL);
	DCHECK_PARAM_PTR_RET("dir", dir, NULL);
	DCHECK_TYPE_RET("fl", fl, EWL_FILELIST_TYPE, NULL);

	cur_dir = ewl_filelist_directory_get(EWL_FILELIST(fl));
	if (!strcmp(dir, ".."))
	{
		char *t, *t2;
	
		snprintf(path, PATH_MAX, "%s", cur_dir);
		t = path;
		t2 = t;
		while ((*t != '\0'))
		{
			if ((*t == '/') && (*(t + 1) != '\0')) t2 = t;
			t++;
		}
		*t2 = '\0';

		/* make sure we always have at least / in there */
		if (path[0] == '\0')
		{
			path[0] = '/';
			path[1] = '\0';
		}
	}
	else
	{
		/* if the current directory is just / we dont' want to end
		 * up with // on the start of the path. So, check to see if
		 * the second item in cur_dir is a \0, and if so just append
		 * "" instead of getting // twice */
		snprintf(path, PATH_MAX, "%s/%s",
				((cur_dir[1] != '\0') ? cur_dir : ""),
				dir);
	}

	DRETURN_PTR(strdup(path), DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @param func: The function to call to actually add the item
 * @return Returns no value.
 * @brief This will read the directory set in the file list, filter each
 * item through the set filter and call @a func if the file is to be
 * displayed.
 */
void
ewl_filelist_directory_read(Ewl_Filelist *fl, 
		void (*func)(Ewl_Filelist *fl, const char *dir, 
							char *file))
{
	Ecore_List *files;
	char path[PATH_MAX];
	const char *dir;
	char *file;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("func", func);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	dir = ewl_filelist_directory_get(fl);
	files = ecore_file_ls(dir);
	if (!files) DRETURN(DLEVEL_STABLE);

	/* if this isn't the root dir add a .. entry */
	if (strcmp(dir, "/"))
		func(fl, dir, "..");

	while ((file = ecore_list_remove_first(files)))
	{
		snprintf(path, PATH_MAX, "%s/%s", dir, file);

		/* check the filter if this isn't a directory */
		if (fl->filter && (!ecore_file_is_dir(path))
				&& fnmatch(fl->filter, file, 0))
			continue;

		if ((ewl_filelist_show_dot_files_get(fl)) 
				|| (file[0] != '.'))
			func(fl, dir, file);

		FREE(file);
	}
	ecore_list_destroy(files);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_filelist_cb_destroy(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Filelist *fl;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fl = EWL_FILELIST(w);

	if (fl->selected) ecore_list_destroy(fl->selected);
	IF_FREE(fl->directory);
	IF_FREE(fl->filter);

	fl->dir_change = NULL;
	fl->filter_change = NULL;
	fl->multiselect_change = NULL;
	fl->show_dot_change = NULL;
	fl->selected_files_change = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

