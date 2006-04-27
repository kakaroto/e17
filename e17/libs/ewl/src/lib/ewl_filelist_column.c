#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

struct Ewl_Filelist_Column_Data
{
	Ewl_Widget *list;
	char *dir;
};
typedef struct Ewl_Filelist_Column_Data Ewl_Filelist_Column_Data;;

static Ewl_View *ewl_filelist_column_view = NULL;

static void ewl_filelist_column_cb_dir_clicked(Ewl_Widget *w, void *event, 
							void *data);
static void ewl_filelist_column_cb_file_clicked(Ewl_Widget *w, void *event, 
							void *data);
static void ewl_filelist_column_row_add(Ewl_Filelist *fl, const char *dir, 
						char *file, void *data);

/**
 * @return Returns the Ewl_View needed to display the filelist_column
 * @brief Retrieves the needed Ewl_View to use the column view
 */
Ewl_View *
ewl_filelist_column_view_get(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_filelist_column_view)
	{
		ewl_filelist_column_view = ewl_view_new();
		ewl_view_constructor_set(ewl_filelist_column_view, 
					ewl_filelist_column_new);
	}

	DRETURN_PTR(ewl_filelist_column_view, DLEVEL_STABLE);
}

/**
 * @return Returns a new Ewl_Filelist_Column or NULL on failure
 * @brief Creates a new Ewl_Filelist_Column widget
 */
Ewl_Widget *
ewl_filelist_column_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Filelist_Column, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_filelist_column_init(EWL_FILELIST_COLUMN(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Filelist_Column to default values
 */
int
ewl_filelist_column_init(Ewl_Filelist_Column *fl)
{
	Ewl_Filelist *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, FALSE);

	if (!ewl_filelist_init(EWL_FILELIST(fl)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(fl), EWL_FILELIST_COLUMN_TYPE);
	ewl_widget_inherit(EWL_WIDGET(fl), EWL_FILELIST_COLUMN_TYPE);
	ewl_object_fill_policy_set(EWL_OBJECT(fl), EWL_FLAG_FILL_ALL);

	list = EWL_FILELIST(fl);
	list->dir_change = ewl_filelist_column_dir_change;
	list->filter_change = ewl_filelist_column_dir_change;
	list->show_dot_change = ewl_filelist_column_dir_change;
	list->selected_file_add = ewl_filelist_column_selected_file_add;
	list->file_name_get = ewl_filelist_column_filename_get;
	list->selected_unselect = ewl_filelist_column_selected_unselect;
	list->shift_handle = ewl_filelist_column_shift_handle;

	ewl_filelist_vscroll_flag_set(list, EWL_SCROLLPANE_FLAG_ALWAYS_HIDDEN);

	fl->dirs = ecore_list_new();	/* XXX NEED to free this on destroy */

	fl->hbox = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(fl->hbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(fl), fl->hbox);
	ewl_widget_show(fl->hbox);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns no value
 * @brief The callback for a directory change
 */
void
ewl_filelist_column_dir_change(Ewl_Filelist *fl)
{
	Ewl_Filelist_Column *list;
	Ewl_Filelist_Column_Data *d;
	Ecore_List *path_list;
	Ewl_Widget *s;
	char *new_path, *p, *t, *dir, path[PATH_MAX];
	int i = 0, count = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_COLUMN_TYPE);

	list = EWL_FILELIST_COLUMN(fl);
	new_path = strdup(ewl_filelist_directory_get(EWL_FILELIST(fl)));

	memset(path, '\0', PATH_MAX);
	path_list = ecore_list_new();

	/* break the path into it's components */
	p = new_path;
	if (p[0] == '/')
	{
		ecore_list_append(path_list, strdup("/"));
		p++;
	}

	while ((t = strchr((p + 1), '/')))
	{
		char tmp;

		t ++;
		tmp = *t;

		*t = '\0';
		ecore_list_append(path_list, strdup(p));

		*t = tmp;
		p = t;
	}

	if (p && (*(p + 1) != '\0'))
		ecore_list_append(path_list, strdup(p));

	/* find the point at which the two lists differ */
	ecore_list_goto_first(list->dirs);
	ecore_list_goto_first(path_list);
	while (1)
	{
		int len1, len2;

		d = ecore_list_next(list->dirs);
		dir = ecore_list_next(path_list);
		if (!d || !dir) break;
		
		/* we do this as a strncmp and use the length of d->dir as
		 * there is the possiblity we have dropped the /. but, we
		 * also check to make sure that d->dir is no more then 1
		 * more then dir to be on the safe side */
		len1 = strlen(d->dir);
		len2 = strlen(dir);

		if (len1 == (len2 - 1))
			len1 = len2;

		if ((len1 == len2) && strncmp(d->dir, dir, len1))
			break;

		i++;
	}

	/* remove all of the nodes after this one */
	ecore_list_goto_index(list->dirs, i);
	while((d = ecore_list_remove(list->dirs)))
	{
		ewl_widget_destroy(d->list);
		IF_FREE(d->dir);
		FREE(d);
	}

	/* build up our base path */
	ecore_list_goto_first(path_list);
	while(count != i)
	{
		strcat(path, ecore_list_next(path_list));
		count ++;
	}

	/* add the rest of the path to the view */
	while ((dir = ecore_list_next(path_list)))
	{
		s = ewl_scrollpane_new();
		ewl_container_child_append(EWL_CONTAINER(list->hbox), s);
		ewl_scrollpane_hscrollbar_flag_set(EWL_SCROLLPANE(s),
					EWL_SCROLLPANE_FLAG_ALWAYS_HIDDEN);
		ewl_widget_show(s);

		/* setup the path data */
		d = NEW(Ewl_Filelist_Column_Data, 1);
		d->list = s;
		d->dir = strdup(dir);
		ecore_list_append(list->dirs, d);

		strcat(path, dir);
		ewl_filelist_directory_read(fl, path, TRUE, 
					ewl_filelist_column_row_add, s);
	}

	ecore_list_destroy(path_list);
	FREE(new_path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns no value
 * @brief The callback to notify of selected files changing
 */
void
ewl_filelist_column_selected_file_add(Ewl_Filelist *fl, const char *file)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("file", file);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @param item: The item to get the name from
 * @return Returns the filename for the given item
 * @brief Retrieves the filename for the given item
 */
const char *
ewl_filelist_column_filename_get(Ewl_Filelist *fl, void *item)
{
	Ewl_Icon *icon;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, NULL);
	DCHECK_PARAM_PTR_RET("item", item, NULL);
	DCHECK_TYPE_RET("fl", fl, EWL_FILELIST_TYPE, NULL);

	icon = EWL_ICON(item);

	DRETURN_PTR(ewl_icon_label_get(icon), DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns no value.
 * @brief This will set all of the icons back to their unselected state
 */
void
ewl_filelist_column_selected_unselect(Ewl_Filelist *fl)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	ewl_filelist_selected_signal_all(fl, "icon,unselect");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to deal with
 * @param clicked: The currently clicked item
 * @return Returns no value
 * @brief Select the appropriate widgets to deal with a shift click
 */
void
ewl_filelist_column_shift_handle(Ewl_Filelist *fl, Ewl_Widget *clicked)
{
	Ewl_Filelist_Column *list;
	Ewl_Filelist_Column_Data *d;
	Ewl_Widget *parent;
	Ewl_Container *dir_parent;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("clicked", clicked);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);
	DCHECK_TYPE("clicked", clicked, EWL_WIDGET_TYPE);

	list = EWL_FILELIST_COLUMN(fl);
	parent = clicked->parent;

	ecore_list_goto_first(list->dirs);
	while ((d = ecore_list_next(list->dirs)))
	{
		dir_parent = EWL_CONTAINER(d->list);
		while (dir_parent->redirect)
			dir_parent = dir_parent->redirect;

		if (dir_parent == EWL_CONTAINER(parent))
			break;
	}

	ewl_filelist_container_shift_handle(fl,
			EWL_CONTAINER(parent), clicked,
			"icon,select", "icon,unselect");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


static void
ewl_filelist_column_cb_dir_clicked(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Filelist_Column *fl;
	Ewl_Filelist_Column_Data *d;
	Ewl_Event_Mouse_Down *event;
	const char *dir;
	char path[PATH_MAX];
	Ewl_Widget *parent;
	Ewl_Container *dir_parent;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev", ev);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fl = data;
	event = ev;

	/* only trigger on lmb */
	if (event->button != 1)
		DRETURN(DLEVEL_STABLE);

	dir = ewl_icon_label_get(EWL_ICON(w));
	memset(path, '\0', PATH_MAX);

	/* we have a directory clicked. we need to take the icon, grab it's
	 * parent then walk the list of nodes in the dir_list till we find
	 * the parent widget. we can assemble a path from that */
	parent = w->parent;

	ecore_list_goto_first(fl->dirs);
	while ((d = ecore_list_next(fl->dirs)))
	{
		strcat(path, d->dir);

		dir_parent = EWL_CONTAINER(d->list);
		while (dir_parent->redirect)
			dir_parent = dir_parent->redirect;

		if (dir_parent == EWL_CONTAINER(parent))
		{
			strcat(path, "/");
			strcat(path, dir);
			break;
		}
	}
	
	ewl_filelist_directory_set(EWL_FILELIST(fl), path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_column_cb_file_clicked(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Filelist_Column *fl;
	Ewl_Filelist_Column_Data *d;
	char path[PATH_MAX];
	Ewl_Widget *parent;
	Ewl_Container *dir_parent;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev", ev);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fl = data;

	/* need to get the files parent and then update the filelist to have
	 * his parent as the top level widget */
	memset(path, '\0', PATH_MAX);

	parent = w->parent;
	ecore_list_goto_first(fl->dirs);
	while ((d = ecore_list_next(fl->dirs)))
	{
		strcat(path, d->dir);

		dir_parent = EWL_CONTAINER(d->list);
		while (dir_parent->redirect)
			dir_parent = dir_parent->redirect;

		if (dir_parent == EWL_CONTAINER(parent))
			break;
	}

	ewl_filelist_directory_set(EWL_FILELIST(data), path);
	ewl_filelist_handle_click(EWL_FILELIST(data), w, ev, 
					"icon,select", "icon,unselect");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_column_row_add(Ewl_Filelist *fl, const char *dir, char *file,
								void *data)
{
	Ewl_Filelist_Column *list;
	Ewl_Widget *icon;
	char *stock, path[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("dir", dir);
	DCHECK_PARAM_PTR("file", file);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	list = EWL_FILELIST_COLUMN(fl);

	icon = ewl_icon_new();
	ewl_box_orientation_set(EWL_BOX(icon),
			EWL_ORIENTATION_HORIZONTAL);
	ewl_object_fill_policy_set(EWL_OBJECT(icon), EWL_FLAG_FILL_VSHRINK);
	ewl_icon_label_set(EWL_ICON(icon), file);

	snprintf(path, PATH_MAX, "%s/%s", dir, file);
	if (ecore_file_is_dir(path))
	{
		stock = "/stock/open";
		ewl_callback_append(icon, EWL_CALLBACK_CLICKED,
				ewl_filelist_column_cb_dir_clicked, fl);
	}
	else
	{
		stock = "/stock/arrow/right";
		ewl_callback_append(icon, EWL_CALLBACK_CLICKED,
				ewl_filelist_column_cb_file_clicked, fl);
	}

	ewl_icon_image_set(EWL_ICON(icon), ewl_theme_path_get(),
			ewl_theme_data_str_get(EWL_WIDGET(icon), stock));

	ewl_container_child_append(EWL_CONTAINER(data), icon);
	ewl_widget_show(icon);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

