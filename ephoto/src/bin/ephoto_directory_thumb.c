
#include <Elementary.h>
#include <Eio.h>

#include "config.h"

typedef struct _Directory_Thumb Directory_Thumb;

struct _Directory_Thumb
{
  Evas_Object *layout;
  Eio_File *file;
  Eina_List *thumbs;
  const char *path;
};

static Eina_Bool
_populate_filter(void *data, const char *file)
{
   const char *type, *basename;

   /* TODO: eio_file_ls_direct() and get more useful parameter than file */
   basename = ecore_file_file_get(file);
   if ((!basename) || (basename[0] == '.'))
     return EINA_FALSE;

   /* TODO: speed up case for jpg/jpeg/png */
   if (!(type = efreet_mime_type_get(file)))
     return EINA_FALSE;
   if (!strncmp(type, "image", 5))
     return EINA_TRUE;

  return EINA_FALSE;
}

static void
_populate_end(void *data)
{
  Directory_Thumb *dt = (Directory_Thumb*) data;

  dt->file = NULL;
 
}


static void
_populate_error(int error, void *data)
{
  Directory_Thumb *dt = (Directory_Thumb*)data;

  dt->file = NULL;

}

static void
_populate_main(void *data, const char *file)
{
  Evas_Object *thumb, *o;
  int position;
  Directory_Thumb *dt = data;
  char buf[4096];

  position = eina_list_count(dt->thumbs);

  if (position > 3 )
    {
      eio_file_cancel(dt->file);
      return;
    }

  thumb = elm_layout_add(dt->layout);
  elm_layout_file_set(thumb, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
		      "/ephoto/thumb/no_border");
	
  evas_object_size_hint_weight_set(thumb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(thumb);

  thumb = elm_layout_add(dt->layout);
  elm_layout_file_set(thumb, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
		      "/ephoto/thumb");
	
  evas_object_size_hint_weight_set(thumb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(thumb);

  o = elm_thumb_add(thumb);
  elm_thumb_file_set(o, file, NULL);
  elm_object_style_set(o, "ephoto");
  evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND,
  				   EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(o, EVAS_HINT_FILL,
  				  EVAS_HINT_FILL);

  evas_object_show(o);
  elm_layout_content_set(thumb, "ephoto.swallow.content", o);

  snprintf(buf, sizeof(buf), "ephoto.swallow.thumb%d", position);
  elm_layout_content_set(dt->layout, buf, thumb);

  evas_object_show(thumb);
  dt->thumbs = eina_list_append(dt->thumbs, thumb);

}

Evas_Object *ephoto_directory_thumb_add(Evas_Object *parent, const char *path)
{
  Directory_Thumb *dt;
  Evas_Object *thumb, *o;

  elm_need_ethumb();
  if (!efreet_mime_init())
    fprintf(stderr, "Could not init efreet_mime!\n");

  dt = calloc(1, sizeof( Directory_Thumb));
  dt->path = eina_stringshare_add(path);
  dt->layout = elm_layout_add(parent);

  if (!elm_layout_file_set(dt->layout, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
			   "/ephoto/directory/thumb/layout"))
    printf(" Error loading group /ephoto/directory/thumb/layout in %s\n", PACKAGE_DATA_DIR "/themes/default/ephoto.edj");
  evas_object_size_hint_weight_set(dt->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(dt->layout);

  thumb = elm_layout_add(dt->layout);
  if (!elm_layout_file_set(thumb, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
			   "/ephoto/thumb/no_border"))
    printf(" Error loading group /ephoto/thumb/no_border  in %s\n", PACKAGE_DATA_DIR "/themes/default/ephoto.edj");
	
  //evas_object_size_hint_weight_set(thumb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(thumb);

  o = elm_image_add(thumb);
  elm_image_file_set(o,  PACKAGE_DATA_DIR "/images/change_directory.png", NULL);
  evas_object_show(o);

  elm_layout_content_set(thumb, "ephoto.swallow.content", o);

  elm_layout_content_set(dt->layout, "ephoto.swallow.thumb1", thumb);

  evas_object_show(dt->layout);
  
  evas_object_data_set(dt->layout, "dt", dt);

  dt->file = eio_file_ls(path,
			 _populate_filter,
			 _populate_main,
			 _populate_end,
			 _populate_error,
			 dt);

  /* TODO add del callback to free dt and its member */
  return dt->layout;
}

