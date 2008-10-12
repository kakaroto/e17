/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_FILEDIALOG_H
#define EWL_FILEDIALOG_H

#include "ewl_dialog.h"
#include "ewl_view.h"

/**
 * @addtogroup Ewl_Filedialog Ewl_Filedialog: A Dialog For Picking Files
 * A simple dialog to show a file selector widget
 *
 * @remarks Inherits from Ewl_Dialog.
 * @if HAVE_IMAGES
 * @image html Ewl_Filedialog_inheritance.png
 * @endif
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
 * @def EWL_FILEDIALOG_IS(w)
 * Returns TRUE if the widget is an Ewl_Filedialog, FALSE otherwise
 */
#define EWL_FILEDIALOG_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_FILEDIALOG_TYPE))

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
 * @brief Inherits from Ewl_Dialog and extends to create a filedialog
 */
struct Ewl_Filedialog
{
        Ewl_Dialog dialog;        /**< The dialog base class */
        Ewl_Widget *fp;                   /**< Ewl_Filepicker */
        Ewl_Widget *menu;        /**< The menu */
};

Ewl_Widget              *ewl_filedialog_new(void);
Ewl_Widget              *ewl_filedialog_multiselect_new(void);
Ewl_Widget              *ewl_filedialog_save_as_new(void);

int                      ewl_filedialog_init(Ewl_Filedialog *fd);

void                     ewl_filedialog_list_view_set(Ewl_Filedialog *fd,
                                                Ewl_Filelist_View view);
Ewl_Filelist_View       *ewl_filedialog_list_view_get(Ewl_Filedialog *fd);

void                     ewl_filedialog_directory_set(Ewl_Filedialog *fd,
                                                const char *dir);
const char               *ewl_filedialog_directory_get(Ewl_Filedialog *fd);

void                     ewl_filedialog_multiselect_set(Ewl_Filedialog *fd,
                                                unsigned int ms);
unsigned int             ewl_filedialog_multiselect_get(Ewl_Filedialog *fd);

void                     ewl_filedialog_show_dot_files_set(Ewl_Filedialog *fd,
                                                unsigned int dot);
unsigned int             ewl_filedialog_show_dot_files_get(Ewl_Filedialog *fd);

void                     ewl_filedialog_show_favorites_set(Ewl_Filedialog *fd,
                                                unsigned int show);
unsigned int             ewl_filedialog_show_favorites_get(Ewl_Filedialog *fd);

void                     ewl_filedialog_selected_file_set(Ewl_Filedialog *fd,
                                                const char *file);
char                    *ewl_filedialog_selected_file_get(Ewl_Filedialog *fd);

void                     ewl_filedialog_selected_files_set(Ewl_Filedialog *fd,
                                                Ecore_List *files);
Ecore_List              *ewl_filedialog_selected_files_get(Ewl_Filedialog *fd);

void                     ewl_filedialog_filter_add(Ewl_Filedialog *fd,
                                                const char *name,
                                                const char *filter,
                                                Ecore_List *mime_types);
void                     ewl_filedialog_save_as_set(Ewl_Filedialog *fd,
                                                unsigned int t);
unsigned int             ewl_filedialog_save_as_get(Ewl_Filedialog *fd);
void                     ewl_filedialog_return_directories_set(Ewl_Filedialog *fd,
                                                unsigned int t);
unsigned int             ewl_filedialog_return_directories_get(Ewl_Filedialog *fd);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_filedialog_cb_delete_window(Ewl_Widget *w, void *ev_data, void *data);

/**
 * @}
 */

#endif
