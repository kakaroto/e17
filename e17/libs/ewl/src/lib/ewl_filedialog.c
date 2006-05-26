#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_filedialog_respond(Ewl_Filedialog *fd, unsigned int response);
static void ewl_filedialog_cb_value_changed(Ewl_Widget *w, void *ev, 
							void *data);
static void ewl_filedialog_cb_mouse_down(Ewl_Widget *w, void *ev, void *data);
static void ewl_filedialog_cb_column_view(Ewl_Widget *w, void *ev, void *data);
static void ewl_filedialog_cb_icon_view(Ewl_Widget *w, void *ev, void *data);
static void ewl_filedialog_cb_list_view(Ewl_Widget *w, void *ev, void *data);

static void ewl_filedialog_cb_show_dot(Ewl_Widget *w, void *ev, void *data);
static void ewl_filedialog_cb_show_favorites(Ewl_Widget *w, void *ev, void *data);

/**
 * @return Returns a new open filedialog if successful, NULL on failure.
 * @brief Create a new open filedialog
 */
Ewl_Widget *
ewl_filedialog_multiselect_new(void)
{
	Ewl_Widget *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fd = ewl_filedialog_new();
	if (!fd) 
		DRETURN_PTR(NULL, DLEVEL_STABLE);
		
	ewl_filedialog_multiselect_set(EWL_FILEDIALOG(fd), TRUE);

	DRETURN_PTR(EWL_WIDGET(fd), DLEVEL_STABLE);
}

/**
 * @return Returns a new filedialog in success, NULL on failure.
 * @brief Create a new filedialog
 */
Ewl_Widget *
ewl_filedialog_new(void)
{
	Ewl_Filedialog *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fd = NEW(Ewl_Filedialog, 1);
	if (!fd) 
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_filedialog_init(fd)) 
	{
		ewl_widget_destroy(EWL_WIDGET(fd));
		fd = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(fd), DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @return Returns no value.
 * @brief Initialize a new filedialog
 */
int
ewl_filedialog_init(Ewl_Filedialog *fd)
{
	Ewl_Widget *w, *menu, *o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, FALSE);

	w = EWL_WIDGET(fd);
 	if (!ewl_dialog_init(EWL_DIALOG(fd)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(w, EWL_FILEDIALOG_TYPE);

	ewl_window_title_set(EWL_WINDOW(fd), "Ewl Filedialog");
	ewl_window_name_set(EWL_WINDOW(fd), "Ewl Filedialog");
	ewl_window_class_set(EWL_WINDOW(fd), "Ewl Filedialog");

	ewl_callback_append(EWL_WIDGET(fd), EWL_CALLBACK_DELETE_WINDOW,
				ewl_filedialog_cb_delete_window, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
				ewl_filedialog_cb_mouse_down, NULL);

	ewl_dialog_active_area_set(EWL_DIALOG(fd), EWL_POSITION_TOP);

	/* the file picker */
	fd->fp = ewl_filepicker_new();
	ewl_widget_internal_set(fd->fp, TRUE);
	ewl_container_child_append(EWL_CONTAINER(fd), fd->fp);
	ewl_callback_append(fd->fp, EWL_CALLBACK_VALUE_CHANGED,
				ewl_filedialog_cb_value_changed, fd);
	ewl_widget_show(fd->fp);

	/* we don't want an action area with this as the filepicker provides
	 * it's own ok/cancel buttons */
	ewl_dialog_has_separator_set(EWL_DIALOG(fd), FALSE);
	ewl_widget_hide(EWL_DIALOG(fd)->action_area);

	fd->menu = ewl_menu_new();
	ewl_button_label_set(EWL_BUTTON(fd->menu), " ");
	ewl_widget_show(fd->menu);

	menu = ewl_menu_new();
	ewl_button_label_set(EWL_BUTTON(menu), "View");
	ewl_container_child_append(EWL_CONTAINER(fd->menu), menu);
	ewl_widget_show(menu);

	o = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(o), "Icon view");
	ewl_container_child_append(EWL_CONTAINER(menu), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
				ewl_filedialog_cb_icon_view, fd);
	ewl_widget_show(o);

	o = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(o), "List view");
	ewl_container_child_append(EWL_CONTAINER(menu), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
				ewl_filedialog_cb_list_view, fd);
	ewl_widget_show(o);

	o = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(o), "Column view");
	ewl_container_child_append(EWL_CONTAINER(menu), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
				ewl_filedialog_cb_column_view, fd);
	ewl_widget_show(o);

	o = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(o), "Show Dot Files");
	ewl_container_child_append(EWL_CONTAINER(fd->menu), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
				ewl_filedialog_cb_show_dot, fd);
	ewl_widget_show(o);

	o = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(o), "Show Favorites");
	ewl_container_child_append(EWL_CONTAINER(fd->menu), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
				ewl_filedialog_cb_show_favorites, fd);
	ewl_widget_show(o);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fd: The filedialog to set the view on
 * @param view: The Ewl_View to set into the dialog
 * @return Returns no value.
 * @brief Set the view to be used for displaying the files in the dialog
 */
void
ewl_filedialog_list_view_set(Ewl_Filedialog *fd, Ewl_View *view)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("fd", fd, EWL_FILEDIALOG_TYPE);

	ewl_filepicker_list_view_set(EWL_FILEPICKER(fd->fp), view);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fd: The filedialog to get the view from
 * @return Returns the Ewl_View set on this file dialog
 * @brief Retrieve the file list view used in this file dialog
 */
Ewl_View *
ewl_filedialog_list_view_get(Ewl_Filedialog *fd)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, NULL);
	DCHECK_TYPE_RET("fd", fd, EWL_FILEDIALOG_TYPE, NULL);

	DRETURN_PTR(ewl_filepicker_list_view_get(EWL_FILEPICKER(fd->fp)), 
							DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog to change paths
 * @param path: the new path used for the filedialog
 * @return Returns no value.
 * @brief Changes the current path of a filedialog.
 */
void
ewl_filedialog_directory_set(Ewl_Filedialog *fd, const char *path)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);
	DCHECK_PARAM_PTR("path", path);
	DCHECK_TYPE("fd", fd, EWL_FILEDIALOG_TYPE);

	ewl_filepicker_directory_set(EWL_FILEPICKER(fd->fp), path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @return Returns the current path of filedialog
 * @brief Retrieve the current filedialog path
 */
const char *
ewl_filedialog_directory_get(Ewl_Filedialog *fd)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, NULL);
	DCHECK_TYPE_RET("fd", fd, EWL_FILEDIALOG_TYPE, NULL);

	DRETURN_PTR(ewl_filepicker_directory_get(EWL_FILEPICKER(fd->fp)),
							DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @param val: 1 to set multiselect, 0 otherwise
 * @return Returns no value.
 * @brief Sets the dialog to multiselect or single select
 */
void
ewl_filedialog_multiselect_set(Ewl_Filedialog *fd, unsigned int val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);
	DCHECK_TYPE("fd", fd, EWL_FILEDIALOG_TYPE);

	ewl_filepicker_multiselect_set(EWL_FILEPICKER(fd->fp), val);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @return Returns the multi select setting (0|1)
 * @brief gets the multiselect setting of the filedialog
 */
unsigned int
ewl_filedialog_multiselect_get(Ewl_Filedialog *fd)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, 0);
	DCHECK_TYPE_RET("fd", fd, EWL_FILEDIALOG_TYPE, 0);

	DRETURN_INT(ewl_filepicker_multiselect_get(EWL_FILEPICKER(fd->fp)), 
							DLEVEL_STABLE);	
}

/** 
 * @param fd: The filedialog to change
 * @param dot: The value to set for show dot files
 * @return Returns no value.
 * @brief Set if the file dialog should show dot files by default 
 */
void
ewl_filedialog_show_dot_files_set(Ewl_Filedialog *fd, unsigned int dot)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);
	DCHECK_TYPE("fd", fd, EWL_FILEDIALOG_TYPE);

	ewl_filepicker_show_dot_files_set(EWL_FILEPICKER(fd->fp), dot);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fd: The filedialog to get the value from
 * @return Returns the show dot file setting of the dialog
 * @brief Returns the current dot file setting of the dialog
 */
unsigned int
ewl_filedialog_show_dot_files_get(Ewl_Filedialog *fd)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, 0);
	DCHECK_TYPE_RET("fd", fd, EWL_FILEDIALOG_TYPE, 0);

	DRETURN_INT(ewl_filepicker_show_dot_files_get(EWL_FILEPICKER(fd->fp)),
								DLEVEL_STABLE);
}

/** 
 * @param fd: The filedialog to set the value into
 * @param show: The setting for show favorites to set
 * @return Returns no value.
 * @brief Specify if the favorites column should be shown or not
 */
void
ewl_filedialog_show_favorites_set(Ewl_Filedialog *fd, unsigned int show)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);
	DCHECK_TYPE("fd", fd, EWL_FILEDIALOG_TYPE);

	ewl_filepicker_show_favorites_set(EWL_FILEPICKER(fd->fp), show);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fd: The filedialog to work with
 * @return Returns the current show favorites setting for the dialog
 * @brief Get the current show favorites setting for the filedialog
 */
unsigned int
ewl_filedialog_show_favorites_get(Ewl_Filedialog *fd)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, 0);
	DCHECK_TYPE_RET("fd", fd, EWL_FILEDIALOG_TYPE, 0);

	DRETURN_INT(ewl_filepicker_show_favorites_get(EWL_FILEPICKER(fd->fp)),
								DLEVEL_STABLE);
}

/**
 * @param fd: The filedialog to work with
 * @param file: The selected file to set
 * @return Returns no value
 * @brief Set the currently selected file into the file dialog
 */
void
ewl_filedialog_selected_file_set(Ewl_Filedialog *fd, const char *file)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);
	DCHECK_TYPE("fd", fd, EWL_FILEDIALOG_TYPE);

	ewl_filepicker_selected_file_set(EWL_FILEPICKER(fd->fp), file);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @return Returns the selected filename including its path
 * @brief Retrieve the selected filename
 */
char *
ewl_filedialog_selected_file_get(Ewl_Filedialog *fd)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, NULL);
	DCHECK_TYPE_RET("fd", fd, EWL_FILEDIALOG_TYPE, NULL);
	
	DRETURN_PTR(ewl_filepicker_selected_file_get(EWL_FILEPICKER(fd->fp)), 
							DLEVEL_STABLE);
}

/**
 * @param fd: The filedialog to work with
 * @param files: The list of filenames to set selected in the dialog
 * @return Returns no value
 * @brief Sets the given files as selected in the filedialog
 */
void
ewl_filedialog_selected_files_set(Ewl_Filedialog *fd, Ecore_List *files)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);
	DCHECK_TYPE("fd", fd, EWL_FILEDIALOG_TYPE);

	ewl_filepicker_selected_files_set(EWL_FILEPICKER(fd->fp), files);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fd: The filedialog
 * @return Returns an Ecore_List of selected items
 * @brief returns all the elements selected by the user
 */
Ecore_List *
ewl_filedialog_selected_files_get(Ewl_Filedialog *fd)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, NULL);
	DCHECK_TYPE_RET("fd", fd, EWL_FILEDIALOG_TYPE, NULL);

	DRETURN_PTR(ewl_filepicker_selected_files_get(EWL_FILEPICKER(fd->fp)), 
								DLEVEL_STABLE);
}

/**
 * @param fd: The filedialog to work with
 * @param name: The name to display for the filter
 * @param filter: The actual regular expression for the filter
 * @return Returns no value
 * @brief Add the filter named @a name to the combo box in the filedialog.
 */
void
ewl_filedialog_filter_add(Ewl_Filedialog *fd, const char *name,
						const char *filter)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);
	DCHECK_PARAM_PTR("name", name);
	DCHECK_PARAM_PTR("filter", filter);
	DCHECK_TYPE("fd", fd, EWL_FILEDIALOG_TYPE);

	ewl_filepicker_filter_add(EWL_FILEPICKER(fd->fp), name, filter);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Internally used callback, override at your own risk.
 */

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief Callback to handle deleting of the filedialog window
 */
void
ewl_filedialog_cb_delete_window(Ewl_Widget *w, void *ev_data __UNUSED__, 
							void *data __UNUSED__)
{
	Ewl_Filedialog *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fd = EWL_FILEDIALOG(w);
	ewl_filedialog_respond(fd, EWL_STOCK_CANCEL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filedialog_respond(Ewl_Filedialog *fd, unsigned int response)
{
        Ewl_Dialog_Event ev;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev.response = response;
        ewl_callback_call_with_event_data(EWL_WIDGET(fd),
                                          EWL_CALLBACK_VALUE_CHANGED, &ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filedialog_cb_value_changed(Ewl_Widget *w __UNUSED__, void *ev, 
							void *data)
{
	Ewl_Filedialog *fd;
	Ewl_Filepicker_Event *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fd = data;
	e = ev;

	ewl_filedialog_respond(fd, e->response);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filedialog_cb_mouse_down(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
	Ewl_Event_Mouse_Down *event;
	Ewl_Filedialog *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);

	event = ev;
	fd = EWL_FILEDIALOG(w);

	if (event->button == 3)
	{
		Ewl_Menu *menu;
		Ewl_Embed *emb;
		int x, y;

		emb = ewl_embed_widget_find(w);
		ewl_embed_window_position_get(emb, &x, &y);

		menu = EWL_MENU(fd->menu);
		ewl_window_move(EWL_WINDOW(menu->base.popup), 
					x + event->x, y + event->y);
		ewl_widget_show(fd->menu);

		ewl_callback_call(EWL_WIDGET(fd->menu),
					EWL_CALLBACK_FOCUS_IN);
		ewl_object_state_remove(EWL_OBJECT(fd->menu),
						EWL_FLAG_STATE_PRESSED);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filedialog_cb_column_view(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
								void *data)
{
	Ewl_Filedialog *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	fd = data;
	ewl_filedialog_list_view_set(fd, ewl_filelist_column_view_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filedialog_cb_icon_view(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
								void *data)
{
	Ewl_Filedialog *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	fd = data;
	ewl_filedialog_list_view_set(fd, ewl_filelist_icon_view_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filedialog_cb_list_view(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
								void *data)
{
	Ewl_Filedialog *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	fd = data;
	ewl_filedialog_list_view_set(fd, ewl_filelist_list_view_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filedialog_cb_show_dot(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
								void *data)
{
	Ewl_Filedialog *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	fd = data;
	ewl_filedialog_show_dot_files_set(fd,
			!ewl_filedialog_show_dot_files_get(fd));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filedialog_cb_show_favorites(Ewl_Widget *w __UNUSED__, 
					void *ev __UNUSED__, void *data)
{
	Ewl_Filedialog *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	fd = data;
	ewl_filedialog_show_favorites_set(fd,
			!ewl_filedialog_show_favorites_get(fd));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


