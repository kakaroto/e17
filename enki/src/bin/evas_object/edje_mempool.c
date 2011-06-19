/*
 * edje_mempool.c
 *
 *  Created on: Jun 19, 2011
 *      Author: watchwolf
 */

#include "edje_mempool.h"
#include "../../define.h"

static Eina_Hash *_hash = NULL;

static void
_data_free(void *data)
{
   Evas_Object *obj = data;
   evas_object_del(obj);
}

Evas_Object *
edje_mempool_object_add(const char *group)
{
   Evas_Object *obj = NULL;

   if (!_hash)
   {
      _hash = eina_hash_string_djb2_new(_data_free);
   }

   Eina_List *l = eina_hash_find(_hash, group);
   if (!l)
   {
      obj = edje_object_add(evas_object_evas_get(enlil_data->win->win));
      edje_object_file_set(obj, Theme, group);

      LOG_INFO("[edje_mempool] create new object %s %p", group, obj);
   }
   else
   {
      obj = eina_list_data_get(l);
      l = eina_list_remove(l, obj);
      eina_hash_del_by_key(_hash, group);
      eina_hash_add(_hash, group, l);

      edje_object_signal_emit(obj, "init", "edje_mempool");

      LOG_INFO("[edje_mempool] recycle object %s %p", group, obj);
   }

   return obj;
}

void
edje_mempool_object_del(Evas_Object *obj)
{
   const char *group;
   if (!obj) return;

   edje_object_file_get(obj, NULL, &group);

   LOG_INFO("[edje_mempool] delete object %s %p", group, obj);
   Eina_List *l = eina_hash_find(_hash, group);
   eina_hash_del_by_key(_hash, group);
   l = eina_list_append(l, obj);
   eina_hash_add(_hash, group, l);

   evas_object_hide(obj);
}
