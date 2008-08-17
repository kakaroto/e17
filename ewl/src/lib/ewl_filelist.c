/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_filelist.h"
#include "ewl_filelist_model.h"
#include "ewl_filelist_view.h"
#include "ewl_tree.h"
#include "ewl_freebox_mvc.h"
#include "ewl_mvc.h"
#include "ewl_icon_theme.h"
#include "ewl_io_manager.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_scrollpane.h"

#include <sys/types.h>
#if HAVE_PWD_H
# include <pwd.h>
#endif /* HAVE_PWD_H */
#if HAVE_GRP_H
# include <grp.h>
#endif /* HAVE_GRP_H */
#include <time.h>

static void ewl_filelist_setup(Ewl_Filelist *fl);
static void ewl_filelist_view_setup(Ewl_Filelist *fl);
static void ewl_filelist_cb_clicked(Ewl_Widget *w, void *ev,
                                                void *data);
void ewl_filelist_monitor_event(void *data, Ecore_File_Monitor *em,
                        Ecore_File_Event event, const char *path);

/**
 * @return Returns a new Ewl_Filelist widget or NULL on failure
 * @brief Creates a new Ewl_Filelist widget
 */
Ewl_Widget *
ewl_filelist_new(void)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Filelist, 1);
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_filelist_init(EWL_FILELIST(w)))
        {
                ewl_widget_destroy(w);
                w = NULL;
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialzie a filelist to default values
 */
int
ewl_filelist_init(Ewl_Filelist *fl)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, FALSE);

        if (!ewl_box_init(EWL_BOX(fl)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(EWL_WIDGET(fl), EWL_FILELIST_TYPE);
        ewl_object_fill_policy_set(EWL_OBJECT(fl), EWL_FLAG_FILL_FILL);

        fl->scroll_flags.h = EWL_SCROLLPANE_FLAG_AUTO_VISIBLE;
        fl->scroll_flags.v = EWL_SCROLLPANE_FLAG_AUTO_VISIBLE;

        ewl_callback_prepend(EWL_WIDGET(fl), EWL_CALLBACK_DESTROY,
                                ewl_filelist_cb_destroy, NULL);

        fl->multiselect = FALSE;
        fl->show_dot = FALSE;

        fl->directory = NULL;
        fl->filter = NULL;
        ewl_filelist_setup(fl);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to setup
 * @return Returns no value
 * @brief Sets up the filelist view and data
 */
static void
ewl_filelist_setup(Ewl_Filelist *fl)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        fl->view = ewl_view_new();
        ewl_view_widget_fetch_set(fl->view,
                                ewl_filelist_view_widget_fetch);
        ewl_view_header_fetch_set(fl->view,
                                ewl_filelist_view_header_fetch);

        fl->model = ewl_model_new();
        ewl_model_data_count_set(fl->model,
                        ewl_filelist_model_data_count);
        ewl_model_data_fetch_set(fl->model,
                        ewl_filelist_model_data_fetch);
        ewl_model_data_sort_set(fl->model,
                        ewl_filelist_model_data_sort);
        ewl_model_column_sortable_set(fl->model,
                        ewl_filelist_model_column_sortable);
        ewl_model_data_unref_set(fl->model,
                        ewl_filelist_model_data_unref);

        /* Set default flag */
        fl->view_flag = EWL_FILELIST_VIEW_ICON;
        ewl_filelist_view_setup(fl);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 */
static void
ewl_filelist_view_setup(Ewl_Filelist *fl)
{
        void *data = NULL;
        Ewl_Widget *p;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        if (fl->controller)
        {
                /* We have to check for the scrollpane used for the freebox */
                p = fl->controller;
                while (p->parent != EWL_WIDGET(fl))
                        p = p->parent;

                /* If there is a scrollpane, destroy it */
                if (p != fl->controller)
                        ewl_widget_destroy(p);
                data = ewl_mvc_data_get(EWL_MVC(fl->controller));
                ewl_mvc_data_set(EWL_MVC(fl->controller), NULL);
                ewl_widget_destroy(fl->controller);
        }

        /* Set expansions callbacks to NULL right off the bat */
        ewl_model_expansion_data_fetch_set(fl->model, NULL);
        ewl_model_data_expandable_set(fl->model, NULL);

        if (fl->view_flag == EWL_FILELIST_VIEW_TREE)
        {
                fl->controller = ewl_tree_new();
                ewl_tree_selection_type_set(EWL_TREE(fl->controller),
                                EWL_TREE_SELECTION_TYPE_ROW);
                ewl_tree_column_count_set(EWL_TREE(fl->controller), 2);
                ewl_model_expansion_data_fetch_set(fl->model,
                        ewl_filelist_model_data_expansion_data_fetch);
                ewl_model_data_expandable_set(fl->model,
                        ewl_filelist_model_data_expandable_get);
                ewl_container_child_append(EWL_CONTAINER(fl), fl->controller);
        }
        else if (fl->view_flag == EWL_FILELIST_VIEW_LIST)
        {
                fl->controller = ewl_tree_new();
                ewl_tree_selection_type_set(EWL_TREE(fl->controller),
                                EWL_TREE_SELECTION_TYPE_ROW);
                ewl_tree_column_count_set(EWL_TREE(fl->controller), 6);
                ewl_container_child_append(EWL_CONTAINER(fl), fl->controller);
        }
        /* Until column view is written just default and throw a warning */
        else if (fl->view_flag == EWL_FILELIST_VIEW_COLUMN)
        {
                p = ewl_scrollpane_new();
                ewl_container_child_append(EWL_CONTAINER(fl), p);
                ewl_widget_show(p);

                fl->controller = ewl_vfreebox_mvc_new();
                ewl_container_child_append(EWL_CONTAINER(p), fl->controller);
                DWARNING("Column view not implemented");
        }
        /* Make icon view default */
        else
        {
                p = ewl_scrollpane_new();
                ewl_container_child_append(EWL_CONTAINER(fl), p);
                ewl_widget_show(p);

                fl->controller = ewl_vfreebox_mvc_new();
                ewl_container_child_append(EWL_CONTAINER(p), fl->controller);
        }

        /* Once we have mvc created, set specifics into */
        if (fl->multiselect)
                ewl_mvc_selection_mode_set(EWL_MVC(fl->controller),
                                EWL_SELECTION_MODE_MULTI);
        else
                ewl_mvc_selection_mode_set(EWL_MVC(fl->controller),
                                EWL_SELECTION_MODE_SINGLE);
        ewl_mvc_view_set(EWL_MVC(fl->controller), fl->view);
        ewl_mvc_model_set(EWL_MVC(fl->controller), fl->model);
        ewl_mvc_data_set(EWL_MVC(fl->controller), data);
        ewl_callback_append(EWL_WIDGET(fl->controller),
                        EWL_CALLBACK_CLICKED, ewl_filelist_cb_clicked, fl);
        ewl_widget_show(fl->controller);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to change the view for
 * @param view: The view to set
 * @return: Returns no value
 * @brief Sets the filelist view
 */
void
ewl_filelist_view_set(Ewl_Filelist *fl, Ewl_Filelist_View view)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        if (fl->view_flag == view)
                DRETURN(DLEVEL_STABLE);

        fl->view_flag = view;
        ewl_filelist_view_setup(fl);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the view from
 * @returns Returns the Ewl_Filelist_View used
 * @brief Gets the filelist view
 */
Ewl_Filelist_View *
ewl_filelist_view_get(Ewl_Filelist *fl)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, NULL);

        DRETURN_PTR(fl->view_flag, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to set the directory into
 * @param dir: The directory to set
 * @return Returns no value
 * @brief Sets the given directory @a dir as the current directory in the
 * filelist
 */
void
ewl_filelist_directory_set(Ewl_Filelist *fl, const char *dir)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        if (dir == NULL)
        {
                IF_FREE(fl->directory);
                fl->directory = NULL;
        }
        else if ((!fl->directory) || (strcmp(dir, fl->directory)))
        {
                Ewl_Event_Action_Response ev_data;

                IF_FREE(fl->directory);
                fl->directory = strdup(dir);

                if (fl->fm)
                        ecore_file_monitor_del(fl->fm);
                fl->fm = ecore_file_monitor_add(fl->directory,
                                ewl_filelist_monitor_event, fl);

                ewl_filelist_refresh(fl);
                ev_data.response = EWL_FILELIST_EVENT_DIR_CHANGE;
                ewl_callback_call_with_event_data(EWL_WIDGET(fl),
                                EWL_CALLBACK_VALUE_CHANGED, &ev_data);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns no value
 * @brief Refreshes the filelist when the directory shown is known to have
 * changed
 */
void
ewl_filelist_refresh(Ewl_Filelist *fl)
{
        Ewl_Filelist_Directory *data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        if (!fl->directory)
                DRETURN(DLEVEL_STABLE);

        data = ewl_mvc_data_get(EWL_MVC(fl->controller));
        if (data) ewl_filelist_model_data_unref(data);

        data = ewl_filelist_model_directory_new(fl->directory,
                                        fl->show_dot, TRUE, fl->filter);
        ewl_mvc_data_set(EWL_MVC(fl->controller), data);
        ewl_mvc_dirty_set(EWL_MVC(fl->controller), TRUE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the current directory from
 * @return Returns the current directory
 * @brief Retrieves the current directory set on the filelist
 */
const char *
ewl_filelist_directory_get(Ewl_Filelist *fl)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, NULL);
        DCHECK_TYPE_RET(fl, EWL_FILELIST_TYPE, NULL);

        DRETURN_PTR(fl->directory, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to set the filter into
 * @param filter: The filter to set
 * @return Returns no value.
 * @brief Sets the given filter into the filelist
 */
void
ewl_filelist_filter_set(Ewl_Filelist *fl, Ewl_Filelist_Filter *filter)
{
        Ewl_Filelist_Directory *dir;
        int ret = 0;
        
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_PARAM_PTR(filter);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        fl->filter = NULL;
        fl->filter = filter;
        dir = ewl_mvc_data_get(EWL_MVC(fl->controller));
        
        /* Set the filelist and test if there is any change in data */
        if (dir)
                ret = ewl_filelist_model_filter_set(dir, filter);
        if (ret)
                ewl_mvc_dirty_set(EWL_MVC(fl->controller), TRUE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the filter from
 * @return Returns the current filter.  Do not free returned filter!
 * @brief Retrieves the current filter set on the filelist
 */
Ewl_Filelist_Filter *
ewl_filelist_filter_get(Ewl_Filelist *fl)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, NULL);
        DCHECK_TYPE_RET(fl, EWL_FILELIST_TYPE, NULL);

        DRETURN_PTR(fl->filter, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to set the multiselect value into
 * @param ms: The multiselect value to set
 * @return Returns no value
 * @brief Sets the given multiselect value into the filelist
 */
void
ewl_filelist_multiselect_set(Ewl_Filelist *fl, unsigned int ms)
{
        Ewl_Event_Action_Response ev_data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        if (fl->multiselect == !!ms)
                DRETURN(DLEVEL_STABLE);

        fl->multiselect = !!ms;
                
        if (fl->multiselect)
        {
                ewl_mvc_selection_mode_set(EWL_MVC(fl->controller),
                                         EWL_SELECTION_MODE_MULTI);
                ev_data.response = EWL_FILELIST_EVENT_MULTI_TRUE;
        }
        else
        {
                ewl_mvc_selection_mode_set(EWL_MVC(fl->controller),
                                         EWL_SELECTION_MODE_SINGLE);
                ev_data.response = EWL_FILELIST_EVENT_MULTI_FALSE;
        }

        ewl_callback_call_with_event_data(EWL_WIDGET(fl),
                        EWL_CALLBACK_VALUE_CHANGED, &ev_data);
        
        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the multiselect value from
 * @return Returns the current multiselect state of the filelist
 * @brief Retrieves the current multiselect state of the filelist
 */
unsigned int
ewl_filelist_multiselect_get(Ewl_Filelist *fl)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, FALSE);
        DCHECK_TYPE_RET(fl, EWL_FILELIST_TYPE, FALSE);

        DRETURN_INT((unsigned int)fl->multiselect, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to set the show dot files setting into
 * @param dot: The value to set into the show dot files field
 * @return Returns no value.
 * @brief Sets the show dot files setting to the given value.
 */
void
ewl_filelist_show_dot_files_set(Ewl_Filelist *fl, unsigned int dot)
{
        Ewl_Filelist_Directory *dir;
        int ret = 0;
        
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        if (fl->show_dot == !!dot)
                DRETURN(DLEVEL_STABLE);

        fl->show_dot = !!dot;
        dir = ewl_mvc_data_get(EWL_MVC(fl->controller));

        /* check if data and then call the model function */
        if (dir)
                ret = ewl_filelist_model_show_dot_files_set(dir, dot);
        if (ret)
                ewl_mvc_dirty_set(EWL_MVC(fl->controller), TRUE);
        
        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the value from
 * @return Returns the current show dot files setting of the filelist
 * @brief Retrieves the current show dot files setting for the filelist
 */
unsigned int
ewl_filelist_show_dot_files_get(Ewl_Filelist *fl)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, FALSE);
        DCHECK_TYPE_RET(fl, EWL_FILELIST_TYPE, FALSE);

        DRETURN_INT((unsigned int)fl->show_dot, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to set the selected file into
 * @param file: The file to set selected
 * @return Returns no value
 * @brief Sets the given file as selected in the filelist
 */
void
ewl_filelist_selected_file_set(Ewl_Filelist *fl, const char *file)
{
        char *filename;
        Ewl_Filelist_Directory *data;
        Ewl_Filelist_File *file_temp;
        int dir = 0, index = -1;
        Ecore_List *temp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        if (!file)
        {
                ewl_mvc_selected_clear(EWL_MVC(fl->controller));
                DRETURN(DLEVEL_STABLE);
        }

        filename = ewl_filelist_expand_path(fl, file);
        data = ewl_mvc_data_get(EWL_MVC(fl->controller));
        if (ecore_file_is_dir(filename))
        {
                temp = data->dirs;
                dir = 1;
        }
        else
                temp = data->files;

        ecore_list_first_goto(temp);
        while ((file_temp = ecore_list_next(temp)))
        {
                if (!strcoll(file_temp->name, file))
                {
                        index = ecore_list_index(temp);
                        break;
                }
        }

        if ((index >= 0) && (dir == 0))
                ewl_mvc_selected_set(EWL_MVC(fl->controller), NULL,
                                        NULL, (index + data->num_dirs - 1), 0);
        
        else if ((index >= 0) && (dir == 1))
                ewl_mvc_selected_set(EWL_MVC(fl->controller), NULL,
                                        NULL, (index - 1), 0);

        FREE(filename);
        ewl_filelist_selected_files_change_notify(fl);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the selected file from
 * @return Returns the file currently selected in the filelist
 * @brief Returns the currently selected file from the filelist
 */
char *
ewl_filelist_selected_file_get(Ewl_Filelist *fl)
{
        Ewl_Filelist_Directory *data;
        Ewl_Filelist_File *file;
        Ewl_Selection_Idx *idx;
        char path[PATH_MAX];
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, NULL);
        DCHECK_TYPE_RET(fl, EWL_FILELIST_TYPE, NULL);

        if (!ewl_mvc_selected_count_get(EWL_MVC(fl->controller)))
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        idx = ewl_mvc_selected_get(EWL_MVC(fl->controller));
        data = EWL_SELECTION(idx)->data;
        if (idx->row < data->num_dirs)
                file = ecore_list_index_goto(data->dirs, idx->row);
        else
        {
                i = (idx->row - data->num_dirs);
                file = ecore_list_index_goto(data->files, i);
        }
        FREE(idx);

        if (!file)
        {
                /* This should be fixed now */
                DWARNING("MVC has selected file, but cannot find the data!");
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        if (!strcmp(file->name, ".."))
                snprintf(path, PATH_MAX, "%s", data->name);

        else if (!strcmp(data->name, "/"))
                snprintf(path, PATH_MAX, "%s%s", data->name, file->name);

        else
                snprintf(path, PATH_MAX, "%s/%s", data->name, file->name);

        DRETURN_PTR(strdup(path), DLEVEL_STABLE);
}

/**
 * @param st_size: The size to convert
 * @return Returns a string representation of the given size
 * @brief Converts the given size into a human readable format
 */
char *
ewl_filelist_size_get(off_t st_size)
{
        double dsize;
        char size[1024], *suffix;

        DENTER_FUNCTION(DLEVEL_STABLE);

        dsize = (double)st_size;
        if (dsize < 1024)
                snprintf(size, sizeof(size), "%.0f b", dsize);
        else
        {
                dsize /= 1024.0;
                if (dsize < 1024)
                        suffix = "kb";
                else
                {
                        dsize /= 1024.0;
                        if (dsize < 1024)
                                suffix = "mb";
                        else
                        {
                                dsize /= 1024.0;
                                suffix = "gb";
                        }
                }
                snprintf(size, sizeof(size), "%.1f %s", dsize, suffix);
        }

        DRETURN_PTR(strdup(size), DLEVEL_STABLE);
}

/**
 * @param st_mode: The mode setting to convert
 * @return Returns the string of the given mode setting
 * @brief Converts the given mode settings into a human readable string
 */
char *
ewl_filelist_perms_get(mode_t st_mode)
{
        char *perm;
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);

        perm = (char *)malloc(sizeof(char) * 10);
        for (i = 0; i < 9; i++)
                perm[i] = '-';

        perm[9] = '\0';

        if ((S_IRUSR & st_mode) == S_IRUSR) perm[0] = 'r';
        if ((S_IWUSR & st_mode) == S_IWUSR) perm[1] = 'w';
        if ((S_IXUSR & st_mode) == S_IXUSR) perm[2] = 'x';

        if ((S_IRGRP & st_mode) == S_IRGRP) perm[3] = 'r';
        if ((S_IWGRP & st_mode) == S_IWGRP) perm[4] = 'w';
        if ((S_IXGRP & st_mode) == S_IXGRP) perm[5] = 'x';

        if ((S_IROTH & st_mode) == S_IROTH) perm[6] = 'r';
        if ((S_IWOTH & st_mode) == S_IWOTH) perm[7] = 'w';
        if ((S_IXOTH & st_mode) == S_IXOTH) perm[8] = 'x';

        DRETURN_PTR(perm, DLEVEL_STABLE);
}

/**
 * @param st_uid: The userid to lookup. On Windows, this parameter
 *                is unused.
 * @return Returns the user name for the given user id
 * @brief Convertes the given user id into the approriate user name
 */
char *
ewl_filelist_username_get(uid_t st_uid)
{
        char name[PATH_MAX];
#ifdef HAVE_PWD_H
        struct passwd *pwd = NULL;
#endif /* HAVE_PWD_H */

        DENTER_FUNCTION(DLEVEL_STABLE);

#ifdef HAVE_PWD_H
        pwd = getpwuid(st_uid);
        if (pwd)
                snprintf(name, PATH_MAX, "%s", pwd->pw_name);
        else
#endif /* HAVE_PWD_H */
                snprintf(name, PATH_MAX, "%-8d", (int)st_uid);

        DRETURN_PTR(strdup(name), DLEVEL_STABLE);
}

/**
 * @param st_gid: The group id to convert On Windows, this parameter
 *                is unused.
 * @return Returns the group name for the given id
 * @brief Converts the given group id into a group name
 */
char *
ewl_filelist_groupname_get(gid_t st_gid)
{
        char name[PATH_MAX];
#ifdef HAVE_GRP_H
        struct group *grp;
#endif /* HAVE_GRP_H */

        DENTER_FUNCTION(DLEVEL_STABLE);

#ifdef HAVE_GRP_H
        grp = getgrgid(st_gid);
        if (grp)
                snprintf(name, PATH_MAX, "%s", grp->gr_name);
        else
#endif /* HAVE_GRP_H */
                snprintf(name, PATH_MAX, "%-8d", (int)st_gid);

        DRETURN_PTR(strdup(name), DLEVEL_STABLE);
}

/**
 * @param st_modtime: The modification time to convert
 * @return Returns the string version of the modtime
 * @brief Converts the given modtime to a human readable string
 */
char *
ewl_filelist_modtime_get(time_t st_modtime)
{
        char *time;

        DENTER_FUNCTION(DLEVEL_STABLE);

        time = ctime(&st_modtime);
        if (time)
        {
                time = strdup(time);
                time[strlen(time) - 1] = '\0';
        }
        else time = strdup("Unknown");

        DRETURN_PTR(time, DLEVEL_STABLE);
}

/**
 * @param fl: The Ewl_Filelist to work with
 * @param path: The file to get the preview for
 * @return Returns the preview widget for the given file
 * @brief Creates and returns a preview widget for the given file
 */
Ewl_Widget *
ewl_filelist_selected_file_preview_get(Ewl_Filelist *fl, const char *path)
{
        Ewl_Widget *box, *icon, *text, *image;
        const char *path2;
        char path3[PATH_MAX], file_info[PATH_MAX];
        char *size, *perms, *username, *groupname, *time;
        struct stat buf;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, NULL);
        DCHECK_PARAM_PTR_RET(path, NULL);
        DCHECK_TYPE_RET(fl, EWL_FILELIST_TYPE, NULL);

        path2 = ewl_filelist_directory_get(EWL_FILELIST(fl));
        snprintf(path3, PATH_MAX, "%s/%s", path2, path);

        stat(path3, &buf);

        size = ewl_filelist_size_get(buf.st_size);
        perms = ewl_filelist_perms_get(buf.st_mode);
        username = ewl_filelist_username_get(buf.st_uid);
        groupname = ewl_filelist_groupname_get(buf.st_gid);
        time = ewl_filelist_modtime_get(buf.st_mtime);

        snprintf(file_info, PATH_MAX,
                                "Size: %s\n"
                                "User ID: %s\n"
                                "Group ID: %s\n"
                                "Permissions: %s\n"
                                "Last Modified: %s\n",
                        size, username, groupname,
                        perms, time);

        box = ewl_vbox_new();
        ewl_widget_show(box);

        image = ewl_image_thumbnail_new();
        ewl_image_proportional_set(EWL_IMAGE(image), TRUE);
        ewl_image_constrain_set(EWL_IMAGE(image), 100);
        ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(image), path3);
        ewl_container_child_append(EWL_CONTAINER(box), image);
        ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
        ewl_widget_show(image);

        text = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(text), file_info);
        ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
        ewl_widget_show(text);

        icon = ewl_icon_new();
        ewl_box_orientation_set(EWL_BOX(icon),
                        EWL_ORIENTATION_VERTICAL);
        ewl_icon_label_set(EWL_ICON(icon), path);
        ewl_icon_extended_data_set(EWL_ICON(icon), text);
        ewl_icon_type_set(EWL_ICON(icon), EWL_ICON_TYPE_LONG);
        ewl_container_child_append(EWL_CONTAINER(box), icon);
        ewl_widget_show(icon);

        FREE(size);
        FREE(perms);
        FREE(username);
        FREE(groupname);
        FREE(time);

        DRETURN_PTR(box, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Return the widget to display
 * @brief Sets up a widget to display when multiple files selected
 */
Ewl_Widget *
ewl_filelist_multi_select_preview_get(Ewl_Filelist *fl)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, NULL);
        DCHECK_TYPE_RET(fl, EWL_FILELIST_TYPE, NULL);

        Ewl_Widget *box, *icon;

        box = ewl_vbox_new();
        ewl_widget_show(box);

        icon = ewl_icon_simple_new();
        ewl_box_orientation_set(EWL_BOX(icon),
                        EWL_ORIENTATION_VERTICAL);
        ewl_icon_label_set(EWL_ICON(icon), "Multiple files selected");
        ewl_container_child_append(EWL_CONTAINER(box), icon);
        ewl_widget_show(icon);

        DRETURN_PTR(box, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to set the selected files into
 * @param files: The Ecore_List of the files
 * @return Returns no value.
 * @brief Sets the given files as selected in the filelist
 */
void
ewl_filelist_selected_files_set(Ewl_Filelist *fl, Ecore_List *files)
{
        Ewl_Filelist_Directory *data;
        Ewl_Filelist_File *file;
        Ecore_List *selected, *temp;
        Ewl_Selection *sel;
        int i, index;
        char *path;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_PARAM_PTR(files);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        /* Get the mvc data and get ready for looping */
        data = ewl_mvc_data_get(EWL_MVC(fl->controller));
        selected = ecore_list_new();

        ecore_list_first_goto(data->dirs);
        ecore_list_first_goto(data->files);
        ecore_list_first_goto(files);

        /* For every file, search for a match */
        for (i = 0; i < ecore_list_count(files); i++)
        {
                path = ecore_list_next(files);

                /* Set temp to whichever list the file could be in */
                if (ecore_file_is_dir(path))
                {
                        temp = data->dirs;
                        index = 0;
                }
                else
                {
                        temp = data->files;
                        index = data->num_dirs;
                }

                /* Search the list and return the index if found */
                while ((file = ecore_list_next(temp)))
                {
                        if (!strcmp(file->name, ecore_file_file_get(path)))
                        {
                                index = (index + ecore_list_index(temp) - 1);
                                sel = ewl_mvc_selection_index_new(fl->model, 
                                                        NULL, index, 0);
                                ecore_list_append(selected, sel);
                                break;
                        }
                }
                ecore_list_first_goto(temp);
        }                

        ewl_mvc_selected_list_set(EWL_MVC(fl->controller), selected);
        ewl_filelist_selected_files_change_notify(fl);
        ecore_list_destroy(files);
        ecore_list_destroy(selected);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the selected files from
 * @return Returns the Ecore_List of selected files in the filelist
 * @brief Retrieves the list of selected files in the filelist
 */
Ecore_List *
ewl_filelist_selected_files_get(Ewl_Filelist *fl)
{
        Ecore_List *selected, *ret;
        Ewl_Selection *sel;
        Ewl_Filelist_Directory *data;
        Ewl_Filelist_File *file;
        char path[PATH_MAX];

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, NULL);
        DCHECK_TYPE_RET(fl, EWL_FILELIST_TYPE, NULL);

        ret = ecore_list_new();

        selected = ewl_mvc_selected_list_get(EWL_MVC(fl->controller));
        ecore_list_first_goto(selected);
        while ((sel = ecore_list_next(selected)))
        {
                data = sel->data;
                /* If using Index instead of range */
                if (sel->type == EWL_SELECTION_TYPE_INDEX)
                {
                        Ewl_Selection_Idx *idx;
                        
                        idx = EWL_SELECTION_IDX(sel);
                        /* Get the file data */
                        if (idx->row < data->num_dirs)
                                file = ecore_list_index_goto(data->dirs, idx->row);
                        else
                                file = ecore_list_index_goto(data->files,
                                                 (idx->row - data->num_dirs));

                        if (!file)
                        {
                                /* This should be fixed now */
                                DWARNING("MVC has selected file, but cannot "
                                                "find the data!");
                                continue;
                        }

                        if (!strcmp(file->name, ".."))
                                snprintf(path, PATH_MAX, "%s", data->name);

                        else if (!strcmp(data->name, "/"))
                                snprintf(path, PATH_MAX, "%s%s",
                                                data->name, file->name);

                        else
                                snprintf(path, PATH_MAX, "%s/%s",
                                                data->name, file->name);
                        
                        ecore_list_append(ret, strdup(path));
                }

                /* If using range instead of index */
                else if (sel->type == EWL_SELECTION_TYPE_RANGE)
                {
                        Ewl_Selection_Range *r;
                        unsigned int i;

                        r = EWL_SELECTION_RANGE(sel);
                        for (i = r->start.row; i <= r->end.row; i++)
                        {
                                /* Get the file data */
                                if (i < data->num_dirs)
                                        file = ecore_list_index_goto(data->dirs, i);
                                else
                                        file = ecore_list_index_goto(data->files,
                                                 (i - data->num_dirs));

                                if (!file)
                                {
                                        /* This should be fixed now */
                                        DWARNING("MVC has selected file, but "
                                                "cannot find the data!");
                                        continue;
                                }

                                if (!strcmp(file->name, ".."))
                                        snprintf(path, PATH_MAX, "%s", data->name);

                                else if (!strcmp(data->name, "/"))
                                        snprintf(path, PATH_MAX, "%s%s",
                                                data->name, file->name);

                                else
                                        snprintf(path, PATH_MAX, "%s/%s",
                                                data->name, file->name);
                                
                                ecore_list_append(ret, strdup(path));
                        }
                }
        }        

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns no value.
 * @brief Notifies interested consumers that the filelist has changed
 * selected values
 */
void
ewl_filelist_selected_files_change_notify(Ewl_Filelist *fl)
{
        Ewl_Event_Action_Response ev_data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        ev_data.response = EWL_FILELIST_EVENT_SELECTION_CHANGE;
        ewl_callback_call_with_event_data(EWL_WIDGET(fl),
                        EWL_CALLBACK_VALUE_CHANGED, &ev_data);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @param v: The value to set for the vertical scrollbar
 * @return Returns no value
 * @brief Sets the value to use for flags on the vertical scrollbar
 */
void
ewl_filelist_vscroll_flag_set(Ewl_Filelist *fl, Ewl_Scrollpane_Flags v)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        fl->scroll_flags.v = v;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns the flags for the vertical scrollbar
 * @brief Retrieves the flags for the vertical scrollbar
 */
Ewl_Scrollpane_Flags
ewl_filelist_vscroll_flag_get(Ewl_Filelist *fl)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, EWL_SCROLLPANE_FLAG_NONE);
        DCHECK_TYPE_RET(fl, EWL_FILELIST_TYPE,
                                        EWL_SCROLLPANE_FLAG_NONE);

        DRETURN_INT(fl->scroll_flags.v, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @param h: The value to set for the horizontal scrollbar
 * @return Returns no value
 * @brief Sets the value to use for flags on the horizontal scrollbar
 */
void
ewl_filelist_hscroll_flag_set(Ewl_Filelist *fl, Ewl_Scrollpane_Flags h)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fl);
        DCHECK_TYPE(fl, EWL_FILELIST_TYPE);

        fl->scroll_flags.h = h;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns the flags for the horizontal scrollbar
 * @brief Retrieves the flags for the horizontal scrollbar
 */
Ewl_Scrollpane_Flags
ewl_filelist_hscroll_flag_get(Ewl_Filelist *fl)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, EWL_SCROLLPANE_FLAG_NONE);
        DCHECK_TYPE_RET(fl, EWL_FILELIST_TYPE,
                                        EWL_SCROLLPANE_FLAG_NONE);

        DRETURN_INT(fl->scroll_flags.h, DLEVEL_STABLE);
}

/**
 * @param path: The path to get the icon for
 * @return Returns the stock icon for the given file
 * @brief Retrieves the stock icon for the given file
 */
const char *
ewl_filelist_stock_icon_get(const char *path)
{
        const char *ret = NULL;
        char *ptr = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(path, NULL);

        if (ecore_file_is_dir(path))
                DRETURN_PTR(EWL_ICON_FOLDER, DLEVEL_STABLE);

        ptr = strrchr(path, '.');
        if (ptr)
        {
                ret = ewl_io_manager_extension_icon_name_get(ptr);
                if (ret) DRETURN_PTR(ret, DLEVEL_STABLE);
        }

        if (ecore_file_can_exec(path))
                ret = EWL_ICON_APPLICATION_X_EXECUTABLE;
        else
                ret = EWL_ICON_TEXT_X_GENERIC;

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to get the path from
 * @param dir: The dir name to append to the path
 * @return Returns the full path to the given directory
 * @brief This will attempt to return the full path to the given directory.
 * It should handle things like .. as well.
 */
char *
ewl_filelist_expand_path(Ewl_Filelist *fl, const char *dir)
{
        char path[PATH_MAX];
        const char *cur_dir;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fl, NULL);
        DCHECK_PARAM_PTR_RET(dir, NULL);
        DCHECK_TYPE_RET(fl, EWL_FILELIST_TYPE, NULL);

        cur_dir = ewl_filelist_directory_get(EWL_FILELIST(fl));
        if (!strcmp(dir, ".."))
        {
                char *t, *t2;

                snprintf(path, PATH_MAX, "%s", cur_dir);
                t = path;
                t2 = t;
                while ((*t != '\0'))
                {
                        if ((*t == '/') && (*(t + 1) != '\0')) t2 = t;
                        t++;
                }
                *t2 = '\0';

                /* make sure we always have at least / in there */
                if (path[0] == '\0')
                {
                        path[0] = '/';
                        path[1] = '\0';
                }
        }
        else
        {
                /* if the current directory is just / we dont' want to end
                 * up with // on the start of the path. So, check to see if
                 * the second item in cur_dir is a \0, and if so just append
                 * "" instead of getting // twice */
                snprintf(path, PATH_MAX, "%s/%s",
                                ((cur_dir[1] != '\0') ? cur_dir : ""),
                                dir);
        }

        DRETURN_PTR(strdup(path), DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_filelist_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Filelist *fl;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_FILELIST_TYPE);
        
        fl = EWL_FILELIST(w);
        IF_FREE(fl->directory);
        IF_FREE(fl->view);
        IF_FREE(fl->model);
        fl->filter = NULL;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_cb_clicked(Ewl_Widget *w, void *ev,
                                void *data __UNUSED__)
{
        Ewl_Event_Mouse_Down *md;
        char *file, *t;
        int i = 0;
        Ewl_Widget *c;
        Ewl_Filelist *fl;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev);

        md = ev;
        fl = data;

        if (!ewl_mvc_selected_count_get(EWL_MVC(fl->controller)))
                DRETURN(DLEVEL_STABLE);

        /* Single clicks only */
        if (md->clicks != 2)
        {
                ewl_filelist_selected_files_change_notify(fl);
                DRETURN(DLEVEL_STABLE);
        }

        /* Ensure that the click is on an icon, need to check for a
         * highlight as the first click callback will create
         */
        c = ewl_container_child_at_recursive_get(EWL_CONTAINER(fl),
                                md->base.x, md->base.y);
        while (c && c->parent)
        {
                if (!ewl_widget_internal_is(c))
                {
                        i = 1;
                        break;
                }
                c = c->parent;
        }

        if (!i)
                DRETURN(DLEVEL_STABLE);

        /* Handle double clicks */
        file = ewl_filelist_selected_file_get(fl);
        /* If .. */
        if (!strcmp(file, fl->directory))
        {
                t = ecore_file_dir_get(fl->directory);
                ewl_filelist_directory_set(fl, t);
                FREE(t);
        }
        /* Change dir if dir, else call above */
        else if (ecore_file_is_dir(file))
                ewl_filelist_directory_set(fl, file);
        
        /* Send signal of file selected */
        else
                ewl_filelist_selected_files_change_notify(fl);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_filelist_monitor_event(void *data, Ecore_File_Monitor *em __UNUSED__,
                                        Ecore_File_Event event __UNUSED__,
                                        const char *path __UNUSED__)
{
        Ewl_Filelist *fl = data;

        ewl_filelist_refresh(fl);
}

