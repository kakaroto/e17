#ifndef EWL_FILELIST_LIST_H
#define EWL_FILELIST_LIST_H

/**
 * @addtogroup Ewl_Filelist_List Ewl_Filelist_List: A listview of the files
 * Provides a list view of the files giving name, size, etc information
 *
 * @{
 */

/**
 * @def EWL_FILELIST_LIST_TYPE
 * The type name for the Ewl_Filelist_List widget
 */
#define EWL_FILELIST_LIST_TYPE "filelist_list"

/**
 * The Ewl_Filelist_List widget provides a list view of the files
 */
typedef struct Ewl_Filelist_List Ewl_Filelist_List;

/**
 * @def EWL_FILELIST_LIST(fl)
 * Typecasts a pointer to an Ewl_Filelist_List pointer.
 */
#define EWL_FILELIST_LIST(fl) ((Ewl_Filelist_List *)fl)

/**
 * Creats a list view on the files
 */
struct Ewl_Filelist_List
{
	Ewl_Filelist list;	/**< Inherit from filelist */
	Ewl_Widget *tree; 	/**< Tree to hold icons */
};

Ewl_View	*ewl_filelist_list_view_get(void);

Ewl_Widget	*ewl_filelist_list_new(void);
int		 ewl_filelist_list_init(Ewl_Filelist_List *fl);

void		 ewl_filelist_list_dir_change(Ewl_Filelist *fl);
void 		 ewl_filelist_list_selected_files_change(Ewl_Filelist *fl);

#endif

