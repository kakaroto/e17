#include "ephoto_utils.h"

const char *get_file_size(int size) {
	char fsize[256];
	double dsize;

	dsize = (double)size;
	if (dsize < 1024)
		snprintf(fsize, sizeof(fsize), "%'.0f Bytes", dsize);
	else {
		dsize /= 1024;
		if (dsize < 1024)
			snprintf(fsize, sizeof(fsize), "%'.0f KB", dsize);
		else {
			dsize /= 1024;
			if (dsize < 1024)
				snprintf(fsize, sizeof(fsize), "%'.0f MB", dsize);
			else {
				dsize /= 1024;
				snprintf(fsize, sizeof(fsize), "%'.0f GB", dsize);
			}
		}
	}
	return strdup(fsize);
}

const char *get_image_pixels(const char *file) {
	char pixels[20];
	int w, h;
	Evas_Imaging_Image *image;

	image = evas_imaging_image_load(file, NULL);
	evas_imaging_image_size_get(image, &w, &h);
	evas_imaging_image_free(image);

	snprintf(pixels, 20, "%d x %d", w, h);

	return strdup(pixels);
}	

Ecore_List *get_directories(const char *directory) {
	Ecore_List *ls, *files;
	char *file;
	char path[4096];

	if (ecore_file_is_dir(directory)) {
		ls = ecore_list_new();
		files = ecore_list_new();
		ecore_list_free_cb_set(files, free);
		if (strcmp(directory, "/"))
			ecore_list_append(files, strdup(".."));
		ls = ecore_file_ls(directory);
		while (!ecore_list_empty_is(ls)) {
			file = (char *)ecore_list_remove(ls);
			if (strncmp(file, ".", 1)) {
				if (strcmp(directory, "/"))
					snprintf(path, 4096, "%s/%s", directory, file);
				else
					snprintf(path, 4096, "%s%s", directory, file);
				if (ecore_file_is_dir(path))
					ecore_list_append(files, strdup(path));
			}
		}
	}
	else
		files = NULL;
	ecore_list_first_goto(files);
	return files;
}

Ecore_List *get_images(const char *directory) {
	Ecore_List *ls, *files;
	char path[4096], *file;

	if (ecore_file_is_dir(directory)) {
		ls = ecore_list_new();
		files = ecore_dlist_new();
		ecore_list_free_cb_set(files, free);
		ls = ecore_file_ls(directory);
		while (!ecore_list_empty_is(ls)) {
			file = (char *)ecore_list_first_remove(ls);
			if (strncmp(file, ".", 1)) {
				const char *type;
				if (strcmp(directory, "/"))
					snprintf(path, 4096, "%s/%s",
							directory, file);
				else
					snprintf(path, 4096, "%s%s",
							directory, file);
				type = efreet_mime_type_get((const char*)path);
				if (!strncmp(type, "image", 5))
					ecore_dlist_append(files, strdup(path));
			}
		}
	}
	else
		files = NULL;
	ecore_dlist_first_goto(files);
	return files;
}

