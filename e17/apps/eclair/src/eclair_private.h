#ifndef _ECLAIR_PRIVATE_H_
#define _ECLAIR_PRIVATE_H_

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <gtk/gtk.h>
#include <pthread.h>

typedef struct _Eclair Eclair;
typedef enum _Eclair_State Eclair_State;
typedef enum _Eclair_Engine Eclair_Engine;
typedef struct _Eclair_Playlist_Media_File Eclair_Playlist_Media_File;
typedef struct _Eclair_Playlist Eclair_Playlist;
typedef struct _Eclair_Subtitle Eclair_Subtitle;
typedef struct _Eclair_Subtitles Eclair_Subtitles;

struct _Eclair_Playlist_Media_File
{
   char *path;
   char *artist;
   char *title;
   char *album;
   char *genre;
   char *comment;
   int length, year, track;
   Evas_Object *playlist_entry;
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

struct _Eclair
{
   //Video related vars
   Ecore_Evas *video_window;
   Evas_Object *video_object;
   Evas_Object *black_background;
   Evas_Object *subtitles_object;
   Eclair_Engine video_engine;

   //Gui related vars
   Ecore_Evas *gui_window;
   Evas_Object *gui_object;
   Evas_Object *gui_draggies;
   Evas_Object *playlist_container;
   int playlist_entry_height;
   double seek_to_pos;
   Evas_Bool dont_update_progressbar;
   Eclair_Engine gui_engine;

   //File chooser related vars
   GtkWidget *file_chooser_widget;
   Evas_Bool file_chooser_th_created;
   pthread_mutex_t file_chooser_mutex;
   pthread_t file_chooser_thread;

   //Metatag related vars
   Evas_List *meta_tag_files_to_scan;
   pthread_mutex_t meta_tag_mutex;
   pthread_cond_t meta_tag_cond;
   pthread_t meta_tag_thread;
   
   Eclair_State state;
   Eclair_Playlist playlist;
   Eclair_Subtitles subtitles;

};

#endif
