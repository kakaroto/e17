#include "ephoto.h"

Ecore_List *get_files(char *directory)
{
	Ecore_List *ls, *files;
	char *file;
	char path[PATH_MAX];

	if (ecore_file_is_dir(directory))
	{
		ls = ecore_list_new();
		files = ecore_list_new();

		ecore_list_append(files, strdup(".."));

		ls = ecore_file_ls(directory);
		while (!ecore_list_is_empty(ls))
		{
			file = ecore_list_remove_first(ls);
			if (strncmp(file, ".", 1))
			{
				if (strcmp(directory, "/"))
				{
					snprintf(path, PATH_MAX, "%s/%s", 
						 directory, file);
				}
				else
				{
					snprintf(path, PATH_MAX, "%s%s", 
						 directory, file);
				}
			}
			if (ecore_file_is_dir(path))
			{
				ecore_list_append(files, strdup(path));
			}
		}
	}
	else
	{
		files = NULL;
	}
	return files;
}
