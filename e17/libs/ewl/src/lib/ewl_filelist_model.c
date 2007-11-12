#include "ewl_base.h"
#include "ewl_filelist_model.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_model.h"
#include "ewl_filelist.h"
#include <dirent.h>
#include <locale.h>

static int ewl_filelist_model_data_name_sort(Ewl_Filelist_File *file1,
				Ewl_Filelist_File *file2);
static int ewl_filelist_model_data_size_sort(Ewl_Filelist_File *file1,
				Ewl_Filelist_File *file2);
static void free_file(Ewl_Filelist_File *file);

/**
 * @param path: The path to the directory to read
 * @param skip_hidden: TRUE skips hidden files, FALSE does not
 * @param show_dot_dot: TRUE shows .. for navigating upwards, FALSE does not
 * @return Returns an Ewl_Filelist_Directory structure
 * @brief Retrieves all files in a directory
 */
Ewl_Filelist_Directory *
ewl_filelist_model_directory_new(const char *path, 
					unsigned char skip_hidden,
					unsigned int show_dot_dot)
{
	Ewl_Filelist_Directory *dir;
	Ewl_Filelist_File *file;
	struct stat st;
	char filename[PATH_MAX], *file_temp;
	int nf = 0, nd = 0;
	Ecore_List *files, *dirs, *all_files;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(path, NULL);

	files = ecore_list_new();
	dirs = ecore_list_new();
	ecore_list_free_cb_set(files, ECORE_FREE_CB(free_file));
	ecore_list_free_cb_set(dirs, ECORE_FREE_CB(free_file));

	all_files = ecore_file_ls(path);

	/* Add in the ".." entry for now */
	if (show_dot_dot)
		ecore_list_prepend(all_files, strdup(path));

	while ((file_temp = ecore_list_first_remove(all_files)))
	{
		/* Handle hidden files */
		if ((skip_hidden) && (file_temp[0] == '.'))
		{
			FREE(file_temp);
			continue;
		}

		/* allocate the memory for the file structure */
		file = NEW(Ewl_Filelist_File, 1);

		/* test for .. */
		if (strcmp(file_temp, path))
		{
			snprintf(filename, PATH_MAX, "%s/%s", path,
							file_temp);
			file->name = ecore_string_instance(file_temp);
		}
		else
		{
			snprintf(filename, PATH_MAX, "%s", file_temp);
			file->name = ecore_string_instance("..");
		}

		/* info about the filename */
		stat(filename, &st);
		file->size = st.st_size;
		file->modtime = st.st_mtime;
		file->mode = st.st_mode;
		file->groupname = st.st_gid;
		file->username = st.st_uid;
		file->is_dir = ecore_file_is_dir(filename);
		file->readable = ecore_file_can_read(filename);
		file->writeable = ecore_file_can_write(filename);

		if ((file->is_dir))
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

	dir->files = files;
	dir->dirs = dirs;
	dir->name = ecore_string_instance(path);
	dir->skip_hidden = !!skip_hidden;
	dir->num_dirs = nd;
	dir->num_files = nf;

	IF_FREE_LIST(all_files);
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
	DRETURN_INT(((column == 0) || (column == 1)), DLEVEL_STABLE);
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

	if (column == 0) ret = strdup(file->name);
	else if (column == 1) ret = ewl_filelist_size_get(file->size);
	else if (column == 2) ret = ewl_filelist_perms_get(file->mode);
	else if (column == 3) ret = ewl_filelist_username_get
						(file->username);
	else if (column == 4) ret = ewl_filelist_groupname_get
						(file->groupname);
	else if (column == 5) ret = ewl_filelist_modtime_get
						(file->modtime);
	else ret = NULL;

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
	DCHECK_PARAM_PTR(sort);

	if (sort == EWL_SORT_DIRECTION_NONE)
		DRETURN(DLEVEL_STABLE);

	/* Remove .. entry to add in later */
	if (fld->num_dirs != 0)
	{
		file = ecore_list_first(fld->dirs);
		if (!strcmp(file->name, ".."))
			root = ecore_list_first_remove(fld->dirs);
	}

	if (column == 0)
	{
		if (sort == EWL_SORT_DIRECTION_ASCENDING)
		{
			ecore_list_sort(fld->dirs,
				ECORE_COMPARE_CB
				(ewl_filelist_model_data_name_sort),
				ECORE_SORT_MIN);

			ecore_list_sort(fld->files,
				ECORE_COMPARE_CB
				(ewl_filelist_model_data_name_sort),
				ECORE_SORT_MIN);
		}
		
		else if (sort == EWL_SORT_DIRECTION_DESCENDING)
		{
			ecore_list_sort(fld->dirs,
				ECORE_COMPARE_CB
				(ewl_filelist_model_data_name_sort),
				ECORE_SORT_MAX);

			ecore_list_sort(fld->files,
				ECORE_COMPARE_CB
				(ewl_filelist_model_data_name_sort),
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
				(ewl_filelist_model_data_size_sort),
				ECORE_SORT_MIN);
		}

		else if (sort == EWL_SORT_DIRECTION_DESCENDING)
		{
			ecore_list_sort(fld->files,
				ECORE_COMPARE_CB
				(ewl_filelist_model_data_size_sort),
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
static int
ewl_filelist_model_data_name_sort(Ewl_Filelist_File *file1, 
					Ewl_Filelist_File *file2)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(file1, 0);
	DCHECK_PARAM_PTR_RET(file2, 0);
	DRETURN_INT((strcoll(file1->name, file2->name)), DLEVEL_STABLE);
}

/**
 * @internal
 */
static int
ewl_filelist_model_data_size_sort(Ewl_Filelist_File *file1,
					Ewl_Filelist_File *file2)
{
	int ret;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(file1, 0);
	DCHECK_PARAM_PTR_RET(file2, 0);

	if (file1->size > file2->size)
		ret = 1;
	else if (file1->size < file2->size)
		ret = -1;
	else ret = 0;

	DRETURN_INT(ret, DLEVEL_STABLE);
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
	if (!strcmp(file->name, ".."))
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
	char path[PATH_MAX];
	Ewl_Filelist_File *file;
	Ewl_Filelist_Directory *fld = data, *subdir;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(data, NULL);

	file = ecore_list_index_goto(fld->dirs, parent);
	snprintf(path, PATH_MAX, "%s/%s", fld->name, file->name);
	subdir = ewl_filelist_model_directory_new
					(path, fld->skip_hidden, 
					 FALSE);

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
	FREE(dir);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 */
static void free_file(Ewl_Filelist_File *file)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(file);

	ecore_string_release(file->name);
	FREE(file);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

