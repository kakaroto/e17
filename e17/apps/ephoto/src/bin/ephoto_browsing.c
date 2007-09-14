#include "ephoto.h"

/*Populate a List of Sub Directories Inside of Directory.*/
Ecore_List *get_directories(const char *directory)
{
	Ecore_List *ls, *files;
	char *file;
	char path[PATH_MAX];

	if (ecore_file_is_dir(directory))
	{
		ls = ecore_list_new();
		files = ecore_list_new();
		ecore_list_free_cb_set(files, free);
		if (strcmp(directory, "/"))
		{
			ecore_list_append(files, strdup(".."));
		}
		ls = ecore_file_ls(directory);
		while (!ecore_list_empty_is(ls))
		{
			file = ecore_list_first_remove(ls);
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
	ecore_list_first_goto(files);
	return files;
}

/*Populate a List of Images Inside of Directory*/
Ecore_List *get_images(const char *directory)
{
        Ecore_List *ls, *files;
        char path[PATH_MAX], *file;

        if (ecore_file_is_dir(directory))
        {
                ls = ecore_list_new();
                files = ecore_dlist_new();
		ecore_dlist_free_cb_set(files, free);

                ls = ecore_file_ls(directory);
                while (!ecore_list_empty_is(ls))
                {
                        file = ecore_list_first_remove(ls);
                        if (strncmp(file, ".", 1))
                        {
				const char *type;

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
				type = efreet_mime_type_get(strdup(path));
				if ((ecore_hash_get(em->types, type)) == "image")
				{
					ecore_dlist_append(files, strdup(path));
				}
			}
		}
	}
	else
	{
		files = NULL;
	}
	ecore_dlist_first_goto(files);
	return files;
}
