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

/** @file etk_cache.h */
#ifndef _ETK_CACHE_H_
#define _ETK_CACHE_H_

#include <Evas.h>

#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Cache Cache system of Etk
 * @brief The cache system is used by widgets that frequently need to load images, such as Etk_Tree or Etk_Iconbox
 * @{
 */


/**
 * @brief A system used to cache image objects for widgets that frequently need to load images
 * @structinfo
 */
struct Etk_Cache
{
   /* private: */
   Eina_List *cached_objects;
   int size;

   void (*free_cb)(Evas_Object *object, void *data);
   void *free_cb_data;
};


Etk_Cache   *etk_cache_new(int size);
void         etk_cache_destroy(Etk_Cache *cache);
void         etk_cache_clear(Etk_Cache *cache);

void         etk_cache_free_callback_set(Etk_Cache *cache, void (*free_cb)(Evas_Object *object, void *data), void *data);
void         etk_cache_size_set(Etk_Cache *cache, int size);
int          etk_cache_size_get(Etk_Cache *cache);
int          etk_cache_num_objects_get(Etk_Cache *cache);

void         etk_cache_add(Etk_Cache *cache, Evas_Object *object, const char *filename, const char *key);
void         etk_cache_remove(Etk_Cache *cache, Evas_Object *object);
Evas_Object *etk_cache_find(Etk_Cache *cache, const char *filename, const char *key);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
