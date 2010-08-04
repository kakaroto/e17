#ifndef EWL_FILELIST_FILE_H
#define EWL_FILELIST_FILE_H

typedef struct Ewl_Filelist_File Ewl_Filelist_File;

/**
 * @def EWL_FILELIST_FILE(fl)
 * Typecasts a pointer to an Ewl_Filelist_File pointer
 */
#define EWL_FILELIST_FILE(fl) ((Ewl_Filelist_File *)fl)

Ewl_Filelist_File       *ewl_filelist_file_new(void);
void                     ewl_filelist_file_destroy(Ewl_Filelist_File *file);

unsigned int             ewl_filelist_file_path_set(Ewl_Filelist_File *file,
                                                const char *path);
char                    *ewl_filelist_file_size_get(const Ewl_Filelist_File
                                                *file);
char                    *ewl_filelist_file_perms_get(const Ewl_Filelist_File
                                                *file);
char                    *ewl_filelist_file_username_get(const Ewl_Filelist_File
                                                *file);
char                    *ewl_filelist_file_groupname_get(const
                                                Ewl_Filelist_File *file);
char                    *ewl_filelist_file_modtime_get(const Ewl_Filelist_File
                                                *file);
const char              *ewl_filelist_file_name_get(const Ewl_Filelist_File
                                                *file);
const char              *ewl_filelist_file_path_get(const Ewl_Filelist_File
                                                *file);

unsigned char            ewl_filelist_file_is_dir(const Ewl_Filelist_File
                                                *file);
unsigned char            ewl_filelist_file_is_readable(const Ewl_Filelist_File
                                                *file);
unsigned char            ewl_filelist_file_is_writeable(const Ewl_Filelist_File
                                                *file);

int                      ewl_filelist_file_name_compare(
                                        const Ewl_Filelist_File *file1,
                                        const Ewl_Filelist_File *file2);
int                      ewl_filelist_file_size_compare(
                                        const Ewl_Filelist_File *file1,
                                        const Ewl_Filelist_File *file2);
int                      ewl_filelist_file_modified_compare(
                                        const Ewl_Filelist_File *file1,
                                        const Ewl_Filelist_File *file2);


void                     ewl_filelist_file_name_set(Ewl_Filelist_File *file,
                                                 const char *path);

#endif
