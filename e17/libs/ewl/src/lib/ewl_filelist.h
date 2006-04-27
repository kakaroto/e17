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

	struct
	{
		Ewl_Widget *base; /**< First select in SHIFT select */
		Ewl_Widget *last; /**< Last selected in SHIFT select */
	} select;		 /**< Data used in SHIFT select */

	struct
	{	Ewl_ScrollPane_Flags h; /**< Horizontal scroll flag */
		Ewl_ScrollPane_Flags v; /**< Vertical scroll flag */
	} scroll_flags;		/**< Flags to modify a containing scrollpane */

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
	void (*shift_handle)(Ewl_Filelist *fl, Ewl_Widget *clicked); /**<
							Callback to handle
							SHIFT clicks */
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

void		 ewl_filelist_vscroll_flag_set(Ewl_Filelist *fl,
						Ewl_ScrollPane_Flags v);
Ewl_ScrollPane_Flags ewl_filelist_vscroll_flag_get(Ewl_Filelist *fl);

void		 ewl_filelist_hscroll_flag_set(Ewl_Filelist *fl,
						Ewl_ScrollPane_Flags h);
Ewl_ScrollPane_Flags ewl_filelist_hscroll_flag_get(Ewl_Filelist *fl);

char 		*ewl_filelist_expand_path(Ewl_Filelist *fl, const char *dir);
void 		 ewl_filelist_directory_read(Ewl_Filelist *fl, 
					const char *dir,
					unsigned int skip_dot_dot,
					void (*func)(Ewl_Filelist *fl, 
						const char *dir, 
						char *file, void *data),
					void *data);
void 		 ewl_filelist_handle_click(Ewl_Filelist *fl, Ewl_Widget *w,
						Ewl_Event_Mouse_Up *ev,
						const char *select_state, 
						const char *unselect_state);
void 		 ewl_filelist_container_shift_handle(Ewl_Filelist *fl, 
					Ewl_Container *c, Ewl_Widget *clicked,
					const char *select_signal, 
					const char *unselect_signal);

/*
 * Internally used functions, override at your own risk
 */
void ewl_filelist_cb_destroy(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

