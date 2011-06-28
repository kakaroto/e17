#include "ephoto.h"

typedef struct _Ephoto_Directory_Thumb Ephoto_Directory_Thumb;
struct _Ephoto_Directory_Thumb
{
   Eio_File *ls;
   Eina_List *objs;
   Ephoto_Entry *entry;
   Eina_Bool canceled:1;
};

static Eina_Hash *_pending_dirs = NULL;

static void
_entry_free(void *data, const Ephoto_Entry *entry __UNUSED__)
{
   Ephoto_Directory_Thumb *dt = data;
   dt->entry = NULL;
}

static void
_ephoto_directory_thumb_free(Ephoto_Directory_Thumb *dt)
{
   if (dt->entry)
     {
        ephoto_entry_free_listener_del(dt->entry, _entry_free, dt);
        eina_hash_del(_pending_dirs, dt->entry->path, dt);
        dt->entry = NULL;
     }

   if (dt->ls)
     {
        dt->canceled = EINA_TRUE;
        eio_file_cancel(dt->ls);
        return;
     }

   free(dt);

   if (_pending_dirs)
     {
        if (!eina_hash_population(_pending_dirs))
          {
             eina_hash_free(_pending_dirs);
             _pending_dirs = NULL;
          }
     }
}

static void
_obj_del(void *data, Evas *e __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   Ephoto_Directory_Thumb *dt = data;
   dt->objs = eina_list_remove(dt->objs, obj);
   if (!dt->objs)
     {
        dt->canceled = EINA_TRUE;
        _ephoto_directory_thumb_free(dt);
     }
}

static Eina_Bool
_populate_filter(void *data __UNUSED__, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   return _ephoto_eina_file_direct_info_image_useful(info);
}

static void
_populate_end(void *data, Eio_File *handler __UNUSED__)
{
   Ephoto_Directory_Thumb *dt = data;
   Evas_Object *obj;
   dt->ls = NULL;

   EINA_LIST_FREE(dt->objs, obj)
     evas_object_event_callback_del_full(obj, EVAS_CALLBACK_DEL, _obj_del, dt);

   if (dt->entry)
     {
        dt->entry->dir_files_checked = EINA_TRUE;
        if ((dt->entry->item) && (!dt->canceled))
          elm_gengrid_item_update(dt->entry->item);
     }

   _ephoto_directory_thumb_free(dt);
}

static void
_populate_error(void *data, Eio_File *handler, int error)
{
   Ephoto_Directory_Thumb *dt = data;
   if (error) ERR("could not populate: %s", strerror(error));
   _populate_end(dt, handler);
}

static void
_populate_main(void *data, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   Ephoto_Directory_Thumb *dt = data;
   Evas_Object *obj;
   const char *file;

   if (!dt->objs) return;
   if (!dt->entry) return;

   obj = dt->objs->data;
   file = eina_stringshare_add(info->path);

   DBG("populate thumbnail %p with path '%s'", obj, file);

   dt->objs = eina_list_remove_list(dt->objs, dt->objs);
   dt->entry->dir_files = eina_list_append(dt->entry->dir_files, file);
   ephoto_thumb_path_set(obj, file);

   evas_object_event_callback_del_full(obj, EVAS_CALLBACK_DEL, _obj_del, dt);
   if ((!dt->objs) && (dt->ls))
     {
        eio_file_cancel(dt->ls);
        dt->ls = NULL;
     }
}

Evas_Object *
ephoto_directory_thumb_add(Evas_Object *parent, Ephoto_Entry *entry)
{
   Ephoto_Directory_Thumb *dt;
   Evas_Object *obj;

   if (_pending_dirs)
     dt = eina_hash_find(_pending_dirs, entry->path);
   else
     {
        dt = NULL;
        _pending_dirs = eina_hash_stringshared_new(NULL);
        EINA_SAFETY_ON_NULL_RETURN_VAL(_pending_dirs, NULL);
     }

   obj = ephoto_thumb_add(entry->ephoto, parent, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, NULL);

   if (!dt)
     {
        dt = calloc(1, sizeof(Ephoto_Directory_Thumb));
        if (!dt)
          {
             ERR("could not allocate memory for Ephoto_Directory_Thumb");
             evas_object_del(obj);
             return NULL;
          }
        ephoto_entry_free_listener_add(entry, _entry_free, dt);
        dt->entry = entry;
        dt->ls = eio_file_direct_ls(entry->path,
                                    _populate_filter,
                                    _populate_main,
                                    _populate_end,
                                    _populate_error,
                                    dt);
        if (!dt->ls)
          {
             ERR("could not create eio_file_direct_ls(%s)", entry->path);
             evas_object_del(obj);
             free(dt);
             return NULL;
          }

        eina_hash_add(_pending_dirs, entry->path, dt);
        DBG("start thread to lookup inside '%s' for thumbnails.", entry->path);
     }
   else
     DBG("thread already started, wait for thumbnails in '%s'", entry->path);

   dt->objs = eina_list_append(dt->objs, obj);

   evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL, _obj_del, dt);
   return obj;
}
