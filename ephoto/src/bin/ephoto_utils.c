#include "ephoto.h"

const char *
file_size_get(int size)
{
        double dsize;
        char fsize[256];

        dsize = (double)size;
        if (dsize < 1024.0) snprintf(fsize, sizeof(fsize), _("%'.0f Bytes"), 
									dsize);
        else
        {
                dsize /= 1024.0;
                if (dsize < 1024) snprintf(fsize, sizeof(fsize), _("%'.0f KB"), 
									dsize);
                else
                {
                        dsize /= 1024.0;
                        if (dsize < 1024) snprintf(fsize, sizeof(fsize), 
							_("%'.0f MB"), dsize);
                        else
                        {
                                dsize /= 1024.0;
                                snprintf(fsize, sizeof(fsize), 
							_("%'.1f GB"), dsize);
                        }
                }
        }
        return strdup(fsize);
}

const char *
image_pixels_string_get(const char *file)
{
        char pixels[PATH_MAX];
        int w, h;
        Evas_Imaging_Image *image;

        image = evas_imaging_image_load(file, NULL);
        evas_imaging_image_size_get(image, &w, &h);
        evas_imaging_image_free(image);

        snprintf(pixels, PATH_MAX, "%d x %d", w, h);

        return strdup(pixels);
}

void 
image_pixels_int_get(const char *file, int *width, int *height)
{
	int w, h;
	Evas_Imaging_Image *image;

        image = evas_imaging_image_load(file, NULL);
        evas_imaging_image_size_get(image, &w, &h);
        evas_imaging_image_free(image);
	
	*width = w;
	*height = h;

	return;
}

Ecore_List *
get_directories(const char *directory)
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
					snprintf(path, PATH_MAX, "%s/%s", 
						 directory, file);
				else
					snprintf(path, PATH_MAX, "%s%s", 
						 directory, file);
				if (ecore_file_is_dir(path))
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

Ecore_List *
get_images(const char *directory)
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
                                        snprintf(path, PATH_MAX, "%s/%s",
                                                 directory, file);
                                else
                                        snprintf(path, PATH_MAX, "%s%s",
                                                 directory, file);
				type = efreet_mime_type_get(strdup(path));
				if (!strncmp(type, "image", 5))
					ecore_dlist_append(files, strdup(path));
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

