#ifndef  SLIDESHOW_OBJECT_INC
#define  SLIDESHOW_OBJECT_INC

#include <Evas.h>
#include <Edje.h>
#include <Ecore_Evas.h>
#include <stdio.h>
#include <Elementary.h>

typedef struct _Slideshow_Item_Class Slideshow_Item_Class;
typedef struct _Slideshow_Item Slideshow_Item;
typedef Evas_Object *
(*SlideshowIconGet)(const void *data, Evas_Object *obj);

struct _Slideshow_Item_Class
{
   SlideshowIconGet icon_get;
};

Evas_Object *
slideshow_object_add(Evas_Object *obj);
void
         slideshow_object_file_set(Evas_Object *obj, const char *file,
                                   const char *group);
Slideshow_Item *
slideshow_object_item_append(Evas_Object *obj, Slideshow_Item_Class *itc,
                             void *data);
void
slideshow_object_item_select(Evas_Object *obj, Slideshow_Item *item);
void *
slideshow_object_item_data_get(Slideshow_Item *item);
Eina_List *
slideshow_object_items_get(Evas_Object *obj);
void
slideshow_object_item_del(Slideshow_Item *item);
void
slideshow_object_item_update(Slideshow_Item *item);

#endif   /* ----- #ifndef SLIDESHOW_OBJECT_INC  ----- */

