#include "ewl_base.h"
#include "ewl_debug.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_filelist_file.h"
#include <dirent.h>
#include <Ecore_File.h>

#include <sys/types.h>
#if HAVE_PWD_H
# include <pwd.h>
#endif /* HAVE_PWD_H */
#if HAVE_GRP_H
# include <grp.h>
#endif /* HAVE_GRP_H */
#include <time.h>
#if HAVE_EVIL
# include <Evil.h>
#endif /* HAVE_EVIL */

/**
 * @brief The data structure for Ewl_Filelist_File
 */
struct Ewl_Filelist_File
{
        const char *path;
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
 * @return Returns a new Ewl_Filelist_File object or NULL on failure
 * @brief Creates a new Ewl_Filelist_File object
 */
Ewl_Filelist_File *
ewl_filelist_file_new(void)
{
        Ewl_Filelist_File *file;

        DENTER_FUNCTION(DLEVEL_STABLE);

        file = NEW(Ewl_Filelist_File, 1);

        DRETURN_PTR(file, DLEVEL_STABLE);
}

/**
 * @param file: The Ewl_Filelist_File to destroy
 * @brief Destroys a Ewl_Filelist_File pointer
 */
void
ewl_filelist_file_destroy(Ewl_Filelist_File *file)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        ecore_string_release(file->name);
        ecore_string_release(file->path);
        FREE(file);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param file: The Ewl_Filelist_File structure
 * @param path: The path to the file to load
 * @return Returns 1 on successful information generation, 0 otherwise
 * @brief Generates and enters the information on the given pathname
 */
unsigned int
ewl_filelist_file_path_set(Ewl_Filelist_File *file, const char *path)
{
        struct stat st;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(path, 0);
        DCHECK_PARAM_PTR_RET(file, 0);

        IF_RELEASE(file->path);
        file->path = ecore_string_instance(path);

        if (stat(path, &st))
                DRETURN_INT(1, DLEVEL_STABLE);

        file->size = st.st_size;
        file->modtime = st.st_mtime;
        file->mode = st.st_mode;
        file->groupname = st.st_gid;
        file->username = st.st_uid;
        file->is_dir = ecore_file_is_dir(path);
        file->readable = ecore_file_can_read(path);
        file->writeable = ecore_file_can_write(path);

        DRETURN_INT(1, DLEVEL_STABLE);
}

/**
 * @param file: The file to convert.
 * @return Returns a string representation of the given size.
 * @brief Converts the size of the given file into a human readable format.
 */
char *
ewl_filelist_file_size_get(const Ewl_Filelist_File *file)
{
        double dsize;
        char size[1024], *suffix;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file, NULL);

        dsize = (double)file->size;
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
 * @param file: The Ewl_Filelist_File to convert
 * @return Returns the string of the given mode setting
 * @brief Converts the mode settings of the file into a human readable string
 */
char *
ewl_filelist_file_perms_get(const Ewl_Filelist_File *file)
{
        char *perm;
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file, NULL);

        perm = (char *)malloc(sizeof(char) * 10);
        for (i = 0; i < 9; i++)
                perm[i] = '-';

        perm[9] = '\0';

        if ((S_IRUSR & file->mode) == S_IRUSR) perm[0] = 'r';
        if ((S_IWUSR & file->mode) == S_IWUSR) perm[1] = 'w';
        if ((S_IXUSR & file->mode) == S_IXUSR) perm[2] = 'x';

        if ((S_IRGRP & file->mode) == S_IRGRP) perm[3] = 'r';
        if ((S_IWGRP & file->mode) == S_IWGRP) perm[4] = 'w';
        if ((S_IXGRP & file->mode) == S_IXGRP) perm[5] = 'x';

        if ((S_IROTH & file->mode) == S_IROTH) perm[6] = 'r';
        if ((S_IWOTH & file->mode) == S_IWOTH) perm[7] = 'w';
        if ((S_IXOTH & file->mode) == S_IXOTH) perm[8] = 'x';

        DRETURN_PTR(perm, DLEVEL_STABLE);
}

/**
 * @param file: The Ewl_Filelist_File to convert.
 * @return Returns the user name for the given user id
 * @brief Convertes the given user id of the file into the appropriate
 *        user name.  On Windows the file parameter is unused.
 */
char *
ewl_filelist_file_username_get(const Ewl_Filelist_File *file)
{
        char name[PATH_MAX];
#ifdef HAVE_PWD_H
        struct passwd *pwd = NULL;
#endif /* HAVE_PWD_H */

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file, NULL);

#ifdef HAVE_PWD_H
        pwd = getpwuid(file->username);
        if (pwd)
                snprintf(name, PATH_MAX, "%s", pwd->pw_name);
        else
#endif /* HAVE_PWD_H */
                snprintf(name, PATH_MAX, "%-8d", (int)file->username);

        DRETURN_PTR(strdup(name), DLEVEL_STABLE);
}

/**
 * @param file: The Ewl_Filelist_File to convert.
 * @return Returns the group name for the given user id
 * @brief Convertes the given group id of the file into the appropriate
 *        group name.  On Windows the file parameter is unused.
 */
char *
ewl_filelist_file_groupname_get(const Ewl_Filelist_File *file)
{
        char name[PATH_MAX];
#ifdef HAVE_GRP_H
        struct group *grp;
#endif /* HAVE_GRP_H */

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file, NULL);

#ifdef HAVE_GRP_H
        grp = getgrgid(file->groupname);
        if (grp)
                snprintf(name, PATH_MAX, "%s", grp->gr_name);
        else
#endif /* HAVE_GRP_H */
                snprintf(name, PATH_MAX, "%-8d", (int)file->groupname);

        DRETURN_PTR(strdup(name), DLEVEL_STABLE);
}

/**
 * @param file: The Ewl_Filelist_File to convert
 * @return Returns the string version of the modtime
 * @brief Converts the modtime of the given file to a human readable string
 */
char *
ewl_filelist_file_modtime_get(const Ewl_Filelist_File *file)
{
        char *time;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file, NULL);

        time = ctime(&(file->modtime));
        if (time)
        {
                time = strdup(time);
                time[strlen(time) - 1] = '\0';
        }
        else time = strdup("Unknown");

        DRETURN_PTR(time, DLEVEL_STABLE);
}

/**
 * @param file: The Ewl_Filelist_File to use
 * @return Returns the file name
 * @brief Gets the file name
 */
const char *
ewl_filelist_file_name_get(const Ewl_Filelist_File *file)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file, NULL);

        DRETURN_PTR(file->name, DLEVEL_STABLE);
}

/**
 * @param file: The Ewl_Filelist_File to use
 * @return Returns the path name
 * @brief Gets the path name
 */
const char *
ewl_filelist_file_path_get(const Ewl_Filelist_File *file)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file, NULL);

        DRETURN_PTR(file->path, DLEVEL_STABLE);
}

/**
 * @param file: The Ewl_Filelist_File to use
 * @return Returns 1 if the file is a directory, 0 otherwise
 * @brief Determines if the file is a directory
 */
unsigned char
ewl_filelist_file_is_dir(const Ewl_Filelist_File *file)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file, 0);

        DRETURN_INT(file->is_dir, DLEVEL_STABLE);
}

/**
 * @param file: The Ewl_Filelist_File to use
 * @return Returns 1 if the file is readable, 0 otherwise
 * @brief Determines if the file is readable
 */
unsigned char
ewl_filelist_file_is_readable(const Ewl_Filelist_File *file)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file, 0);

        DRETURN_INT(file->readable, DLEVEL_STABLE);
}

/**
 * @param file: The Ewl_Filelist_File to use
 * @return Returns 1 if the file is writeable, 0 otherwise
 * @brief Determines if the file is writeable
 */
unsigned char
ewl_filelist_file_is_writeable(const Ewl_Filelist_File *file)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file, 0);

        DRETURN_INT(file->writeable, DLEVEL_STABLE);
}

/**
 * @param file: The Ewl_Filelist_File to use
 * @param name: The name of the file
 * @return Returns no value;
 * @brief Sets the file name
 */
void
ewl_filelist_file_name_set(Ewl_Filelist_File *file, const char *name)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(file);
        DCHECK_PARAM_PTR(name);

        file->name = ecore_string_instance(name);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param file1: The first file to compare
 * @param file2: The second file to compare
 * @return Returns the string comparison
 * @brief Used to sort the filename column
 */
int
ewl_filelist_file_name_compare(const Ewl_Filelist_File *file1,
                                const Ewl_Filelist_File *file2)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file1, 0);
        DCHECK_PARAM_PTR_RET(file2, 0);
        DRETURN_INT((strcoll(file1->name, file2->name)), DLEVEL_STABLE);
}

/**
 * @param file1: The first file to compare
 * @param file2: The second file to compare
 * @return Returns the size comparison
 * @brief Used to sort the size column
 */
int
ewl_filelist_file_size_compare(const Ewl_Filelist_File *file1,
                                const Ewl_Filelist_File *file2)
{
        int ret;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file1, 0);
        DCHECK_PARAM_PTR_RET(file2, 0);

        if (file1->size > file2->size)
                ret = 1;
        else if (file1->size < file2->size)
                ret = -1;
        else
                ret = 0;

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param file1: The first file to compare
 * @param file2: The second file to compare
 * @return Returns the modified time comparison
 * @brief Used to sort the modified time column
 */
int
ewl_filelist_file_modified_compare(const Ewl_Filelist_File *file1,
                                const Ewl_Filelist_File *file2)
{
        int ret;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(file1, 0);
        DCHECK_PARAM_PTR_RET(file2, 0);

        if (file1->modtime > file2->modtime)
                ret = 1;
        else if (file1->modtime < file2->modtime)
                ret = -1;
        else
                ret = 0;

        DRETURN_INT(ret, DLEVEL_STABLE);
}

