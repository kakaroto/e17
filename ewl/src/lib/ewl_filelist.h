/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_FILELIST_H
#define EWL_FILELIST_H

#include "ewl_box.h"
#include "ewl_image.h"
#include "ewl_icon.h"
#include "ewl_text.h"
#include "ewl_model.h"
#include "ewl_view.h"

#include <Ecore_File.h>

#ifndef uid_t
# define uid_t unsigned long
#endif

#ifndef gid_t
# define gid_t unsigned long
#endif

/**
 * @addtogroup Ewl_Filelist Ewl_Filelist: The base widget for the filelists
 * A base class to be extended to provide different views on the file list
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_Filelist_inheritance.png
 * @endif
 *
 * @{
 */
 
/**
 * @def EWL_FILELIST_FILTER_TYPE
 * The type name
 */
#define EWL_FILELIST_FILTER_TYPE "filelist_filter"

typedef struct Ewl_Filelist_Filter Ewl_Filelist_Filter;

/**
 * @def EWL_FILELIST_FILTER(ff)
 * Typecasts a pointer to an Ewl_Filelist_Filter
 */
#define EWL_FILELIST_FILTER(ff), ((Ewl_Filelist_Filter *)ff)

/**
 * @brief The data structure for Ewl_Filelist_Filter
 */
struct Ewl_Filelist_Filter
{
                char *name;
                char *extension;
                Ecore_List *mime_list;
};

/**
 * @def EWL_FILELIST_TYPE
 * The type name for the Ewl_Filelist widget
 */
#define EWL_FILELIST_TYPE "filelist"

/**
 * @def EWL_FILELIST_IS(w)
 * Returns TRUE if the widget is an Ewl_Filelist, FALSE otherwise
 */
#define EWL_FILELIST_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_FILELIST_TYPE))

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
 * @brief Inherits from Ewl_Box and extends to display a file list.
 */
struct Ewl_Filelist
{
        Ewl_Box box;                     /**< Inherits from Ewl_Box */
        Ewl_Widget *controller;          /**< Must inherit from mvc **/

        Ewl_View *view;                  /**< The view for mvc */
        Ewl_Model *model;                /**< The model for the mvc */

        char *directory;                 /**< The directory to display */
        Ewl_Filelist_Filter *filter;     /**< The file filter to employ */

        unsigned char multiselect:1;     /**< Allow multiple file selctions */
        unsigned char show_dot:1;        /**< Show . files */

        Ewl_Filelist_View view_flag;     /**< The view to use for controller */

        struct
        {        Ewl_Scrollpane_Flags h; /**< Horizontal scroll flag */
                Ewl_Scrollpane_Flags v;  /**< Vertical scroll flag */
        } scroll_flags;                  /**< Flags to modify a containing scrollpane */
        Ecore_File_Monitor *fm;          /**< File monitor */
};

/**
 * @def EWL_FILELIST_FILE_TYPE
 * The type name
 */
#define EWL_FILELIST_FILE_TYPE "filelist_file"

typedef struct Ewl_Filelist_File Ewl_Filelist_File;

/**
 * @def EWL_FILELIST_FILE(fl)
 * Typecasts a pointer to an Ewl_Filelist_File pointer
 */
#define EWL_FILELIST_FILE(fl) ((Ewl_Filelist_File *)fl)

/**
 * @brief The data structure for Ewl_Filelist_File
 */
struct Ewl_Filelist_File
{
        const char *name;
        off_t size;
        mode_t mode;
        uid_t username;
        gid_t groupname;
        time_t modtime;
        unsigned char readable:1;
        unsigned char writeable:1;
        unsigned char is_dir:1;
};

/**
 * @def EWL_FILELIST_DIRECTORY_TYPE
 * The type name
 */
#define EWL_FILELIST_DIRECTORY_TYPE "filelist_directory"

typedef struct Ewl_Filelist_Directory Ewl_Filelist_Directory;

/**
 * @def EWL_FILELIST_DIRECTORY(fl)
 * Typecasts a pointer to an Ewl_Filelist_Directory pointer
 */
#define EWL_FILELIST_DIRECTORY(fl) ((Ewl_Filelist_Directory *)fl)

/**
 * @brief The data structure for Ewl_Filelist_Directory
 */
struct Ewl_Filelist_Directory
{
        const char *name;
        Ecore_List *rfiles;
        Ecore_List *rdirs;
        Ecore_List *files;
        Ecore_List *dirs;
        Ewl_Filelist_Filter *filter;
        unsigned char show_dot:1;
        unsigned int num_dirs;
        unsigned int num_files;
};

Ewl_Widget              *ewl_filelist_new(void);

int                      ewl_filelist_init(Ewl_Filelist *fl);

void                     ewl_filelist_directory_set(Ewl_Filelist *fl,
                                                        const char *dir);
const char              *ewl_filelist_directory_get(Ewl_Filelist *fl);
void                     ewl_filelist_refresh(Ewl_Filelist *fl);

void                     ewl_filelist_filter_set(Ewl_Filelist *fl,
                                                        Ewl_Filelist_Filter *filter);
Ewl_Filelist_Filter     *ewl_filelist_filter_get(Ewl_Filelist *fl);

void                     ewl_filelist_view_set(Ewl_Filelist *fl,
                                                Ewl_Filelist_View view);
Ewl_Filelist_View       *ewl_filelist_view_get(Ewl_Filelist *fl);


void                     ewl_filelist_multiselect_set(Ewl_Filelist *fl,
                                                        unsigned int ms);
unsigned int             ewl_filelist_multiselect_get(Ewl_Filelist *fl);

void                     ewl_filelist_show_dot_files_set(Ewl_Filelist *fl,
                                                        unsigned int dot);
unsigned int             ewl_filelist_show_dot_files_get(Ewl_Filelist *fl);

void                     ewl_filelist_selected_file_set(Ewl_Filelist *fl,
                                                         const char *file);
char                    *ewl_filelist_selected_file_get(Ewl_Filelist *fl);
char                    *ewl_filelist_size_get(off_t st_size);
char                    *ewl_filelist_perms_get(mode_t st_mode);
char                    *ewl_filelist_username_get(uid_t st_uid);
char                    *ewl_filelist_groupname_get(gid_t st_gid);
char                    *ewl_filelist_modtime_get(time_t modtime);
Ewl_Widget              *ewl_filelist_selected_file_preview_get(Ewl_Filelist *fl,
                                                        const char *path);
Ewl_Widget              *ewl_filelist_multi_select_preview_get(Ewl_Filelist *fl);

void                     ewl_filelist_selected_files_set(Ewl_Filelist *fl,
                                                        Ecore_List *files);
Ecore_List              *ewl_filelist_selected_files_get(Ewl_Filelist *fl);
void                     ewl_filelist_selected_files_change_notify(Ewl_Filelist *fl);

void                     ewl_filelist_selected_signal_all(Ewl_Filelist *fl,
                                                const char *signal);

void                     ewl_filelist_vscroll_flag_set(Ewl_Filelist *fl,
                                                Ewl_Scrollpane_Flags v);
Ewl_Scrollpane_Flags     ewl_filelist_vscroll_flag_get(Ewl_Filelist *fl);

void                     ewl_filelist_hscroll_flag_set(Ewl_Filelist *fl,
                                                Ewl_Scrollpane_Flags h);
Ewl_Scrollpane_Flags     ewl_filelist_hscroll_flag_get(Ewl_Filelist *fl);

const char              *ewl_filelist_stock_icon_get(const char *path);

char                    *ewl_filelist_expand_path(Ewl_Filelist *fl, const char *dir);

/*
 * Internally used functions, override at your own risk
 */
void ewl_filelist_cb_destroy(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

