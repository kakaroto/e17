/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_filepicker.h"
#include "ewl_filelist.h"
#include "ewl_button.h"
#include "ewl_combo.h"
#include "ewl_entry.h"
#include "ewl_label.h"
#include "ewl_model.h"
#include "ewl_dialog.h"
#include "ewl_entry.h"
#include "ewl_window.h"
#include "ewl_toolbar.h"
#include "ewl_icon_theme.h"
#include "ewl_scrollpane.h"
#include "ewl_grid.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

typedef struct Ewl_Filepicker_Dialog Ewl_Filepicker_Dialog;

/**
 * @brief Used to store information about the filepicker
 */
struct Ewl_Filepicker_Dialog
{
        Ewl_Filepicker *fp;
        Ewl_Widget *d;
        Ewl_Widget *e;
};

static void ewl_filepicker_favorites_populate(Ewl_Filepicker *fp);
static void ewl_filepicker_cb_list_value_changed(Ewl_Widget *w, void *ev,
                                                                void *data);
static void ewl_filepicker_cb_button_clicked(Ewl_Widget *w, void *ev,
                                                        void *data);
static void ewl_filepicker_cb_destroy(Ewl_Widget *w, void *ev, void *data);

static void ewl_filepicker_path_populate(Ewl_Filepicker *fp, char *path);
static void ewl_filepicker_cb_path_change(Ewl_Widget *w, void *ev,
                                                        void *data);
static void ewl_filepicker_cb_filter_change(Ewl_Widget *w, void *ev,
                                                        void *data);
static void ewl_filepicker_cb_path_entry_change(Ewl_Widget *w, void *ev,
                                                        void *data);

static void ewl_filepicker_cb_type_change(Ewl_Widget *w, void *ev,
                                                        void *data);
static void ewl_filepicker_cb_dir_new(Ewl_Widget *w, void *ev,
                                                        void *data);
static void ewl_filepicker_cb_dialog_response(Ewl_Widget *w, void *ev,
                                                        void *data);
static void ewl_filepicker_cb_dialog_delete(Ewl_Widget *w, void *ev,
                                                        void *data);
static Ewl_Widget *ewl_filepicker_cb_type_header(void *data, unsigned int col,
                                                        void *pr_data);
static Ewl_Widget *ewl_filepicker_cb_type_widget(void *data, unsigned int row,
                                                        unsigned int col,
                                                        void *pr_data);

static Ewl_Widget *ewl_filepicker_cb_path_header(void *data, 
                                                        unsigned int col,
                                                        void *pr_data);

static void ewl_filepicker_filter_free_cb(Ewl_Filelist_Filter *filter);
static void ewl_filepicker_type_change(Ewl_Filepicker *fp);


/**
 * @return Returns a new Ewl_Filepicker widget or NULL on failure
 * @brief Creates a new Ewl_Filepicker widget
 */
Ewl_Widget *
ewl_filepicker_new(void)
{
        Ewl_Widget *w = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Filepicker, 1);
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_filepicker_init(EWL_FILEPICKER(w)))
        {
                ewl_widget_destroy(w);
                w = NULL;
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialize an Ewl_Filepicker to default values
 */
int
ewl_filepicker_init(Ewl_Filepicker *fp)
{
        Ewl_Widget *o, *box, *grid;
        const char *path;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fp, FALSE);

        if (!ewl_box_init(EWL_BOX(fp)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        fp->ret_dir = TRUE;
        ewl_box_orientation_set(EWL_BOX(fp), EWL_ORIENTATION_VERTICAL);
        ewl_widget_inherit(EWL_WIDGET(fp), EWL_FILEPICKER_TYPE);
        ewl_widget_appearance_set(EWL_WIDGET(fp), EWL_FILEPICKER_TYPE);

        ewl_callback_prepend(EWL_WIDGET(fp), EWL_CALLBACK_DESTROY,
                                ewl_filepicker_cb_destroy, NULL);

        /* FIXME that's the wrong way to do it */
        ewl_object_preferred_inner_size_set(EWL_OBJECT(fp), 300, 100);

        /*
         * The path combo
         */
        fp->mvc_path.model = ewl_model_ecore_list_instance();
        fp->mvc_path.view = ewl_label_view_get();
        ewl_view_header_fetch_set(fp->mvc_path.view,
                                        ewl_filepicker_cb_path_header);
        fp->path = ecore_list_new();
        ecore_list_free_cb_set(fp->path, ECORE_FREE_CB(free));

        box = ewl_htoolbar_new();
        ewl_container_child_append(EWL_CONTAINER(fp), box);
        ewl_object_fill_policy_set(EWL_OBJECT(box),
                        EWL_FLAG_FILL_NONE | EWL_FLAG_FILL_HFILL);
        ewl_widget_show(box);

        fp->dir_button = ewl_icon_new();
        ewl_icon_alt_text_set(EWL_ICON(fp->dir_button), "New Directory");

        path = ewl_icon_theme_icon_path_get(EWL_ICON_FOLDER_NEW,
                        EWL_ICON_SIZE_MEDIUM);
        if (path)
                ewl_icon_image_set(EWL_ICON(fp->dir_button), path, NULL);
        ewl_callback_append(fp->dir_button, EWL_CALLBACK_CLICKED,
                        ewl_filepicker_cb_dir_new, fp);
        ewl_container_child_append(EWL_CONTAINER(box), fp->dir_button);
        ewl_object_fill_policy_set(EWL_OBJECT(fp->dir_button),
                                EWL_FLAG_FILL_NONE);
        ewl_widget_show(fp->dir_button);

        fp->mvc_path.combo = ewl_combo_new();
        ewl_combo_editable_set(EWL_COMBO(fp->mvc_path.combo), TRUE);
        ewl_container_child_prepend(EWL_CONTAINER(box),
                                        fp->mvc_path.combo);
        ewl_mvc_model_set(EWL_MVC(fp->mvc_path.combo),
                                                fp->mvc_path.model);
        ewl_mvc_view_set(EWL_MVC(fp->mvc_path.combo),
                                                fp->mvc_path.view);
        ewl_mvc_private_data_set(EWL_MVC(fp->mvc_path.combo), fp);
        ewl_mvc_data_set(EWL_MVC(fp->mvc_path.combo), fp->path);
        ewl_callback_append(fp->mvc_path.combo,
                                EWL_CALLBACK_VALUE_CHANGED,
                                ewl_filepicker_cb_path_change, fp);
        ewl_object_fill_policy_set(EWL_OBJECT(fp->mvc_path.combo),
                                EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINKABLE);
        ewl_widget_show(fp->mvc_path.combo);

        o = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(fp), o);
        ewl_widget_show(o);

        fp->favorites_box = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(o), fp->favorites_box);
        ewl_object_fill_policy_set(EWL_OBJECT(fp->favorites_box),
                                EWL_FLAG_FILL_HSHRINKABLE | EWL_FLAG_FILL_VFILL);
        ewl_filepicker_favorites_populate(fp);
        ewl_filepicker_show_favorites_set(fp, FALSE);
        
        fp->file_list = ewl_filelist_new();
        ewl_container_child_append(EWL_CONTAINER(o), fp->file_list);
        ewl_callback_append(EWL_WIDGET(fp->file_list),
                                EWL_CALLBACK_VALUE_CHANGED,
                                ewl_filepicker_cb_list_value_changed,
                                fp);
        ewl_widget_show(fp->file_list);

        grid = ewl_grid_new();
        ewl_container_child_append(EWL_CONTAINER(fp), grid);
        ewl_grid_dimensions_set(EWL_GRID(grid), 2, 2);
        ewl_grid_column_preferred_w_use(EWL_GRID(grid), 1);
        ewl_object_fill_policy_set(EWL_OBJECT(grid),
                        EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINKABLE);
        ewl_widget_show(grid);

        fp->file_entry = ewl_entry_new();
        ewl_container_child_append(EWL_CONTAINER(grid), fp->file_entry);
        ewl_grid_child_position_set(EWL_GRID(grid), fp->file_entry, 0, 0, 0, 0);
        ewl_grid_hhomogeneous_set(EWL_GRID(grid), FALSE);
        ewl_widget_show(fp->file_entry);
        
        /*
         * The filter combo
         */
        fp->filters = ecore_list_new();
        ecore_list_free_cb_set(fp->filters,
                        ECORE_FREE_CB(ewl_filepicker_filter_free_cb));

        fp->mvc_filters.model = ewl_model_ecore_list_instance();
        fp->mvc_filters.view = ewl_view_new();
        ewl_view_widget_fetch_set(fp->mvc_filters.view,
                                ewl_filepicker_cb_type_widget);
        ewl_view_header_fetch_set(fp->mvc_filters.view,
                                ewl_filepicker_cb_type_header);

        fp->mvc_filters.combo = ewl_combo_new();
        ewl_mvc_model_set(EWL_MVC(fp->mvc_filters.combo), fp->mvc_filters.model);
        ewl_mvc_view_set(EWL_MVC(fp->mvc_filters.combo), fp->mvc_filters.view);
        ewl_mvc_private_data_set(EWL_MVC(fp->mvc_filters.combo), fp);
        ewl_mvc_data_set(EWL_MVC(fp->mvc_filters.combo), fp->filters);
        ewl_combo_editable_set(EWL_COMBO(fp->mvc_filters.combo), TRUE);
        ewl_callback_append(fp->mvc_filters.combo, EWL_CALLBACK_VALUE_CHANGED,
                                ewl_filepicker_cb_type_change, fp);
        ewl_container_child_append(EWL_CONTAINER(grid), fp->mvc_filters.combo);
        ewl_grid_child_position_set(EWL_GRID(grid), fp->mvc_filters.combo,
                                0, 0, 1, 1);
        ewl_object_fill_policy_set(EWL_OBJECT(fp->mvc_filters.combo),
                                EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINKABLE);
        ewl_widget_show(fp->mvc_filters.combo);

        fp->ret_button = ewl_button_new();
        ewl_container_child_append(EWL_CONTAINER(grid), fp->ret_button);
        ewl_grid_child_position_set(EWL_GRID(grid), fp->ret_button, 1, 1, 0, 0);
        ewl_stock_type_set(EWL_STOCK(fp->ret_button), EWL_STOCK_OPEN);
        ewl_callback_append(fp->ret_button, EWL_CALLBACK_CLICKED,
                                ewl_filepicker_cb_button_clicked, fp);
        ewl_object_fill_policy_set(EWL_OBJECT(fp->ret_button),
                                                EWL_FLAG_FILL_SHRINKABLE);
        ewl_widget_show(fp->ret_button);

        o = ewl_button_new();
        ewl_container_child_append(EWL_CONTAINER(grid), o);
        ewl_grid_child_position_set(EWL_GRID(grid), o, 1, 1, 1, 1);
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_CANCEL);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                ewl_filepicker_cb_button_clicked, fp);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINKABLE);
        ewl_widget_show(o);
        
        /* Set up filelist */
        ewl_filepicker_filter_add(fp, "All files", NULL, NULL);
        ewl_filepicker_directory_set(fp, NULL);
        ewl_mvc_selected_set(EWL_MVC(fp->mvc_filters.combo),
                        NULL, fp->filters, 0, 0);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @return Returns a created Save As widget or NULL on failure
 * @brief A convenience function to create a Save As widget
 */
Ewl_Widget *
ewl_filepicker_save_as_new(void)
{
        Ewl_Widget *ret;

        DENTER_FUNCTION(DLEVEL_STABLE);
        
        ret = ewl_filepicker_new();
        ewl_filepicker_save_as_set(EWL_FILEPICKER(ret), TRUE);

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @internal
 * @param fp: The filepicker to change the type of
 * @return Returns no value
 * @brief Changes the filepicker to either an open or save as type
 */
static void
ewl_filepicker_type_change(Ewl_Filepicker *fp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

        if (fp->saveas_dialog)
        {
                ewl_widget_enable(fp->file_entry);
                ewl_stock_type_set(EWL_STOCK(fp->ret_button), EWL_STOCK_SAVE);
        }
        else
        {
                if (ewl_filelist_multiselect_get(EWL_FILELIST(fp->file_list)))
                        ewl_widget_disable(fp->file_entry);
                ewl_stock_type_set(EWL_STOCK(fp->ret_button), EWL_STOCK_OPEN);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to work with
 * @param t: Non-zero to set the filepicker to a save type
 * @return Returns no value
 * @brief Sets the type for the filepicker
 */
void
ewl_filepicker_save_as_set(Ewl_Filepicker *fp, unsigned int t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

        if (fp->saveas_dialog == t)
                DRETURN(DLEVEL_STABLE);

        fp->saveas_dialog = !!t;
        ewl_filepicker_type_change(fp);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to work with
 * @return Returns non-zero if the filepicker is currently a Save As type
 * @brief Gets the current type of the filepicker
 */
unsigned int
ewl_filepicker_save_as_get(Ewl_Filepicker *fp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fp, FALSE);
        DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, FALSE);

        DRETURN_INT(fp->saveas_dialog, DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to work with
 * @param t: Non-zero to allow the filepicker to open directories
 * @return Returns no value
 * @brief Sets the filepicker's policy on returning directories
 */
void
ewl_filepicker_return_directories_set(Ewl_Filepicker *fp, unsigned int t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

        fp->ret_dir = !!t;
        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to work with
 * @return Returns non-zero if the filepicker can return directories
 * @brief Gets the filepicker's policy on returning directories
 */
unsigned int
ewl_filepicker_return_directories_get(Ewl_Filepicker *fp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fp, FALSE);
        DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, FALSE);

        DRETURN_INT(fp->ret_dir, DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to change
 * @param show: The show favorites setting to apply
 * @return Returns no value
 * @brief Sets the show favorites setting of the file picker
 */
void
ewl_filepicker_show_favorites_set(Ewl_Filepicker *fp, unsigned int show)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

        if (show == fp->show_favorites)
                DRETURN(DLEVEL_STABLE);

        fp->show_favorites = !!show;
        if (fp->show_favorites)
                ewl_widget_show(fp->favorites_box);
        else
                ewl_widget_hide(fp->favorites_box);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the show favorites value from
 * @return Returns the current show favorites setting of the filepicker
 * @brief Retrieves the current show favorites setting of the filepicker
 */
unsigned int
ewl_filepicker_show_favorites_get(Ewl_Filepicker *fp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fp, 0);
        DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, 0);

        DRETURN_INT(fp->show_favorites, DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to change
 * @param view: The view to use for the file list
 * @return Returns no value
 * @brief Sets the given view to be used to display the file list
 */
void
ewl_filepicker_list_view_set(Ewl_Filepicker *fp, Ewl_Filelist_View view)
{
        Ewl_Filelist *temp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);
        
        temp = EWL_FILELIST(fp->file_list);
        
        if (temp->view_flag == view)
                DRETURN(DLEVEL_STABLE);
        ewl_filelist_view_set(EWL_FILELIST(fp->file_list), view);
        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the value from
 * @return Returns the current view set on the filepicker
 * @brief Retrieves the current view used for the file list
 */
Ewl_Filelist_View
ewl_filepicker_list_view_get(Ewl_Filepicker *fp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fp, EWL_FILELIST_VIEW_ICON);
        DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, EWL_FILELIST_VIEW_ICON);

        DRETURN_INT(ewl_filelist_view_get(EWL_FILELIST(fp->file_list)),
                                                        DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to set the directory on
 * @param dir: The directory to set
 * @return Returns no value.
 * @brief Set the current directory to be displayed in the filepicker
 */
void
ewl_filepicker_directory_set(Ewl_Filepicker *fp, const char *dir)
{
        char *tmp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

        /* if no directory is given try to default to the HOME directory,
         * and if that failes try /tmp and if that fails try / */
        if (!dir)
        {
                char *t;

                t = getenv("HOME");
                if (t) tmp = strdup(t);
                else tmp = strdup("/tmp");

                if (!ecore_file_exists(tmp))
                {
                        FREE(tmp);
                        tmp = strdup("/");
                }
        }
        else
                tmp = strdup(dir);

        if ((ecore_file_is_dir(tmp)) && (ecore_file_can_read(tmp)))
        {
                ewl_filepicker_path_populate(fp, tmp);
                ewl_filelist_directory_set(EWL_FILELIST(fp->file_list), tmp);
        }

        FREE(tmp);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the directory from
 * @return Returns the current directory displayed in the file dialog
 * @brief Retrieve the current directory from the filepicker
 */
const char *
ewl_filepicker_directory_get(Ewl_Filepicker *fp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fp, NULL);
        DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, NULL);

        DRETURN_PTR(ewl_filelist_directory_get(EWL_FILELIST(fp->file_list)),
                                                        DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to set the filter on
 * @param filter: The filter to set on the filepicker
 * @return Returns no value.
 * @brief Set the given filter into the file picker and updates the combo
 */
void
ewl_filepicker_filter_set(Ewl_Filepicker *fp,
                                        Ewl_Filelist_Filter *filter)
{
        int idx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

        if (filter == ewl_filepicker_filter_get(fp))
                DRETURN(DLEVEL_STABLE);

        ecore_list_goto(fp->filters, filter);
        idx = ecore_list_index(fp->filters);
        ewl_mvc_selected_set(EWL_MVC(fp->mvc_filters.combo), NULL, fp->filters, idx, 0);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the filter from
 * @return Returns the current filter used in the filepicker
 * @brief Retrieves the current filter used in the filepicker.  Do not free the filter!
 */
Ewl_Filelist_Filter *
ewl_filepicker_filter_get(Ewl_Filepicker *fp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fp, NULL);
        DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, NULL);

        DRETURN_PTR(ewl_filelist_filter_get(EWL_FILELIST(fp->file_list)),
                                                        DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to set the multi-select flag
 * @param ms: The multi-select setting to place on the filepicker
 * @return Returns no value.
 * @brief Sets the multiselect state of the filepicker to the given value
 */
void
ewl_filepicker_multiselect_set(Ewl_Filepicker *fp, unsigned int ms)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

        ewl_filelist_multiselect_set(EWL_FILELIST(fp->file_list), ms);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the multiselect value from
 * @return Returns the current multi-select value of the filepicker
 * @brief Retrieves the current multi-select value from the filepicker
 */
unsigned int
ewl_filepicker_multiselect_get(Ewl_Filepicker *fp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fp, FALSE);
        DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, FALSE);

        DRETURN_INT(ewl_filelist_multiselect_get(EWL_FILELIST(fp->file_list)),
                                                                DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to set the value into
 * @param dot: The show dot files setting to set.
 * @return Returns no value.
 * @brief Set the show dot files setting of the filepicker to the given
 * value
 */
void
ewl_filepicker_show_dot_files_set(Ewl_Filepicker *fp, unsigned int dot)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

        ewl_filelist_show_dot_files_set(EWL_FILELIST(fp->file_list), dot);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the showdot files setting from
 * @return Returns the current show dot files setting of the filepicker
 * @brief Retrieves the current show dot files setting of the filepicker
 */
unsigned int
ewl_filepicker_show_dot_files_get(Ewl_Filepicker *fp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fp, FALSE);
        DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, FALSE);

        DRETURN_INT(ewl_filelist_show_dot_files_get(EWL_FILELIST(fp->file_list)),
                                                                DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to set the selected file into
 * @param file: The file to set selected in the filepicker
 * @return Returns no value
 * @brief Sets the given file as selected in the filepicker
 */
void
ewl_filepicker_selected_file_set(Ewl_Filepicker *fp, const char *file)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

        ewl_filelist_selected_file_set(EWL_FILELIST(fp->file_list), file);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the selected file from
 * @return Returns the full path of the currently selected file in the filepicker
 * @brief Retrieves the currently selected file from the filepicker
 */
char *
ewl_filepicker_selected_file_get(Ewl_Filepicker *fp)
{
        char *file, *ret;
        const char *dir;
        size_t len, dir_len, file_len;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fp, NULL);
        DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, NULL);

        /* we try to grab whats in the text entry first as the user may have
         * changed the selected name. Otherwise we try to grab whats in the
         * selected array from the file list */
        file = ewl_text_text_get(EWL_TEXT(fp->file_entry));
        if (!file)
                ret = ewl_filelist_selected_file_get(EWL_FILELIST(fp->file_list));
        else
        {
                dir = ewl_filelist_directory_get(EWL_FILELIST(fp->file_list));
                file_len = strlen(file);
                if (file_len)
                {
                        if (file[0] != '/')
                        {
                                dir_len = strlen(dir);
                                if (dir_len == 1)
                                        dir_len = 0;
                                len = dir_len + file_len + 2;
                                ret = NEW(char, len);
                                strcat(ret, dir);
                                if (dir_len > 1)
                                        strcat(ret, "/");
                                strcat(ret, file);
                        }
                        else
                        {
                                ret = file;
                                file = NULL;
                        }
                }
                else
                        ret = NULL;
        }

        IF_FREE(file);
        if ((!fp->ret_dir) && (ret) && (ecore_file_is_dir(ret)))
                FREE(ret);

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to set the selected files into
 * @param files: The Ecore_List of selected files to set into the filepicker
 * @return Returns no value
 * @brief Sets the given files as selected in the filepicker
 */
void
ewl_filepicker_selected_files_set(Ewl_Filepicker *fp, Ecore_List *files)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

        ewl_filelist_selected_files_set(EWL_FILELIST(fp->file_list), files);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to work with
 * @return Returns the Ecore_List of selected files in the filepicker
 * @brief Retrieves the list of selected files in the filepicker
 */
Ecore_List *
ewl_filepicker_selected_files_get(Ewl_Filepicker *fp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fp, NULL);
        DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, NULL);

        DRETURN_PTR(ewl_filelist_selected_files_get(EWL_FILELIST(fp->file_list)),
                                                                DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to work with
 * @param name: The name to display for the filter
 * @param extension: The extension to filter for
 * @param mime_types: The mime types to filter for
 * @return Returns the filter created. Do not free the returned filter.
 * @brief Add the filter named @a name to the combo box in the filepicker.
 */
Ewl_Filelist_Filter *
ewl_filepicker_filter_add(Ewl_Filepicker *fp, const char *name,
                                                const char *extension,
                                                Ecore_List *mime_types)
{
        Ewl_Filelist_Filter *f;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fp, NULL);
        DCHECK_PARAM_PTR_RET(name, NULL);
        DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, NULL);

        f= NEW(Ewl_Filelist_Filter, 1);
        if (!f)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        f->name = strdup(name);
        if (extension) f->extension = strdup(extension);
        else f->extension = NULL;
        if (mime_types) f->mime_list = mime_types;
        else f->mime_list = NULL;

        ecore_list_prepend(fp->filters, f);
        ewl_mvc_dirty_set(EWL_MVC(fp->mvc_filters.combo), TRUE);
        ewl_mvc_selected_set(EWL_MVC(fp->mvc_filters.combo), NULL, fp->filters, 0, 0);

        DRETURN_PTR(f, DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_list_value_changed(Ewl_Widget *w, void *ev, void *data)
{
        Ewl_Filepicker *fp;
        Ewl_Filelist *fl;
        Ewl_Event_Action_Response *e;
        char *file;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);

        fl = EWL_FILELIST(w);
        fp = data;
        e = ev;

        /* clear the text and get the selected file */
        file = ewl_filelist_selected_file_get(fl);

        if (e->response == EWL_FILELIST_EVENT_DIR_CHANGE)
        {
                char *dir;

                dir = strdup(ewl_filelist_directory_get(fl));
                ewl_filepicker_path_populate(fp, dir);

                if (!ecore_file_can_write(dir))
                        ewl_widget_disable(fp->dir_button);
                else
                        ewl_widget_enable(fp->dir_button);
                ewl_text_clear(EWL_TEXT(fp->file_entry));
                FREE(dir);
        }
        else if (e->response == EWL_FILELIST_EVENT_SELECTION_CHANGE)
        {
                if (ecore_file_is_dir(file))
                {
                        if ((!fp->saveas_dialog) && (!fl->multiselect) &&
                                                (fp->ret_dir))
                                ewl_text_text_set(EWL_TEXT(fp->file_entry),
                                                file);
                }
                else
                {
                        if (!fl->multiselect)
                                ewl_text_text_set(EWL_TEXT(fp->file_entry),
                                        ecore_file_file_get(file));
                }
        }

        else if ((e->response == EWL_FILELIST_EVENT_MULTI_TRUE) &&
                                (!fp->saveas_dialog))
        {
                ewl_widget_disable(fp->file_entry);
                ewl_text_clear(EWL_TEXT(fp->file_entry));
        }

        else if ((e->response == EWL_FILELIST_EVENT_MULTI_FALSE) &&
                                (!fp->saveas_dialog))
        {
                if ((ecore_file_is_dir(file)) && (fp->ret_dir))
                {
                        ewl_text_text_set(EWL_TEXT(fp->file_entry),
                                                file);
                }
                else
                {
                        ewl_text_text_set(EWL_TEXT(fp->file_entry),
                                        ecore_file_file_get(file));
                }
                ewl_widget_enable(fp->file_entry);
        }

        FREE(file);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_button_clicked(Ewl_Widget *w, void *ev __UNUSED__,
                                                        void *data)
{
        Ewl_Filepicker *fp;
        Ewl_Stock *s;
        Ewl_Event_Action_Response e;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        fp = data;
        s = EWL_STOCK(w);

        e.response = ewl_stock_type_get(s);

        if (e.response == EWL_STOCK_CANCEL)
        {
                ewl_text_text_set(EWL_TEXT(fp->file_entry), NULL);
                ewl_filepicker_selected_file_set(fp, NULL);
        }
        ewl_callback_call_with_event_data(EWL_WIDGET(fp),
                                        EWL_CALLBACK_VALUE_CHANGED, &e);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_favorites_populate(Ewl_Filepicker *fp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

        /* XXX Write me */

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_filepicker_cb_type_widget(void *data, unsigned int row __UNUSED__,
                                unsigned int col __UNUSED__,
                                void *pr_data __UNUSED__)
{
        Ewl_Filelist_Filter *filter;
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, NULL);

        filter = data;

        w = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(w), filter->name);

        DRETURN_PTR(w, DLEVEL_STABLE);
}


static Ewl_Widget *
ewl_filepicker_cb_type_header(void *data, unsigned int col __UNUSED__,
                                void *pr_data)
{
        Ewl_Filepicker *fp;
        Ewl_Filelist_Filter *filter;
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(pr_data, NULL);
        DCHECK_TYPE_RET(pr_data, EWL_FILEPICKER_TYPE, NULL);

        fp = EWL_FILEPICKER(pr_data);
        filter = data;

        w = ewl_entry_new();
        if (filter)
                ewl_text_text_set(EWL_TEXT(w), filter->name);
        ewl_callback_append(w, EWL_CALLBACK_VALUE_CHANGED,
                                ewl_filepicker_cb_filter_change, fp);

        DRETURN_PTR(w, DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_filepicker_cb_path_header(void *data, unsigned int col __UNUSED__,
                                void *pr_data)
{
        Ewl_Filepicker *fp;
        Ewl_Widget *entry;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(pr_data, NULL);
        DCHECK_TYPE_RET(pr_data, EWL_FILEPICKER_TYPE, NULL);

        fp = EWL_FILEPICKER(pr_data);

        entry = ewl_entry_new();
        ewl_text_text_set(EWL_TEXT(entry), (const char *)data);
        ewl_callback_append(entry, EWL_CALLBACK_VALUE_CHANGED,
                                ewl_filepicker_cb_path_entry_change, fp);

        DRETURN_PTR(entry, DLEVEL_STABLE);
}

static void
ewl_filepicker_path_populate(Ewl_Filepicker *fp, char *path)
{
        char *p, *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fp);
        DCHECK_PARAM_PTR(path);
        DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

        /* Clear the list and free the data first */
        while ((p = ecore_list_first_remove(fp->path)))
                FREE(p);

        ecore_list_append(fp->path, strdup("/"));

        p = path;
        while ((t = strchr((p + 1), '/')))
        {
                *t = '\0';
                ecore_list_prepend(fp->path, strdup(path));

                *t = '/';
                p = t;
        }

        /* do we have more stuff? if so, make sure we get the last path */
        if (p && (*(p + 1) != '\0'))
                ecore_list_prepend(fp->path, strdup(path));

        ewl_mvc_dirty_set(EWL_MVC(fp->mvc_path.combo), TRUE);
        ewl_mvc_selected_set(EWL_MVC(fp->mvc_path.combo), NULL, fp->path, 0, 0);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_path_change(Ewl_Widget *w, void *ev __UNUSED__,
                                                        void *data)
{
        Ewl_Filepicker *fp;
        Ewl_Selection_Idx *idx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        fp = data;
        idx = ewl_mvc_selected_get(EWL_MVC(w));

        if (idx->row == 0)
        {
                free(idx);
                DRETURN(DLEVEL_STABLE);
        }

        ecore_list_index_goto(fp->path, idx->row);
        ewl_filepicker_directory_set(fp, ecore_list_current(fp->path));
        free(idx);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_filter_change(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Filepicker *fp;
        char *name;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        fp = data;
        name = ewl_text_text_get(EWL_TEXT(w));
        
        if (!name)
                DRETURN(DLEVEL_STABLE);
        
        ewl_filepicker_filter_add(fp, name, name, NULL);
        ewl_filepicker_cb_type_change(fp->mvc_filters.combo, NULL, fp);
        FREE(name);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_path_entry_change(Ewl_Widget *w, void *ev __UNUSED__,
                                                        void *data)
{
        Ewl_Filepicker *fp;
        char *path;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        fp = data;
        path = ewl_text_text_get(EWL_TEXT(w));

        if (!path)
                DRETURN(DLEVEL_STABLE);

        ewl_filepicker_directory_set(fp, path);
        FREE(path);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_type_change(Ewl_Widget *w, void *ev __UNUSED__,
                                                        void *data)
{
        Ewl_Filepicker *fp;
        Ewl_Filelist_Filter *filter;
        Ewl_Selection_Idx *idx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        fp = data;
        idx = ewl_mvc_selected_get(EWL_MVC(w));
        if (idx)
        {
                ecore_list_index_goto(fp->filters, idx->row);
                filter = ecore_list_current(fp->filters);
                ewl_filelist_filter_set(EWL_FILELIST(fp->file_list), filter);
        }
        
        FREE(idx);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Filepicker *fp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        fp = EWL_FILEPICKER(w);

        IF_FREE(fp->mvc_path.model);
        IF_FREE(fp->mvc_path.view);
        IF_FREE(fp->mvc_filters.model);
        IF_FREE(fp->mvc_filters.view);
        IF_FREE_LIST(fp->path);
        IF_FREE_LIST(fp->filters);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_filter_free_cb(Ewl_Filelist_Filter *filter)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        if (!filter) DRETURN(DLEVEL_STABLE);

        IF_FREE(filter->name);
        IF_FREE(filter->extension);
        IF_FREE_LIST(filter->mime_list);

        FREE(filter);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_dir_new(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *data)
{
        Ewl_Widget *d;
        Ewl_Embed *win;
        Ewl_Widget *o;
        Ewl_Widget *box;
        const char *path;
        Ewl_Filepicker_Dialog *resp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);

        resp = NEW(Ewl_Filepicker_Dialog, 1);
        win = ewl_embed_widget_find(w);

        d = ewl_dialog_new();
        ewl_window_title_set(EWL_WINDOW(d), "Create New Folder");
        ewl_window_name_set(EWL_WINDOW(d), "Create New Folder");
        ewl_window_class_set(EWL_WINDOW(d), "Create New Folder");
        ewl_window_transient_for_foreign(EWL_WINDOW(d), win->canvas_window);
        ewl_callback_append(d, EWL_CALLBACK_DELETE_WINDOW,
                                ewl_filepicker_cb_dialog_delete, resp);
        ewl_object_fill_policy_set(EWL_OBJECT(d), EWL_FLAG_FILL_NONE);
        ewl_dialog_active_area_set(EWL_DIALOG(d), EWL_POSITION_TOP);
        ewl_widget_show(d);

        box = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(d), box);
        ewl_object_padding_set(EWL_OBJECT(box), 5, 5, 5, 0);
        ewl_widget_show(box);

        path = ewl_icon_theme_icon_path_get(EWL_ICON_FOLDER_NEW,
                                        32);
        o = ewl_image_new();
        if (path) ewl_image_file_path_set(EWL_IMAGE(o), path);
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        o = ewl_entry_new();
        ewl_text_text_set(EWL_TEXT(o), "New Folder");
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                ewl_filepicker_cb_dialog_response, resp);
        ewl_widget_show(o);

        ewl_dialog_active_area_set(EWL_DIALOG(d), EWL_POSITION_BOTTOM);
        resp->fp = EWL_FILEPICKER(data);
        resp->d = d;
        resp->e = o;

        o = ewl_button_new();
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_OK);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                ewl_filepicker_cb_dialog_response, resp);
        ewl_container_child_append(EWL_CONTAINER(d), o);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_CANCEL);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                ewl_filepicker_cb_dialog_response, resp);
        ewl_container_child_append(EWL_CONTAINER(d), o);
        ewl_widget_show(o);
}

static void
ewl_filepicker_cb_dialog_delete(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *data)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_widget_destroy(w);
        FREE(data);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_dialog_response(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *data)
{
        Ewl_Filepicker_Dialog *resp;
        const char *path;
        char *name, res[PATH_MAX];
        Ewl_Stock_Type response;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        resp = data;

        /* There is a callback on the entry that calls this */
        if (ewl_widget_type_is(w, EWL_STOCK_TYPE))
                response = ewl_stock_type_get(EWL_STOCK(w));
        else
                response = EWL_STOCK_OK;

        name = ewl_text_text_get(EWL_TEXT(resp->e));
        
        if ((response == EWL_STOCK_CANCEL) || (!name))
        {
                ewl_widget_destroy(resp->d);
                FREE(resp);
                DRETURN(DLEVEL_STABLE);
        }

        path = ewl_filelist_directory_get(EWL_FILELIST(resp->fp->file_list));

        if (name[0] == '/')
                snprintf(res, PATH_MAX, "%s%s", path, name);
        else
                snprintf(res, PATH_MAX, "%s/%s", path, name);

        if (ecore_file_mkpath(res))
                ewl_filepicker_directory_set(EWL_FILEPICKER(resp->fp), res);

        ewl_widget_destroy(resp->d);
        FREE(name);
        FREE(resp);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

