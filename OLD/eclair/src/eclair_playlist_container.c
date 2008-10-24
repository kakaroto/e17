#include "eclair_playlist_container.h"
#include <Edje.h>
#include <Ecore_File.h>
#include <math.h>
#include "eclair.h"
#include "eclair_window.h"
#include "eclair_media_file.h"
#include "eclair_playlist.h"

static Evas_Smart *playlist_container_smart;
void eclair_playlist_container_update(Evas_Object *obj);
static int _eclair_playlist_container_scroll_timer(void *data);
static int _eclair_playlist_container_scroll_to_timer(void *data);
static void _eclair_playlist_container_smart_add(Evas_Object *obj);
static void _eclair_playlist_container_smart_del(Evas_Object *obj);
static void _eclair_playlist_container_smart_layer_set(Evas_Object *obj, int layer);
static void _eclair_playlist_container_smart_raise(Evas_Object *obj);
static void _eclair_playlist_container_smart_lower(Evas_Object *obj);
static void _eclair_playlist_container_smart_stack_above(Evas_Object *obj, Evas_Object *above);
static void _eclair_playlist_container_smart_stack_below(Evas_Object *obj, Evas_Object *above);
static void _eclair_playlist_container_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _eclair_playlist_container_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _eclair_playlist_container_smart_show(Evas_Object *obj);
static void _eclair_playlist_container_smart_hide(Evas_Object *obj);
static void _eclair_playlist_container_smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _eclair_playlist_container_smart_clip_set(Evas_Object *obj, Evas_Object *clip);
static void _eclair_playlist_container_smart_clip_unset(Evas_Object *obj);
static void _eclair_playlist_container_entry_down_cb(void *data, Evas *evas, Evas_Object *entry, void *event_info);
static void _eclair_playlist_container_wheel_cb(void *data, Evas *evas, Evas_Object *playlist_container, void *event_info);
static void _eclair_playlist_container_scroll_percent_changed_cb(void *data, Evas_Object *obj, void *event_info);

//Create a new playlist container object
Evas_Object *eclair_playlist_container_object_add(Evas *evas, Eclair *eclair)
{
   if (!playlist_container_smart)
   {
      playlist_container_smart = evas_smart_new("eclair_playlist_container",
         _eclair_playlist_container_smart_add,
         _eclair_playlist_container_smart_del,
         _eclair_playlist_container_smart_layer_set,
         _eclair_playlist_container_smart_raise,
         _eclair_playlist_container_smart_lower,
         _eclair_playlist_container_smart_stack_above,
         _eclair_playlist_container_smart_stack_below,
         _eclair_playlist_container_smart_move,
         _eclair_playlist_container_smart_resize,
         _eclair_playlist_container_smart_show,
         _eclair_playlist_container_smart_hide,
         _eclair_playlist_container_smart_color_set,
         _eclair_playlist_container_smart_clip_set,
         _eclair_playlist_container_smart_clip_unset,
         eclair);
   }
   return evas_object_smart_add(evas, playlist_container_smart);
}

//Set the path of the theme used for the playlist entry
void eclair_playlist_container_set_entry_theme_path(Evas_Object *obj, const char *entry_theme_path)
{
   Eclair_Playlist_Container *playlist_container;
   Evas_Object *entry;
   Evas_Coord entry_height;
   char *color_string;
   Eclair_Color *color, *color2;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)))
      return;

   free(playlist_container->entry_theme_path);
   playlist_container->entry_theme_path = strdup(entry_theme_path);

   entry = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(entry, playlist_container->entry_theme_path, "eclair_playlist_entry");
   edje_object_size_min_get(entry, NULL, &entry_height);
   playlist_container->entry_height = (int)entry_height;

   //Load color values
   color = &playlist_container->normal_entry_bg_color;
   color_string = edje_file_data_get(entry_theme_path, "normal_entry_bg_color");
   if (!color_string || sscanf(color_string, "%d %d %d %d", &color->r, &color->g, &color->b, &color->a) != 4)
   {
      color->r = 0;
      color->g = 0;
      color->b = 0;
      color->a = 0;
   }
   else
      evas_color_argb_premul(color->a, &color->r, &color->g, &color->b);
   free(color_string);

   color = &playlist_container->normal_entry_fg_color;
   color_string = edje_file_data_get(entry_theme_path, "normal_entry_fg_color");
   if (!color_string || sscanf(color_string, "%d %d %d %d", &color->r, &color->g, &color->b, &color->a) != 4)
   {
      color->r = 255;
      color->g = 255;
      color->b = 255;
      color->a = 255;
   }
   else
      evas_color_argb_premul(color->a, &color->r, &color->g, &color->b);
   free(color_string);

   color = &playlist_container->selected_entry_bg_color;
   color2 = &playlist_container->normal_entry_bg_color;
   color_string = edje_file_data_get(entry_theme_path, "selected_entry_bg_color");
   if (!color_string || sscanf(color_string, "%d %d %d %d", &color->r, &color->g, &color->b, &color->a) != 4)
   {
      color->r = color2->r;
      color->g = color2->g;
      color->b = color2->b;
      color->a = color2->a;
   }
   else
      evas_color_argb_premul(color->a, &color->r, &color->g, &color->b);
   free(color_string);

   color = &playlist_container->selected_entry_fg_color;
   color2 = &playlist_container->normal_entry_fg_color;
   color_string = edje_file_data_get(entry_theme_path, "selected_entry_fg_color");
   if (!color_string || sscanf(color_string, "%d %d %d %d", &color->r, &color->g, &color->b, &color->a) != 4)
   {
      color->r = color2->r;
      color->g = color2->g;
      color->b = color2->b;
      color->a = color2->a;
   }
   else
      evas_color_argb_premul(color->a, &color->r, &color->g, &color->b);
   free(color_string);

   color = &playlist_container->current_entry_bg_color;
   color2 = &playlist_container->normal_entry_bg_color;
   color_string = edje_file_data_get(entry_theme_path, "current_entry_bg_color");
   if (!color_string || sscanf(color_string, "%d %d %d %d", &color->r, &color->g, &color->b, &color->a) != 4)
   {
      color->r = color2->r;
      color->g = color2->g;
      color->b = color2->b;
      color->a = color2->a;
   }
   else
      evas_color_argb_premul(color->a, &color->r, &color->g, &color->b);
   free(color_string);

   color = &playlist_container->current_entry_fg_color;
   color2 = &playlist_container->normal_entry_fg_color;
   color_string = edje_file_data_get(entry_theme_path, "current_entry_fg_color");
   if (!color_string || sscanf(color_string, "%d %d %d %d", &color->r, &color->g, &color->b, &color->a) != 4)
   {
      color->r = color2->r;
      color->g = color2->g;
      color->b = color2->b;
      color->a = color2->a;
   }
   else
      evas_color_argb_premul(color->a, &color->r, &color->g, &color->b);
   free(color_string);

   evas_object_del(entry);
   eclair_playlist_container_update(obj);
   //TODO: resize the container to recreate the entry objects with the new theme
}

//Set the list of media files used by the playlist_container
void eclair_playlist_container_set_media_list(Evas_Object *obj, Eina_List **media_list)
{
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)))
      return;

   playlist_container->entries = media_list;
   eclair_playlist_container_update(obj);   
}

//Update the playlist container
void eclair_playlist_container_update(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;
   Eina_List *l, *l2;
   Eina_List *first_visible = NULL;
   Eclair_Playlist_Container_Object *entry_object;
   Eclair_Media_File *entry_file;
   int i;
   Evas_Coord x, y, w, h;
   int invisible_height, offset, first_visible_nth, delta;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)) ||
      playlist_container->entry_height <= 0 || !playlist_container->entries)
      return;

   evas_object_geometry_get(obj, &x, &y, &w, &h);
   invisible_height = (eina_list_count(*playlist_container->entries) * playlist_container->entry_height) - h;
   if (invisible_height < 0)
      invisible_height = 0;
   offset = invisible_height * playlist_container->scroll_percent;
   first_visible_nth = offset / playlist_container->entry_height;
   delta = offset - (first_visible_nth * playlist_container->entry_height);

   for (l = *playlist_container->entries, i = 0; l; l = l->next, i++)
   {
      if (i == first_visible_nth)
         first_visible = l;
      if ((entry_file = l->data))
         entry_file->container_object = NULL;
   }

   for (l = playlist_container->entry_objects, l2 = first_visible, i = 0; l; l = l->next)
   {
      if (!(entry_object = l->data))
         continue;
      evas_object_move(entry_object->rect, x, y + i * playlist_container->entry_height - delta);
      evas_object_resize(entry_object->rect, w, playlist_container->entry_height);
      evas_object_move(entry_object->text, x, y + i * playlist_container->entry_height - delta);
      evas_object_resize(entry_object->text, w, playlist_container->entry_height);
      if (l2 && (entry_file = l2->data))
      {
         evas_object_data_set(entry_object->rect, "media_file", entry_file);
         evas_object_data_set(entry_object->text, "media_file", entry_file);
         entry_file->container_object = entry_object;
         eclair_media_file_update(playlist_container->eclair, entry_file);
         evas_object_show(entry_object->rect);
         evas_object_show(entry_object->text);
         l2 = l2->next;
         i++;
      }
      else
      {
         evas_object_data_set(entry_object->rect, "media_file", NULL);
         evas_object_hide(entry_object->text);
         evas_object_hide(entry_object->rect);
      }
   }
}

//Set the scroll percent of the playlist container
void eclair_playlist_container_scroll_percent_set(Evas_Object *obj, double percent)
{
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)))
      return;

   if (playlist_container->scroll_percent != percent)
   {
      playlist_container->scroll_percent = percent;
      if (playlist_container->scroll_percent < 0.0)
         playlist_container->scroll_percent = 0.0;
      else if (playlist_container->scroll_percent > 1.0)
         playlist_container->scroll_percent = 1.0;
      eclair_playlist_container_update(obj);
      evas_object_smart_callback_call(obj, "eclair_playlist_container_scroll_percent_changed", obj);
   }
}

//Return the scroll percent of the playlist container
double eclair_playlist_container_scroll_percent_get(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)))
      return 0.0;

   return playlist_container->scroll_percent;
}

//Scroll the playlist container
void eclair_playlist_container_scroll(Evas_Object *obj, double num_entries)
{
   double percent;
   Evas_Coord h;
   double hidden_items;
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)) ||
      playlist_container->entry_height <= 0 || !playlist_container->entries)
      return;

   evas_object_geometry_get(obj, NULL, NULL, NULL, &h);
   hidden_items = eina_list_count(*playlist_container->entries) - ((float)h / playlist_container->entry_height);
   if (hidden_items > 0.0)
   {
      percent = playlist_container->scroll_percent + (num_entries / hidden_items);
      eclair_playlist_container_scroll_percent_set(obj, percent);
   }
}

//Start scrolling the playlist
//Code from the default layout of esmart container
void eclair_playlist_container_scroll_start(Evas_Object *obj, double speed)
{
   int length;
   Evas_Coord h;
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)) ||
      playlist_container->entry_height <= 0 || !playlist_container->entries)
      return;

   evas_object_geometry_get(obj, NULL, NULL, NULL, &h);
   length = eina_list_count(*playlist_container->entries) * playlist_container->entry_height;
   
   if (length <= h)
      return;

   playlist_container->scroll_speed = speed;
   playlist_container->scroll_start_time = ecore_time_get();
   playlist_container->scroll_timer = ecore_timer_add(0.05, _eclair_playlist_container_scroll_timer, obj);
}

//Stop scrolling the playlist
//Code from the default layout of esmart container
void eclair_playlist_container_scroll_stop(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)))
      return;

   if (playlist_container->scroll_timer)
   {
      ecore_timer_del(playlist_container->scroll_timer);
      playlist_container->scroll_timer = NULL;
   }
}

//Update the scroll percent of the playlist container
//Code from the default layout of esmart container
static int _eclair_playlist_container_scroll_timer(void *data)
{
   Evas_Object *obj;
   Eclair_Playlist_Container *playlist_container;
   double dt, dx;

   if (!(obj = data) || !(playlist_container = evas_object_smart_data_get(obj)))
      return 0;

   dt = ecore_time_get() - playlist_container->scroll_start_time;
   dx = (1 - exp(-dt)) * playlist_container->scroll_speed;

   eclair_playlist_container_scroll(obj, dx); 
   
   return 1;
}

//Scroll the playlist container up to the element
void eclair_playlist_container_scroll_to_list(Evas_Object *obj, Eina_List *element)
{
   Eclair_Playlist_Container *playlist_container;
   Eina_List *l;
   Evas_Coord container_height;
   int i, start_offset, element_offset;

   if (!obj || !element || !(playlist_container = evas_object_smart_data_get(obj)) ||
      playlist_container->entry_height <= 0 || !playlist_container->entries)
      return;

   for (l = *playlist_container->entries, i = 0; l; l = l->next, i++)
   {
      if (l == element)
      {
         if (eclair_playlist_container_nth_element_is_visible(obj, i, 1))
            return;

         element_offset = i * playlist_container->entry_height;
         start_offset = eclair_playlist_container_offset_get(obj);
         if (element_offset < start_offset)
            playlist_container->scroll_to_percent = eclair_playlist_container_percent_get_from_offset(obj, element_offset);
         else
         {
            evas_object_geometry_get(obj, NULL, NULL, NULL, &container_height);
            playlist_container->scroll_to_percent = eclair_playlist_container_percent_get_from_offset(obj, element_offset - container_height + playlist_container->entry_height);
         }
         playlist_container->scroll_to_speed = (playlist_container->scroll_to_percent - playlist_container->scroll_percent) / 8.0;
         playlist_container->scroll_to_timer = ecore_timer_add(0.05, _eclair_playlist_container_scroll_to_timer, obj);
      }
   }
}

//Update the scroll percent of the playlist container to scroll up to playlist_container->scroll_to_percent
static int _eclair_playlist_container_scroll_to_timer(void *data)
{
   Evas_Object *obj;
   Eclair_Playlist_Container *playlist_container;
   double scroll_percent;

   if (!(obj = data) || !(playlist_container = evas_object_smart_data_get(obj)))
      return 0;

   scroll_percent = playlist_container->scroll_percent + playlist_container->scroll_to_speed;
   if ((playlist_container->scroll_to_speed > 0 && scroll_percent >= playlist_container->scroll_to_percent) ||
      (playlist_container->scroll_to_speed < 0 && scroll_percent <= playlist_container->scroll_to_percent))
   {
      eclair_playlist_container_scroll_percent_set(obj, playlist_container->scroll_to_percent);
      playlist_container->scroll_to_timer = NULL;
      return 0;
   }

   eclair_playlist_container_scroll_percent_set(obj, scroll_percent);

   return 1;
}

//Return the offset according to the scroll percent
int eclair_playlist_container_offset_get_from_percent(Evas_Object *obj, double scroll_percent)
{
   Eclair_Playlist_Container *playlist_container;
   Evas_Coord container_height;
   int hidden_height;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)) ||
      !playlist_container->entries || playlist_container->entry_height <= 0)
      return 0;

   evas_object_geometry_get(obj, NULL, NULL, NULL, &container_height);
   hidden_height = (playlist_container->entry_height * eina_list_count(*playlist_container->entries)) - container_height;

   if (hidden_height <= 0)
      return 0;
   else
      return (int)(scroll_percent * hidden_height);
}

//Return the scroll percent according to the offset
double eclair_playlist_container_percent_get_from_offset(Evas_Object *obj, int offset)
{
   Eclair_Playlist_Container *playlist_container;
   double scroll_percent;
   Evas_Coord container_height;
   int hidden_height;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)) ||
      !playlist_container->entries || playlist_container->entry_height <= 0)
      return 0;

   evas_object_geometry_get(obj, NULL, NULL, NULL, &container_height);
   hidden_height = (playlist_container->entry_height * eina_list_count(*playlist_container->entries)) - container_height;
   scroll_percent = (double)offset / hidden_height;
   if (scroll_percent < 0)
      scroll_percent = 0;
   else if (scroll_percent >= 1)
      scroll_percent = 1;
   return scroll_percent;
}

//Return the offset of the playlist container
int eclair_playlist_container_offset_get(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;
   
   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)))
      return 0;

   return eclair_playlist_container_offset_get_from_percent(obj, playlist_container->scroll_percent);
}

//Set the offset of the playlist container
void eclair_playlist_container_offset_set(Evas_Object *obj, int offset)
{
   Eclair_Playlist_Container *playlist_container;
   
   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)))
      return;

   eclair_playlist_container_scroll_percent_set(obj, eclair_playlist_container_percent_get_from_offset(obj, offset));
}

//Return 1 if the nth element is visible in the playlist container (if it is entirely visible if entirely == 1)
Evas_Bool eclair_playlist_container_nth_element_is_visible(Evas_Object *obj, int n, Evas_Bool entirely)
{
   int start_offset, element_offset;
   Evas_Coord container_height;
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)) || playlist_container->entry_height <= 0)
      return 0;
   
   start_offset = eclair_playlist_container_offset_get(obj);
   element_offset = n * playlist_container->entry_height;
   evas_object_geometry_get(obj, NULL, NULL, NULL, &container_height);
   if (entirely)
      return (element_offset >= start_offset && (element_offset + playlist_container->entry_height) <= (start_offset + container_height));
   else
      return (element_offset - playlist_container->entry_height > start_offset && element_offset < (start_offset + container_height));
}

//Select the files in the playlist container according to the media_file selected and the modifiers
void eclair_playlist_container_select_file(Evas_Object *obj, Eclair_Media_File *media_file, Evas_Modifier *modifiers)
{
   Eclair_Playlist_Container *playlist_container;
   Eina_List *l;
   Eclair_Media_File *current_file;
   Evas_Bool selected = 0;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)) ||
         !playlist_container->entries || !media_file)
      return;

   if (evas_key_modifier_is_set(modifiers, "Shift"))
   {
      if (!evas_key_modifier_is_set(modifiers, "Control"))
         eclair_playlist_container_select_none(obj);

      if (!playlist_container->last_selected)
      {
         media_file->selected = 1;
         playlist_container->last_selected = media_file;
         eclair_media_file_update(playlist_container->eclair, media_file);
      }
      else
      {
         for (l = *playlist_container->entries; l; l = l->next)
         {
            if (!(current_file = l->data))
               continue;
            if (current_file == playlist_container->last_selected || current_file == media_file)
            {
               current_file->selected = 1;
               selected = !selected;
            }
            else
               current_file->selected = selected || current_file->selected;
         }
         if (selected)
         {
            eclair_playlist_container_select_none(obj);
            media_file->selected = 1;
            playlist_container->last_selected = media_file;
            eclair_media_file_update(playlist_container->eclair, media_file);
         }
         else
            eclair_playlist_container_update(obj);
      }
   }
   else if (evas_key_modifier_is_set(modifiers, "Control"))
   {
      media_file->selected = !media_file->selected;
      playlist_container->last_selected = media_file;
      eclair_media_file_update(playlist_container->eclair, media_file);
   }
   else
   {
      eclair_playlist_container_select_none(obj);
      media_file->selected = 1;
      playlist_container->last_selected = media_file;
      eclair_media_file_update(playlist_container->eclair, media_file);
   }
}

//Select all the media files
void eclair_playlist_container_select_all(Evas_Object *obj)
{
   Eina_List *l;
   Eclair_Media_File *media_file;
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)) || !playlist_container->entries)
      return;

   for (l = *playlist_container->entries; l; l = l->next)
   {
      if ((media_file = l->data))
         media_file->selected = 1;
   }
   eclair_playlist_container_update(obj);
}

//Unselect all the media files
void eclair_playlist_container_select_none(Evas_Object *obj)
{
   Eina_List *l;
   Eclair_Media_File *media_file;
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)) || !playlist_container->entries)
      return;

   for (l = *playlist_container->entries; l; l = l->next)
   {
      if ((media_file = l->data))
         media_file->selected = 0;
   }
   eclair_playlist_container_update(obj);
}

//Invert the selection of media files in the playlist container
void eclair_playlist_container_invert_selection(Evas_Object *obj)
{
   Eina_List *l;
   Eclair_Media_File *media_file;
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = evas_object_smart_data_get(obj)) || !playlist_container->entries)
      return;

   for (l = *playlist_container->entries; l; l = l->next)
   {
      if ((media_file = l->data))
         media_file->selected = !media_file->selected;
   }
   eclair_playlist_container_update(obj);
}

//------------------------------
// Smart functions
//------------------------------

static void _eclair_playlist_container_smart_add(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;
   Evas *evas;

   evas = evas_object_evas_get(obj);
   playlist_container = calloc(1, sizeof(Eclair_Playlist_Container));
   playlist_container->eclair = evas_smart_data_get(playlist_container_smart);
   playlist_container->scroll_timer = NULL;
   playlist_container->scroll_to_timer = NULL;

   playlist_container->clip = evas_object_rectangle_add(evas);
   evas_object_repeat_events_set(playlist_container->clip, 1);
   evas_object_color_set(playlist_container->clip, 255, 255, 255, 255);
   evas_object_smart_member_add(playlist_container->clip, obj);

   playlist_container->grabber = evas_object_rectangle_add(evas);
   evas_object_repeat_events_set(playlist_container->grabber, 1);
   evas_object_color_set(playlist_container->grabber, 255, 255, 255, 0);
   evas_object_clip_set(playlist_container->grabber, playlist_container->clip);
   evas_object_smart_member_add(playlist_container->grabber, obj);

   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_WHEEL, _eclair_playlist_container_wheel_cb, NULL);
   evas_object_smart_callback_add(obj, "eclair_playlist_container_scroll_percent_changed", _eclair_playlist_container_scroll_percent_changed_cb, playlist_container->eclair);

   evas_object_smart_data_set(obj, playlist_container);
}

static void _eclair_playlist_container_smart_del(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;
   Eina_List *l;
   Eclair_Playlist_Container_Object *entry;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   for (l = playlist_container->entry_objects; l; l = l->next)
   {
      if ((entry = l->data))
      {
         evas_object_del(entry->rect);
         evas_object_del(entry->text);
         free(entry);
      }
   }

   if (playlist_container->scroll_timer)
      ecore_timer_del(playlist_container->scroll_timer);
   if (playlist_container->scroll_to_timer)
      ecore_timer_del(playlist_container->scroll_to_timer);

   eina_list_free(playlist_container->entry_objects);
   evas_object_del(playlist_container->clip);
   evas_object_del(playlist_container->grabber);
   free(playlist_container->entry_theme_path);
   free(playlist_container);
}

static void _eclair_playlist_container_smart_layer_set(Evas_Object *obj, int layer)
{
   Eclair_Playlist_Container *playlist_container;
   Eina_List *l;
   Eclair_Playlist_Container_Object *entry;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   for (l = playlist_container->entry_objects; l; l = l->next)
   {
      if ((entry = l->data))
      {
         evas_object_layer_set(entry->rect, layer);
         evas_object_layer_set(entry->text, layer);
      }
   }
   evas_object_layer_set(playlist_container->clip, layer);
   evas_object_layer_set(playlist_container->grabber, layer);
}

static void _eclair_playlist_container_smart_raise(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;
   Eina_List *l;
   Eclair_Playlist_Container_Object *entry;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   for (l = playlist_container->entry_objects; l; l = l->next)
   {
      if ((entry = l->data))
      {
         evas_object_raise(entry->rect);
         evas_object_raise(entry->text);
      }
   }
   evas_object_raise(playlist_container->clip);
   evas_object_raise(playlist_container->grabber);
}

static void _eclair_playlist_container_smart_lower(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;
   Eina_List *l;
   Eclair_Playlist_Container_Object *entry;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   for (l = playlist_container->entry_objects; l; l = l->next)
   {
      if ((entry = l->data))
      {
         evas_object_lower(entry->rect);
         evas_object_lower(entry->text);
      }
   }
   evas_object_lower(playlist_container->clip);
   evas_object_lower(playlist_container->grabber);
}

static void _eclair_playlist_container_smart_stack_above(Evas_Object *obj, Evas_Object *above)
{
   Eclair_Playlist_Container *playlist_container;
   Eina_List *l;
   Eclair_Playlist_Container_Object *entry;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   for (l = playlist_container->entry_objects; l; l = l->next)
   {
      if ((entry = l->data))
      {
         evas_object_stack_above(entry->rect, above);
         evas_object_stack_above(entry->text, above);
      }
   }
   evas_object_stack_above(playlist_container->clip, above);
   evas_object_stack_above(playlist_container->grabber, above);
}

static void _eclair_playlist_container_smart_stack_below(Evas_Object *obj, Evas_Object *below)
{
   Eclair_Playlist_Container *playlist_container;
   Eina_List *l;
   Eclair_Playlist_Container_Object *entry;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   for (l = playlist_container->entry_objects; l; l = l->next)
   {
      if ((entry = l->data))
      {
         evas_object_stack_below(entry->rect, below);
         evas_object_stack_below(entry->text, below);
      }
   }
   evas_object_stack_below(playlist_container->clip, below);
   evas_object_stack_below(playlist_container->grabber, below);
}

static void _eclair_playlist_container_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Eclair_Playlist_Container *playlist_container;
   Evas_Coord current_x, current_y;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   evas_object_geometry_get(obj, &current_x, &current_y, NULL, NULL);
   if (x == current_x && y == current_y)
      return;

   evas_object_move(playlist_container->clip, x, y);
   evas_object_move(playlist_container->grabber, x, y);
   eclair_playlist_container_update(obj);
}

static void _eclair_playlist_container_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Eclair_Playlist_Container *playlist_container;
   Eina_List *l;
   Eclair_Playlist_Container_Object *entry;
   int num_entries;
   Evas_Coord current_w, current_h;
   Evas *evas;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   evas = evas_object_evas_get(obj);
   evas_object_geometry_get(obj, NULL, NULL, &current_w, &current_h);
   if (w == current_w && h == current_h)
      return;

   if (playlist_container->entry_theme_path && playlist_container->entry_height > 0)
   {
      num_entries = ceil((float)h / playlist_container->entry_height) + 1;
      if (num_entries < 0)
         num_entries = 0;
   
      while(eina_list_count(playlist_container->entry_objects) < num_entries)
      {
         entry = malloc(sizeof(Eclair_Playlist_Container_Object));
         entry->rect = evas_object_rectangle_add(evas);
         evas_object_clip_set(entry->rect, playlist_container->clip);
         evas_object_event_callback_add(entry->rect, EVAS_CALLBACK_MOUSE_DOWN, _eclair_playlist_container_entry_down_cb, playlist_container->eclair);
         evas_object_repeat_events_set(entry->rect, 1);
         evas_object_smart_member_add(entry->rect, obj);
         evas_object_stack_above(entry->rect, obj);

         entry->text = edje_object_add(evas);
         edje_object_file_set(entry->text, playlist_container->entry_theme_path, "eclair_playlist_entry");
         evas_object_clip_set(entry->text, playlist_container->clip);
         evas_object_repeat_events_set(entry->text, 1);
         evas_object_event_callback_add(entry->text, EVAS_CALLBACK_MOUSE_DOWN, _eclair_playlist_container_entry_down_cb, playlist_container->eclair);
         evas_object_smart_member_add(entry->text, obj);
         evas_object_stack_above(entry->text, entry->rect);
         playlist_container->entry_objects = eina_list_append(playlist_container->entry_objects, entry);
      }
      while(eina_list_count(playlist_container->entry_objects) > num_entries)
      {
         l = eina_list_last(playlist_container->entry_objects);
         if ((entry = l->data))
         {
            evas_object_del(entry->rect);
            evas_object_del(entry->text);
            free(entry);
         }
         playlist_container->entry_objects = eina_list_remove_list(playlist_container->entry_objects, l);
      }
   }
   evas_object_resize(playlist_container->clip, w, h);
   evas_object_resize(playlist_container->grabber, w, h);
   eclair_playlist_container_update(obj);
}

static void _eclair_playlist_container_smart_show(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   evas_object_show(playlist_container->clip);
   evas_object_show(playlist_container->grabber);
}

static void _eclair_playlist_container_smart_hide(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   evas_object_hide(playlist_container->clip);
   evas_object_hide(playlist_container->grabber);
}

static void _eclair_playlist_container_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
   Eclair_Playlist_Container *playlist_container;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   evas_object_color_set(playlist_container->clip, r, g, b, a);
}


static void _eclair_playlist_container_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
   Eclair_Playlist_Container *playlist_container;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   evas_object_clip_set(playlist_container->clip, clip);
}

static void _eclair_playlist_container_smart_clip_unset(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;

   if (!(playlist_container = evas_object_smart_data_get(obj)))
      return;

   evas_object_clip_unset(playlist_container->clip);
}

//------------------------------
// Callbacks
//------------------------------

//Called when the user drags the scrollbar button of the playlist
void eclair_playlist_container_scrollbar_drag_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   Eclair *eclair;
   double y;

   if (!(eclair = data) || !eclair->playlist_container_owner)
      return;

   edje_object_part_drag_value_get(eclair->playlist_container_owner->edje_object, "playlist_scrollbar_drag", NULL, &y);
   eclair_playlist_container_scroll_percent_set(eclair->playlist_container, y);
}

//Called when the user wants to scroll the playlist
void eclair_playlist_container_scroll_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   Eclair *eclair;

   if (!(eclair = data) || !eclair->playlist_container)
      return;

   if (strcmp(emission, "eclair_playlist_scroll_down_start") == 0)
      eclair_playlist_container_scroll_start(eclair->playlist_container, 2.0);
   else if (strcmp(emission, "eclair_playlist_scroll_up_start") == 0)
      eclair_playlist_container_scroll_start(eclair->playlist_container, -2.0);
   else
      eclair_playlist_container_scroll_stop(eclair->playlist_container);
}

//Called when the user clicks on an entry in the playlist 
static void _eclair_playlist_container_entry_down_cb(void *data, Evas *evas, Evas_Object *entry, void *event_info)
{
   Eclair *eclair;
   Eclair_Media_File *media_file;
   Evas_Event_Mouse_Down *event;

   event = event_info;
   if (!(eclair = data) || !(media_file = evas_object_data_get(entry, "media_file")) || event->button != 1)
      return;

   if (event->flags == EVAS_BUTTON_NONE)
      eclair_playlist_container_select_file(eclair->playlist_container, media_file, event->modifiers);
   else if (event->flags == EVAS_BUTTON_DOUBLE_CLICK)
   {
      eclair_playlist_current_set(&eclair->playlist, media_file);
      eclair_play_current(eclair);
   }
}

//Called when the scroll percent of the playlist container is changed
static void _eclair_playlist_container_scroll_percent_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eclair *eclair;

   if ((eclair = data) && eclair->playlist_container_owner)
      edje_object_part_drag_value_set(eclair->playlist_container_owner->edje_object, "playlist_scrollbar_drag", 0, eclair_playlist_container_scroll_percent_get(obj));
}

//Called when user uses wheel mouse over playlist container
static void _eclair_playlist_container_wheel_cb(void *data, Evas *evas, Evas_Object *playlist_container, void *event_info)
{
   Evas_Event_Mouse_Wheel *event;

   event = event_info;
   eclair_playlist_container_scroll(playlist_container, event->z);
}
