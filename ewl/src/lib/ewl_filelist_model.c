#include "ewl_base.h"
#include "ewl_filelist_model.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_model.h"
#include "ewl_filelist.h"
#include "ewl_filelist_file.h"
#include "ewl_io_manager.h"
#include <fnmatch.h>

void ewl_filelist_model_filter(Ewl_Filelist_Directory *dir);
static unsigned int ewl_filelist_model_filter_main
                                (Ewl_Filelist_Directory *dir,
                                Ewl_Filelist_File *file);

/**
 * @param path: The path to the directory to read
 * @param show_dot: TRUE shows dot files, FALSE does not
 * @param show_dot_dot: TRUE shows .. for navigating upwards, FALSE does not
 * @param filter: The Ewl_Filelist_Filter to use
 * @return Returns an Ewl_Filelist_Directory structure
 * @brief Retrieves all files in a directory
 */
Ewl_Filelist_Directory *
ewl_filelist_model_directory_new(const char *path,
                                        unsigned char show_dot,
                                        unsigned int show_dot_dot,
                                        Ewl_Filelist_Filter *filter)
{
        Ewl_Filelist_Directory *dir;
        Ewl_Filelist_File *file;
        char filename[PATH_MAX], *file_temp;
        int nf = 0, nd = 0;
        Eina_List *all_files;
        Ecore_List *dirs, *files;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(path, NULL);

        files = ecore_list_new();
        dirs = ecore_list_new();
        ecore_list_free_cb_set(files,
                        ECORE_FREE_CB(ewl_filelist_file_destroy));
        ecore_list_free_cb_set(dirs,
                        ECORE_FREE_CB(ewl_filelist_file_destroy));

        all_files = ecore_file_ls(path);
        if (!all_files)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        /* Add in the ".." entry for now */
        if ((show_dot_dot) && (strcmp(path, "/")))
                all_files = eina_list_prepend(all_files, strdup(path));

        EINA_LIST_FREE(all_files, file_temp)
        {
                /* allocate the memory for the file structure */
                file = ewl_filelist_file_new();
                if (!file)
                {
                        ecore_list_destroy(files);
                        ecore_list_destroy(dirs);
                        DRETURN_PTR(NULL, DLEVEL_STABLE);
                }

                /* test for .. */
                if (strcmp(file_temp, path))
                {
                        snprintf(filename, PATH_MAX, "%s/%s", path,
                                                        file_temp);
                        ewl_filelist_file_name_set(file, file_temp);
                }
                else
                {
                        snprintf(filename, PATH_MAX, "%s", file_temp);
                        ewl_filelist_file_name_set(file, "..");
                }

                /* Generate the file information */
                if (!ewl_filelist_file_path_set(file, filename))
                {
                        ewl_filelist_file_destroy(file);
                        ecore_list_destroy(files);
                        ecore_list_destroy(dirs);
                        DRETURN_PTR(NULL, DLEVEL_STABLE);
                }

                if (ewl_filelist_file_is_dir(file))
                {
                        ecore_list_append(dirs, file);
                        nd = nd + 1;
                }

                else
                {
                        ecore_list_append(files, file);
                        nf = nf + 1;
                }

                FREE(file_temp);
        }

        /* create the new directory structure */
        dir = NEW(Ewl_Filelist_Directory, 1);

        dir->rfiles = files;
        dir->rdirs = dirs;
        dir->files = ecore_list_new();
        dir->dirs = ecore_list_new();
        dir->name = ecore_string_instance(path);
        dir->show_dot = !!show_dot;
        dir->filter = filter;
        dir->num_dirs = nd;
        dir->num_files = nf;

        /* Filter the directory */
        ewl_filelist_model_filter(dir);

        DRETURN_PTR(dir, DLEVEL_STABLE);
}


/*
 * @internal
 */
int
ewl_filelist_model_column_sortable(void *data __UNUSED__, 
                                        unsigned int column)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DRETURN_INT(((column == 0) || (column == 1) ||
                                (column == 5)), DLEVEL_STABLE);
}

/**
 * @internal
 */
unsigned int
ewl_filelist_model_data_count(void *data)
{
        Ewl_Filelist_Directory *fld = data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, 0);

        DRETURN_INT((fld->num_dirs + fld->num_files), DLEVEL_STABLE);
}

/**
 * @internal
 */
void *
ewl_filelist_model_data_fetch(void *data, unsigned int row, 
                                                unsigned int column)
{
        Ewl_Filelist_Directory *fld;
        Ewl_Filelist_File *file;
        int i;
        void *ret;
        const char *name;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, NULL);

        fld = data;

        /* Check if in dirs or files list */
        if (row < fld->num_dirs)
                file = ecore_list_index_goto(fld->dirs, row);
        else
        {
                i = (row - fld->num_dirs);
                file = ecore_list_index_goto(fld->files, i);
        }

        name = ewl_filelist_file_name_get(file);
        if ((column == 0) && (strcmp(name, "..")))
                ret = strdup(ewl_filelist_file_path_get(file));
        else if (column == 1) ret = ewl_filelist_file_size_get(file);
        else if (column == 2) ret = ewl_filelist_file_perms_get(file);
        else if (column == 3) ret = ewl_filelist_file_username_get(file);
        else if (column == 4) ret = ewl_filelist_file_groupname_get(file);
        else if (column == 5) ret = ewl_filelist_file_modtime_get(file);
        else ret = strdup(name);

        /* ret needs to be freed by the view or with model_data_free_set */
        DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @internal
 */
void
ewl_filelist_model_data_sort(void *data, unsigned int column,
                                                Ewl_Sort_Direction sort)
{

        Ewl_Filelist_Directory *fld = data;
        Ewl_Filelist_File *file, *root = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);

        if (sort == EWL_SORT_DIRECTION_NONE)
                DRETURN(DLEVEL_STABLE);

        /* Remove .. entry to add in later */
        if (fld->num_dirs != 0)
        {
                file = ecore_list_first(fld->dirs);
                if (!strcmp(ewl_filelist_file_name_get(file), ".."))
                        root = ecore_list_first_remove(fld->dirs);
        }

        if (column == 0)
        {
                if (sort == EWL_SORT_DIRECTION_ASCENDING)
                {
                        ecore_list_sort(fld->dirs,
                                ECORE_COMPARE_CB
                                (ewl_filelist_file_name_compare),
                                ECORE_SORT_MIN);

                        ecore_list_sort(fld->files,
                                ECORE_COMPARE_CB
                                (ewl_filelist_file_name_compare),
                                ECORE_SORT_MIN);
                }

                else if (sort == EWL_SORT_DIRECTION_DESCENDING)
                {
                        ecore_list_sort(fld->dirs,
                                ECORE_COMPARE_CB
                                (ewl_filelist_file_name_compare),
                                ECORE_SORT_MAX);

                        ecore_list_sort(fld->files,
                                ECORE_COMPARE_CB
                                (ewl_filelist_file_name_compare),
                                ECORE_SORT_MAX);
                }
        }

        /* size sort does nothing for directories so don't sort dirs */
        else if (column == 1)
        {
                if (sort == EWL_SORT_DIRECTION_ASCENDING)
                {
                        ecore_list_sort(fld->files,
                                ECORE_COMPARE_CB
                                (ewl_filelist_file_size_compare),
                                ECORE_SORT_MIN);
                }

                else if (sort == EWL_SORT_DIRECTION_DESCENDING)
                {
                        ecore_list_sort(fld->files,
                                ECORE_COMPARE_CB
                                (ewl_filelist_file_size_compare),
                                ECORE_SORT_MAX);
                }
        }
        else if (column == 5)
        {
                if (sort == EWL_SORT_DIRECTION_ASCENDING)
                {
                        ecore_list_sort(fld->dirs,
                                ECORE_COMPARE_CB
                                (ewl_filelist_file_modified_compare),
                                ECORE_SORT_MIN);

                        ecore_list_sort(fld->files,
                                ECORE_COMPARE_CB
                                (ewl_filelist_file_modified_compare),
                                ECORE_SORT_MIN);
                }

                else if (sort == EWL_SORT_DIRECTION_DESCENDING)
                {
                        ecore_list_sort(fld->dirs,
                                ECORE_COMPARE_CB
                                (ewl_filelist_file_modified_compare),
                                ECORE_SORT_MAX);

                        ecore_list_sort(fld->files,
                                ECORE_COMPARE_CB
                                (ewl_filelist_file_modified_compare),
                                ECORE_SORT_MAX);
                }
        }

        /* Put .. entry back in */
        if (root)
                ecore_list_prepend(fld->dirs, root);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 */
int
ewl_filelist_model_data_expandable_get(void *data, unsigned int row)
{
        Ewl_Filelist_File *file;
        Ewl_Filelist_Directory *fld = data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, 0);

        if (row >= fld->num_dirs)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        file = ecore_list_index_goto(fld->dirs, row);
        if (!strcmp(ewl_filelist_file_name_get(file), ".."))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 */
void *
ewl_filelist_model_data_expansion_data_fetch(void *data, 
                                                unsigned int parent)
{
        Ewl_Filelist_File *file;
        Ewl_Filelist_Directory *fld = data, *subdir;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, NULL);

        file = ecore_list_index_goto(fld->dirs, parent);
        subdir = ewl_filelist_model_directory_new
                                        (ewl_filelist_file_path_get(file),
                                        fld->show_dot,
                                        FALSE, fld->filter);

        DRETURN_PTR(subdir, DLEVEL_STABLE);
}

/**
 * @internal
 */
unsigned int 
ewl_filelist_model_data_unref(void *data)
{
        Ewl_Filelist_Directory *dir;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, FALSE);

        dir = data;
        ecore_string_release(dir->name);
        ecore_list_destroy(dir->files);
        ecore_list_destroy(dir->dirs);
        ecore_list_destroy(dir->rfiles);
        ecore_list_destroy(dir->rdirs);
        FREE(dir);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param dir: The Ewl_Filelist_Directory to work with
 * @param show_dot: TRUE shows dot files, FALSE does not
 * @return Returns TRUE if a change in data, FALSE if not
 * @brief This function sets whether the filelist should contain hidden files
 */
unsigned int
ewl_filelist_model_show_dot_files_set(Ewl_Filelist_Directory *dir,
                                                        unsigned int show_dot)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dir, FALSE);

        /* If nothing has changed, leave */
        if (show_dot == !!dir->show_dot)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        /* Else set value in */
        dir->show_dot = !!show_dot;

        /* Refilter the files */
        ewl_filelist_model_filter(dir);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param dir: The Ewl_Filelist_Directory to work with
 * @return Returns if the model shows dot files
 * @brief Returns if the model shows dot files
 */
unsigned int
ewl_filelist_model_show_dot_files_get(Ewl_Filelist_Directory *dir)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dir, FALSE);
        DRETURN_INT((unsigned int)dir->show_dot, DLEVEL_STABLE);
}

/**
 * @param dir: The directory to filter
 * @return Does not return a value
 * @brief Filters a directory
 */
void
ewl_filelist_model_filter(Ewl_Filelist_Directory *dir)
{
        Ewl_Filelist_File *file;
        int ret = 0;
        const char *name;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(dir);

        /* Set up temporary filelist */
        ecore_list_clear(dir->files);
        ecore_list_clear(dir->dirs);


        /* Hidden files first */
        if (!dir->show_dot)
        {

                /* Run through files and filter hidden first, then others */
                ecore_list_first_goto(dir->rfiles);
                while ((file = ecore_list_next(dir->rfiles)))
                {
                        ret = ewl_filelist_model_filter_main(dir, file);
                        name = ewl_filelist_file_name_get(file);
                        if ((name[0] != '.') && (ret))
                               ecore_list_append(dir->files, file);

                }

                /* Now directories */
                ecore_list_first_goto(dir->rdirs);
                while ((file = ecore_list_next(dir->rdirs)))
                {
                        name = ewl_filelist_file_name_get(file);
                        if ((name[0] != '.') || (!strcmp(name, "..")))
                                ecore_list_append(dir->dirs, file);
                }
        }

        /* If showing hidden */
        else
        {
                /* Still loop through to check if pass filter */
                ecore_list_first_goto(dir->rfiles);
                while ((file = ecore_list_next(dir->rfiles)))
                {
                        ret = ewl_filelist_model_filter_main(dir, file);
                        if (ret)
                                ecore_list_append(dir->files, file);
                }

                ecore_list_first_goto(dir->rdirs);
                while ((file = ecore_list_next(dir->rdirs)))
                        ecore_list_append(dir->dirs, file);
        }


        /* Set numbers into directory data */
        dir->num_dirs = ecore_list_count(dir->dirs);
        dir->num_files = ecore_list_count(dir->files);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param dir: The directory to set a filter upon
 * @param filter: The Ewl_Filelist_Filter to use
 * @returns: Returns TRUE on data change, FALSE if not
 * @brief Sets a filter onto a directory
 */
unsigned int
ewl_filelist_model_filter_set(Ewl_Filelist_Directory *dir,
                                        Ewl_Filelist_Filter *filter)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dir, FALSE);
        DCHECK_PARAM_PTR_RET(filter, FALSE);

        /* If nothing has changed, leave */
        if ((dir->filter) && (!memcmp(dir->filter, filter,
                                                sizeof(dir->filter))))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        /* Set the filter and call the function */
        dir->filter = filter;
        ewl_filelist_model_filter(dir);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param dir: The directory to get the filter for
 * @returns: Returns the Ewl_Filelist_Filter used
 * @brief Gets the filter used on a directory
 */
Ewl_Filelist_Filter *
ewl_filelist_model_filter_get(Ewl_Filelist_Directory *dir)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dir, NULL);
        DRETURN_PTR(dir->filter, DLEVEL_STABLE);
}

/**
 * @internal
 * @param dir: The directory to get the filter from
 * @param file: The file to test
 * @returns: Returns TRUE if passes the filter, FALSE otherwise
 * @brief Tests a file against a filter
 */
static unsigned int
ewl_filelist_model_filter_main(Ewl_Filelist_Directory *dir,
                                        Ewl_Filelist_File *file)
{
        Ewl_Filelist_Filter *filter = dir->filter;
        const char *mime_check, *name;
        char *mime_given;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dir, FALSE);
        DCHECK_PARAM_PTR_RET(file, FALSE);

        /* If there's no filter return all files */
        if (!filter)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        name = ewl_filelist_file_name_get(file);
        /* First check mime types */
        if (filter->mime_list)
        {
                mime_check = ewl_io_manager_uri_mime_type_get(name);
                ecore_list_first_goto(filter->mime_list);
                while ((mime_given = ecore_list_next(filter->mime_list)))
                {
                        if ((mime_check) && (!strcmp(mime_given,
                                                        mime_check)))
                                DRETURN_INT(TRUE, DLEVEL_STABLE);
                }
        }

        /* Next check extension */
        else if (filter->extension)
        {
                if (!fnmatch(filter->extension, name, 0))
                        DRETURN_INT(TRUE, DLEVEL_STABLE);
        }

        /* If no filter at all is in effect */
        else if ((!filter->extension) && (!filter->mime_list))
        {
                DRETURN_INT(TRUE, DLEVEL_STABLE);
        }

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

