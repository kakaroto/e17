#include "ephoto.h"

#define SLIDER_MAX 512
#define SLIDER_MIN 128

#define PARENT_DIR "Up"

typedef struct _Ephoto_Thumb_Browser Ephoto_Thumb_Browser;

struct _Ephoto_Thumb_Browser
{
   Ephoto *ephoto;
   Evas_Object *layout;
   Evas_Object *edje;
   Evas_Object *grid;
   Eio_File *ls;
   struct {
      Ecore_Job *change_dir;
   } job;
};

static char *
_ephoto_thumb_item_label_get(void *data, Evas_Object *obj, const char *part)
{
   Ephoto_Entry *e = data;
   return strdup(e->label);
}

static Evas_Object *
_ephoto_thumb_dir_icon_get(void *data, Evas_Object *obj, const char *part)
{
   Ephoto_Entry *e = data;
   const char *f;
   int n;

   if (strncmp(part, "elm.swallow.icon.", sizeof("elm.swallow.icon.") - 1) != 0)
     return NULL;

   n = atoi(part + sizeof("elm.swallow.icon.") - 1);
   if (n < 1)
     return NULL;
   n--;

   f = eina_list_nth(e->dir_files, n);
   if (f)
     return ephoto_thumb_add(e->ephoto, obj, f);

   if (e->dir_files_checked)
     return NULL;

   return ephoto_directory_thumb_add(obj, e);
}

static Eina_Bool
_ephoto_thumb_dir_state_get(void *data, Evas_Object *obj, const char *part)
{
   Ephoto_Entry *e = data;
   int n;

   if (strcmp(part, "have_files") == 0)
     return !!e->dir_files;

   if (strncmp(part, "have_file.", sizeof("have_file.") - 1) != 0)
     return EINA_FALSE;

   n = atoi(part + sizeof("have_file.") - 1);
   if (n < 1)
     return EINA_FALSE;
   return n <= eina_list_count(e->dir_files);
}

static Evas_Object *
_ephoto_thumb_file_icon_get(void *data, Evas_Object *obj, const char *part)
{
   Ephoto_Entry *e = data;
   return ephoto_thumb_add(e->ephoto, obj, e->path);
}

static void
_ephoto_thumb_item_del(void *data, Evas_Object *obj)
{
   Ephoto_Entry *e = data;
   e->item = NULL;
}

static const Elm_Gengrid_Item_Class _ephoto_thumb_dir_class = {
  "dir",
  {
    _ephoto_thumb_item_label_get,
    _ephoto_thumb_dir_icon_get,
    _ephoto_thumb_dir_state_get,
    _ephoto_thumb_item_del
  }
};

static const Elm_Gengrid_Item_Class _ephoto_thumb_up_class = {
  "up",
  {
    _ephoto_thumb_item_label_get,
    NULL,
    NULL,
    _ephoto_thumb_item_del
  }
};

static const Elm_Gengrid_Item_Class _ephoto_thumb_file_class = {
  "file",
  {
    _ephoto_thumb_item_label_get,
    _ephoto_thumb_file_icon_get,
    NULL,
    _ephoto_thumb_item_del
  }
};

static void
_ephoto_populate_main(void *data, const Eina_File_Direct_Info *info)
{
   Ephoto_Thumb_Browser *tb = data;
   const Elm_Gengrid_Item_Class *ic;
   Edje_Message_Int msg;
   Ephoto_Entry *e;

   e = ephoto_entry_new(tb->ephoto, info->path, info->path + info->name_start);
   if (info->dirent->d_type == DT_DIR) e->is_dir = EINA_TRUE;
   else if (info->dirent->d_type == DT_REG) e->is_dir = EINA_FALSE;
   else e->is_dir = !_ephoto_eina_file_direct_info_image_useful(info);

   if (e->is_dir) ic = &_ephoto_thumb_dir_class;
   else           ic = &_ephoto_thumb_file_class;

   /* TODO: add elm_gengrid_item_prepend(), elm_gengrid_item_insert_before(),
    * elm_gengrid_item_insert_after().
    * Then eina_list_search_sorted_near_list() to see where to insert it.
    */
   e->item = elm_gengrid_item_append(tb->grid, ic, e, NULL, NULL);
   if (!e->item)
     {
        ERR("could not add item to grid: path '%s'", info->path);
        ephoto_entry_free(e);
        return;
     }

   tb->ephoto->entries = eina_list_append(tb->ephoto->entries, e);

   msg.val = eina_list_count(tb->ephoto->entries);
   edje_object_message_send(tb->edje, EDJE_MESSAGE_INT, 1, &msg);
   DBG("populate add '%s'", e->path);
}

static Eina_Bool
_ephoto_populate_filter(void *data, const Eina_File_Direct_Info *info)
{
   const char *bname = info->path + info->name_start;

   if (bname[0] == '.') return EINA_FALSE;
   if (info->dirent->d_type == DT_DIR) return EINA_TRUE;
   if (info->dirent->d_type == DT_UNKNOWN)
     {
        struct stat st;
        if ((stat(info->path, &st) == 0) && (S_ISDIR(st.st_mode)))
          return EINA_TRUE;
     }

   return _ephoto_eina_file_direct_info_image_useful(info);
}

static void
_ephoto_populate_end(void *data)
{
   Ephoto_Thumb_Browser *tb = data;
   tb->ls = NULL;
}

static void
_ephoto_populate_error(int error, void *data)
{
   Ephoto_Thumb_Browser *tb = data;
   if (error) ERR("could not populate: %s", strerror(error));
   tb->ls = NULL;
   edje_object_signal_emit(tb->edje, "populate,stop", "ephoto");
   edje_object_signal_emit(tb->edje, "populate,error", "ephoto");
}

static void
_ephoto_populate_entries(Ephoto_Thumb_Browser *tb)
{
   Edje_External_Param param;
   char *parent_dir;
   DBG("populate from '%s'", tb->ephoto->config->directory);

   elm_gengrid_clear(tb->grid);
   ephoto_entries_free(tb->ephoto);

   parent_dir = ecore_file_dir_get(tb->ephoto->config->directory);
   if (parent_dir)
     {
        Ephoto_Entry *e = ephoto_entry_new(tb->ephoto, parent_dir, PARENT_DIR);
        free(parent_dir);
        EINA_SAFETY_ON_NULL_RETURN(e);
        e->is_up = EINA_TRUE;
        e->is_dir = EINA_TRUE;
        e->item = elm_gengrid_item_append
          (tb->grid, &_ephoto_thumb_up_class, e, NULL, NULL);
        /* does not go into entries as it is always the first - no sort! */
     }

   param.name = "text";
   param.type = EDJE_EXTERNAL_PARAM_TYPE_STRING;
   param.s = tb->ephoto->config->directory;
   edje_object_part_external_param_set(tb->edje, "ephoto.location", &param);
   edje_object_signal_emit(tb->edje, "location,set", "ephoto");

   edje_object_signal_emit(tb->edje, "populate,start", "ephoto");
   tb->ls = eio_file_direct_ls(tb->ephoto->config->directory,
                               _ephoto_populate_filter,
                               _ephoto_populate_main,
                               _ephoto_populate_end,
                               _ephoto_populate_error,
                               tb);
}

static void
_ephoto_thumb_change_dir(void *data)
{
   Ephoto_Thumb_Browser *tb = data;
   tb->job.change_dir = NULL;
   _ephoto_populate_entries(tb);
}

static void
_ephoto_thumb_selected(void *data, Evas_Object *o, void *event_info)
{
   Ephoto_Thumb_Browser *tb = data;
   Elm_Gengrid_Item *it = event_info;
   Ephoto_Entry *e = elm_gengrid_item_data_get(it);

   if (e->is_dir)
     ephoto_thumb_browser_directory_set(tb->layout, e->path);
   else
     evas_object_smart_callback_call(tb->layout, "view", e);
}

static void
_changed_dir(void *data, Evas_Object *o, const char *emission, const char *source)
{
   Ephoto_Thumb_Browser *tb = data;
   Edje_External_Param p;

   p.name = "text";
   p.type = EDJE_EXTERNAL_PARAM_TYPE_STRING;
   p.s = NULL;
   if (!edje_object_part_external_param_get(tb->edje, "ephoto.location", &p))
     return;

   ephoto_thumb_browser_directory_set(tb->layout, p.s);
}

static void
_layout_del(void *data, Evas *e, Evas_Object *o, void *event_info)
{
   Ephoto_Thumb_Browser *tb = data;
   if (tb->ls) eio_file_cancel(tb->ls);
   if (tb->job.change_dir) ecore_job_del(tb->job.change_dir);
   free(tb);
}

Evas_Object *
ephoto_thumb_browser_add(Ephoto *ephoto, Evas_Object *parent)
{
   Evas_Object *layout = elm_layout_add(parent);
   Ephoto_Thumb_Browser *tb;

   EINA_SAFETY_ON_NULL_RETURN_VAL(layout, NULL);

   tb = calloc(1, sizeof(Ephoto_Thumb_Browser));
   EINA_SAFETY_ON_NULL_GOTO(tb, error);
   tb->ephoto = ephoto;
   tb->layout = layout;
   tb->edje = elm_layout_edje_get(layout);
   evas_object_event_callback_add(layout, EVAS_CALLBACK_DEL, _layout_del, tb);
   evas_object_data_set(layout, "thumb_browser", tb);
   edje_object_signal_callback_add
     (tb->edje, "location,changed", "ephoto", _changed_dir, tb);

   if (!elm_layout_file_set(layout, THEME_FILE, "ephoto/browser/layout"))
     {
        ERR("could not load group 'ephoto/browser/layout' from file %s",
            THEME_FILE);
        goto error;
     }

   tb->grid = elm_gengrid_add(layout);
   EINA_SAFETY_ON_NULL_GOTO(tb->grid, error);
   elm_object_style_set(tb->grid, "ephoto");

   elm_gengrid_align_set(tb->grid, 0.5, 0.5);
   elm_gengrid_bounce_set(tb->grid, EINA_FALSE, EINA_TRUE);
   elm_gengrid_item_size_set
     (tb->grid, tb->ephoto->config->thumb_size, tb->ephoto->config->thumb_size);
   evas_object_size_hint_align_set
     (tb->grid, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set
     (tb->grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   evas_object_smart_callback_add
     (tb->grid, "selected", _ephoto_thumb_selected, tb);

   elm_layout_content_set(tb->layout, "ephoto.thumb.swallow", tb->grid);

   return layout;

 error:
   evas_object_del(layout);
   return NULL;
}

void
ephoto_thumb_browser_directory_set(Evas_Object *obj, const char *path)
{
   Ephoto_Thumb_Browser *tb = evas_object_data_get(obj, "thumb_browser");
   EINA_SAFETY_ON_NULL_RETURN(tb);

   eina_stringshare_replace(&tb->ephoto->config->directory, path);
   if (tb->job.change_dir) ecore_job_del(tb->job.change_dir);
   tb->job.change_dir = ecore_job_add(_ephoto_thumb_change_dir, tb);
}
