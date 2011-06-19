#ifndef  PHOTOS_LIST_OBJECT_INC

#define  PHOTOS_LIST_OBJECT_INC

#include <Evas.h>
#include <Edje.h>
#include <Ecore_Evas.h>
#include <stdio.h>
#include <Elementary.h>

typedef struct PL_Header_Item PL_Header_Item;
typedef struct PL_Header_Item_Class PL_Header_Item_Class;
typedef Evas_Object *
(*PLHeaderItemIconGetFunc)(const void *data, Evas_Object *obj);

typedef struct PL_Child_Item PL_Child_Item;
typedef struct PL_Child_Item_Class PL_Child_Item_Class;
typedef Evas_Object *
(*PLChildItemIconGetFunc)(const void *data, Evas_Object *obj);

struct PL_Header_Item_Class
{
   struct
   {
      PLHeaderItemIconGetFunc icon_get;
   } func;
};

struct PL_Child_Item_Class
{
   struct
   {
      PLChildItemIconGetFunc icon_get;
   } func;
};

Evas_Object *
photos_list_object_add(Evas_Object *obj);

void
photos_list_object_freeze(Evas_Object *obj, int freeze);
void
photos_list_object_clear(Evas_Object *obj);

void
photos_list_object_sub_items_size_set(Evas_Object *obj, int w, int h);
void
photos_list_object_header_height_set(Evas_Object *obj, int h);

void
photos_list_object_multiselect_set(Evas_Object *obj, Eina_Bool b);
Eina_Bool
photos_list_object_multiselect_get(Evas_Object *obj);

PL_Header_Item *
photos_list_object_item_header_append(Evas_Object *obj,
                                      PL_Header_Item_Class *itc, void *data);
PL_Header_Item *
photos_list_object_item_header_append_relative(Evas_Object *obj,
                                               PL_Header_Item_Class *itc,
                                               void *data,
                                               PL_Header_Item *relative);
void
photos_list_object_header_move_after(PL_Header_Item *header,
                                     PL_Header_Item *relative);
void
photos_list_object_header_bring_in(PL_Header_Item *item);
void
photos_list_object_header_goto(PL_Header_Item *item);
void
photos_list_object_top_goto(Evas_Object *obj);

void
photos_list_object_header_childs_del(PL_Header_Item *item);

void
photos_list_object_child_move_after(PL_Child_Item *item,
                                    PL_Child_Item *relative);

void
photos_list_object_item_update(PL_Child_Item *item);
void
photos_list_object_header_update(PL_Header_Item *header);
void
photos_list_object_item_del(PL_Child_Item *item);
void
photos_list_object_header_del(PL_Header_Item *header);
const Evas_Object *
photos_list_object_item_object_get(PL_Child_Item *item);
const Evas_Object *
photos_list_object_header_object_get(PL_Header_Item *header);
void
photos_list_object_item_bring_in(PL_Child_Item *child);

void
photos_list_object_hide_all(Evas_Object *obj);
void
photos_list_object_show_all(Evas_Object *obj);
void
photos_list_object_header_show(PL_Header_Item *header);
void
photos_list_object_item_show(PL_Child_Item *child);
void
photos_list_object_item_hide(PL_Child_Item *child);

PL_Child_Item *
photos_list_object_item_append(Evas_Object *obj, PL_Child_Item_Class *itc,
                               PL_Header_Item *header, void *data);
PL_Child_Item *
photos_list_object_item_append_relative(Evas_Object *obj,
                                        PL_Child_Item_Class *itc,
                                        PL_Header_Item *header, void *data,
                                        PL_Child_Item *relative);
Eina_List *
photos_list_object_selected_get(Evas_Object *obj);
void *
photos_list_object_item_data_get(PL_Child_Item *item);
void
photos_list_object_item_selected_set(PL_Child_Item *item, Eina_Bool selected);

#endif   /* ----- #ifndef PHOTOS_LIST_OBJECT_INC  ----- */

