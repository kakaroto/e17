#include <Ewl.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include "ewl_fileselector.h"
#include "ewl_filedialog.h"


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
	if (!fd)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_filedialog_init(fd, type);

	DRETURN_PTR(EWL_WIDGET(fd), DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @param type: the filedialog type
 * @return Returns no value.
 * @brief Initialize a new filedialog
 */
void ewl_filedialog_init(Ewl_Filedialog * fd, Ewl_Filedialog_Type type)
{
	Ewl_Widget *w;
	Ewl_Widget *button;
	Ewl_Widget *box;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);

	w = EWL_WIDGET(fd);

 	ewl_box_init(EWL_BOX(fd), EWL_ORIENTATION_VERTICAL);
	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_ALL);
	ewl_widget_appearance_set(w, "filedialog");

	/* the file selector */

	fd->fs = ewl_fileselector_new();
	if (fd->fs) {
		char *tmp = getenv("HOME");
		ewl_fileselector_configure_cb(EWL_FILESELECTOR(fd->fs), (tmp ? tmp : "/"));
		ewl_container_child_append(EWL_CONTAINER(fd), fd->fs);
		ewl_widget_show(fd->fs);
	}

	box = ewl_hbox_new();
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

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog
 * @return Returns the current path of filedialog
 * @brief Retrieve the current filedialog path
 */
char *ewl_filedialog_path_get(Ewl_Filedialog * fd)
{
	return ewl_fileselector_path_get(EWL_FILESELECTOR(fd->fs));
}

/**
 * @param fd: the filedialog
 * @return Returns the selected filename including its path
 * @brief Retrieve the selected filename
 */
char *ewl_filedialog_file_get(Ewl_Filedialog * fd)
{
	return ewl_fileselector_file_get(EWL_FILESELECTOR(fd->fs));
}

/**
 * @param fd: the filedialog to change paths
 * @param path: the new path used for the filedialog
 * @return Returns no value.
 * @brief Changes the current path of a filedialog.
 */
void ewl_filedialog_path_set(Ewl_Filedialog * fd, char *path)
{
	ewl_fileselector_path_set(EWL_FILESELECTOR(fd->fs), path);
}

/*
 * Internally used callback, override at your own risk.
 */
void ewl_filedialog_click_cb(Ewl_Widget * w, void *ev_data, void* data)
{
	Ewl_Filedialog *fd = EWL_FILEDIALOG(data);

	ewl_callback_call_with_event_data(EWL_WIDGET(fd),
					  EWL_CALLBACK_VALUE_CHANGED,
					  &EWL_BUTTON_STOCK(w)->response_id);
}


