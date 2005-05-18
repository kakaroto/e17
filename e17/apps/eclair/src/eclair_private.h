#ifndef _ECLAIR_PRIVATE_H_
#define _ECLAIR_PRIVATE_H_

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include <libxml/tree.h>

typedef struct _Eclair Eclair;
typedef enum _Eclair_State Eclair_State;
typedef enum _Eclair_Engine Eclair_Engine;
typedef struct _Eclair_Media_File Eclair_Media_File;
typedef struct _Eclair_Playlist Eclair_Playlist;
typedef struct _Eclair_Subtitle Eclair_Subtitle;
typedef struct _Eclair_Subtitles Eclair_Subtitles;
typedef enum _Eclair_Add_File_State Eclair_Add_File_State;
typedef struct _Eclair_Meta_Tag_Manager Eclair_Meta_Tag_Manager;
typedef struct _Eclair_Cover_Manager Eclair_Cover_Manager;
typedef struct _Eclair_Config Eclair_Config;
typedef enum _Eclair_Config_Type Eclair_Config_Type;
typedef enum _Eclair_Drop_Object Eclair_Drop_Object;
typedef struct _Eclair_Dialogs_Manager Eclair_Dialogs_Manager;
typedef struct _Eclair_Playlist_Container Eclair_Playlist_Container;
typedef struct _Eclair_Playlist_Container_Object Eclair_Playlist_Container_Object;
typedef struct _Eclair_Color Eclair_Color;

struct _Eclair_Color
{
   int r, g, b, a;
};

struct _Eclair_Playlist_Container_Object
{
   Evas_Object *rect;
   Evas_Object *text;
};

struct _Eclair_Playlist_Container
{
   Evas_Object *clip;
   Evas_Object *grabber;
   Evas_List *entry_objects;
   Evas_List **entries;
   Eclair_Media_File *last_selected;
   int entry_height;
   double scroll_percent;
   Ecore_Timer *scroll_timer;
   double scroll_speed;
   double scroll_start_time;
   char *entry_theme_path;
   Eclair_Color selected_entry_bg_color;
   Eclair_Color selected_entry_fg_color;
   Eclair_Color current_entry_bg_color;
   Eclair_Color current_entry_fg_color;
   Eclair *eclair;
};

struct _Eclair_Config
{
   char config_dir_path[256];
   char covers_dir_path[256];
   char config_file_path[256];
   xmlDocPtr config_doc;
   xmlNodePtr root_node;
};

enum _Eclair_Add_File_State
{
   ECLAIR_IDLE = 0,
   ECLAIR_ADDING_FILE_TO_ADD,
   ECLAIR_ADDING_FILE_TO_TREAT
};

struct _Eclair_Dialogs_Manager
{
   GtkWidget *file_chooser_dialog;
   GtkWidget *menu_widget;
   gboolean should_popup_menu;
   gboolean should_open_file_chooser;
   pthread_t dialogs_thread;
   Eclair *eclair;
};

struct _Eclair_Cover_Manager
{
   Eclair_Add_File_State cover_add_state;
   Evas_List *cover_files_to_add;
   Evas_List *cover_files_to_treat;
   Evas_List *not_in_amazon_db;
   Evas_Bool cover_delete_thread;
   Eclair *eclair;
   pthread_cond_t cover_cond;
   pthread_mutex_t cover_mutex;
   pthread_t cover_thread;
};

struct _Eclair_Meta_Tag_Manager
{
   Eclair_Add_File_State meta_tag_add_state;
   Evas_List *meta_tag_files_to_add;
   Evas_List *meta_tag_files_to_scan;
   Evas_Bool meta_tag_delete_thread;
   pthread_cond_t meta_tag_cond;
   pthread_mutex_t meta_tag_mutex;
   pthread_t meta_tag_thread;
};

struct _Eclair_Media_File
{
   char *path;
   char *cover_path;
   char *artist;
   char *title;
   char *album;
   char *genre;
   char *comment;
   int length, track;
   short year;
   Eclair_Playlist_Container_Object *container_object;
   Evas_Bool selected;
};

struct _Eclair_Playlist
{
   Evas_List *playlist;
   Evas_List *current;
   Evas_Bool shuffle;
   Evas_Bool repeat;
   Eclair *eclair;
};

enum _Eclair_State
{
   ECLAIR_PLAYING = 0,
   ECLAIR_PAUSE,
   ECLAIR_STOP
};

enum _Eclair_Engine
{
   ECLAIR_SOFTWARE = 0,
   ECLAIR_GL
};

struct _Eclair_Subtitle
{
   double start, end;
   char *subtitle;
};

struct _Eclair_Subtitles
{
   Evas_List *subtitles;
   Evas_Bool enable;
};

enum _Eclair_Drop_Object
{
   ECLAIR_DROP_NONE = 0,
   ECLAIR_DROP_COVER,
   ECLAIR_DROP_PLAYLIST
};

struct _Eclair
{
   //Video related vars
   Ecore_Evas *video_window;
   Evas_Object *video_object;
   Evas_Object *black_background;
   Evas_Object *subtitles_object;
   Eclair_Engine video_engine;
   pthread_t video_create_thread;

   //Gui related vars
   Ecore_X_Window gui_x_window;
   Ecore_Evas *gui_window;
   Evas_Object *gui_object;
   Evas_Object *gui_draggies;
   Evas_Object *gui_cover, *gui_previous_cover;
   Evas_Object *playlist_container;
   int playlist_entry_height;
   double seek_to_pos;
   Evas_Bool use_progress_bar_drag_for_time;
   Evas_Bool dont_update_progressbar;
   Eclair_Engine gui_engine;
   char *gui_theme_file;
   Eclair_Drop_Object gui_drop_object;

   //Core vars
   Eclair_State state;
   Eclair_Playlist playlist;
   Eclair_Subtitles subtitles;
   Eclair_Dialogs_Manager dialogs_manager;
   Eclair_Meta_Tag_Manager meta_tag_manager;
   Eclair_Cover_Manager cover_manager;
   Eclair_Config config;

   //Arguments
   int *argc;
   char ***argv;
};

#endif
