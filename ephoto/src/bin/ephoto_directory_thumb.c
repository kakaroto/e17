#include "ephoto.h"

typedef struct _Directory_Thumb Directory_Thumb;

struct _Directory_Thumb
{
  Evas_Object *layout;
  Eio_File *file;
  Eina_List *frames;
  const char *path;
  int theme_thumb_count;
};

static void
_layout_del(void *data, Evas *e, Evas_Object *layout, void *event_info)
{
   Directory_Thumb *dt = data;
   if (dt->file) eio_file_cancel(dt->file);
   eina_list_free(dt->frames);
   eina_stringshare_del(dt->path);
   free(dt);
}

static Eina_Bool
_populate_filter(void *data, const Eina_File_Direct_Info *info)
{
   return _ephoto_eina_file_direct_info_image_useful(info);
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
_populate_main(void *data, const Eina_File_Direct_Info *info)
{
  Evas_Object *frame, *image;
  int position;
  Directory_Thumb *dt = data;
  char buf[4096];

  position = eina_list_count(dt->frames);

  if (position > dt->theme_thumb_count)
    {
       if (dt->file)
         {
            eio_file_cancel(dt->file);
            dt->file = NULL;
         }
       return;
    }

  frame = elm_layout_add(dt->layout);
  if (!elm_layout_file_set
      (frame, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "/ephoto/thumb"))
    ERR("could not load group '/ephoto/thumb' from file '%s'",
        PACKAGE_DATA_DIR "/themes/default/ephoto.edj");
  evas_object_size_hint_weight_set(frame, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(frame);

  image = elm_thumb_add(frame);
  elm_thumb_file_set(image, info->path, NULL);
  elm_object_style_set(image, "ephoto");
  evas_object_size_hint_weight_set(image, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(image, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_layout_content_set(frame, "ephoto.swallow.content", image);

  snprintf(buf, sizeof(buf), "ephoto.swallow.thumb%d", position);
  elm_layout_content_set(dt->layout, buf, frame);

  dt->frames = eina_list_append(dt->frames, frame);
}

Evas_Object *
ephoto_directory_thumb_add(Evas_Object *parent, const char *path)
{
  Directory_Thumb *dt;
  Evas_Object *thumb, *placeholder;
  const char *s;

  dt = calloc(1, sizeof( Directory_Thumb));
  dt->path = eina_stringshare_add(path);
  dt->layout = elm_layout_add(parent);

  if (!elm_layout_file_set
      (dt->layout, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
       "/ephoto/directory/thumb/layout"))
    ERR("could not load group '/ephoto/directory/thumb/layout' from file '%s'",
        PACKAGE_DATA_DIR "/themes/default/ephoto.edj");
  evas_object_size_hint_weight_set
    (dt->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(dt->layout);
  evas_object_data_set(dt->layout, "dt", dt);
  evas_object_event_callback_add
    (dt->layout, EVAS_CALLBACK_DEL, _layout_del, dt);

  thumb = elm_layout_add(dt->layout);
  if (!elm_layout_file_set
      (thumb, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
       "/ephoto/thumb/no_border"))
    ERR("could not load group '/ephoto/thumb/no_border' from file '%s'",
        PACKAGE_DATA_DIR "/themes/default/ephoto.edj");
  //evas_object_size_hint_weight_set(thumb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_layout_content_set(dt->layout, "ephoto.swallow.thumb1", thumb);

  placeholder = edje_object_add(evas_object_evas_get(dt->layout));
  edje_object_file_set
    (placeholder, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
     "/ephoto/directory/no-preview");
  elm_layout_content_set(thumb, "ephoto.swallow.content", placeholder);

  s = edje_object_data_get(elm_layout_edje_get(dt->layout), "thumbs");
  if (s)
    {
       dt->theme_thumb_count = atoi(s);

       /* TODO: fix "Up" to be another type of directory and do not need
        * to check for existence here */
       if ((dt->theme_thumb_count > 0) && (ecore_file_exists(path)))
         dt->file = eio_file_direct_ls(path,
                                       _populate_filter,
                                       _populate_main,
                                       _populate_end,
                                       _populate_error,
                                       dt);
    }

  return dt->layout;
}
