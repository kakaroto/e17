// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#ifndef MAIN_H
#define MAIN_H


#include <Elementary.h>
#include <string.h>

#include <Enlil.h>
#include "evas_object/photos_list_object.h"
#include "evas_object/photo_object.h"
#include "evas_object/tabpanel.h"

#include "../../config.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define D_(s) gettext(s)

#define THEME PACKAGE_DATA_DIR"/themes/default.edj"

extern int APP_LOG_DOMAIN;
#define LOG_DOMAIN APP_LOG_DOMAIN

extern const char *media_player;

typedef struct enlil_data Enlil_Data;
typedef struct enlil_win Enlil_Win;
typedef struct list_left List_Left;
typedef struct map Map;
typedef struct list_photo List_Photo;
typedef struct enlil_album_data Enlil_Album_Data;
typedef struct enlil_photo_data Enlil_Photo_Data;
typedef struct geocaching_data Geocaching_Data;
typedef struct enlil_collection_data Enlil_Collection_Data;
typedef struct enlil_tag_data Enlil_Tag_Data;
typedef struct panel_image Panel_Image;
typedef struct panel_geocaching Panel_Geocaching;
typedef struct album_menu Album_Menu;
typedef struct photo_menu Photo_Menu;
typedef struct collection_menu Collection_Menu;
typedef struct Download Download;
typedef struct Upload Upload;
typedef struct tag_menu Tag_Menu;
typedef struct inwin Inwin;


struct enlil_data
{
    Enlil_Root *root;
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

    Evas_Object *notify_load;
    Evas_Object *notify_load_lbl;

    Evas_Object *notify_sync;
    Evas_Object *notify_sync_lbl;

    Eina_List *auto_open;
};


typedef enum Photo_Flickr_Enum
{
   PHOTO_FLICKR_NONE,
   PHOTO_FLICKR_NOTUPTODATE,
   PHOTO_FLICKR_FLICKRNOTUPTODATE,
   PHOTO_FLICKR_NOTINFLICKR
} Photo_Flickr_Enum;
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
	Eina_Bool album_flickr_notuptodate;
	Eina_Bool album_notinflickr;
	Eina_Bool album_notuptodate;
	Eina_Bool photos_notinlocal; //photo which are on flickr but not in the local library
	Eina_List *photos_notinlocal_name; //list of char *

	struct
	  {
	     Evas_Object *win;
	     Evas_Object *pb;

	     struct {
		  Evas_Object *pager;
		  Evas_Object *bt;
		  Evas_Object *pb;
		  Eina_Bool is_updating;
	     } notinlocal;

	     struct {
		  Evas_Object *pager;
		  Evas_Object *bt;
		  Evas_Object *pb;
		  Eina_Bool is_updating;
	     } notinflickr;

	     struct {
		  Evas_Object *pager;
		  Evas_Object *bt;
		  Evas_Object *pb;
		  Eina_Bool is_updating;
	     } flickrupdate;



	     Evas_Object *tb;
	     Evas_Object *fr;

	     Evas_Object *bt1;
	     Evas_Object *bt2;
	     Evas_Object *bt3;
	     Evas_Object *bt4;
	     Evas_Object *bt5;
	     Evas_Object *bt6;
	  } inwin;

	Eina_List *jobs; // list of Enlil_Flickr_Job*
     } flickr_sync;
};

struct enlil_photo_data
{
    Enlil_Data *enlil_data;
    PL_Child_Item *list_photo_item;
    Elm_Slideshow_Item *slideshow_item;
	Eina_List *slideshow_object_items; //list of Slideshow_Item*

    Panel_Image *panel_image;
    Enlil_Exif_Job *exif_job;
    Eina_Bool clear_exif_data;

    Enlil_IPTC_Job *iptc_job;
    Eina_Bool clear_iptc_data;

    Elm_Map_Marker *marker;

    int cant_create_thumb;

    struct
     {
	Photo_Flickr_Enum state;
	Eina_List *jobs; // list of Enlil_Flickr_Job*
     } flickr_sync;
};

struct enlil_collection_data
{
   Enlil_Data *enlil_data;
   Enlil_Collection *col;

   Elm_Genlist_Item * list_col_item;
};

struct enlil_tag_data
{
   Enlil_Data *enlil_data;
   Enlil_Tag *tag;

   Elm_Genlist_Item * list_tag_item;
};

struct geocaching_data
{
   Enlil_Geocaching *gp;

   Elm_Map_Marker *marker;
   Panel_Geocaching *panel_geocaching;
};

struct enlil_win
{
    Evas_Object *win;
    Evas_Object *bg;
};

struct list_left
{
    Enlil_Data *enlil_data;

    Eina_Bool is_map;

    Evas_Object *bx;
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

typedef void (*Inwin_Del)  (void *data);
typedef void (*Inwin_Apply)  (void *data);
typedef void (*Inwin_Close)  (void *data);
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
   INWIN_PREFERENCES
};

struct inwin
{
   Evas_Object *inwin;
   Inwin_Type type;

   Inwin_Del del_cb;
   Inwin_Apply apply_cb;
   Inwin_Close close_cb;
   void *data;

   Evas_Object *gl;
   Evas_Object *entry;
   Evas_Object *bt_apply;

   Enlil_Photo *photo;
   Eina_List *photos;
   Enlil_Album *album;
   const char *file;
};

struct Download
{
   Evas_Object *main;
   Evas_Object *pb;
   Evas_Object *lbl;
};

struct Upload
{
   Evas_Object *main;
   Evas_Object *pb;
   Evas_Object *lbl;
};



extern Enlil_Photo *current_photo;
extern Enlil_Data *enlil_data;


const char *album_flickr_edje_signal_get(Enlil_Album *album);
const char *photo_flickr_edje_signal_get(Photo_Flickr_Enum e);

/* main window */
Enlil_Win *enlil_win_new();
void enlil_win_bg_set(Enlil_Win *win, const char *file);

void root_set(const char *root_path);
void close_cb(void *data, Evas_Object *obj, void *event_info);

void select_list_photo();

Evas_Object *menu_photo_new(Evas_Object *parent);

Album_Menu *album_collection_menu_new(Evas_Object *parent, Enlil_Album *album);
Album_Menu *album_tag_menu_new(Evas_Object *parent, Enlil_Album *album);
Album_Menu *album_menu_new(Evas_Object *parent, Enlil_Album *album);
Album_Menu *album_sorts_menu_new(Evas_Object *parent, Enlil_Album *album);
void album_menu_free(Album_Menu *album_menu);

Collection_Menu *collection_menu_new(Evas_Object *parent, Enlil_Collection *col);
void collection_menu_free(Collection_Menu *col_menu);

Tag_Menu *tag_menu_new(Evas_Object *parent, Enlil_Tag *tag);
void tag_menu_free(Tag_Menu *tag_menu);

Photo_Menu *photo_tag_menu_new(Evas_Object *parent, Enlil_Photo *photo);
Photo_Menu *photo_menu_new(Evas_Object *parent, Enlil_Photo *photo, Eina_List *photos);
void photo_menu_free(Photo_Menu *photo_menu);


void enlil_album_data_free(Enlil_Album *album, void *_data);
void enlil_photo_data_free(Enlil_Photo *photo, void *_data);
void enlil_collection_data_free(Enlil_Collection *col, void *_data);
void enlil_tag_data_free(Enlil_Tag *tag, void *_data);;
void enlil_geocaching_data_free(Enlil_Geocaching *gp, void *_data);

/* notifications windows */
void notify_sync_content_set(Enlil_Data *enlil_data, const char *msg, Eina_Bool loading);
void notify_load_content_set(Enlil_Data *enlil_data, const char *msg, Eina_Bool loading);

Panel_Image *panel_image_new(Evas_Object *parent, Enlil_Photo *photo);
void panel_image_free(Panel_Image **panel_image);
void panel_image_1_1(Enlil_Photo *photo);
void panel_image_fit(Enlil_Photo *photo);
void panel_image_fill(Enlil_Photo *photo);
void panel_image_rotation_90(Enlil_Photo *photo);
void panel_image_rotation_R90(Enlil_Photo *photo);
void panel_image_rotation_180(Enlil_Photo *photo);
void panel_image_flip_vertical(Enlil_Photo *photo);
void panel_image_flip_horizontal(Enlil_Photo *photo);
void panel_image_blur(Enlil_Photo *photo);
void panel_image_sharpen(Enlil_Photo *photo);
void panel_image_sepia(Enlil_Photo *photo);
void panel_image_grayscale(Enlil_Photo *photo);
void panel_image_save_as(Enlil_Photo *photo);
void panel_image_save(Enlil_Photo *photo);
void panel_image_undo(Enlil_Photo *photo);
void panel_image_redo(Enlil_Photo *photo);
void panel_image_exifs_update(Enlil_Photo *photo);
void panel_image_iptcs_update(Enlil_Photo *photo);

Panel_Geocaching *panel_geocaching_new(Evas_Object *parent, Enlil_Geocaching *geocaching);
void panel_geocaching_free(Panel_Geocaching **panel_geocaching);

/* Left list */
List_Left *list_left_new(Evas_Object *win);
void list_left_data_set(List_Left *list_album, Enlil_Data *data);
void list_left_add(List_Left *list_album, Enlil_Album *album);
void list_left_remove(List_Left *list_album, Enlil_Album *album);
void list_left_update(List_Left *list_album, Enlil_Album *album);

void list_left_col_add(List_Left *list_album, Enlil_Collection *col);
void list_left_append_relative(List_Left *list_left, Enlil_Album *album, Elm_Genlist_Item *relative);
void list_left_col_album_add(List_Left *list_album, Enlil_Collection *col, Enlil_Album *album);
void list_left_col_album_remove(List_Left *list_album, Enlil_Collection *col, Enlil_Album *album);

void list_left_tag_add(List_Left *list_album, Enlil_Tag *tag);

/* Map */
Map *map_new(Evas_Object *win);
void map_photo_add(Map *map, Enlil_Photo *photo);
void map_photo_update(Map *map, Enlil_Photo *photo);
void map_photo_remove(Map *map, Enlil_Photo *photo);
void map_free(Map *map);
void map_geocaching_add(Map *map, Enlil_Geocaching *gp);
void map_geocaching_update(Map *map, Enlil_Geocaching *gp);
void map_geocaching_remove(Map *map, Enlil_Geocaching *gp);

/* List of photos */
List_Photo *list_photo_new(Evas_Object *win);
void list_photo_data_set(List_Photo *list_photo, Enlil_Data *enlil_data);
void list_photo_album_add(List_Photo *list_photo, Enlil_Album *album);
void list_photo_album_append_relative(List_Photo *list_photo, Enlil_Album *album, PL_Header_Item *relative);
void list_photo_photo_add(List_Photo *list_photo, Enlil_Album *album, Enlil_Photo *photo);
void list_photo_photo_append_relative(List_Photo *list_photo, Enlil_Album *album, Enlil_Photo *photo, PL_Child_Item *relative);

//import
Evas_Object *import_new(Evas_Object *win);
void import_album_new(Enlil_Album *album);

//slideshow
void slideshow_show();
void slideshow_album_add(Enlil_Album *_album, Enlil_Photo *photo);
void slideshow_root_add(Enlil_Root *root, Enlil_Photo *photo);
void slideshow_clear();
void slideshow_hide();

//inwin
void inwin_free(Inwin *inwin);
Inwin *inwin_tag_new_new(Inwin_Del del_cb, void *data, Eina_List *photos);
Inwin *inwin_album_tag_new_new(Inwin_Del del_cb, void *data, Enlil_Album *album);
Inwin *inwin_collection_new_new(Inwin_Del del_cb, void *data, Enlil_Album *album);
Inwin *inwin_album_new_new(Inwin_Del del_cb, void *data);
Inwin *inwin_photo_delete_new(Evas_Object *win, Inwin_Del del_cb, void *data, Eina_List *photos);
Inwin *inwin_album_delete_new(Inwin_Del del_cb, void *data, Enlil_Album *album);
Inwin *inwin_photo_move_album_new(Inwin_Del del_cb, void *data, Eina_List *photos);
Inwin *inwin_photo_save_new(Inwin_Del del_cb, Inwin_Apply apply_cb, Inwin_Close close_cb,
      void *data, Enlil_Photo *photo);
Inwin *inwin_album_rename_new(Inwin_Del del_cb, void *data, Enlil_Album *album);
Inwin *inwin_save_as_file_exists_new(Inwin_Del del_cb, Inwin_Apply apply_cb, void *data, const char *file);
Inwin *inwin_preferences_new();

Evas_Object *flickr_menu_new(Evas_Object *win);
void flickr_menu_animated_set(Eina_Bool animated);

/* Download manager */
Download *download_new(Evas_Object *parent);
void download_free(Download **_ul);
void download_add(Download *ul, const char *source, Enlil_Photo *photo);

/* Upload manager */
Upload *upload_new(Evas_Object *parent);
void upload_free(Upload **_ul);
void upload_add(Upload *ul, Enlil_Photo *photo);
void upload_album_create_add(Upload *ul, Enlil_Album *album);




/* Photo manager callbacks */
void sync_done_cb(void *data, Enlil_Sync *sync);
void sync_start_cb(void *data, Enlil_Sync *sync);
void sync_error_cb(void *data, Enlil_Sync *sync,  Sync_Error error, const char* msg);
void sync_album_new_cb(void *data, Enlil_Sync *sync, Enlil_Root*root, Enlil_Album *album);
void sync_album_update_cb(void *data, Enlil_Sync *sync, Enlil_Root*root, Enlil_Album *album);
void sync_album_disappear_cb(void *data, Enlil_Sync *sync, Enlil_Root*root, Enlil_Album *album);
void sync_photo_new_cb(void *data, Enlil_Sync *sync,Enlil_Album *album, Enlil_Photo *photo);
void sync_photo_update_cb(void *data, Enlil_Sync *sync,Enlil_Album *album, Enlil_Photo *photo);
void sync_photo_disappear_cb(void *data, Enlil_Sync *sync,Enlil_Album *album, Enlil_Photo *photo);

void load_done_cb(void *data, Enlil_Load *load, int nb_albums, int nb_photos);
void load_error_cb(void *data, Enlil_Load *load,  Load_Error error, const char* msg);
void load_album_done_cb(void *data, Enlil_Load *load, Enlil_Root*root, Enlil_Album *album);

void monitor_album_new_cb(void *data, Enlil_Root*root, const char *path);
void monitor_album_delete_cb(void *data, Enlil_Root*root, const char *path);
void monitor_album_update_cb(void *data, Enlil_Root *root, Enlil_Album *album);
void monitor_enlil_delete_cb(void *data, Enlil_Root*root);
void monitor_photo_new_cb(void *data, Enlil_Root*root, Enlil_Album *album, const char *path);
void monitor_photo_delete_cb(void *data, Enlil_Root*root, Enlil_Album *album, const char *path);
void monitor_photo_update_cb(void *data, Enlil_Root*root, Enlil_Album *album, const char *path);

void thumb_done_cb(void *data, Enlil_Photo *photo, const char *file);
void thumb_error_cb(void *data, Enlil_Photo *photo);

void collection_new_cb(void *data, Enlil_Root *root, Enlil_Collection *col);
void collection_delete_cb(void *data, Enlil_Root *root, Enlil_Collection *col);
void collection_album_new_cb(void *data, Enlil_Root *root, Enlil_Collection *col, Enlil_Album *album);
void collection_album_delete_cb(void *data, Enlil_Root *root, Enlil_Collection *col, Enlil_Album *album);

void tag_new_cb(void *data, Enlil_Root *root, Enlil_Tag *tag);
void tag_delete_cb(void *data, Enlil_Root *root, Enlil_Tag *tag);
void tag_photo_new_cb(void *data, Enlil_Root *root, Enlil_Tag *tag, Enlil_Photo *photo);
void tag_photo_delete_cb(void *data, Enlil_Root *root, Enlil_Tag *tag, Enlil_Photo *photo);

void exif_load_done(void *data, Enlil_Exif_Job *job, Eina_List *exifs);
void iptc_load_done(void *data, Enlil_IPTC_Job *job, Eina_List *iptcs);

void geocaching_done_cb(void *data, Eina_Hash *db);
void geocaching_remove_marker_cb(void *data, Eina_Hash *db);


void flickr_job_start_cb(void *data, Enlil_Flickr_Job* job, Enlil_Album *album, Enlil_Photo *photo);
void flickr_job_done_cb(void *data, Enlil_Flickr_Job* job, Enlil_Album *album, Enlil_Photo *photo);

void flickr_album_new_cb(void *data, Enlil_Root *root, Enlil_Album *album);
void flickr_album_flickrnotuptodate_cb(void *data, Enlil_Root *root, Enlil_Album *album);
void flickr_album_notuptodate_cb(void *data, Enlil_Root *root, Enlil_Album *album);
void flickr_album_notinflickr_cb(void *data, Enlil_Root *root, Enlil_Album *album);
void flickr_album_uptodate_cb(void *data, Enlil_Root *root, Enlil_Album *album);
void flickr_error_cb(void *data, Enlil_Root *root);

void flickr_photo_new_cb(void *data, Enlil_Album *album, const char *photo_name, const char *photo_id);
void flickr_photo_notinflickr_cb(void *data, Enlil_Album *album, Enlil_Photo *photo);
void flickr_photo_known_cb(void *data, Enlil_Album *album, Enlil_Photo *photo);

void flickr_photo_flickrnotuptodate_cb(void *data, Enlil_Photo *photo);
void flickr_photo_notuptodate_cb(void *data, Enlil_Photo *photo);
void flickr_photo_uptodate_cb(void *data, Enlil_Photo *photo);

void flickr_album_error_cb(void *data, Enlil_Album *album);
void flickr_photo_error_cb(void *data, Enlil_Photo *photo);

void flickr_photos_notinlocal_sizes_get_cb(void *data, Eina_List *sizes, Eina_Bool error);

//main menu
Evas_Object *main_menu_new(Evas_Object *parent);
void main_menu_update_libraries_list(Eina_List *list);
void main_menu_loading_disable_set(Eina_Bool disabled);
void main_menu_sync_disable_set(Eina_Bool disabled);
void main_menu_noroot_disabled_set(Eina_Bool disabled);

//inwin flickr sync
Evas_Object *flickr_sync_new(Evas_Object *win, Enlil_Album *album);
void flickr_sync_update(Enlil_Album *album);


#endif
