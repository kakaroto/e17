/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_cache.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_cache.h"

#include <stdlib.h>
#include <string.h>

#include "etk_utils.h"

/**
 * @addtogroup Etk_Cache
 * @{
 */

static void _etk_cache_object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

typedef struct Etk_Cache_Item
{
   char *filename;
   char *key;
   Evas_Object *object;
} Etk_Cache_Item;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Creates a new cache system that you can use to cache image objects or Edje objects. You usually don't need
 * to use that, except if you are implementing your own widget or your own tree model which may need to load a lot
 * of images efficiently
 * @param size the max number of objects the cache system could store
 * @return Returns the new cache system
 * @note You will need to destroy it with etk_cache_destroy() when you no longer need it
 */
Etk_Cache *etk_cache_new(int size)
{
   Etk_Cache *cache;

   cache = malloc(sizeof(Etk_Cache));
   cache->cached_objects = NULL;
   cache->size = ETK_MAX(0, size);
   cache->free_cb = NULL;
   cache->free_cb_data = NULL;

   return cache;
}

/**
 * @brief Destroys the cache system: it destroys all the cached objects, and frees the memory used by the cache system
 * @param cache the cache system to destroy
 */
void etk_cache_destroy(Etk_Cache *cache)
{
   if (!cache)
      return;

   etk_cache_clear(cache);
   free(cache);
}

/**
 * @brief Clears the cache system: it destroys all the cached objects. The cache system remains still usable
 * @param cache the cache system to clear
 */
void etk_cache_clear(Etk_Cache *cache)
{
   Etk_Cache_Item *item;

   if (!cache)
      return;

   while (cache->cached_objects)
   {
      item = cache->cached_objects->data;
      evas_object_del(item->object);
   }
}

/**
 * @brief Sets the callback to call when a cached object is deleted (if it is there is no more space in the cache,
 * or if the cache is being cleared)
 * @param cache a cache system
 * @param free_cb the function to call when a cached object is deleted. @a object is the object that will be deleted,
 * and @a data is the user data you gave (see the next param)
 * @param data a data pointer that will be passed to the callback when it is called
 */
void etk_cache_free_callback_set(Etk_Cache *cache, void (*free_cb)(Evas_Object *object, void *data), void *data)
{
   if (!cache)
      return;

   cache->free_cb = free_cb;
   cache->free_cb_data = data;
}

/**
 * @brief Sets the max number of objects that the cache system can contain. If the new size is smaller than current
 * number of objects in the cache, the oldest objects that can't fit in the new cache size will be destroyed
 * @param cache the cache system to resize
 * @param size the new size (max number of objects) of the cache system
 */
void etk_cache_size_set(Etk_Cache *cache, int size)
{
   Etk_Cache_Item *item;
   int num_objects;

   if (!cache)
      return;

   cache->size = ETK_MAX(0, size);
   num_objects = etk_cache_size_get(cache);

   /* Destroys the objects that can't fit anymore */
   while (cache->cached_objects && num_objects > cache->size)
   {
      item = cache->cached_objects->data;
      evas_object_del(item->object);
      num_objects--;
   }
}

/**
 * @brief Gets the max number of objects that can be stored by the cache system
 * @param cache a cache system
 * @return Returns the max number of objects that can be stored by the cache system
 */
int etk_cache_size_get(Etk_Cache *cache)
{
   if (!cache)
      return 0;
   return cache->size;
}

/**
 * @brief Gets the current number of objects stored in the cache system
 * @param cache a cache system
 * @return Returns the current number of objects stored by the cache system
 */
int etk_cache_num_objects_get(Etk_Cache *cache)
{
   if (!cache)
      return 0;
   return eina_list_count(cache->cached_objects);
}

/**
 * @brief Adds an Evas image object or an Edje object in the cache system. If the cache is already full, the oldest
 * object will be removed. The object to cache will also be automatically hidden
 * @param cache a cache system
 * @param object the Evas image object or the Edje object to cache
 * @param filename the filename associated to the object
 * @param key the key associated to the object (the group for an Edje object, the key for an image from an Eet file,
 * or NULL otherwise)
 * @note Once the object is added to the cache, you should keep no reference to it. It may for example be deleted if
 * there is no more space in the cache system
 */
void etk_cache_add(Etk_Cache *cache, Evas_Object *object, const char *filename, const char *key)
{
   Etk_Cache_Item *item;
   Eina_List *l;

   if (!cache || !object || cache->size <= 0 || !filename)
      return;

   /* If the object is already cached, we move it at the end of the cache */
   if ((l = evas_object_data_get(object, "_Etk_Cache::Node")))
   {
      item = l->data;
      if (item->filename != filename)
      {
         free(item->filename);
         item->filename = strdup(filename);
      }
      if (item->key != key)
      {
         free(item->key);
         item->key = strdup(key);
      }
      cache->cached_objects = eina_list_remove_list(cache->cached_objects, l);
      cache->cached_objects = eina_list_append(cache->cached_objects, item);
      evas_object_data_set(item->object, "_Etk_Cache::Node", eina_list_last(cache->cached_objects));
      return;
   }

   /* If no more space is available, we remove the oldest object of the cache */
   if (eina_list_count(cache->cached_objects) >= cache->size)
   {
      item = cache->cached_objects->data;
      //evas_object_event_callback_call(item->object, EVAS_CALLBACK_FREE, NULL);
      evas_object_event_callback_del(item->object, EVAS_CALLBACK_FREE, _etk_cache_object_deleted_cb);
      evas_object_del(item->object);
   }

   /* We create a new cache-item for the object and we add it to the cache */
   item = malloc(sizeof(Etk_Cache_Item));
   item->filename = strdup(filename);
   item->key = key ? strdup(key) : NULL;
   item->object = object;

   evas_object_hide(object);
   evas_object_event_callback_add(object, EVAS_CALLBACK_FREE, _etk_cache_object_deleted_cb, cache);

   cache->cached_objects = eina_list_append(cache->cached_objects, item);
   evas_object_data_set(item->object, "_Etk_Cache::Node", eina_list_last(cache->cached_objects));
}

/**
 * @brief Removes an object from the cache. The object won't be destroyed.
 * @param cache a cache system
 * @param object the object to remove from the cache system
 */
void etk_cache_remove(Etk_Cache *cache, Evas_Object *object)
{
   Etk_Cache_Item *item;
   Eina_List *l;

   if (!cache || !object)
      return;

   if ((l = evas_object_data_get(object, "_Etk_Cache::Node")))
   {
      item = l->data;

      evas_object_data_del(object, "_Etk_Cache::Node");
      evas_object_event_callback_del(object, EVAS_CALLBACK_FREE, _etk_cache_object_deleted_cb);
      free(item->filename);
      free(item->key);
      free(item);

      cache->cached_objects = eina_list_remove_list(cache->cached_objects, l);
   }
}

/**
 * @brief Finds an object in the cache according to its filename and its key. If the object is present in the cache,
 * it will be removed from the cache and returned. Otherwise NULL is returned
 * @param cache the cache system where to find the object
 * @param filename the filename of the object to find
 * @param key the key associated to the object to find (the group for an Edje object, the key for an image from an
 * Eet file, or NULL otherwise)
 * @return Returns an object corresponding to the given filename and key, or NULL if no such object is cached
 */
Evas_Object *etk_cache_find(Etk_Cache *cache, const char *filename, const char *key)
{
   Etk_Cache_Item *item;
   Eina_List *l;
   Evas_Object *object;

   if (!cache || !filename)
      return NULL;

   for (l = cache->cached_objects; l; l = l->next)
   {
      item = l->data;
      if (strcmp(item->filename, filename) == 0 && (strcmp(item->key ? item->key : "", key ? key : "") == 0))
      {
         object = item->object;

         evas_object_data_del(object, "_Etk_Cache::Node");
         evas_object_event_callback_del(object, EVAS_CALLBACK_FREE, _etk_cache_object_deleted_cb);
         free(item->filename);
         free(item->key);
         free(item);

         cache->cached_objects = eina_list_remove(cache->cached_objects, item);
         return object;
      }
   }

   return NULL;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Called when an object of the cache is deleted */
static void _etk_cache_object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Cache *cache;

   if (!(cache = data))
      return;

   etk_cache_remove(cache, obj);
   if (cache->free_cb)
      cache->free_cb(obj, cache->free_cb_data);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Cache
 *
 * You will almost never have to use Etk_Cache directly, except if you are implementing a new widget that needs to
 * load a lot of images efficiently, or if you are creating a new tree-model that displays images. @n
 *
 * The cache system is a simple pool where you can store a limited number of Evas objects associated to a filename and
 * to a key. If you try to add a new object with etk_cache_add() while the cache is already full, the oldest object
 * of the cache will be destroyed. You can then retrieve a cached object from its filename/key with etk_cache_find(). @n
 * You can also dynamically change the maximum size of the cache system with etk_cache_size_set() and know the current
 * number of cached objects with etk_cache_num_objects_get().
 */
