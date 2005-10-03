#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns a new open filedialog if successful, NULL on failure.
 * @brief Create a new open filedialog
 */
Ewl_Widget *ewl_filedialog_multiselect_new(void)
{
	Ewl_Widget *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fd = ewl_filedialog_new(EWL_FILEDIALOG_TYPE_OPEN);
	if (!fd) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}
	ewl_filedialog_multiselect_set(EWL_FILEDIALOG(fd), TRUE);

	DRETURN_PTR(EWL_WIDGET(fd), DLEVEL_STABLE);
}

/**
 * @param type: type of dialog to display
 * @return Returns a new filedialog in success, NULL on failure.
 * @brief Create a new filedialog
 */
Ewl_Widget *ewl_filedialog_new(Ewl_Filedialog_Type type)
{
	Ewl_Filedialog *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fd = NEW(Ewl_Filedialog, 1);
	if (!fd) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_filedialog_init(fd, type)) {
		ewl_widget_destroy(EWL_WIDGET(fd));
		fd = NULL;
	}
	DRETURN_PTR(EWL_WIDGET(fd), DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @param type: the filedialog type
 * @return Returns no value.
 * @brief Initialize a new filedialog
 */
int ewl_filedialog_init(Ewl_Filedialog * fd, Ewl_Filedialog_Type type)
{
	Ewl_Widget *w;
	Ewl_Widget *button;
	Ewl_Widget *box;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, FALSE);

	w = EWL_WIDGET(fd);

 	if (!ewl_box_init(EWL_BOX(fd)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_box_orientation_set(EWL_BOX(fd), EWL_ORIENTATION_VERTICAL);
	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_ALL);
	ewl_widget_appearance_set(w, "filedialog");
	ewl_widget_inherit(w, "filedialog");

	/* the file selector */
	fd->fs = ewl_fileselector_new();
	if (!fd->fs) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	ewl_container_child_append(EWL_CONTAINER(fd), fd->fs);
	ewl_widget_show(fd->fs);

	box = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_HFILL |
			EWL_FLAG_FILL_HSHRINK);
	ewl_object_alignment_set(EWL_OBJECT(box), EWL_FLAG_ALIGN_RIGHT);
	ewl_container_child_append(EWL_CONTAINER(fd), box);
	ewl_widget_show(box);

	/* Buttons */
	if (type == EWL_FILEDIALOG_TYPE_OPEN) 
		button = ewl_button_stock_with_id_new(EWL_STOCK_OPEN,
						EWL_RESPONSE_OPEN);
	else
		button = ewl_button_stock_with_id_new(EWL_STOCK_SAVE,
						EWL_RESPONSE_SAVE);
                
	ewl_callback_append(button, EWL_CALLBACK_CLICKED,
						ewl_filedialog_click_cb, fd);
	ewl_container_child_append(EWL_CONTAINER(box), button);
	ewl_widget_show(button);

	button = ewl_button_stock_with_id_new(EWL_STOCK_CANCEL,
					 EWL_RESPONSE_CANCEL);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED,
						ewl_filedialog_click_cb, fd);
	ewl_container_child_append(EWL_CONTAINER(box), button);
	ewl_widget_show(button);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @return Returns the current path of filedialog
 * @brief Retrieve the current filedialog path
 */
char *ewl_filedialog_path_get(Ewl_Filedialog * fd)
{
	char *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, NULL);

	s = ewl_fileselector_path_get(EWL_FILESELECTOR(fd->fs)); 

	DRETURN_PTR(s, DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @return Returns the selected filename including its path
 * @brief Retrieve the selected filename
 */
char *ewl_filedialog_file_get(Ewl_Filedialog * fd)
{
	char *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, NULL);
	
	s = ewl_fileselector_file_get(EWL_FILESELECTOR(fd->fs));

	DRETURN_PTR(s, DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog to change paths
 * @param path: the new path used for the filedialog
 * @return Returns no value.
 * @brief Changes the current path of a filedialog.
 */
void ewl_filedialog_path_set(Ewl_Filedialog * fd, char *path)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);
	DCHECK_PARAM_PTR("path", path);

	ewl_fileselector_path_set(EWL_FILESELECTOR(fd->fs), path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @param val: 1 to set multiselect, 0 otherwise
 * @return Returns no value.
 * @brief Sets the dialog to multiselect or single select
 */
void ewl_filedialog_multiselect_set(Ewl_Filedialog *fd, unsigned int val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);

	ewl_fileselector_multiselect_set(EWL_FILESELECTOR(fd->fs), val);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @return Returns the multi select setting (0|1)
 * @brief gets the multiselect setting of the filedialog
 */
unsigned int ewl_filedialog_multiselect_get(Ewl_Filedialog *fd)
{
	unsigned int val;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, 0);

	val = ewl_fileselector_multiselect_get(EWL_FILESELECTOR(fd->fs));

	DRETURN_INT(val, DLEVEL_STABLE);	
}

/**
 * @param fd: The filedialog
 * @return Returns an Ecore_List of selected items
 * @brief returns all the elements selected by the user
 */
Ecore_List *ewl_filedialog_select_list_get(Ewl_Filedialog *fd)
{
	Ecore_List *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fd", fd, NULL);

	list = ewl_fileselector_select_list_get(EWL_FILESELECTOR(fd->fs));

	DRETURN_PTR(list, DLEVEL_STABLE);
}

/*
 * Internally used callback, override at your own risk.
 */
void ewl_filedialog_click_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
								void* data)
{
	Ewl_Filedialog *fd = EWL_FILEDIALOG(data);

	ewl_callback_call_with_event_data(EWL_WIDGET(fd),
					  EWL_CALLBACK_VALUE_CHANGED,
					  &EWL_BUTTON_STOCK(w)->response_id);
}


