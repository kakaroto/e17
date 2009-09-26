#include "ephoto.h"

static void move_left(void *data, Evas_Object *obj, const char *emission, const char *source);
static void move_right(void *data, Evas_Object *obj, const char *emission, const char *source);
static Eina_List *get_image_files(const char *directory);

/*Show the Image Browser*/
void show_image_browser(void)
{
	Evas_Object *o;
	int x, y, w, h;

	o = edje_object_add(em->e);
	edje_object_file_set(o, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "/ephoto/image/browser");
	edje_object_part_swallow(em->bg, "ephoto.swallow.content", o);
	evas_object_show(o);
	em->image_browser = o;

	edje_object_part_geometry_get(em->bg, "ephoto.swallow.content", &x, &y, &w, &h);

	o = ephoto_table_add(em->e);
	ephoto_table_padding_set(o, 20, 20);
	ephoto_table_viewport_set(o, w, h);
	evas_object_show(o);
	edje_object_part_swallow(em->image_browser, "ephoto.swallow.content", o);
	em->image_browser_tbl = o;

	edje_object_signal_callback_add(em->image_browser, "mouse,up,1", "move_left", move_left, NULL);

        edje_object_signal_callback_add(em->image_browser, "mouse,up,1", "move_right", move_right, NULL);
}

void hide_image_browser(void)
{
	evas_object_hide(em->image_browser);
}

static void move_left(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	ephoto_table_prev_page(em->image_browser_tbl);
	printf("Left\n");
}

static void move_right(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	ephoto_table_next_page(em->image_browser_tbl);
	printf("Right\n");
}

/*Get a list of images and begin to thumbnail them*/
/*They will be passed to create_thumbnail for creation*/
void populate_thumbnails(void)
{
	char *dir, *image;
	char cwd[PATH_MAX];

	dir = getcwd(cwd, PATH_MAX);
	em->images = get_image_files(cwd);
			
	while(eina_list_data_get(em->images))
	{
		image = eina_list_data_get(em->images);

		ephoto_table_pack(em->image_browser_tbl, image); 
		em->images = eina_list_next(em->images);
	}
}

/*Get Images From A Certain Directory*/
/*Parameter Directory Is The Directory To Get Images From*/
static Eina_List *get_image_files(const char *directory)
{
	Eina_List *ls; 
	Eina_List *images = NULL;
	char path[PATH_MAX], *file;

	if (ecore_file_is_dir(directory))
	{
		ls = ecore_file_ls(directory);
		file = eina_list_data_get(ls);
		while (file)
		{
			const char *type;
				
			if (strcmp(directory, "/"))
				snprintf(path, PATH_MAX, "%s/%s",
						directory, file);
			else
				snprintf(path, PATH_MAX, "%s%s",
						directory, file);
			type = efreet_mime_type_get((const char *)path);
			if (!strncmp(type, "image", 5))
				images = eina_list_append(images, strdup(path));
			ls = eina_list_next(ls);
			file = eina_list_data_get(ls);
		}
		eina_list_free(ls);
	}
	else
		images = NULL;
	return images;
}

