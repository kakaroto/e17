#include "eclair_playlist_container.h"
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <math.h>
#include "eclair.h"
#include "eclair_callbacks.h"
#include "eclair_media_file.h"

static Evas_Smart *playlist_container_smart;
void eclair_playlist_container_update(Evas_Object *obj);
static int _eclair_playlist_container_scroll_timer(void *data);
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
   Eclair_Color *color;

   if (!obj || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   free(playlist_container->entry_theme_path);
   playlist_container->entry_theme_path = strdup(entry_theme_path);

   entry = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(entry, playlist_container->entry_theme_path, "eclair_playlist_entry");
   edje_object_size_min_get(entry, NULL, &entry_height);
   playlist_container->entry_height = (int)entry_height;

   color = &playlist_container->selected_entry_bg_color;
   color_string = edje_file_data_get(entry_theme_path, "selected_entry_bg_color");
   if (!color_string || sscanf(color_string, "%d %d %d %d", &color->r, &color->g, &color->b, &color->a) != 4)
   {   color->r = 255; color->g = 255; color->b = 255; color->a = 0; }

   color = &playlist_container->selected_entry_fg_color;
   color_string = edje_file_data_get(entry_theme_path, "selected_entry_fg_color");
   if (!color_string || sscanf(color_string, "%d %d %d %d", &color->r, &color->g, &color->b, &color->a) != 4)
   {   color->r = 255; color->g = 255; color->b = 255; color->a = 255; }

   color = &playlist_container->current_entry_bg_color;
   color_string = edje_file_data_get(entry_theme_path, "current_entry_bg_color");
   if (!color_string || sscanf(color_string, "%d %d %d %d", &color->r, &color->g, &color->b, &color->a) != 4)
   {   color->r = 255; color->g = 255; color->b = 255; color->a = 0; }

   color = &playlist_container->current_entry_fg_color;
   color_string = edje_file_data_get(entry_theme_path, "current_entry_fg_color");
   if (!color_string || sscanf(color_string, "%d %d %d %d", &color->r, &color->g, &color->b, &color->a) != 4)
   {   color->r = 255; color->g = 255; color->b = 255; color->a = 255; }


   evas_object_del(entry);
   eclair_playlist_container_update(obj);
   //TODO: resize the container to recreate the entry objects with the new theme
}

//Set the list of media files used by the playlist_container
void eclair_playlist_container_set_media_list(Evas_Object *obj, Evas_List **media_list)
{
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   playlist_container->entries = media_list;
   eclair_playlist_container_update(obj);   
}

//Update the playlist container
void eclair_playlist_container_update(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;
   Evas_List *l, *l2;
   Evas_List *first_visible = NULL;
   Eclair_Playlist_Container_Object *entry_object;
   Eclair_Media_File *entry_file;
   int i;
   Evas_Coord x, y, w, h;
   int invisible_height, offset, first_visible_nth, delta;

   if (!obj || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)) ||
      playlist_container->entry_height <= 0 || !playlist_container->entries)
      return;

   evas_object_geometry_get(obj, &x, &y, &w, &h);
   invisible_height = (evas_list_count(*playlist_container->entries) * playlist_container->entry_height) - h;
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
      if (!(entry_object = (Eclair_Playlist_Container_Object *)l->data))
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

//Scroll the playlist container
void eclair_playlist_container_scroll(Evas_Object *obj, double num_entries)
{
   double percent;
   Evas_Coord h;
   double hidden_items;
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)) ||
      playlist_container->entry_height <= 0 || !playlist_container->entries)
      return;

   evas_object_geometry_get(obj, NULL, NULL, NULL, &h);
   hidden_items = evas_list_count(*playlist_container->entries) - ((float)h / playlist_container->entry_height);
   if (hidden_items > 0.0)
   {
      percent = playlist_container->scroll_percent + (num_entries / hidden_items);
      eclair_playlist_container_scroll_percent_set(obj, percent);
   }
}

//Set the scroll percent of the playlist container
void eclair_playlist_container_scroll_percent_set(Evas_Object *obj, double percent)
{
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
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

   if (!obj || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return 0.0;

   return playlist_container->scroll_percent;
}

//Start scrolling the playlist
//Code from the default layout of esmart container
void eclair_playlist_container_scroll_start(Evas_Object *obj, double speed)
{
   int length;
   Evas_Coord h;
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)) ||
      !playlist_container->entry_theme_path || playlist_container->entry_height <= 0 || !playlist_container->entries)
      return;

   evas_object_geometry_get(obj, NULL, NULL, NULL, &h);
   length = evas_list_count(*playlist_container->entries) * playlist_container->entry_height;
   
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

   if (!obj || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
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

   if (!(obj = data) || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return 0;

   dt = ecore_time_get() - playlist_container->scroll_start_time;
   dx = (1 - exp(-dt)) * playlist_container->scroll_speed;

   eclair_playlist_container_scroll(obj, dx); 
   
   return 1;
}

//Select the files in the playlist container according to the media_file selected and the modifiers
void eclair_playlist_container_select_file(Evas_Object *obj, Eclair_Media_File *media_file, Evas_Modifier *modifiers)
{
   Eclair_Playlist_Container *playlist_container;
   Evas_List *l;
   Eclair_Media_File *current_file;
   Evas_Bool selected = 0;

   if (!obj || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)) ||
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
            if (!(current_file = (Eclair_Media_File *)l->data))
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
   Evas_List *l;
   Eclair_Media_File *media_file;
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)) || !playlist_container->entries)
      return;

   for (l = *playlist_container->entries; l; l = l->next)
   {
      if ((media_file = (Eclair_Media_File *)l->data))
         media_file->selected = 1;
   }
   eclair_playlist_container_update(obj);
}

//Unselect all the media files
void eclair_playlist_container_select_none(Evas_Object *obj)
{
   Evas_List *l;
   Eclair_Media_File *media_file;
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)) || !playlist_container->entries)
      return;

   for (l = *playlist_container->entries; l; l = l->next)
   {
      if ((media_file = (Eclair_Media_File *)l->data))
         media_file->selected = 0;
   }
   eclair_playlist_container_update(obj);
}

//Invert the selection of media files in the playlist container
void eclair_playlist_container_invert_selection(Evas_Object *obj)
{
   Evas_List *l;
   Eclair_Media_File *media_file;
   Eclair_Playlist_Container *playlist_container;

   if (!obj || !(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)) || !playlist_container->entries)
      return;

   for (l = *playlist_container->entries; l; l = l->next)
   {
      if ((media_file = (Eclair_Media_File *)l->data))
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
   playlist_container = (Eclair_Playlist_Container *)calloc(1, sizeof(Eclair_Playlist_Container));
   playlist_container->eclair = (Eclair *)evas_smart_data_get(playlist_container_smart);

   playlist_container->clip = evas_object_rectangle_add(evas);
   evas_object_repeat_events_set(playlist_container->clip, 1);
   evas_object_color_set(playlist_container->clip, 255, 255, 255, 255);
   evas_object_smart_member_add(playlist_container->clip, obj);

   playlist_container->grabber = evas_object_rectangle_add(evas);
   evas_object_repeat_events_set(playlist_container->grabber, 1);
   evas_object_color_set(playlist_container->grabber, 255, 255, 255, 0);
   evas_object_clip_set(playlist_container->grabber, playlist_container->clip);
   evas_object_smart_member_add(playlist_container->grabber, obj);

   evas_object_smart_data_set(obj, playlist_container);
}

static void _eclair_playlist_container_smart_del(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;
   Evas_List *l;
   Eclair_Playlist_Container_Object *entry;

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   for (l = playlist_container->entry_objects; l; l = l->next)
   {
      if ((entry = (Eclair_Playlist_Container_Object *)l->data))
      {
         evas_object_del(entry->rect);
         evas_object_del(entry->text);
         free(entry);
      }
   }
   evas_list_free(playlist_container->entry_objects);
   evas_object_del(playlist_container->clip);
   evas_object_del(playlist_container->grabber);
   free(playlist_container->entry_theme_path);
   free(playlist_container);
}

static void _eclair_playlist_container_smart_layer_set(Evas_Object *obj, int layer)
{
   Eclair_Playlist_Container *playlist_container;
   Evas_List *l;
   Eclair_Playlist_Container_Object *entry;

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   for (l = playlist_container->entry_objects; l; l = l->next)
   {
      if ((entry = (Eclair_Playlist_Container_Object *)l->data))
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
   Evas_List *l;
   Eclair_Playlist_Container_Object *entry;

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   for (l = playlist_container->entry_objects; l; l = l->next)
   {
      if ((entry = (Eclair_Playlist_Container_Object *)l->data))
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
   Evas_List *l;
   Eclair_Playlist_Container_Object *entry;

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   for (l = playlist_container->entry_objects; l; l = l->next)
   {
      if ((entry = (Eclair_Playlist_Container_Object *)l->data))
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
   Evas_List *l;
   Eclair_Playlist_Container_Object *entry;

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   for (l = playlist_container->entry_objects; l; l = l->next)
   {
      if ((entry = (Eclair_Playlist_Container_Object *)l->data))
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
   Evas_List *l;
   Eclair_Playlist_Container_Object *entry;

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   for (l = playlist_container->entry_objects; l; l = l->next)
   {
      if ((entry = (Eclair_Playlist_Container_Object *)l->data))
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

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
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
   Evas_List *l;
   Eclair_Playlist_Container_Object *entry;
   int num_entries;
   Evas_Coord current_w, current_h;
   Evas *evas;

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
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
   
      while(evas_list_count(playlist_container->entry_objects) < num_entries)
      {
         entry = (Eclair_Playlist_Container_Object *)malloc(sizeof(Eclair_Playlist_Container_Object));
         entry->rect = evas_object_rectangle_add(evas);
         evas_object_clip_set(entry->rect, playlist_container->clip);
         evas_object_event_callback_add(entry->rect, EVAS_CALLBACK_MOUSE_DOWN, eclair_gui_entry_down_cb, playlist_container->eclair);
         evas_object_repeat_events_set(entry->rect, 1);
         evas_object_smart_member_add(entry->rect, obj);

         entry->text = edje_object_add(evas);
         edje_object_file_set(entry->text, playlist_container->entry_theme_path, "eclair_playlist_entry");
         evas_object_clip_set(entry->text, playlist_container->clip);
         evas_object_repeat_events_set(entry->text, 1);
         evas_object_event_callback_add(entry->text, EVAS_CALLBACK_MOUSE_DOWN, eclair_gui_entry_down_cb, playlist_container->eclair);
         evas_object_smart_member_add(entry->text, obj);
         playlist_container->entry_objects = evas_list_append(playlist_container->entry_objects, entry);
      }
      while(evas_list_count(playlist_container->entry_objects) > num_entries)
      {
         l = evas_list_last(playlist_container->entry_objects);
         if ((entry = (Eclair_Playlist_Container_Object *)l->data))
         {
            evas_object_del(entry->rect);
            evas_object_del(entry->text);
            free(entry);
         }
         playlist_container->entry_objects = evas_list_remove_list(playlist_container->entry_objects, l);
      }
   }
   evas_object_resize(playlist_container->clip, w, h);
   evas_object_resize(playlist_container->grabber, w, h);
   eclair_playlist_container_update(obj);
}

static void _eclair_playlist_container_smart_show(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   evas_object_show(playlist_container->clip);
   evas_object_show(playlist_container->grabber);
}

static void _eclair_playlist_container_smart_hide(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   evas_object_hide(playlist_container->clip);
   evas_object_hide(playlist_container->grabber);
}

static void _eclair_playlist_container_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
   Eclair_Playlist_Container *playlist_container;

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   evas_object_color_set(playlist_container->clip, r, g, b, a);
}


static void _eclair_playlist_container_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
   Eclair_Playlist_Container *playlist_container;

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   evas_object_clip_set(playlist_container->clip, clip);
}

static void _eclair_playlist_container_smart_clip_unset(Evas_Object *obj)
{
   Eclair_Playlist_Container *playlist_container;

   if (!(playlist_container = (Eclair_Playlist_Container *)evas_object_smart_data_get(obj)))
      return;

   evas_object_clip_unset(playlist_container->clip);
}
