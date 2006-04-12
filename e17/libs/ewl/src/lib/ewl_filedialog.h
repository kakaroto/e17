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
	Ewl_Dialog dialog;	/**< The dialog base class */
	Ewl_Widget *fp;	   	/**< Ewl_Filepicker */
	Ewl_Widget *menu_float;	/**< Floater to hold the menu */
	Ewl_Widget *menu;	/**< The menu */
};

Ewl_Widget 	*ewl_filedialog_multiselect_new(void);
Ewl_Widget 	*ewl_filedialog_new(void);
int		 ewl_filedialog_init(Ewl_Filedialog *fd);

void		 ewl_filedialog_list_view_set(Ewl_Filedialog *fd, 
						Ewl_View *view);
Ewl_View	*ewl_filedialog_list_view_get(Ewl_Filedialog *fd);

void		 ewl_filedialog_directory_set(Ewl_Filedialog *fd,
						const char *dir);
const char	*ewl_filedialog_directory_get(Ewl_Filedialog *fd);

void		 ewl_filedialog_multiselect_set(Ewl_Filedialog *fd,
						unsigned int ms);
unsigned int	 ewl_filedialog_multiselect_get(Ewl_Filedialog *fd);

void		 ewl_filedialog_show_dot_files_set(Ewl_Filedialog *fd,
						unsigned int dot);
unsigned int	 ewl_filedialog_show_dot_files_get(Ewl_Filedialog *fd);

void		 ewl_filedialog_show_favorites_set(Ewl_Filedialog *fd,
						unsigned int show);
unsigned int	 ewl_filedialog_show_favorites_get(Ewl_Filedialog *fd);

void		 ewl_filedialog_selected_file_set(Ewl_Filedialog *fd,
						const char *file);
char		*ewl_filedialog_selected_file_get(Ewl_Filedialog *fd);

void		 ewl_filedialog_selected_files_set(Ewl_Filedialog *fd,
						Ecore_List *files);
Ecore_List	*ewl_filedialog_selected_files_get(Ewl_Filedialog *fd);

void		 ewl_filedialog_filter_add(Ewl_Filedialog *fd,
						const char *name,
						const char *filter);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_filedialog_cb_delete_window(Ewl_Widget *w, void *ev_data, void *data);

/**
 * @}
 */

#endif
