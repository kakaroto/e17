#ifndef EWL_FILELIST_H
#define EWL_FILELIST_H

/**
 * @addtogroup Ewl_Filelist Ewl_Filelist: The base widget for the filelists
 * A base class to be extended to provide different views on the file list
 *
 * @{
 */

/**
 * @def EWL_FILELIST_TYPE
 * The type name for the Ewl_Filelist widget
 */
#define EWL_FILELIST_TYPE "filelist"

/**
 * The Ewl_Filelist provides a base class for the file lists 
 */
typedef struct Ewl_Filelist Ewl_Filelist;

/**
 * @def EWL_FILELIST(fl)
 * Typecasts a pointer to an Ewl_Filelist pointer.
 */
#define EWL_FILELIST(fl) ((Ewl_Filelist *)fl)

/**
 * Base class for the file lists
 */
struct Ewl_Filelist
{
	Ewl_Box box;		/**< Inherits from Ewl_Box */

	Ecore_List *selected;	/**< The selected files */
	char *directory;	/**< The directory to display */
	char *filter;		/**< The file filter to employ */

	unsigned char multiselect:1;	/**< Allow multiple file selctions */
	unsigned char show_dot_files:1;	/**< Show . files */

	Ewl_Widget *last_selected; /**< The last selected icon */
	Ewl_Widget *base_selected; /**< First select in SHIFT select */

	void (*dir_change)(Ewl_Filelist *fl);	/**< Callback to notify of
							directory change */
	void (*filter_change)(Ewl_Filelist *fl);	/**< Callback to notify
							of filter change */
	void (*multiselect_change)(Ewl_Filelist *fl); /**< Callback to notify
							of multilselect state 
							change */
	void (*show_dot_change)(Ewl_Filelist *fl);	/**< Callback to notify
							of show dot file 
							setting change */
	void (*selected_unselect)(Ewl_Filelist *fl); /**< Callback to
							unselect all files */
	void (*selected_file_add)(Ewl_Filelist *fl, const char *file); /**< 
							Callback to 
							notify of a change
							to the selected
							files */
	const char *(*file_name_get)(Ewl_Filelist *fl, void *file); /**< 
							Callback to get the 
							selected filename */
};

int		 ewl_filelist_init(Ewl_Filelist *fl);

void		 ewl_filelist_directory_set(Ewl_Filelist *fl,
							const char *dir);
const char	*ewl_filelist_directory_get(Ewl_Filelist *fl);

void		 ewl_filelist_filter_set(Ewl_Filelist *fl,
							const char *filter);
const char	*ewl_filelist_filter_get(Ewl_Filelist *fl);

void		 ewl_filelist_multiselect_set(Ewl_Filelist *fl,
							unsigned int ms);
unsigned int	 ewl_filelist_multiselect_get(Ewl_Filelist *fl);

void	 	 ewl_filelist_show_dot_files_set(Ewl_Filelist *fl,
							unsigned int dot);
unsigned int	 ewl_filelist_show_dot_files_get(Ewl_Filelist *fl);

void		 ewl_filelist_selected_file_set(Ewl_Filelist *fl,
 							const char *file);
char	 	*ewl_filelist_selected_file_get(Ewl_Filelist *fl);

void		 ewl_filelist_selected_files_set(Ewl_Filelist *fl,
                                                        Ecore_List *files);
Ecore_List	*ewl_filelist_selected_files_get(Ewl_Filelist *fl);
void 		 ewl_filelist_selected_files_change_notify(Ewl_Filelist *fl);

void		 ewl_filelist_selected_signal_all(Ewl_Filelist *fl, 
						const char *signal);

char 		*ewl_filelist_expand_path(Ewl_Filelist *fl, const char *dir);
void 		 ewl_filelist_directory_read(Ewl_Filelist *fl, 
					void (*func)(Ewl_Filelist *fl, 
							const char *dir, 
							char *file));
void 		 ewl_filelist_handle_click(Ewl_Filelist *fl, Ewl_Widget *w,
						Ewl_Event_Mouse_Up *ev,
						const char *select_state, 
						const char *unselect_state);

/*
 * Internally used functions, override at your own risk
 */
void ewl_filelist_cb_destroy(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

