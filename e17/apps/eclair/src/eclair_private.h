#ifndef _ECLAIR_PRIVATE_H_
#define _ECLAIR_PRIVATE_H_

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <pthread.h>
#include <libxml/tree.h>
#include <sqlite3.h>

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
typedef enum _Eclair_Dialog_File_Chooser_Type Eclair_Dialog_File_Chooser_Type;
typedef struct _Eclair_Dialogs_Manager Eclair_Dialogs_Manager;
typedef struct _Eclair_Playlist_Container Eclair_Playlist_Container;
typedef struct _Eclair_Playlist_Container_Object Eclair_Playlist_Container_Object;
typedef struct _Eclair_Color Eclair_Color;
typedef struct _Eclair_Window Eclair_Window;
typedef struct _Eclair_Database Eclair_Database;

struct _Eclair_Database
{
   sqlite3 *db;
   Eclair *eclair;
};

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
   //Container vars
   Evas_Object *clip;
   Evas_Object *grabber;
   Evas_List *entry_objects;
   Evas_List **entries;
   Eclair_Media_File *last_selected;
   double scroll_percent;

   //Scroll vars
   double scroll_start_time;
   double scroll_speed;
   Ecore_Timer *scroll_timer;

   //Scroll_To vars
   double scroll_to_percent;
   double scroll_to_speed;
   Ecore_Timer *scroll_to_timer;

   //Theme vars
   char *entry_theme_path;
   int entry_height;
   Eclair_Color normal_entry_bg_color;
   Eclair_Color normal_entry_fg_color;
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
   char default_playlist_path[256];
   char database_path[256];
   xmlDocPtr config_doc;
   xmlNodePtr root_node;
};

enum _Eclair_Dialog_File_Chooser_Type
{
   ECLAIR_FC_NONE = 0,
   ECLAIR_FC_ADD_FILES,
   ECLAIR_FC_LOAD_PLAYLIST,
   ECLAIR_FC_SAVE_PLAYLIST
};

struct _Eclair_Dialogs_Manager
{
   //File chooser widget vars
   GladeXML *file_chooser_xml;
   GtkWidget *file_chooser_all_button;
   GtkWidget *file_chooser_none_button;
   GtkWidget *file_chooser_save_playlist_button;
   GtkWidget *file_chooser_load_playlist_button;
   GtkWidget *file_chooser_add_files_button;
   GtkWidget *file_chooser_cancel_button;
   GtkWidget *file_chooser_widget;
   GtkWidget *file_chooser_dialog;

   //Menu widget vars
   GladeXML *menu_xml;
   GtkWidget *menu_widget;

   //Search window vars
   GladeXML *search_window_xml;
   GtkWidget *search_window_window;

   gboolean should_popup_menu;
   Eclair_Dialog_File_Chooser_Type should_open_file_chooser;
   gboolean should_open_search_window;
   gboolean should_quit;

   pthread_t dialogs_thread;

   Eclair *eclair;
};

enum _Eclair_Add_File_State
{
   ECLAIR_IDLE = 0,
   ECLAIR_ADDING_FILE_TO_ADD,
   ECLAIR_ADDING_FILE_TO_TREAT
};

struct _Eclair_Cover_Manager
{
   Eclair_Add_File_State cover_add_state;
   Evas_List *cover_files_to_add;
   Evas_List *cover_files_to_treat;
   Evas_List *not_in_amazon_db;

   Evas_Bool cover_should_treat_files;
   Evas_Bool cover_delete_thread;
   pthread_cond_t cover_cond;
   pthread_mutex_t cover_mutex;
   pthread_t cover_thread;

   Eclair *eclair;
};

struct _Eclair_Meta_Tag_Manager
{
   Eclair_Add_File_State meta_tag_add_state;
   Evas_List *meta_tag_files_to_add;
   Evas_List *meta_tag_files_to_scan;

   Evas_Bool meta_tag_should_scan_files;
   Evas_Bool meta_tag_delete_thread;
   pthread_cond_t meta_tag_cond;
   pthread_mutex_t meta_tag_mutex;
   pthread_t meta_tag_thread;
};

struct _Eclair_Media_File
{
   //Meta tags
   char *path;
   char *cover_path;
   char *artist;
   char *title;
   char *album;
   char *genre;
   char *comment;
   int length;
   int track;
   int year;
   int bitrate;
   int samplerate;

   Evas_List *shuffle_node;
   Eclair_Playlist_Container_Object *container_object;
   unsigned char selected : 1;
   unsigned char in_meta_tag_process : 1;
   unsigned char in_cover_process : 1;
   unsigned char delete_me : 1;
};

struct _Eclair_Playlist
{
   Evas_List *playlist;
   Evas_List *current;
   Evas_List *shuffle_list;
   Evas_List *removed_media_files;
   Evas_Bool shuffle;
   Evas_Bool repeat;
   Ecore_Timer *media_files_destructor_timer;

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

struct _Eclair_Window
{
   Ecore_X_Window x_window;
   Ecore_Evas *ecore_window;
   Evas *evas;
   Evas_Object *edje_object;
   Evas_Object *draggies;
   Evas_Coord min_width;
   Evas_Coord min_height;
   Evas_Coord max_width;
   Evas_Coord max_height;
   char *window_name;
   int x, y;
   Eclair *eclair;

   unsigned char should_resize : 1;
   unsigned char borderless : 1;
   unsigned char shaded : 1;
   unsigned char resizable : 1;
   unsigned char main_window : 1;
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
   Eclair_Window *gui_window;
   Eclair_Window *playlist_window;
   Eclair_Window *equalizer_window;
   Eclair_Window *cover_window;
   Evas_Object *playlist_container;
   Eclair_Window *playlist_container_owner;
   Evas_Object *previous_cover;
   Evas_Object *cover;
   Eclair_Window *cover_owner;

   Evas_Bool use_progress_bar_drag_for_time;
   Evas_Bool dont_update_progressbar;
   char *gui_theme_file;
   int playlist_entry_height;
   Eclair_Engine gui_engine;
   Eclair_Drop_Object drop_object;
   Eclair_Window *drop_window;

   //Core vars
   Eclair_State state;
   Eclair_Playlist playlist;
   Eclair_Subtitles subtitles;
   Eclair_Dialogs_Manager dialogs_manager;
   Eclair_Meta_Tag_Manager meta_tag_manager;
   Eclair_Cover_Manager cover_manager;
   Eclair_Config config;
   Eclair_Database database;
   Evas_Bool start_playing;
   double seek_to_pos;

   //Arguments
   int *argc;
   char ***argv;
};

#endif
