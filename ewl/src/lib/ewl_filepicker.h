/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_FILEPICKER_H
#define EWL_FILEPICKER_H

#include "ewl_box.h"
#include "ewl_view.h"
#include "ewl_model.h"
#include "ewl_filelist.h"

/**
 * @addtogroup Ewl_Filepicker Ewl_Filepicker: Allows selecting files from a list
 * A widget to allow selecting ifles
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_Filepicker_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_FILEPICKER_TYPE
 * The type name for the Ewl_Filepicker widget
 */
#define EWL_FILEPICKER_TYPE "filepicker"

/**
 * @def EWL_FILEPICKER_IS(w)
 * Returns TRUE if the widget is an Ewl_Filepicker, FALSE otherwise
 */
#define EWL_FILEPICKER_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_FILEPICKER_TYPE))

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
 * @brief Inherits from Ewl_Box and extends to provide a filepicker.
 */
struct Ewl_Filepicker
{
        Ewl_Box box;                    /**< Inherit from Ewl_Box */

        Ewl_Widget *file_list;          /**< The file list widet */

        Ewl_Widget *type_combo;         /**< File type combo */
        Ewl_Widget *file_entry;         /**< Filename entry box */
        Ewl_Widget *favorites_box;      /**< Favoirte directory box */
        Ewl_Widget *path_combo;         /**< Path combo box */
        Ewl_Widget *dir_button;         /**< Button to create a directory */
        Ewl_Widget *ret_button;         /**< The save/open button */

        Ecore_List *path;               /**< The path components */
        Ecore_List *filters;            /**< The type filters */

        struct
        {
                Ewl_Model *model;
                Ewl_View *view;
                Ewl_Widget *combo;
        } mvc_filters, mvc_path;

        unsigned char show_favorites:1; /**< Show the favorite box */
        unsigned char saveas_dialog:1;  /**< Open or save dialog */
        unsigned char ret_dir:1;        /**< Allow directories to be returned */

};

Ewl_Widget              *ewl_filepicker_new(void);
int                      ewl_filepicker_init(Ewl_Filepicker *fp);

void                     ewl_filepicker_directory_set(Ewl_Filepicker *fp,
                                                const char *dir);
const char              *ewl_filepicker_directory_get(Ewl_Filepicker *fp);

void                     ewl_filepicker_filter_set(Ewl_Filepicker *fp,
                                                Ewl_Filelist_Filter *filter);
Ewl_Filelist_Filter     *ewl_filepicker_filter_get(Ewl_Filepicker *fp);

void                     ewl_filepicker_multiselect_set(Ewl_Filepicker *fp,
                                                unsigned int ms);
unsigned int             ewl_filepicker_multiselect_get(Ewl_Filepicker *fp);

void                     ewl_filepicker_show_dot_files_set(Ewl_Filepicker *fp,
                                                unsigned int dot);
unsigned int             ewl_filepicker_show_dot_files_get(Ewl_Filepicker *fp);

void                     ewl_filepicker_selected_file_set(Ewl_Filepicker *fp,
                                                const char *file);
char                    *ewl_filepicker_selected_file_get(Ewl_Filepicker *fp);

void                     ewl_filepicker_selected_files_set(Ewl_Filepicker *fp,
                                                Ecore_List *files);
Ecore_List              *ewl_filepicker_selected_files_get(Ewl_Filepicker *fp);

void                     ewl_filepicker_show_favorites_set(Ewl_Filepicker *fp,
                                                unsigned int show);
unsigned int             ewl_filepicker_show_favorites_get(Ewl_Filepicker *fp);

void                     ewl_filepicker_list_view_set(Ewl_Filepicker *fp,
                                                Ewl_Filelist_View view);
Ewl_Filelist_View       *ewl_filepicker_list_view_get(Ewl_Filepicker *fp);

Ewl_Filelist_Filter     *ewl_filepicker_filter_add(Ewl_Filepicker *fp,
                                                const char *name,
                                                const char *extension,
                                                Ecore_List *mime_types);
void                     ewl_filepicker_save_as_set(Ewl_Filepicker *fp,
                                                unsigned int t);
unsigned int             ewl_filepicker_save_as_get(Ewl_Filepicker *fp);
void                     ewl_filepicker_return_directories_set(Ewl_Filepicker *fp,
                                                unsigned int t);
unsigned int             ewl_filepicker_return_directories_get(Ewl_Filepicker *fp);
Ewl_Widget              *ewl_filepicker_save_as_new(void);
/**
 * @}
 */

#endif

