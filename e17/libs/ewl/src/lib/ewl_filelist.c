#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_filelist_signal_between(Ewl_Filelist *fl, Ewl_Container *c,
						int add, const char *signal, 
						int a_idx, Ewl_Widget *a, 
						int b_idx, Ewl_Widget *b);

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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	/* clean out the old set of selected files */
	if (fl->selected_unselect) fl->selected_unselect(fl);
	ecore_list_clear(fl->selected);
	if (fl->selected_file_add) fl->selected_file_add(fl, file);

	ewl_filelist_selected_files_change_notify(fl);

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
	void *widget;
	const char *file = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, NULL);
	DCHECK_TYPE_RET("fl", fl, EWL_FILELIST_TYPE, 0);

	ecore_list_goto_first(fl->selected);
	widget = ecore_list_current(fl->selected);
	if (fl->file_name_get) file = fl->file_name_get(fl, widget);

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
	DCHECK_PARAM_PTR("files", files);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	/* clean out the old set of selected files */
	if (fl->selected_unselect) fl->selected_unselect(fl);
	ecore_list_clear(fl->selected);

	ecore_list_goto_first(files);
	while ((file = ecore_list_next(files)))
		if (fl->selected_file_add) fl->selected_file_add(fl, file);

	ewl_filelist_selected_files_change_notify(fl);

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
	Ecore_List *selected;
	void *item;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, NULL);
	DCHECK_TYPE_RET("fl", fl, EWL_FILELIST_TYPE, NULL);

	selected = ecore_list_new();
	ecore_list_goto_first(fl->selected);
	while ((item = ecore_list_next(fl->selected)))
	{
		const char *file;
		file = fl->file_name_get(fl, item);
		ecore_list_append(selected, strdup(file));
	}

	DRETURN_INT(selected, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns no value.
 * @brief Notifies interested consumers that the filelist has changed
 * selected values 
 */
void
ewl_filelist_selected_files_change_notify(Ewl_Filelist *fl)
{
	Ewl_Filelist_Event ev_data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	ev_data.type = EWL_FILELIST_EVENT_TYPE_SELECTION_CHANGE;
	ewl_callback_call_with_event_data(EWL_WIDGET(fl), 
			EWL_CALLBACK_VALUE_CHANGED, &ev_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @param signal: The signal to send
 * @return Returns no value
 * @brief Signals all of the selected widgets with the given signal
 */
void
ewl_filelist_selected_signal_all(Ewl_Filelist *fl, const char *signal)
{
	Ewl_Widget *item;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("signal", signal);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	ecore_list_goto_first(fl->selected);
	while ((item = ecore_list_next(fl->selected)))
		ewl_widget_state_set(item, signal);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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
	Ecore_List *all_files, *files, *dirs;
	char path[PATH_MAX];
	const char *dir;
	char *file;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("func", func);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	dir = ewl_filelist_directory_get(fl);
	all_files = ecore_file_ls(dir);
	if (!all_files) DRETURN(DLEVEL_STABLE);

	files = ecore_list_new();
	dirs = ecore_list_new();

	/* if this isn't the root dir add a .. entry */
	if (strcmp(dir, "/"))
		ecore_list_append(dirs, strdup(".."));

	while ((file = ecore_list_remove_first(all_files)))
	{
		int is_dir;

		snprintf(path, PATH_MAX, "%s/%s", dir, file);
		is_dir = ecore_file_is_dir(path);

		/* check the filter if this isn't a directory */
		if (fl->filter && (!is_dir) && fnmatch(fl->filter, file, 0))
			continue;

		if ((!ewl_filelist_show_dot_files_get(fl)) 
				&& (file[0] == '.'))
			continue;

		if (is_dir) ecore_list_append(dirs, file);
		else ecore_list_append(files, file);
	}

	/* XXX will need to do sorting here ... */
	while ((file = ecore_list_remove_first(dirs)))
	{
		func(fl, dir, file);
		FREE(file);
	}
	
	while ((file = ecore_list_remove_first(files)))
	{
		func(fl, dir, file);
		FREE(file);
	}

	ecore_list_destroy(all_files);
	ecore_list_destroy(files);
	ecore_list_destroy(dirs);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @param w: The widget that was clicked
 * @param select_state: Signal to send to goto select state
 * @param unselect_state: Signal to send to goto unselect state
 * @return Returns no value.
 * @brief Adds or removes the given widget from the select list as needed 
 */
void
ewl_filelist_handle_click(Ewl_Filelist *fl, Ewl_Widget *w,
				Ewl_Event_Mouse_Up *ev,
				const char *select_state, 
				const char *unselect_state)
{
	int multi = FALSE;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev", ev);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* only trigger on lmb */
	if (ev->button != 1) 
		DRETURN(DLEVEL_STABLE);

	/* are the multiselect keys pressed? */
	if ((ev->modifiers & EWL_KEY_MODIFIER_SHIFT)
			|| (ev->modifiers & EWL_KEY_MODIFIER_CTRL))
		multi = TRUE;

	/* we are not in multiselect mode, or the multiselect keys aren't
	 * pressed */
	if (!ewl_filelist_multiselect_get(fl) || (!multi))
	{
		if (fl->selected_unselect) fl->selected_unselect(fl);
		ecore_list_clear(fl->selected);

		if (select_state)
			ewl_widget_state_set(w, select_state);

		ecore_list_append(fl->selected, w);
		ewl_filelist_selected_files_change_notify(fl);

		fl->select.base = w;
		fl->select.last = NULL;

		DRETURN(DLEVEL_STABLE);
	}

	/* ok, we're in multiselect mode and either shift or ctrl are
	 * pressed */

	if (ev->modifiers & EWL_KEY_MODIFIER_SHIFT)
	{
		/* we have no base selected so this is the first click with
		 * the shift. set base and set the clicked as selected */
		if (!fl->select.base)
		{
			fl->select.base = w;
			fl->select.last = NULL;

			if (fl->selected_unselect) fl->selected_unselect(fl);
			ecore_list_clear(fl->selected);
		}
		else
		{
			if (fl->shift_handle) fl->shift_handle(fl, w);
			fl->select.last = w;
		}

		if (select_state) ewl_widget_state_set(w, select_state);
		ecore_list_append(fl->selected, w);

		ewl_filelist_selected_files_change_notify(fl);
	}
	else
	{
		void *item;

		fl->select.base = w;
		fl->select.last = NULL;

		item = ecore_list_goto(fl->selected, w);
		if (item)
		{
			if (unselect_state)
				ewl_widget_state_set(w, unselect_state);
			ecore_list_remove(fl->selected);
		}
		else
		{
			if (select_state)
				ewl_widget_state_set(w, select_state);
			ecore_list_append(fl->selected, w);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @param c: The container to select/unselect from
 * @param clicked: The clicked widget
 * @param select_signal: The signal to send on select
 * @param unselect_signal: The signal to send on unselect
 * @return Returns no value
 * @brief Handles the select/deselect of widgets in the given container @a c
 */
void
ewl_filelist_container_shift_handle(Ewl_Filelist *fl, 
			Ewl_Container *c, Ewl_Widget *clicked,
			const char *select_signal, 
			const char *unselect_signal)
{
	int base_idx, last_idx = -1, cur_idx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("clicked", clicked);
	DCHECK_PARAM_PTR("select_signal", select_signal);
	DCHECK_PARAM_PTR("unselect_signal", unselect_signal);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("clicked", clicked, EWL_WIDGET_TYPE);

	ecore_list_goto(c->children, fl->select.base);
	base_idx = ecore_list_index(c->children);

	ecore_list_goto(c->children, clicked);
	cur_idx = ecore_list_index(c->children);

	if (fl->select.last)
	{
		ecore_list_goto(c->children, fl->select.last);
		last_idx = ecore_list_index(c->children);
	}

	if (last_idx < 0)
	{
		ewl_filelist_signal_between(fl, c, TRUE, select_signal, base_idx,
					fl->select.base, cur_idx, clicked);
	}
	else
	{
		/* user selected more, just tag on whats between the last
		 * click and the current click */
		if (((cur_idx < last_idx) && (last_idx < base_idx))
				|| ((base_idx < last_idx) 
					&& (last_idx < cur_idx)))
		{
			ewl_filelist_signal_between(fl, c, TRUE, select_signal, 
						last_idx, fl->select.last, 
						cur_idx, clicked);
		}
		else
		{
			/* unselect stuff between last and our current index */
			ewl_filelist_signal_between(fl, c, FALSE,
					unselect_signal, last_idx,
					fl->select.last, cur_idx, clicked);

			/* make sure the last selected is removed */
			ewl_widget_state_set(fl->select.last, unselect_signal);
			ecore_list_goto(fl->selected, fl->select.last);
			ecore_list_remove(fl->selected);

			/* if we moved over the base point we need to
			 * reseelct some stuff */
			if (!((last_idx < base_idx) && (cur_idx < base_idx))
					&& !((last_idx > base_idx) 
						&& (cur_idx > base_idx)))
			{
				ewl_filelist_signal_between(fl, c, TRUE,
						select_signal, base_idx,
						fl->select.base, cur_idx, 
						clicked);
			}
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_signal_between(Ewl_Filelist *fl, Ewl_Container *c, int add, 
						const char *signal, 
						int a_idx, Ewl_Widget *a, 
						int b_idx, Ewl_Widget *b)
{
	Ewl_Widget *start, *end, *cur;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("signal", signal);
	DCHECK_PARAM_PTR("a", a);
	DCHECK_PARAM_PTR("b", b);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("a", a, EWL_WIDGET_TYPE);
	DCHECK_TYPE("b", b, EWL_WIDGET_TYPE);

	if (a_idx < b_idx)
	{
		start = a;
		end = b;
	}
	else
	{
		start = b;
		end = a;
	}

	/* select all the widgets between the base and clicked
	 * point, excluding the start/end points */
	ecore_list_goto(c->children, start);
	ecore_list_next(c->children);
	while ((cur = ecore_list_next(c->children)))
	{
		if (cur == end) break;

		if (add)
		{
			ewl_widget_state_set(cur, signal);
			ecore_list_append(fl->selected, cur);
		}
		else
		{
			/* don't remove the base selected widget */
			if (cur != fl->select.base)
			{
				ecore_list_goto(fl->selected, cur);
				ecore_list_remove(fl->selected);
				ewl_widget_state_set(cur, signal);
			}
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_filelist_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
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
	fl->selected_file_add = NULL;
	fl->file_name_get = NULL;
	fl->selected_unselect = NULL;
	fl->shift_handle = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

