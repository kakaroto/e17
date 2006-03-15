#ifndef EWL_FILEDIALOG_H
#define EWL_FILEDIALOG_H

/**
 * @addtogroup Ewl_Filedialog Ewl_Filedialog: A Dialog For Picking Files
 * A simple dialog to show a file selector widget
 *
 * @{
 */

/**
 * @themekey /filedialog/file
 * @themekey /filedialog/group
 */

/**
 * @def EWL_FILEDIALOG_TYPE
 * The type name for the Ewl_Filedialog widget
 */
#define EWL_FILEDIALOG_TYPE "filedialog"

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
 * Creates a dialog for the fileselector
 */
struct Ewl_Filedialog
{
	Ewl_Dialog dialog;        /**< The dialog base class */

	Ewl_Filedialog_Type type; /**< Current type of filedialog */
	Ewl_Widget *fs;           /**< Ewl_Fileselector */
	Ewl_Widget *type_btn;     /**< Either the open or save button */
};

Ewl_Widget 		*ewl_filedialog_multiselect_new(void);
Ewl_Widget 		*ewl_filedialog_new(void);

Ewl_Filedialog_Type	 ewl_filedialog_type_get(Ewl_Filedialog *fd);
void			 ewl_filedialog_type_set(Ewl_Filedialog *fd, 
						Ewl_Filedialog_Type type);
int			 ewl_filedialog_init(Ewl_Filedialog *fd);
char			*ewl_filedialog_path_get(Ewl_Filedialog *fd);
char			*ewl_filedialog_file_get(Ewl_Filedialog *fd);
void			 ewl_filedialog_path_set(Ewl_Filedialog *fd, char *path);

void			 ewl_filedialog_multiselect_set(Ewl_Filedialog *fd, 
							unsigned int val);
unsigned int		 ewl_filedialog_multiselect_get(Ewl_Filedialog *fd);

Ecore_List 		*ewl_filedialog_select_list_get(Ewl_Filedialog *fd);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_filedialog_click_cb (Ewl_Widget *w, void *ev_data, void *data);
void ewl_filedialog_delete_window_cb(Ewl_Widget *w, void *ev_data, void *data);

/**
 * @}
 */

#endif
