#include "ephoto.h"

static int get_thumbnails(void *data);
static Ecore_Idler *idler;
static DIR *direc;

/*Show the Image Browser*/
void add_image_browser(void)
{
	Evas_Object *o;

	o = ephoto_table_add(em->e);
	ephoto_table_padding_set(o, 20, 20);
	em->image_browser = o;
}

void show_image_browser(void)
{
	int w, h;
	evas_object_show(em->image_browser);
	edje_object_part_swallow(em->bg, "ephoto.swallow.content", em->image_browser);
	edje_object_part_geometry_get(em->bg, "ephoto.swallow.content", 0, 0, &w, &h);
	ephoto_table_viewport_set(em->image_browser, w, h);
}

void hide_image_browser(void)
{
	evas_object_hide(em->image_browser);
	edje_object_part_unswallow(em->bg, em->image_browser);
}

/*Get a list of images and begin to thumbnail them*/
/*They will be passed to create_thumbnail for creation*/
void populate_thumbnails(void)
{
	char *dir, cwd[PATH_MAX];

	dir = getcwd(cwd, PATH_MAX);
	direc = opendir(dir);
	idler = ecore_idler_add(get_thumbnails, strdup(dir));
}

static int get_thumbnails(void *data)
{
	const char *type;
	char *dir, path[PATH_MAX];
	int i;
	struct dirent *d;

	dir = data;

	for (i = 0; i <= 8; i++)
	{
		d = readdir(direc);
		if (!d)
		{
			ecore_idler_del(idler);
			closedir(direc);
			return 0;
		}
		else
		{
			if (strcmp(dir, "/"))
                                snprintf(path, PATH_MAX, "%s/%s",
                                                dir, d->d_name);
                        else
                                snprintf(path, PATH_MAX, "%s%s",
                                                dir, d->d_name);
			type = efreet_mime_type_get((const char *)path);
			if (!strncmp(type, "image", 5))
			{
				em->images = eina_list_append(em->images, strdup(path));
				ephoto_table_pack(em->image_browser, strdup(path));
			}
		}
	}

	return 1;
}

