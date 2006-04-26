#ifndef EWL_FILELIST_COLUMN_H
#define EWL_FILELIST_COLUMN_H

/**
 * @addtogroup Ewl_Filelist_Column Ewl_Filelist_Column: An expanding column view
 * A view of the files using a new column for each directory.
 *
 * @{
 */

/**
 * @def EWL_FILELIST_COLUMN_TYPE
 * The type name for the Ewl_Filelist_Column widget
 */
#define EWL_FILELIST_COLUMN_TYPE "filelist_column"

#define EWL_FILELIST_COLUMN(fl) ((Ewl_Filelist_Column *)fl)

/**
 * The Ewl_Filelist_Icon provides an icon view of the files
 */
typedef struct Ewl_Filelist_Column Ewl_Filelist_Column;

struct Ewl_Filelist_Column
{
	Ewl_Filelist list;      /**< Inherit from filelist */
	Ewl_Widget *hbox;    /**<Hbox to contain the trees**/
	Ewl_Widget *tree;    /**< Tree to hold directories */
};

Ewl_View	*ewl_filelist_column_view_get(void);

Ewl_Widget	*ewl_filelist_column_new(void);
int		 ewl_filelist_column_init(Ewl_Filelist_Column *fl);

void		 ewl_filelist_column_dir_change(Ewl_Filelist *fl);

/**
 * @}
 */

#endif

