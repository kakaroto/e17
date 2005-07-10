#ifndef _ECLAIR_PLAYLIST_CONTAINER_H_
#define _ECLAIR_PLAYLIST_CONTAINER_H_

#include <Evas.h>
#include <Ecore.h>
#include "eclair_types.h"

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

Evas_Object *eclair_playlist_container_object_add(Evas *evas, Eclair *eclair);
void eclair_playlist_container_set_entry_theme_path(Evas_Object *obj, const char *entry_theme_path);
void eclair_playlist_container_set_media_list(Evas_Object *obj, Evas_List **media_list);
void eclair_playlist_container_update(Evas_Object *obj);

void eclair_playlist_container_scroll(Evas_Object *obj, double num_entries);
void eclair_playlist_container_scroll_percent_set(Evas_Object *obj, double percent);
double eclair_playlist_container_scroll_percent_get(Evas_Object *obj);
void eclair_playlist_container_scroll_start(Evas_Object *obj, double speed);
void eclair_playlist_container_scroll_stop(Evas_Object *obj);
void eclair_playlist_container_scroll_to_list(Evas_Object *obj, Evas_List *element);
int eclair_playlist_container_offset_get_from_percent(Evas_Object *obj, double scroll_percent);
double eclair_playlist_container_percent_get_from_offset(Evas_Object *obj, int offset);
int eclair_playlist_container_offset_get(Evas_Object *obj);
void eclair_playlist_container_offset_set(Evas_Object *obj, int offset);
Evas_Bool eclair_playlist_container_nth_element_is_visible(Evas_Object *obj, int n, Evas_Bool entirely);

void eclair_playlist_container_select_file(Evas_Object *obj, Eclair_Media_File *media_file, Evas_Modifier *modifiers);
void eclair_playlist_container_select_all(Evas_Object *obj);
void eclair_playlist_container_select_none(Evas_Object *obj);
void eclair_playlist_container_invert_selection(Evas_Object *obj);

void eclair_playlist_container_scrollbar_drag_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_playlist_container_scroll_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);

#endif
