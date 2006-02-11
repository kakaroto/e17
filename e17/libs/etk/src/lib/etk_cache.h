/** @file etk_cache.h */
#ifndef _ETK_CACHE_H_
#define _ETK_CACHE_H_

#include <Evas.h>

void etk_cache_shutdown();
void etk_cache_empty(Evas *evas);

void etk_cache_image_object_size_set(Evas *evas, int size);
int etk_cache_image_object_size_get(Evas *evas);
void etk_cache_edje_object_size_set(Evas *evas, int size);
int etk_cache_edje_object_size_get(Evas *evas);

void etk_cache_image_object_add(Evas_Object *image_object);
void etk_cache_image_object_remove(Evas_Object *image_object);
Evas_Object *etk_cache_image_object_find(Evas *evas, const char *filename);

void etk_cache_edje_object_add(Evas_Object *edje_object);
void etk_cache_edje_object_add_with_state(Evas_Object *edje_object, int state);
void etk_cache_edje_object_remove(Evas_Object *edje_object);
Evas_Object *etk_cache_edje_object_find(Evas *evas, const char *filename, const char *group);
Evas_Object *etk_cache_edje_object_find_with_state(Evas *evas, const char *filename, const char *group, int state);


#endif
