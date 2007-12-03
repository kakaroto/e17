#ifndef EWL_FILELIST_MODEL_H
#define EWL_FILELIST_MODEL_H

/**
 * @addtogroup Ewl_Filelist_Model Ewl_Filelist_Model: The model for the filelist
 * @brief A model for the filelist
 *
 * @{
 */

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
 *  * @def EWL_FILELIST_DIRECTORY_TYPE
 *   * The type name
 *    */
#define EWL_FILELIST_DIRECTORY_TYPE "filelist_directory"

typedef struct Ewl_Filelist_Directory Ewl_Filelist_Directory;

/**
 *  * @def EWL_FILELIST_DIRECTORY(fl)
 *   * Typecasts a pointer to an Ewl_Filelist_Directory pointer
 *    */
#define EWL_FILELIST_DIRECTORY(fl) ((Ewl_Filelist_Directory *)fl)

/**
 *  * @brief The data structure for Ewl_Filelist_Directory
 *   */
struct Ewl_Filelist_Directory
{
	const char *name;
	Ecore_List *files;
	Ecore_List *dirs;
	unsigned char skip_hidden:1;
	unsigned int num_dirs;
	unsigned int num_files;
};

Ewl_Filelist_Directory	*ewl_filelist_model_directory_new(const char *path,
	       					unsigned char skip_hidden,
						unsigned int show_dot_dot);
unsigned int ewl_filelist_model_data_count(void *data);
void *ewl_filelist_model_data_fetch(void *data, unsigned int row,
						unsigned int column);
void ewl_filelist_model_data_sort(void *data, unsigned int column,
						Ewl_Sort_Direction sort);
int ewl_filelist_model_data_expandable_get(void *data, 
						unsigned int row);
void *ewl_filelist_model_data_expansion_data_fetch(void *data, 
						unsigned int parent);
unsigned int ewl_filelist_model_data_unref(void *data);
int ewl_filelist_model_column_sortable(void *data, 
						unsigned int column);

/**
 * @}
 */

#endif

