#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_filedialog_respond(Ewl_Filedialog *fd, unsigned int response);

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
	if (!fd) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}
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
	if (!fd) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_filedialog_init(fd)) {
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
	Ewl_Widget *w, *o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, FALSE);

	w = EWL_WIDGET(fd);
 	if (!ewl_dialog_init(EWL_DIALOG(fd)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(w, EWL_FILEDIALOG_TYPE);

	ewl_window_title_set(EWL_WINDOW(fd), "Ewl Filedialog");
	ewl_window_name_set(EWL_WINDOW(fd), "Ewl Filedialog");
	ewl_window_class_set(EWL_WINDOW(fd), "Ewl Filedialog");

	ewl_dialog_action_position_set(EWL_DIALOG(fd), EWL_POSITION_BOTTOM);

	ewl_callback_append(EWL_WIDGET(fd), EWL_CALLBACK_DELETE_WINDOW,
				ewl_filedialog_delete_window_cb, NULL);

	ewl_dialog_active_area_set(EWL_DIALOG(fd), EWL_POSITION_TOP);

	/* the file selector */
	fd->fs = ewl_fileselector_new();
	ewl_widget_internal_set(fd->fs, TRUE);
	ewl_container_child_append(EWL_CONTAINER(fd), fd->fs);
	ewl_widget_show(fd->fs);

	ewl_dialog_active_area_set(EWL_DIALOG(fd), EWL_POSITION_BOTTOM);

	/* Buttons */
	o = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(fd), o);
	ewl_button_stock_type_set(EWL_BUTTON(o), EWL_STOCK_OPEN);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
					ewl_filedialog_click_cb, fd);
	ewl_widget_show(o);
	fd->type_btn = o;

	o = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(fd), o);
	ewl_button_stock_type_set(EWL_BUTTON(o), EWL_STOCK_CANCEL);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
					ewl_filedialog_click_cb, fd);
	ewl_widget_show(o);

	/* set the top active so the tooltips go to the right spot */
	ewl_dialog_active_area_set(EWL_DIALOG(fd), EWL_POSITION_TOP);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog to get the current type
 * @return Returns the current file dialog type.
 * @brief Retrieve the current filedialog type.
 */
Ewl_Filedialog_Type
ewl_filedialog_type_get(Ewl_Filedialog *fd)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, EWL_FILEDIALOG_TYPE_OPEN);
	DCHECK_TYPE_RET("fd", fd, EWL_FILEDIALOG_TYPE, EWL_FILEDIALOG_TYPE_OPEN);

	DRETURN_INT(fd->type, DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog to change types
 * @param t: The type to set the filedialog too
 * @return Returns no value.
 * @brief Change the current filedialog type.
 */
void
ewl_filedialog_type_set(Ewl_Filedialog *fd, Ewl_Filedialog_Type t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);
	DCHECK_TYPE("fd", fd, EWL_FILEDIALOG_TYPE);

	if (t == EWL_FILEDIALOG_TYPE_OPEN) 
		ewl_button_stock_type_set(EWL_BUTTON(fd->type_btn), EWL_STOCK_OPEN);

	else if (t == EWL_FILEDIALOG_TYPE_SAVE) 
		ewl_button_stock_type_set(EWL_BUTTON(fd->type_btn), EWL_STOCK_SAVE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @return Returns the current path of filedialog
 * @brief Retrieve the current filedialog path
 */
char *
ewl_filedialog_path_get(Ewl_Filedialog *fd)
{
	char *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, NULL);
	DCHECK_TYPE("fd", fd, EWL_FILEDIALOG_TYPE);

	s = ewl_fileselector_path_get(EWL_FILESELECTOR(fd->fs)); 

	DRETURN_PTR(s, DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @return Returns the selected filename including its path
 * @brief Retrieve the selected filename
 */
char *
ewl_filedialog_file_get(Ewl_Filedialog *fd)
{
	char *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, NULL);
	DCHECK_TYPE_RET("fd", fd, EWL_FILEDIALOG_TYPE, NULL);
	
	s = ewl_fileselector_file_get(EWL_FILESELECTOR(fd->fs));

	DRETURN_PTR(s, DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog to change paths
 * @param path: the new path used for the filedialog
 * @return Returns no value.
 * @brief Changes the current path of a filedialog.
 */
void
ewl_filedialog_path_set(Ewl_Filedialog *fd, char *path)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);
	DCHECK_PARAM_PTR("path", path);
	DCHECK_TYPE("fd", fd, EWL_FILEDIALOG_TYPE);

	ewl_fileselector_path_set(EWL_FILESELECTOR(fd->fs), path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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

	ewl_fileselector_multiselect_set(EWL_FILESELECTOR(fd->fs), val);

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
	unsigned int val;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, 0);
	DCHECK_TYPE_RET("fd", fd, EWL_FILEDIALOG_TYPE, 0);

	val = ewl_fileselector_multiselect_get(EWL_FILESELECTOR(fd->fs));

	DRETURN_INT(val, DLEVEL_STABLE);	
}

/**
 * @param fd: The filedialog
 * @return Returns an Ecore_List of selected items
 * @brief returns all the elements selected by the user
 */
Ecore_List *
ewl_filedialog_select_list_get(Ewl_Filedialog *fd)
{
	Ecore_List *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, NULL);
	DCHECK_TYPE_RET("fd", fd, EWL_FILEDIALOG_TYPE, NULL);

	list = ewl_fileselector_select_list_get(EWL_FILESELECTOR(fd->fs));

	DRETURN_PTR(list, DLEVEL_STABLE);
}

/*
 * Internally used callback, override at your own risk.
 */
void
ewl_filedialog_click_cb(Ewl_Widget *w, void *ev_data __UNUSED__, void *data)
{
	Ewl_Filedialog *fd;
	unsigned int resp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fd = EWL_FILEDIALOG(data);
	resp = ewl_button_stock_type_get(EWL_BUTTON(w));
	ewl_filedialog_respond(fd, resp);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_filedialog_delete_window_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
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

