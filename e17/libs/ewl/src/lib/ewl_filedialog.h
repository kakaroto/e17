#ifndef __EWL_FILEDIALOG_H__
#define __EWL_FILEDIALOG_H__

/**
 * @file ewl_filedialog.h
 * @defgroup Ewl_Filedialog Filedialog: A Dialog For Picking Files
 *
 * The filedialog is intended to be used for a simple file chooser. It can be
 * placed inside any other container, and provides the ability to pack extra
 * buttons or widgets along the left side. It currently supports two types, an
 * Open and a Save dialog.
 *
 * The normal use of the filedialog is to create a new one the first time an
 * event occurs that requires one. Setting a callback for
 * EWL_CALLBACK_VALUE_CHANGED, allows the programmer to determine when the
 * Open/Save buttons were chosen. If the event data on the callback is NULL,
 * Cancel was clicked, otherwise, the event data is a pointer to the chosen
 * file(s).
 *
 * @{
 */

/**
 * @themekey /filedialog/file
 * @themekey /filedialog/group
 */

/**
 * The Ewl_Filedialog provides a filedialog
 */
typedef struct Ewl_Filedialog Ewl_Filedialog;

/**
 * @def EWL_FILEDIALOG(fd)
 * Typecasts a pointer to an Ewl_Filedialog pointer.
 */
#define EWL_FILEDIALOG(fd) ((Ewl_Filedialog *) fd)

/**
 * @struct Ewl_Filedialog
 * Creates a floating widget with different filedialog components.
 */
struct Ewl_Filedialog
{
	Ewl_Box	box;              /**< Box containing the fileselector */

	Ewl_Filedialog_Type type; /**< Current type of filedialog */
	Ewl_Widget *fs;           /**< Ewl_Fileselector */
	Ewl_Widget *confirm;      /**< Confirmation OK/Save button */
	Ewl_Widget *cancel;       /**< Cancel button */
};

/**
 * The Ewl_Filedialog_Event returned as the EWL_CALLBACK_VALUE_CHANGED
 * event
 */
typedef struct Ewl_Filedialog_Event Ewl_Filedialog_Event;

/**
 * @define EWL_FILEDIALOG_EVENT(e)
 * Typecasts a pointer to an Ewl_Filedialog_Event pointer
 */
#define EWL_FILEDIALOG_EVENT(e) ((Ewl_Filedialog_Event *)e)

/**
 * @struct Ewl_Filedialog_Event
 * Stores the reponse from the filedialog
 */
struct Ewl_Filedialog_Event
{
	unsigned int response;
};

Ewl_Widget *ewl_filedialog_multiselect_new(void);
Ewl_Widget *ewl_filedialog_new(void);

Ewl_Filedialog_Type ewl_filedialog_type_get(Ewl_Filedialog *fd);
void                ewl_filedialog_type_set(Ewl_Filedialog *fd, Ewl_Filedialog_Type type);
int                 ewl_filedialog_init(Ewl_Filedialog *fd);
char               *ewl_filedialog_path_get(Ewl_Filedialog *fd);
char               *ewl_filedialog_file_get(Ewl_Filedialog *fd);
void                ewl_filedialog_path_set(Ewl_Filedialog *fd, char *path);

void        ewl_filedialog_multiselect_set(Ewl_Filedialog *fd, 
					     unsigned int val);
unsigned int ewl_filedialog_multiselect_get(Ewl_Filedialog *fd);

Ecore_List *ewl_filedialog_select_list_get(Ewl_Filedialog *fd);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_filedialog_click_cb (Ewl_Widget *w, void *ev_data, void *data);


/**
 * @}
 */

#endif /* __EWL_FS_H__ */
