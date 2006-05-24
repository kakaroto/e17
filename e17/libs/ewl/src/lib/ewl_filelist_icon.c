#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static Ewl_View *ewl_filelist_icon_view = NULL;

static void ewl_filelist_icon_cb_dir_clicked(Ewl_Widget *w, 
						void *ev, void *data);
static void ewl_filelist_icon_cb_icon_clicked(Ewl_Widget *w,
						void *ev, void *data);
static void ewl_filelist_icon_icon_add(Ewl_Filelist *fl, const char *dir, 
						char *file, void *data);

/**
 * @return Returns the Ewl_View needed to display the filelist_icon
 * @brief Retrieves the needed Ewl_View to use the icon view
 */
Ewl_View *
ewl_filelist_icon_view_get(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	
	if (!ewl_filelist_icon_view)
	{
		ewl_filelist_icon_view = ewl_view_new();
		ewl_view_constructor_set(ewl_filelist_icon_view,
						ewl_filelist_icon_new);
	}

	DRETURN_PTR(ewl_filelist_icon_view, DLEVEL_STABLE);
}

/**
 * @return Returns a new Ewl_Filelist_Icon or NULL on failure
 * @brief Creates a new Ewl_Filelist_Icon widget 
 */
Ewl_Widget *
ewl_filelist_icon_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Filelist_Icon, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_filelist_icon_init(EWL_FILELIST_ICON(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Filelist_Icon to default values
 */
int
ewl_filelist_icon_init(Ewl_Filelist_Icon *fl)
{
	Ewl_Filelist *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, FALSE);

	if (!ewl_filelist_init(EWL_FILELIST(fl)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(fl), EWL_FILELIST_ICON_TYPE);
	ewl_widget_inherit(EWL_WIDGET(fl), EWL_FILELIST_ICON_TYPE);

	list = EWL_FILELIST(fl);
	list->dir_change = ewl_filelist_icon_dir_change;
	list->filter_change = ewl_filelist_icon_dir_change;
	list->show_dot_change = ewl_filelist_icon_dir_change;
	list->selected_file_add = ewl_filelist_icon_selected_file_add;
	list->file_name_get = ewl_filelist_icon_filename_get;
	list->selected_unselect = ewl_filelist_icon_selected_unselect;
	list->shift_handle = ewl_filelist_icon_shift_handle;

	fl->freebox = ewl_vfreebox_new();
	ewl_container_child_append(EWL_CONTAINER(fl), fl->freebox);
	ewl_widget_show(fl->freebox);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns no value
 * @brief The callback for a directory change
 */
void
ewl_filelist_icon_dir_change(Ewl_Filelist *fl)
{
	Ewl_Filelist_Icon *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);

	list = EWL_FILELIST_ICON(fl);
	ewl_container_reset(EWL_CONTAINER(list->freebox));
	ewl_filelist_directory_read(fl, ewl_filelist_directory_get(fl), 
				FALSE, ewl_filelist_icon_icon_add, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @param file: The file to set selected
 * @return Returns no value
 * @brief The callback to notify of selected files changing
 */
void
ewl_filelist_icon_selected_file_add(Ewl_Filelist *fl, const char *file)
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
ewl_filelist_icon_filename_get(Ewl_Filelist *fl, void *item)
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
 * @internal
 * @param fl: The filelist to work with
 * @return Returns no value.
 * @brief This will set all of the icons back to their unselected state
 */
void
ewl_filelist_icon_selected_unselect(Ewl_Filelist *fl)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	ewl_filelist_selected_signal_all(fl, "icon,unselect");

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
ewl_filelist_icon_shift_handle(Ewl_Filelist *fl, Ewl_Widget *clicked)
{
	Ewl_Filelist_Icon *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("clicked", clicked);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);
	DCHECK_TYPE("clicked", clicked, EWL_WIDGET_TYPE);

	list = EWL_FILELIST_ICON(fl);
	ewl_filelist_container_shift_handle(fl,
			EWL_CONTAINER(list->freebox), clicked,
			"icon,select", "icon,unselect");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_icon_cb_dir_clicked(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Filelist_Icon *fl;
	Ewl_Icon *icon;
	const char *dir;
	char *path;
	Ewl_Event_Mouse_Down *event;

	DENTER_FUNCTION(DLEVEL_STABLE);

	icon = EWL_ICON(w);
	fl = data;
	event = ev;

	/* only trigger on lmb */
	if (event->button != 1) 
		DRETURN(DLEVEL_STABLE);

	dir = ewl_icon_label_get(icon);
	path = ewl_filelist_expand_path(EWL_FILELIST(fl), dir);
	ewl_filelist_directory_set(EWL_FILELIST(fl), path);

	FREE(path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_icon_cb_icon_clicked(Ewl_Widget *w, void *ev, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev", ev);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_filelist_handle_click(EWL_FILELIST(data), w, ev, 
					"icon,select", "icon,unselect");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_icon_icon_add(Ewl_Filelist *fl, const char *dir, char *file,
							void *data __UNUSED__)
{
	Ewl_Filelist_Icon *list;
	Ewl_Widget *icon;
	char *stock, path[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("dir", dir);
	DCHECK_PARAM_PTR("file", file);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	list = EWL_FILELIST_ICON(fl);

	icon = ewl_icon_new();
	ewl_box_orientation_set(EWL_BOX(icon),
			EWL_ORIENTATION_HORIZONTAL);
	ewl_icon_label_set(EWL_ICON(icon), file);

	snprintf(path, PATH_MAX, "%s/%s", dir, file);
	if (ecore_file_is_dir(path))
	{
		stock = "/stock/open";
		ewl_callback_append(icon, EWL_CALLBACK_CLICKED,
				ewl_filelist_icon_cb_dir_clicked, fl);
	}
	else
	{
		stock = "/stock/arrow/right";
		ewl_callback_append(icon, EWL_CALLBACK_CLICKED,
				ewl_filelist_icon_cb_icon_clicked, fl);
	}

	ewl_icon_image_set(EWL_ICON(icon), ewl_theme_path_get(),
			ewl_theme_data_str_get(EWL_WIDGET(icon), stock));

	ewl_container_child_append(EWL_CONTAINER(list->freebox), icon);
	ewl_widget_show(icon);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

