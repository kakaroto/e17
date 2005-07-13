#include "eclair_callbacks.h"
#include <string.h>
#include <Emotion.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include "eclair.h"
#include "eclair_playlist.h"
#include "eclair_playlist_container.h"
#include "eclair_dialogs.h"
#include "eclair_window.h"

//Called when eclair is closed
int eclair_exit_cb(void *data, int type, void *event)
{
   eclair_shutdown(data);

   return 1;
}

//Called when the user clicks on on of the ecore windows
int eclair_mouse_up_cb(void *data, int type, void *event)
{
   Eclair *eclair;
   Ecore_X_Event_Mouse_Button_Up *mouse_event;

   if (!(eclair = data))
      return 1;

   mouse_event = event;
   //TODO: Ecore event bug? mouse_event win and x_window doesn't match but they should.
   //I need to add 1 to x_window?!
   if (mouse_event->button == 3 && (eclair->gui_window && mouse_event->win == eclair->gui_window->x_window + 1))
      eclair_dialogs_popup_menu(&eclair->dialogs_manager);

   return 1;
}

//Called when a key is pressed
void eclair_key_press_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	Eclair *eclair;
   Evas_Event_Key_Down *ev;

   if (!(eclair = data))
      return;

   ev = event_info;
   if (strcmp(ev->key, "f") == 0 && eclair->video.video_window)
      ecore_evas_fullscreen_set(eclair->video.video_window, !ecore_evas_fullscreen_get(eclair->video.video_window));
   else if (strcmp(ev->key, "space") == 0 || strcmp(ev->key, "p") == 0)
   {
      if (eclair->state == ECLAIR_PLAYING)
         eclair_pause(eclair);
      else
         eclair_play(eclair);
   }
   else if (strcmp(ev->key, "escape") == 0 || strcmp(ev->key, "q") == 0)
   {
      eclair_shutdown(eclair);
      return;
   }
   else if (eclair->video.video_object && (strcmp(ev->key, "Left") == 0 ||
      strcmp(ev->key, "Right") == 0 || strcmp(ev->key, "Down") == 0 ||
      strcmp(ev->key, "Up") == 0 || strcmp(ev->key, "Prior") == 0 ||
      strcmp(ev->key, "Next") == 0 || strcmp(ev->key, "Home") == 0))
   {
      double media_position;

      media_position = eclair_position_get(eclair);   
      if (strcmp(ev->key, "Left") == 0)
         media_position -= 15.0;
      else if (strcmp(ev->key, "Right") == 0)
         media_position += 15.0;
      else if (strcmp(ev->key, "Down") == 0)
         media_position -= 60.0;
      else if (strcmp(ev->key, "Up") == 0)
         media_position += 60.0;
      else if (strcmp(ev->key, "Next") == 0)
         media_position -= 600.0;
      else if (strcmp(ev->key, "Prior") == 0)
         media_position += 600.0;
      else if (strcmp(ev->key, "Home") == 0)
         media_position = 0.0;
      eclair_position_set(eclair, media_position);
   }
   else if (eclair->gui_window && (strcmp(ev->key, "KP_Divide") == 0 || strcmp(ev->key, "slash") == 0 ||
      strcmp(ev->key, "KP_Multiply") == 0 || strcmp(ev->key, "asterisk") == 0))
   {
      double volume;

      edje_object_part_drag_value_get(eclair->gui_window->edje_object, "volume_bar_drag", &volume, 0);
      if (strcmp(ev->key, "KP_Divide") == 0 || strcmp(ev->key, "slash") == 0)
         volume -= (1.0 / 100);
      else if (strcmp(ev->key, "KP_Multiply") == 0 || strcmp (ev->key, "asterisk") == 0)
         volume += (1.0 / 100);

      eclair_audio_level_set(eclair, volume);
   }
   else if (strcmp(ev->key, "Delete") == 0)
      eclair_playlist_remove_selected_media_files(&eclair->playlist);
   else if (strcmp(ev->key, "a") == 0 && evas_key_modifier_is_set(ev->modifiers, "Control"))
      eclair_playlist_container_select_all(eclair->playlist_container);
}

//Called when the gui send a message
void eclair_message_cb(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg)
{
   Eclair *eclair;
   
   if (!(eclair = data))
      return;

   switch (id)
   {
      default:
         break;
   }
}

//Called when an object is dragged over the gui
int eclair_dnd_position_cb(void *data, int type, void *event)
{
   Evas_Coord window_x, window_y, x, y;
   Ecore_X_Rectangle rect;
   Ecore_X_Event_Xdnd_Position *ev;
   Eclair *eclair;

   if (!(eclair = data))
      return 1;

   ev = event;
   if (eclair->gui_window && ev->win == eclair->gui_window->x_window)
      eclair->drop_window = eclair->gui_window;
   else if (eclair->playlist_window && ev->win == eclair->playlist_window->x_window)
      eclair->drop_window = eclair->playlist_window;
   else if (eclair->cover_window && ev->win == eclair->cover_window->x_window)
      eclair->drop_window = eclair->cover_window;
   else
      return 1;
   ecore_evas_geometry_get(eclair->drop_window->ecore_window, &window_x, &window_y, NULL, NULL);
   x = ev->position.x - window_x;
   y = ev->position.y - window_y;

   if (eclair->playlist_container && eclair->playlist_container_owner == eclair->drop_window && evas_object_visible_get(eclair->playlist_container))
   {
      evas_object_geometry_get(eclair->playlist_container, &rect.x, &rect.y, &rect.width, &rect.height);
      if (x >= rect.x && x <= (rect.x + rect.width) && y >= rect.y && y <= (rect.y + rect.height))
      {
         rect.x += window_x;
         rect.y += window_y;
         ecore_x_dnd_send_status(1, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
         eclair->drop_object = ECLAIR_DROP_PLAYLIST;
         return 1;
      }
   }
   if (eclair->cover && eclair->cover_owner == eclair->drop_window  && evas_object_visible_get(eclair->cover))
   {
      evas_object_geometry_get(eclair->cover, &rect.x, &rect.y, &rect.width, &rect.height);
      if (x >= rect.x && x <= (rect.x + rect.width) && y >= rect.y && y <= (rect.y + rect.height))
      {
         rect.x += window_x;
         rect.y += window_y;
         ecore_x_dnd_send_status(1, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
         eclair->drop_object = ECLAIR_DROP_COVER;
         return 1;
      }
   }  

   ecore_evas_geometry_get(eclair->drop_window->ecore_window, &rect.x, &rect.y, &rect.width, &rect.height);
   rect.x += window_x;
   rect.y += window_y;
   ecore_x_dnd_send_status(0, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
   eclair->drop_object = ECLAIR_DROP_NONE;
   return 1;
}

//Called when an object is dropped on the gui
int eclair_dnd_drop_cb(void *data, int type, void *event)
{
   Eclair *eclair;

   if (!(eclair = data) || !eclair->drop_window)
      return 1;

   ecore_x_selection_xdnd_request(eclair->drop_window->x_window, "text/uri-list");
   return 1;
}

//Treat the files dropped
int eclair_dnd_selection_cb(void *data, int type, void *event)
{
   Ecore_X_Event_Selection_Notify *ev;
   Eclair *eclair;
   Ecore_X_Selection_Data_Files *files;
   int i;

   ev = event;
   if (!(eclair = data) || !eclair->drop_window || eclair->drop_object == ECLAIR_DROP_NONE
      || ev->selection != ECORE_X_SELECTION_XDND || !(files = ev->data) || files->num_files <= 0)
   {
      ecore_x_dnd_send_finished();
      return 1;
   }

   if (eclair->drop_object == ECLAIR_DROP_PLAYLIST)
   {
      for (i = 0; i < files->num_files; i++)
         eclair_playlist_add_uri(&eclair->playlist, files->files[i], 0, 1);
      eclair_playlist_container_update(eclair->playlist_container);
   }
   else if (eclair->drop_object == ECLAIR_DROP_COVER)
      eclair_cover_current_set(&eclair->cover_manager, files->files[0]);

   ecore_x_dnd_send_finished();

   return 1;
}

//Called when the user clicks on open button
void eclair_open_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   eclair_dialogs_open_fc_add_files(data);
}

//Called when the user clicks on play button
void eclair_play_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   eclair_play(data);
}

//Called when the user clicks on prev button
void eclair_prev_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   eclair_play_prev(data);
}

//Called when the user clicks on next button
void eclair_next_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   eclair_play_next(data);
}

//Called when the user clicks on pause button
void eclair_pause_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   eclair_pause(data);
}

//Called when the user clicks on stop button
void eclair_stop_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   eclair_stop(data);
}

//Called when user enables or disables repeating
void eclair_repeat_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   Eclair *eclair;

   if (!(eclair = data))
      return;

   if (strcmp(emission, "eclair_repeat_enable") == 0)
      eclair_playlist_set_repeat(&eclair->playlist, 1);
   else
      eclair_playlist_set_repeat(&eclair->playlist, 0);
}

//Called when user enables or disables shuffling
void eclair_shuffle_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   Eclair *eclair;

   if (!(eclair = data))
      return;

   if (strcmp(emission, "eclair_shuffle_enable") == 0)
      eclair_playlist_set_shuffle(&eclair->playlist, 1);
   else
      eclair_playlist_set_shuffle(&eclair->playlist, 0);
}

//Called when the user clicks of the "load playlist" button  
void eclair_playlist_load_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   eclair_dialogs_open_fc_load_playlist(data);
}

//Called when the user clicks of the "save playlist" button
void eclair_playlist_save_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   eclair_dialogs_open_fc_save_playlist(data);
}

//Called when the user clicks of the "remove all" button
void eclair_remove_all_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   eclair_playlist_empty(data);
}

//Called when the user clicks of the "remove selected" button
void eclair_remove_selected_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   eclair_playlist_remove_selected_media_files(data);
}

//Called when the user clicks of the "remove unselected" button
void eclair_remove_unselected_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   eclair_playlist_remove_unselected_media_files(data);
}

//Called when the user clicks of the "select all" button
void eclair_select_all_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_container_select_all(eclair->playlist_container);
}

//Called when the user clicks of the "select none" button
void eclair_select_none_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_container_select_none(eclair->playlist_container);
}

//Called when the user clicks of the "invert selection" button
void eclair_select_inverse_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_container_invert_selection(eclair->playlist_container);
}

//Called when the user drags the progress bar button
void eclair_progress_bar_drag_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   Eclair *eclair;
   double progress_rate;

   if (!(eclair = data) || !eclair->gui_window)
      return;

   if (strcmp(emission, "drag,start") == 0)
      eclair->use_progress_bar_drag_for_time = 1;      
   else if (strcmp(emission, "drag,stop") == 0)
   {
      eclair->use_progress_bar_drag_for_time = 0;
      edje_object_part_drag_value_get(eclair->gui_window->edje_object, "progress_bar_drag", &progress_rate, NULL);
      eclair_progress_rate_set(eclair, progress_rate);
   }
}

//Called when the user drags the volume bar button
void eclair_volume_bar_drag_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source)
{
   Eclair *eclair;
   double volume;

   if (!(eclair = data) || !eclair->gui_window)
      return;

   edje_object_part_drag_value_get(eclair->gui_window->edje_object, "volume_bar_drag", &volume, NULL);
   eclair_audio_level_set(eclair, volume);
}
