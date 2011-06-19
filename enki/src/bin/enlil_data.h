/**
 * Data type associated ho a enlil object
 */

#ifndef ENLIL_DATA_H_
#define ENLIL_DATA_H_

#include <Enlil.h>

typedef struct enlil_data Enlil_Data;
typedef struct enlil_album_data Enlil_Album_Data;
typedef struct enlil_photo_data Enlil_Photo_Data;
typedef struct geocaching_data Geocaching_Data;
typedef struct enlil_collection_data Enlil_Collection_Data;
typedef struct enlil_tag_data Enlil_Tag_Data;

#include "netsync.h"
#include "download.h"
#include "upload.h"
#include "objects.h"
#include "evas_object/photos_list_object.h"

struct enlil_data
{
   Enlil_Library *library;
   Enlil_Sync *sync;
   Enlil_Load *load;

   Download *dl;
   Upload *ul;

   int nb_photos, nb_albums;

   Enlil_Win *win;
   Evas_Object *vbox;
   Tabpanel *tabpanel;
   Tabpanel_Item *library_item;

   List_Left *list_left;
   List_Photo *list_photo;
   Map *map;

   Evas_Object *main_menu;

   Eina_List *auto_open;
};

/**
 * @brief Data associated to all albums. Some information are stored inside as
 * the item's reference in the album list. This allows to have a direct access, for example
 * for deleting an album from a genlist
 */
struct enlil_album_data
{
   Enlil_Data *enlil_data;
   Elm_Genlist_Item *list_album_item;
   Elm_Genlist_Item *import_list_album_item;
   PL_Header_Item *list_photo_item;

   //list of Album_Collection*
   Eina_List *albums_col;

   Inwin *inwin_photo_move_album;
   Elm_Genlist_Item *photo_move_album_list_album_item;

   struct
   {
      Evas_Object *icon;
      Eina_Bool album_netsync_notuptodate;
      Eina_Bool album_notinnetsync;
      int nb_photos_dl;
      Eina_Bool album_local_notuptodate;
      Eina_Bool photos_notinlocal; //photo which are on netsync but not in the local library
      Eina_List *photos_notinlocal_name; //list of char *

      struct
      {
         Evas_Object *win;
         Evas_Object *pb;

         struct
         {
            Evas_Object *pager;
            Evas_Object *bt;
            Evas_Object *pb;
            Eina_Bool is_updating;
         } notinlocal;

         struct
         {
            Evas_Object *pager;
            Evas_Object *bt;
            Evas_Object *pb;
            Eina_Bool is_updating;
         } notinnetsync;

         struct
         {
            Evas_Object *pager;
            Evas_Object *bt;
            Evas_Object *pb;
            Eina_Bool is_updating;
         } netsyncupdate;

         struct
         {
            Evas_Object *pager;
            Evas_Object *bt;
            Evas_Object *pb;
            Eina_Bool is_updating;
         } tags_local_notuptodate;

         struct
         {
            Evas_Object *pager;
            Evas_Object *bt;
            Evas_Object *pb;
            Eina_Bool is_updating;
         } tags_netsync_notuptodate;

         Evas_Object *tb;
         Evas_Object *fr;

         Evas_Object *bt1;
         Evas_Object *bt2;
         Evas_Object *bt3;
         Evas_Object *bt4;
         Evas_Object *bt5;
         Evas_Object *bt6;
      } inwin;

      Eina_List *jobs; // list of Enlil_NetSync_Job*
      Eina_Bool is_sync; //true if a sync job is running on the album
   } netsync;
};

struct enlil_photo_data
{
   Enlil_Data *enlil_data;
   PL_Child_Item *list_photo_item;
   Elm_Slideshow_Item *slideshow_item;
   Eina_List *slideshow_object_items; //list of Slideshow_Item*

   Elm_Gengrid_Item *library_item;

   Panel_Image *panel_image;
   Enlil_Exif_Job *exif_job;
   Eina_Bool clear_exif_data;

   Enlil_IPTC_Job *iptc_job;
   Eina_Bool clear_iptc_data;

   Elm_Map_Marker *marker;

   int cant_create_thumb;

   struct
   {
      Photo_NetSync_Enum state;
      Photo_NetSync_Enum state_tags;
      Eina_List *jobs; // list of Enlil_NetSync_Job*
      Eina_Bool is_sync; //true if a sync job is running on the photo
   } netsync;

   struct
   {
      Elm_Map_Route *route;
   } route;
};

struct enlil_collection_data
{
   Enlil_Data *enlil_data;
   Enlil_Collection *col;

   Elm_Genlist_Item *list_col_item;
};

struct enlil_tag_data
{
   Enlil_Data *enlil_data;
   Enlil_Tag *tag;

   Elm_Genlist_Item *list_tag_item;
};

struct geocaching_data
{
   Enlil_Geocaching *gp;

   Elm_Map_Marker *marker;
   Panel_Geocaching *panel_geocaching;
};

#endif /* ENLIL_DATA_H_ */
