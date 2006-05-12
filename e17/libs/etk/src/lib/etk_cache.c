/** @file etk_cache.c */
#include "etk_cache.h"
#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include "etk_utils.h"

/**
 * @addtogroup Etk_Cache
 * @{
 */

typedef struct Etk_Cache_Image_Object_Item
{
   char *filename;
   Evas_Object *image_object;
} Etk_Cache_Image_Object_Item;

typedef struct Etk_Cache_Edje_Object_Item
{
   char *filename;
   char *group;
   int state;
   Evas_Object *edje_object;
} Etk_Cache_Edje_Object_Item;

typedef struct Etk_Cache_System
{
   Evas *evas;
   
   int max_image_objects;
   Evas_List *cached_image_objects;
   Evas_Hash *image_object_hash;
   
   int max_edje_objects;
   Evas_List *cached_edje_objects;
   Evas_Hash *edje_object_hash;
} Etk_Cache_System;

static Etk_Cache_System *_etk_cache_system_create(Evas *evas);
static Etk_Cache_System *_etk_cache_system_get(Evas *evas);
static void _etk_cache_system_empty(Etk_Cache_System *cache_system);
static void _etk_cache_system_clean(Etk_Cache_System *cache_system);
static char *_etk_cache_edje_key_generate(const char *filename, const char *group);

static Evas_List *_etk_cache_systems = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Shutdowns the cache system
 */
void etk_cache_shutdown()
{
   while (_etk_cache_systems)
   {
      _etk_cache_system_empty(_etk_cache_systems->data);
      _etk_cache_systems = evas_list_remove_list(_etk_cache_systems, _etk_cache_systems);
   }
}

/**
 * @brief Empties the cache system of the evas @a evas
 * @param evas the evas whose cache system should be emptied
 */
void etk_cache_empty(Evas *evas)
{
   _etk_cache_system_empty(_etk_cache_system_get(evas));
}

/**
 * @brief Sets the number of image objects from @a evas that can be stored in the cache. @n
 * Do not call it manually, unless you know what you are doing
 * @param evas the evas which the cache system is attached to
 * @param size the number of image objects from @a evas that the cache can store
 */
void etk_cache_image_object_size_set(Evas *evas, int size)
{
   Etk_Cache_System *cache_system;
   
   if (!(cache_system = _etk_cache_system_get(evas)) && !(cache_system = _etk_cache_system_create(evas)))
      return;
   
   cache_system->max_image_objects = ETK_MIN(0, size);
   _etk_cache_system_clean(cache_system);
}

/**
 * @brief Gets the number of image objects from @a evas that can be stored in the cache
 * @param evas the evas which the cache system is attached to
 * @return Returns the number of image objects from @a evas that the cache can store
 */
int etk_cache_image_object_size_get(Evas *evas)
{
   Etk_Cache_System *cache_system;
   
   if (!(cache_system = _etk_cache_system_get(evas)))
      return 0;
   return cache_system->max_image_objects;
}

/**
 * @brief Sets the number of edje objects from @a evas that can be stored in the cache. @n
 * Do not call it manually, unless you know what what you are doing
 * @param evas the evas which the cache system is attached to
 * @param size the number of edje objects from @a evas that the cache can store
 */
void etk_cache_edje_object_size_set(Evas *evas, int size)
{
   Etk_Cache_System *cache_system;
   
   if (!(cache_system = _etk_cache_system_get(evas)) && !(cache_system = _etk_cache_system_create(evas)))
      return;
   
   cache_system->max_edje_objects = ETK_MIN(0, size);
   _etk_cache_system_clean(cache_system);
}

/**
 * @brief Gets the number of edje objects from @a evas that can be stored in the cache
 * @param evas the evas which the cache system is attached to
 * @return Returns the number of edje objects from @a evas that the cache can store
 */
int etk_cache_edje_object_size_get(Evas *evas)
{
   Etk_Cache_System *cache_system;
   
   if (!(cache_system = _etk_cache_system_get(evas)))
      return 0;
   return cache_system->max_edje_objects;
}

/**
 * @brief Adds an image object to the cache, so it will be available for a further use.
 * The object will be automatically hidden
 * @param image_object the image object to cache
 * @note You should not keep references of the image objects that you cache:
 * they can be destroyed if there is not enough space in the cache
 */
void etk_cache_image_object_add(Evas_Object *image_object)
{
   char *filename;
   Evas *evas;
   Etk_Cache_System *cache_system;
   Evas_List *item_list;
   Etk_Cache_Image_Object_Item *new_item;
   
   if (!image_object)
      return;
   
   evas_object_image_file_get(image_object, &filename, NULL);
   if (!filename || !(evas = evas_object_evas_get(image_object)))
      return;
   if (!(cache_system = _etk_cache_system_get(evas)) && !(cache_system = _etk_cache_system_create(evas)))
      return;
   
   new_item = malloc(sizeof(Etk_Cache_Image_Object_Item));
   new_item->filename = strdup(filename);
   new_item->image_object = image_object;
   evas_object_color_set(new_item->image_object, 255, 255, 255, 255);
   evas_object_clip_unset(new_item->image_object);
   evas_object_hide(new_item->image_object);
   
   if ((item_list = evas_hash_find(cache_system->image_object_hash, new_item->filename)))
   {
      item_list = evas_list_prepend(item_list, new_item);
      evas_hash_modify(cache_system->image_object_hash, new_item->filename, item_list);
   }
   else
   {
      item_list = evas_list_prepend(item_list, new_item);
      cache_system->image_object_hash = evas_hash_add(cache_system->image_object_hash, new_item->filename, item_list);
   }
   
   cache_system->cached_image_objects = evas_list_prepend(cache_system->cached_image_objects, new_item);
   _etk_cache_system_clean(cache_system);
}

/**
 * @brief Removes an image object from the cache. The image object will not be deleted
 * @param image_object the object to remove from the cache
 */
void etk_cache_image_object_remove(Evas_Object *image_object)
{
   char *filename;
   Evas *evas;
   Etk_Cache_System *cache_system;
   Evas_List *item_list, *l;
   Etk_Cache_Image_Object_Item *item = NULL;
   
   if (!image_object)
      return;
   
   evas_object_image_file_get(image_object, &filename, NULL);
   if (!filename || !(evas = evas_object_evas_get(image_object)))
      return;
   if (!(cache_system = _etk_cache_system_get(evas)))
      return;
   
   if ((item_list = evas_hash_find(cache_system->image_object_hash, filename)))
   {
      for (l = item_list; l; l = l->next)
      {
         item = l->data;
         if (item->image_object == image_object)
         {
            if ((item_list = evas_list_remove_list(item_list, l)))
               evas_hash_modify(cache_system->image_object_hash, filename, item_list);
            else
               cache_system->image_object_hash = evas_hash_del(cache_system->image_object_hash, filename, NULL);
            break;
         }
         item = NULL;
      }
   }
   
   if (item)
   {
      cache_system->cached_image_objects = evas_list_remove(cache_system->cached_image_objects, item);
      free(item->filename);
      free(item);
   }
}

/**
 * @brief Finds in the cache an image object on evas @a evas and whose filename is @a filename. @n
 * If an image is found, it will be removed from the cache
 * @param evas the evas which the searched object should belong
 * @param filename the filename of the image to find
 * @return Returns the corresponding image object, or NULL on failure
 * @note The object will need to be shown
 */
Evas_Object *etk_cache_image_object_find(Evas *evas, const char *filename)
{
   Etk_Cache_System *cache_system;
   Evas_List *items;
   Etk_Cache_Image_Object_Item *item;
   Evas_Object *object;
   
   if (!evas || !filename || !(cache_system = _etk_cache_system_get(evas)))
      return NULL;
   if (!(items = evas_hash_find(cache_system->image_object_hash, filename)))
      return NULL;
   
   item = items->data;
   object = item->image_object;
   
   if ((items = evas_list_remove_list(items, items)))
      evas_hash_modify(cache_system->image_object_hash, item->filename, items);
   else
      cache_system->image_object_hash = evas_hash_del(cache_system->image_object_hash, item->filename, NULL);
   
   cache_system->cached_image_objects = evas_list_remove(cache_system->cached_image_objects, item);
   free(item->filename);
   free(item);
   
   return object;
}

/**
 * @brief Adds an edje object to the cache, so it will be available for a further use.
 * The object will be automatically hidden
 * @param edje_object the edje object to cache
 * @note You shouldn't keep references of the edje objects that you cache:
 * they could be destroyed if there is not enough space in the cache
 */
void etk_cache_edje_object_add(Evas_Object *edje_object)
{
   etk_cache_edje_object_add_with_state(edje_object, -1);
}

/**
 * @brief Adds an edje object to the cache, so it will be available for a further use.
 * The object will be automatically hidden
 * @param edje_object the edje object to cache
 * @param state The state id to associate to the object.
 * It can be used later if you want to find an edje object in the cache with a specific state. @n
 * if @a state is < 0, the function will have the same effect than etk_cache_edje_object_add()
 * @note You shouldn't keep references of the edje objects you cache:
 * they could be destroyed if there is not enough space in the cache
 */
void etk_cache_edje_object_add_with_state(Evas_Object *edje_object, int state)
{
   const char *filename;
   const char *group;
   char *key;
   Evas *evas;
   Etk_Cache_System *cache_system;
   Evas_List *item_list;
   Etk_Cache_Edje_Object_Item *new_item;
   
   if (!edje_object)
      return;
   
   edje_object_file_get(edje_object, &filename, &group);
   if (!filename || !group || !(evas = evas_object_evas_get(edje_object)))
      return;
   if (!(cache_system = _etk_cache_system_get(evas)) && !(cache_system = _etk_cache_system_create(evas)))
      return;
   
   new_item = malloc(sizeof(Etk_Cache_Edje_Object_Item));
   new_item->filename = strdup(filename);
   new_item->group = strdup(group);
   new_item->state = ETK_MIN(state, -1);
   new_item->edje_object = edje_object;
   evas_object_color_set(new_item->edje_object, 255, 255, 255, 255);
   evas_object_clip_unset(new_item->edje_object);
   evas_object_hide(new_item->edje_object);
   
   key = _etk_cache_edje_key_generate(new_item->filename, new_item->group);
   if ((item_list = evas_hash_find(cache_system->edje_object_hash, key)))
   {
      item_list = evas_list_prepend(item_list, new_item);
      evas_hash_modify(cache_system->edje_object_hash, key, item_list);
   }
   else
   {
      item_list = evas_list_prepend(item_list, new_item);
      cache_system->edje_object_hash = evas_hash_add(cache_system->edje_object_hash, key, item_list);
   }
   free(key);
   
   cache_system->cached_edje_objects = evas_list_prepend(cache_system->cached_edje_objects, new_item);
   _etk_cache_system_clean(cache_system);
}

/**
 * @brief Removes an edje object from the cache. The edje object will not be deleted
 * @param edje_object the object to remove from the cache
 */
void etk_cache_edje_object_remove(Evas_Object *edje_object)
{
   const char *filename;
   const char *group;
   char *key;
   Evas *evas;
   Etk_Cache_System *cache_system;
   Evas_List *item_list, *l;
   Etk_Cache_Edje_Object_Item *item = NULL;
   
   if (!edje_object)
      return;
   
   edje_object_file_get(edje_object, &filename, &group);
   if (!filename || !group || !(evas = evas_object_evas_get(edje_object)))
      return;
   if (!(cache_system = _etk_cache_system_get(evas)))
      return;
   
   key = _etk_cache_edje_key_generate(filename, group);
   if ((item_list = evas_hash_find(cache_system->edje_object_hash, key)))
   {
      for (l = item_list; l; l = l->next)
      {
         item = l->data;
         if (item->edje_object == edje_object)
         {
            if ((item_list = evas_list_remove_list(item_list, l)))
               evas_hash_modify(cache_system->edje_object_hash, key, item_list);
            else
               cache_system->edje_object_hash = evas_hash_del(cache_system->edje_object_hash, key, NULL);
            break;
         }
         item = NULL;
      }
   }
   free(key);
   
   if (item)
   {
      cache_system->cached_edje_objects = evas_list_remove(cache_system->cached_edje_objects, item);
      free(item->filename);
      free(item->group);
      free(item);
   }
}

/**
 * @brief Finds in the cache an edje object on evas @a evas and whose filename is @a filename. @n
 * If an edje object is found, it will be removed from the cache
 * @param evas the evas which the searched object should belong
 * @param filename the filename of the edje object to find
 * @param group the group of the edje object to find
 * @return Returns the corresponding edje object, or NULL on failure
 * @note The object will need to be shown
 */
Evas_Object *etk_cache_edje_object_find(Evas *evas, const char *filename, const char *group)
{
   return etk_cache_edje_object_find_with_state(evas, filename, group, -1);
}

/**
 * @brief Finds in the cache an edje object on evas @a evas and whose filename is @a filename. @n
 * If an edje object is found, it will be removed from the cache
 * @param evas the evas which the searched object should belong
 * @param filename the filename of the edje object to find
 * @param group the group of the edje object to find
 * @param state the state of the edje object to find
 * If @a state is < 0, any edje object with the correct filename and group will be returned
 * @return Returns the corresponding edje object, or NULL on failure
 * @note The object will need to be shown
 */
Evas_Object *etk_cache_edje_object_find_with_state(Evas *evas, const char *filename, const char *group, int state)
{
   char *key;
   Etk_Cache_System *cache_system;
   Evas_List *items, *l;
   Etk_Cache_Edje_Object_Item *item = NULL;
   Evas_Object *object;
   
   if (!evas || !filename || !group || !(cache_system = _etk_cache_system_get(evas)))
      return NULL;
   
   key = _etk_cache_edje_key_generate(filename, group);
   if (!(items = evas_hash_find(cache_system->edje_object_hash, key)))
   {
      free(key);
      return NULL;
   }
   
   if (state < 0)
      item = items->data;
   else
   {
      for (l = items; l; l = l->next)
      {
         item = l->data;
         if (item->state == state)
            break;
         item = NULL;
      }
   }
   
   if (!item)
   {
      free(key);
      return NULL;
   }
   
   object = item->edje_object;
   
   if ((items = evas_list_remove(items, item)))
      evas_hash_modify(cache_system->edje_object_hash, key, items);
   else
      cache_system->edje_object_hash = evas_hash_del(cache_system->edje_object_hash, key, NULL);
   free(key);
   
   cache_system->cached_edje_objects = evas_list_remove(cache_system->cached_edje_objects, item);
   free(item->filename);
   free(item->group);
   free(item);
   
   return object;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Creates a new cache system attached to an evas */
static Etk_Cache_System *_etk_cache_system_create(Evas *evas)
{
   Etk_Cache_System *cache_system;
   
   if (!evas)
      return NULL;
   
   cache_system = malloc(sizeof(Etk_Cache_System));
   cache_system->evas = evas;
   
   cache_system->max_image_objects = 256;
   cache_system->cached_image_objects = NULL;
   cache_system->image_object_hash = NULL;
   
   cache_system->max_edje_objects = 256;
   cache_system->cached_edje_objects = NULL;
   cache_system->edje_object_hash = NULL;
   
   _etk_cache_systems = evas_list_append(_etk_cache_systems, cache_system);
   return cache_system;
}

/* Gets the cache system linked to the evas */
static Etk_Cache_System *_etk_cache_system_get(Evas *evas)
{
   Evas_List *l;
   Etk_Cache_System *cache_system;
   
   for (l = _etk_cache_systems; l; l = l->next)
   {
      cache_system = l->data;
      if (cache_system->evas == evas)
         return cache_system;
   }
   return NULL;
}

/* Empties a cache system */
static void _etk_cache_system_empty(Etk_Cache_System *cache_system)
{
   Etk_Cache_Image_Object_Item *image_item;
   Etk_Cache_Edje_Object_Item *edje_item;
   
   if (!cache_system)
      return;
   
   while (cache_system->cached_image_objects)
   {
      image_item = cache_system->cached_image_objects->data;
      evas_object_del(image_item->image_object);
      free(image_item->filename);
      free(image_item);
      cache_system->cached_image_objects = evas_list_remove_list(cache_system->cached_image_objects,
         cache_system->cached_image_objects);
   }
   evas_hash_free(cache_system->image_object_hash);
   cache_system->image_object_hash = NULL;
   
   while (cache_system->cached_edje_objects)
   {
      edje_item = cache_system->cached_edje_objects->data;
      evas_object_del(edje_item->edje_object);
      free(edje_item->filename);
      free(edje_item->group);
      free(edje_item);
      cache_system->cached_edje_objects = evas_list_remove_list(cache_system->cached_edje_objects,
         cache_system->cached_edje_objects);
   }
   evas_hash_free(cache_system->edje_object_hash);
   cache_system->edje_object_hash = NULL;
}

/* Removes the image and the edje objects that can't be stored in the cache system */
static void _etk_cache_system_clean(Etk_Cache_System *cache_system)
{
   Etk_Cache_Image_Object_Item *image_item;
   Etk_Cache_Edje_Object_Item *edje_item;
   Evas_List *l, *l2, *items;
   char *key;
   int count;
   
   if (!cache_system)
      return;
   
   /* Cleans the cache of the image objects */
   count = evas_list_count(cache_system->cached_image_objects);
   for (l = evas_list_last(cache_system->cached_image_objects); l && count > cache_system->max_image_objects; l = l2)
   {
      image_item = l->data;
      
      if ((items = evas_hash_find(cache_system->image_object_hash, image_item->filename)))
      {
         if ((items = evas_list_remove(items, image_item)))
            evas_hash_modify(cache_system->image_object_hash, image_item->filename, items);
         else
            cache_system->image_object_hash = evas_hash_del(cache_system->image_object_hash, image_item->filename, NULL);
      }
      
      evas_object_del(image_item->image_object);
      free(image_item->filename);
      free(image_item);
      
      l2 = l->prev;
      cache_system->cached_image_objects = evas_list_remove_list(cache_system->cached_image_objects, l);
   }
   
   /* Cleans the cache of the edje objects */
   count = evas_list_count(cache_system->cached_edje_objects);
   for (l = evas_list_last(cache_system->cached_edje_objects); l && count > cache_system->max_edje_objects; l = l2)
   {
      edje_item = l->data;
      
      key = _etk_cache_edje_key_generate(edje_item->filename, edje_item->group);
      if ((items = evas_hash_find(cache_system->edje_object_hash, key)))
      {
         if ((items = evas_list_remove(items, edje_item)))
            evas_hash_modify(cache_system->edje_object_hash, key, items);
         else
            cache_system->edje_object_hash = evas_hash_del(cache_system->edje_object_hash, key, NULL);
      }
      free(key);
      
      evas_object_del(edje_item->edje_object);
      free(edje_item->filename);
      free(edje_item->group);
      free(edje_item);
      
      l2 = l->prev;
      cache_system->cached_edje_objects = evas_list_remove_list(cache_system->cached_edje_objects, l);
   }
}

/* Generates the key to use in hash table according to the filename and the group. The key will have to be freed */
static char *_etk_cache_edje_key_generate(const char *filename, const char *group)
{
   char *key;
   
   if (!filename || !group)
      return NULL;
   
   key = malloc(strlen(filename) + strlen(group) + 2);
   sprintf(key, "%s/%s", filename, group);
   return key;
}

/** @} */
