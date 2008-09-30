#include "ephoto.h"

void
ephoto_db_init(void)
{
	char path[PATH_MAX];
	char path2[PATH_MAX];
	FILE *f;

	snprintf(path, PATH_MAX, "%s/.ephoto/.albums", getenv("HOME"));
	snprintf(path2, PATH_MAX, "%s/.ephoto", getenv("HOME"));

	if (!ecore_file_exists(path))
	{
		if (!ecore_file_exists(path2))
		{
			ecore_file_mkdir(path2);
		}
		f = fopen(path, "w");
		if (!f)
			return;
		fprintf(f, "#Ephoto Albums\n");
		fclose(f);
		ephoto_db_add_album("Complete Library",
				"Every Image In The Collection");
	}
	
	return;
}

void 
ephoto_db_add_album(const char *name, const char *description)
{
	char path[PATH_MAX], path2[PATH_MAX];
	FILE *a;
	FILE *f;

	snprintf(path, PATH_MAX, "%s/.ephoto/.albums", getenv("HOME"));
	snprintf(path2, PATH_MAX, "%s/.ephoto/.%s", getenv("HOME"), name);
	f = fopen(path, "a+");
	fprintf(f, "%s\n", name);
	fprintf(f, "/*%s\n", description);
	fclose(f);

	a = fopen(path2, "w");
	fprintf(a, "#%s\n", name);
	fprintf(a, "#%s\n", description);
	fclose(a);

	return;
}

void 
ephoto_db_delete_album(const char *name)
{
	return;
}

void 
ephoto_db_add_images(const char *album, Ecore_List *images)
{
	char path[PATH_MAX];
	char path2[PATH_MAX];
	char *image;
	FILE *a;
	FILE *cl;

	snprintf(path, PATH_MAX, "%s/.ephoto/.%s", getenv("HOME"), album);
	snprintf(path2, PATH_MAX, "%s/.ephoto/.Complete Library", getenv("HOME"));
	
	a = fopen(path, "a+");
	cl = fopen(path2, "a+");
	while (!ecore_list_empty_is(images))
	{
		image = ecore_list_first_remove(images);
		fprintf(a, "%s\n", image);
		fprintf(cl, "%s\n", image);
	}
	fclose(a);
	fclose(cl);

	return;
}

void 
ephoto_db_delete_image(const char *album, const char *path)
{
	return;
}	

Ecore_List *
ephoto_db_list_albums()
{
	char album[PATH_MAX], path[PATH_MAX];
	Ecore_List *albums;
	FILE *f;

	snprintf(path, PATH_MAX, "%s/.ephoto/.albums", getenv("HOME"));
	f = fopen(path, "r");

	albums = ecore_dlist_new();
	while(fgets(album, PATH_MAX, f))
	{
		if (strncmp(album, "#", 1) && strncmp(album, "/*", 1))
		{
			album[strlen(album)-1] = '\0';
			ecore_dlist_append(albums, strdup(album));
		}
	}
	fclose(f);
	ecore_dlist_first_goto(albums);

	return albums;
}

Ecore_List *
ephoto_db_list_images(const char *album)
{
	char image[PATH_MAX];
	char path[PATH_MAX];
	Ecore_List *images;
	FILE *a;

	images = ecore_dlist_new();
	snprintf(path, PATH_MAX, "%s/.ephoto/.%s", getenv("HOME"), album);
	a = fopen(path, "r");

	while(fgets(image, PATH_MAX, a))
	{
		if (strncmp(image, "#", 1))
		{
			image[strlen(image)-1] = '\0';
			ecore_dlist_append(images, strdup(image));
		}
	}

	ecore_dlist_first_goto(images);
	
	fclose(a);
	
	return images;
}

