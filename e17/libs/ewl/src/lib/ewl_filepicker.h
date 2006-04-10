#ifndef EWL_FILEPICKER_H
#define EWL_FILEPICKER_H

/** 
 * @addtogroup Ewl_Filepicker Ewl_Filepicker: Allows selecting files from a list
 * A widget to allow selecting ifles
 */

/**
 * @def EWL_FILEPICKER_TYPE
 * The type name for the Ewl_Filepicker widget
 */
#define EWL_FILEPICKER_TYPE "filepicker"

/**
 * The Ewl_Filepicker provides a widget for picking files
 */
typedef struct Ewl_Filepicker Ewl_Filepicker;

/**
 * @def EWL_FILEPICKER(fp)
 * Typecast a pointer to an Ewl_Filepicker pointer.
 */
#define EWL_FILEPICKER(fp) ((Ewl_Filepicker *)fp)

/**
 * Creates a widget for selecting files
 */
struct Ewl_Filepicker
{
	Ewl_Box box;			/**< Inherit from Ewl_Box */

	Ewl_Widget *file_list;		/**< The file list widet */
	Ewl_Widget *file_list_box;	/**< Box to hold the file list widget */

	Ewl_Widget *type_combo;		/**< File type combo */
	Ewl_Widget *file_entry;		/**< Filename entry box */
	Ewl_Widget *favorites_box;	/**< Favoirte directory box */
	Ewl_Widget *path_combo;		/**< Path combo box */

	Ecore_List *path;		/**< The path components */
	Ecore_List *filters;		/**< The type filters */

	unsigned char show_favorites:1;	/**< Show the favorite box */
	Ewl_View *view;			/**< The filelist view to use */
};

Ewl_Widget	*ewl_filepicker_new(void);
int		 ewl_filepicker_init(Ewl_Filepicker *fp);

void		 ewl_filepicker_directory_set(Ewl_Filepicker *fp,
							const char *dir);
const char	*ewl_filepicker_directory_get(Ewl_Filepicker *fp);

void		 ewl_filepicker_filter_set(Ewl_Filepicker *fp,
							const char *filter);
const char	*ewl_filepicker_filter_get(Ewl_Filepicker *fp);

void		 ewl_filepicker_multiselect_set(Ewl_Filepicker *fp,
							unsigned int ms);
unsigned int	 ewl_filepicker_multiselect_get(Ewl_Filepicker *fp);

void		 ewl_filepicker_show_dot_files_set(Ewl_Filepicker *fp,
							unsigned int dot);
unsigned int	 ewl_filepicker_show_dot_files_get(Ewl_Filepicker *fp);

void		 ewl_filepicker_selected_file_set(Ewl_Filepicker *fp,
							const char *file);
char		*ewl_filepicker_selected_file_get(Ewl_Filepicker *fp);

void             ewl_filepicker_selected_files_set(Ewl_Filepicker *fp,
							Ecore_List *files);
Ecore_List	*ewl_filepicker_selected_files_get(Ewl_Filepicker *fp);

void		 ewl_filepicker_show_favorites_set(Ewl_Filepicker *fp,
						unsigned int show);
unsigned int	 ewl_filepicker_show_favorites_get(Ewl_Filepicker *fp);

void		 ewl_filepicker_list_view_set(Ewl_Filepicker *fp,
							Ewl_View *view);
Ewl_View 	*ewl_filepicker_list_view_get(Ewl_Filepicker *fp);

void 		 ewl_filepicker_filter_add(Ewl_Filepicker *fp, 
						const char *name, 
						const char *filter);

#endif

