#include "ephoto.h"

static void _ephoto_window_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);
static void _ephoto_thumb_populate(void);
static Eina_Bool _ephoto_thumb_populate_filter(void *data __UNUSED__, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info);
static void _ephoto_thumb_populate_main(void *data __UNUSED__, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info);
static void _ephoto_thumb_populate_end(void *data __UNUSED__, Eio_File *handler __UNUSED__);
static void _ephoto_thumb_populate_error(void *data __UNUSED__, Eio_File *handler __UNUSED__, int error);

Evas_Object *
ephoto_window_add(void)
{
   ephoto->win = elm_win_add(NULL, "ephoto", ELM_WIN_BASIC);
   if (!ephoto->win)
     return NULL;
   evas_object_resize(ephoto->win, 600, 400);
   ephoto_title_set("Ephoto - Picture Enlightenment");
   evas_object_smart_callback_add
     (ephoto->win, "delete,request", _ephoto_window_del, NULL);
   evas_object_show(ephoto->win);

   ephoto->bg = elm_bg_add(ephoto->win);
   evas_object_size_hint_weight_set
     (ephoto->bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(ephoto->bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(ephoto->win, ephoto->bg);
   evas_object_show(ephoto->bg);

   ephoto->pager = elm_pager_add(ephoto->win);
   elm_object_style_set(ephoto->pager, "fade");
   evas_object_size_hint_weight_set
     (ephoto->pager, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set
     (ephoto->pager, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(ephoto->win, ephoto->pager);
   evas_object_show(ephoto->pager);

   ephoto->flow_browser = ephoto_flow_browser_add();
   elm_pager_content_push(ephoto->pager, ephoto->flow_browser);

   ephoto->thumb_browser = ephoto_thumb_browser_add();
   elm_pager_content_push(ephoto->pager, ephoto->thumb_browser);

   elm_pager_content_promote(ephoto->pager, ephoto->thumb_browser);
   ephoto->state = EPHOTO_STATE_THUMB;

   if (!ephoto->directory && !ephoto->file)
     {
        char buf[PATH_MAX], *cwd;
        cwd = getcwd(buf, PATH_MAX);
        ephoto->directory = eina_stringshare_add(cwd);
     }
   else if (ephoto->file)
       ephoto->directory = eina_stringshare_add(ecore_file_dir_get(ephoto->file));

   ephoto->current_index = ephoto->images;
   _ephoto_thumb_populate();

   return ephoto->win;
}

void
ephoto_title_set(const char *title)
{
   elm_win_title_set(ephoto->win, title);
}

static void
_ephoto_window_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   ephoto_thumb_browser_del();
   ephoto_flow_browser_del();
   evas_object_del(ephoto->win);
}

static void
_ephoto_thumb_populate(void)
{
   ephoto->ls = eio_file_stat_ls(ephoto->directory,
                                 _ephoto_thumb_populate_filter,
                                 _ephoto_thumb_populate_main,
                                 _ephoto_thumb_populate_end,
                                 _ephoto_thumb_populate_error,
                                 NULL);
}

static Eina_Bool
_ephoto_thumb_populate_filter(void *data __UNUSED__, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   const char *ext;

   ext = strrchr(info->path, '.');
   if (ext)
     {
        if ((!strncasecmp(ext, ".jpeg", 5)) ||
            (!strncasecmp(ext, ".jpg", 5)) ||
            (!strncasecmp(ext, ".png", 5)) ||
            (!strncasecmp(ext, ".gif", 5)) ||
            (!strncasecmp(ext, ".svg", 5)))
              return EINA_TRUE;
     }
   return EINA_FALSE;
}

static void
_ephoto_thumb_populate_main(void *data __UNUSED__, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   ephoto->images = eina_list_append(ephoto->images, info->path);

   ephoto_thumb_browser_thumb_append(info->path);
   if (ephoto->file && !strcmp(ephoto->file, info->path))
     ephoto->current_index = ephoto->images;
}

static void
_ephoto_thumb_populate_end(void *data __UNUSED__, Eio_File *handler __UNUSED__)
{
   ephoto->ls = NULL;
   if (!ephoto->current_index)
     ephoto->current_index = eina_list_nth_list(ephoto->images, 0);   
}

static void
_ephoto_thumb_populate_error(void *data __UNUSED__, Eio_File *handler __UNUSED__, int error)
{
   if (error)
      printf("Error while populating images: %s\n", strerror(error));
}

