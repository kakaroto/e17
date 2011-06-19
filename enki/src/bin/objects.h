#ifndef OBJECTS_H_
#define OBJECTS_H_

#include "evas_object/tabpanel.h"
#include "evas_object/photos_list_object.h"

typedef struct enlil_win Enlil_Win;
typedef struct list_left List_Left;
typedef struct map Map;
typedef struct list_photo List_Photo;
typedef struct panel_image Panel_Image;
typedef struct panel_geocaching Panel_Geocaching;
typedef struct album_menu Album_Menu;
typedef struct photo_menu Photo_Menu;
typedef struct collection_menu Collection_Menu;
typedef struct tag_menu Tag_Menu;
typedef struct inwin Inwin;

#include "enlil_data.h"

struct enlil_win
{
   Evas_Object *win;
   Evas_Object *bg;
};

struct list_left
{
   Enlil_Data *enlil_data;

   Eina_Bool is_map;
   Evas_Object *obj;

   Tabpanel *tabpanel;

   Tabpanel *tb_liste_map;
   Evas_Object *panels_map;

   Evas_Object *map;

   Evas_Object *gl_albums;
   Evas_Object *gl_collections;
   Evas_Object *gl_tags;
};

struct map
{
   Evas_Object *bx;
   Evas_Object *map;
   Evas_Object *sl;
   Evas_Object *rect;
   Evas_Object *display_geocaching;
   Evas_Object *display_photos;

   Enlil_Photo *selected;
};

struct list_photo
{
   Enlil_Data *enlil_data;

   Evas_Object *o_list;
   Evas_Object *bx;
   Evas_Object *sl;
   Evas_Object *multiselection;

   Evas_Object *lbl_nb_albums_photos;

   int photo_w;
   int photo_h;
};

struct panel_geocaching
{
   Tabpanel_Item *tabpanel_item;
   Evas_Object *rect;
   Evas_Object *map;

   Enlil_Geocaching *geocaching;
   Elm_Map_Marker *marker;

   Elm_Map_Marker_Class *itc;
   Elm_Map_Group_Class *itc_group;
};

struct panel_image
{
   Tabpanel_Item *tabpanel_item;

   Enlil_Photo *photo;

   Evas_Object *panes;
   double panes_size;

   Evas_Object *panes_h;
   double panes_h_size;

   Evas_Object *photocam;
   Evas_Object *sl;
   Evas_Object *rect;

   Evas_Object *menu;
   Evas_Object *tb;

   struct
   {
      Evas_Object *slideshow;
   } slideshow;

   Evas_Object *entry_name;
   Evas_Object *entry_description;
   Evas_Object *entry_author;
   Evas_Object *lbl_file_size;
   Eina_Bool save_description_name;
   Ecore_Timer *timer_description_name;
   struct
   {
      Evas_Object *undo;
      Evas_Object *redo;

      Elm_Menu_Item *item_undo;
      Elm_Menu_Item *item_redo;

      Eina_List *items_undo;
      Eina_List *items_redo;
   } undo;

   //list of Enlil_Trans_Job
   Eina_List *jobs_trans;

   Evas_Object *notify_trans;
   Evas_Object *notify_trans_bx;
   //list of Evas_Object *
   Eina_List *notify_trans_items;

   Enlil_Trans_History *history;

   struct
   {
      Evas_Object *gl;
      Evas_Object *size;
   } exifs;

   struct
   {
      Evas_Object *gl;
   } iptcs;

   Tabpanel *tabpanel;

   struct
   {
      Eina_Bool save;
      const char *path;
   } save;

   Evas_Object *inwin;
};

struct album_menu
{
   Enlil_Album *album;
   Evas_Object *menu;
   Evas_Object *inwin;
   Evas_Object *entry;
};

struct photo_menu
{
   Enlil_Photo *photo;
   Eina_List *photos;
   Evas_Object *menu;
};

struct collection_menu
{
   Enlil_Collection *col;
   Evas_Object *menu;
};

struct tag_menu
{
   Enlil_Tag *tag;
   Evas_Object *menu;
};

typedef void
(*Inwin_Del)(void *data);
typedef void
(*Inwin_Apply)(void *data);
typedef void
(*Inwin_Close)(void *data);
typedef enum inwin_type Inwin_Type;
enum inwin_type
{
   INWIN_TAG_NEW,
   INWIN_ALBUM_TAG_NEW,
   INWIN_COLLECTION_NEW,
   INWIN_ALBUM_NEW,
   INWIN_PHOTO_DELETE,
   INWIN_ALBUM_DELETE,
   INWIN_PHOTO_MOVE_ALBUM,
   INWIN_PHOTO_SAVE,
   INWIN_ALBUM_RENAME,
   INWIN_SAVE_AS_FILE_EXISTS,
   INWIN_PREFERENCES,
   INWIN_LIBRARY_DELETE,
   INWIN_LOGIN_FAILED
};

struct inwin
{
   Evas_Object *inwin;
   Inwin_Type type;

   Inwin_Del del_cb;
   Inwin_Apply apply_cb;
   Inwin_Close close_cb;
   void *data;

   Eina_List *themes; //list of char*
   Evas_Object *gl;
   Evas_Object *entry;
   Evas_Object *entry2;
   Evas_Object *entry3;
   Evas_Object *entry4;
   Evas_Object *entry5;
   Evas_Object *bt_apply;
   Evas_Object *check;

   Enlil_Photo *photo;
   Eina_List *photos;
   Enlil_Album *album;
   const char *file;
};

Enlil_Win *
enlil_win_new();
void
enlil_win_bg_set(Enlil_Win *win, const char *file);

Evas_Object *
menu_photo_new(Evas_Object *parent);
Album_Menu *
album_collection_menu_new(Evas_Object *parent, Enlil_Album *album);
Album_Menu *
album_tag_menu_new(Evas_Object *parent, Enlil_Album *album);
Album_Menu *
album_menu_new(Evas_Object *parent, Enlil_Album *album);
Album_Menu *
album_sorts_menu_new(Evas_Object *parent, Enlil_Album *album);
void
album_menu_free(Album_Menu *album_menu);

Collection_Menu *
collection_menu_new(Evas_Object *parent, Enlil_Collection *col);
void
collection_menu_free(Collection_Menu *col_menu);

Tag_Menu *
tag_menu_new(Evas_Object *parent, Enlil_Tag *tag);
void
tag_menu_free(Tag_Menu *tag_menu);

Photo_Menu *
photo_tag_menu_new(Evas_Object *parent, Enlil_Photo *photo);
Photo_Menu *
photo_menu_new(Evas_Object *parent, Enlil_Photo *photo, Eina_List *photos);
void
photo_menu_init(List_Photo *enlil_photo, Evas_Object *edje);
void
photo_menu_free(Photo_Menu *photo_menu);

Panel_Image *
panel_image_new(Evas_Object *parent, Enlil_Photo *photo);
void
panel_image_free(Panel_Image **panel_image);
void
panel_image_1_1(Enlil_Photo *photo);
void
panel_image_fit(Enlil_Photo *photo);
void
panel_image_fill(Enlil_Photo *photo);
void
panel_image_rotation_90(Enlil_Photo *photo);
void
panel_image_rotation_R90(Enlil_Photo *photo);
void
panel_image_rotation_180(Enlil_Photo *photo);
void
panel_image_flip_vertical(Enlil_Photo *photo);
void
panel_image_flip_horizontal(Enlil_Photo *photo);
void
panel_image_blur(Enlil_Photo *photo);
void
panel_image_sharpen(Enlil_Photo *photo);
void
panel_image_sepia(Enlil_Photo *photo);
void
panel_image_grayscale(Enlil_Photo *photo);
void
panel_image_save_as(Enlil_Photo *photo);
void
panel_image_save(Enlil_Photo *photo);
void
panel_image_undo(Enlil_Photo *photo);
void
panel_image_redo(Enlil_Photo *photo);
void
panel_image_exifs_update(Enlil_Photo *photo);
void
panel_image_iptcs_update(Enlil_Photo *photo);

Panel_Geocaching *
panel_geocaching_new(Evas_Object *parent, Enlil_Geocaching *geocaching);
void
panel_geocaching_free(Panel_Geocaching **panel_geocaching);

/* Left list */
List_Left *
list_left_new(Evas_Object *win);
void
list_left_data_set(List_Left *list_album, Enlil_Data *data);
void
list_left_add(List_Left *list_album, Enlil_Album *album);
void
list_left_remove(List_Left *list_album, Enlil_Album *album);
void
list_left_update(List_Left *list_album, Enlil_Album *album);

void
list_left_col_add(List_Left *list_album, Enlil_Collection *col);
void
list_left_append_relative(List_Left *list_left, Enlil_Album *album,
                          Elm_Genlist_Item *relative);
void
list_left_col_album_add(List_Left *list_album, Enlil_Collection *col,
                        Enlil_Album *album);
void
list_left_col_album_remove(List_Left *list_album, Enlil_Collection *col,
                           Enlil_Album *album);

void
list_left_tag_add(List_Left *list_album, Enlil_Tag *tag);

/* Map */
Map *
map_new(Evas_Object *win);
void
map_photo_add(Map *map, Enlil_Photo *photo);
void
map_photo_update(Map *map, Enlil_Photo *photo);
void
map_photo_remove(Map *map, Enlil_Photo *photo);
void
map_free(Map *map);
void
map_geocaching_add(Map *map, Enlil_Geocaching *gp);
void
map_geocaching_update(Map *map, Enlil_Geocaching *gp);
void
map_geocaching_remove(Map *map, Enlil_Geocaching *gp);

/* List of photos */
List_Photo *
list_photo_new(Evas_Object *win);
void
list_photo_data_set(List_Photo *list_photo, Enlil_Data *enlil_data);
void
list_photo_album_add(List_Photo *list_photo, Enlil_Album *album);
void
list_photo_album_append_relative(List_Photo *list_photo, Enlil_Album *album,
                                 PL_Header_Item *relative);
void
list_photo_photo_add(List_Photo *list_photo, Enlil_Album *album,
                     Enlil_Photo *photo);
void
list_photo_photo_append_relative(List_Photo *list_photo, Enlil_Album *album,
                                 Enlil_Photo *photo, PL_Child_Item *relative);

//import
Evas_Object *
import_new(Evas_Object *win);
void
import_album_new(Enlil_Album *album);

//inwin
void
inwin_free(Inwin *inwin);
Inwin *
inwin_login_failed_new();
Inwin *
inwin_tag_new_new(Inwin_Del del_cb, void *data, Eina_List *photos);
Inwin *
inwin_album_tag_new_new(Inwin_Del del_cb, void *data, Enlil_Album *album);
Inwin *
inwin_collection_new_new(Inwin_Del del_cb, void *data, Enlil_Album *album);
Inwin *
inwin_album_new_new(Inwin_Del del_cb, void *data);
Inwin *
inwin_photo_delete_new(Evas_Object *win, Inwin_Del del_cb, void *data,
                       Eina_List *photos);
Inwin *
inwin_album_delete_new(Inwin_Del del_cb, void *data, Enlil_Album *album);
Inwin *
inwin_photo_move_album_new(Inwin_Del del_cb, void *data, Eina_List *photos);
Inwin *
inwin_photo_save_new(Inwin_Del del_cb, Inwin_Apply apply_cb,
                     Inwin_Close close_cb, void *data, Enlil_Photo *photo);
Inwin *
inwin_album_rename_new(Inwin_Del del_cb, void *data, Enlil_Album *album);
Inwin *
inwin_save_as_file_exists_new(Inwin_Del del_cb, Inwin_Apply apply_cb,
                              void *data, const char *file);
Inwin *
inwin_preferences_new();
Inwin *
inwin_library_delete_new(Enlil_Data *enlil_data);
Inwin *
inwin_netsync_error_new(const char *message);

//main menu
void
main_menu_new(Evas_Object *parent);
void
main_menu_update_libraries_list();
void
main_menu_loading_disable_set(Eina_Bool disabled);
void
main_menu_sync_disable_set(Eina_Bool disabled);
void
main_menu_nolibrary_disabled_set(Eina_Bool disabled);

#endif /* OBJECTS_H_ */
