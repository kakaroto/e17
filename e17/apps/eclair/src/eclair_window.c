#include "eclair_window.h"
#include <Ecore_Evas.h>
#include <Esmart/Esmart_Draggies.h>
#include "eclair.h"
#include "eclair_callbacks.h"
#include "eclair_config.h"

//Create a new window from the group "eclair_"window_name"_body" of the file "edje_file" with the engine "engine" and the title "title"
//"window_name" is also used to find some props of the window in the edje file ("data" section)
//Return NULL if failed. A created window has to be deleted by eclair_window_del()
Eclair_Window *eclair_window_create(const char *edje_file, const char *window_name, const char *title, Eclair_Engine engine, Eclair *eclair, Evas_Bool main_window)
{
   Eclair_Window *new_window;
   char *prop_name, *prop_value, *config_name, *edje_group;
   int window_w, window_h, opened;

   if (!edje_file || !window_name || !eclair)
      return NULL;

   new_window = malloc(sizeof(Eclair_Window));
   new_window->eclair = eclair;
   new_window->should_resize = 0;
   new_window->borderless = 1;
   new_window->shaded = 1;
   new_window->resizable = 1;
   new_window->main_window = main_window;
   new_window->window_name = strdup(window_name);

   if (engine == ECLAIR_GL)
   {
      new_window->ecore_window = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 0, 0);
      new_window->x_window = ecore_evas_gl_x11_window_get(new_window->ecore_window);
   }
   else
   {
      new_window->ecore_window = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
      new_window->x_window = ecore_evas_software_x11_window_get(new_window->ecore_window);
   }
   if (title)
      ecore_evas_title_set(new_window->ecore_window, title);
   ecore_evas_name_class_set(new_window->ecore_window, "eclair", "eclair");
   ecore_evas_borderless_set(new_window->ecore_window, 1);

	ecore_x_dnd_aware_set(new_window->x_window, 1);
	ecore_x_dnd_type_set(new_window->x_window, "*", 1);

   new_window->evas = ecore_evas_get(new_window->ecore_window);
   new_window->edje_object = edje_object_add(new_window->evas);
   edje_group = malloc(strlen("eclair_") + strlen(window_name) + strlen("_body") + 1);
   sprintf(edje_group, "eclair_%s_body", window_name);
   if (!edje_object_file_set(new_window->edje_object, edje_file, edje_group))
   {
      evas_object_del(new_window->edje_object);
      ecore_evas_free(new_window->ecore_window);
      free(new_window);
      free(edje_group);
      return NULL;
   }
   free(edje_group);
   evas_object_move(new_window->edje_object, 0, 0);
   evas_object_show(new_window->edje_object);
   //Read window props
   if (new_window->window_name)
   {
      prop_name = malloc(strlen(new_window->window_name) + strlen("_borderless") + 1);
      sprintf(prop_name, "%s_borderless", new_window->window_name);
      if ((prop_value = edje_file_data_get(edje_file, prop_name)) && strcmp(prop_value, "0") == 0)
      {
         new_window->borderless = 0;
         free(prop_value);
      }
      free(prop_name);

      prop_name = malloc(strlen(new_window->window_name) + strlen("_shaded") + 1);
      sprintf(prop_name, "%s_shaded", new_window->window_name);
      if ((prop_value = edje_file_data_get(edje_file, prop_name)) && strcmp(prop_value, "0") == 0)
      {
         new_window->shaded = 0;
         free(prop_value);
      }
      free(prop_name);

      prop_name = malloc(strlen(new_window->window_name) + strlen("_resizable") + 1);
      sprintf(prop_name, "%s_resizable", new_window->window_name);
      if ((prop_value = edje_file_data_get(edje_file, prop_name)) && strcmp(prop_value, "0") == 0)
      {
         new_window->resizable = 0;
         free(prop_value);
      }
      free(prop_name);
   }
   ecore_evas_borderless_set(new_window->ecore_window, new_window->borderless);
   ecore_evas_shaped_set(new_window->ecore_window, new_window->shaded);

   new_window->draggies = esmart_draggies_new(new_window->ecore_window);
   esmart_draggies_button_set(new_window->draggies, 1);
   evas_object_move(new_window->draggies, 0, 0);
   evas_object_layer_set(new_window->draggies, -1);
   evas_object_show(new_window->draggies);

   edje_object_size_min_get(new_window->edje_object, &new_window->min_width, &new_window->min_height);
   edje_object_size_max_get(new_window->edje_object, &new_window->max_width, &new_window->max_height);
   if (new_window->max_width < new_window->min_width)
      new_window->max_width = new_window->min_width;
   if (new_window->max_height < new_window->min_height)
      new_window->max_height = new_window->min_height;

   ecore_evas_size_min_set(new_window->ecore_window, new_window->min_width, new_window->min_height);
   if (new_window->resizable)
      ecore_evas_size_max_set(new_window->ecore_window, new_window->max_width, new_window->max_height);
   else
      ecore_evas_size_max_set(new_window->ecore_window, new_window->min_width, new_window->min_height);

   config_name = malloc(strlen(new_window->window_name) + strlen("_window") + 1);
   sprintf(config_name, "%s_window", new_window->window_name);
   if (!eclair_config_get_prop_int(&new_window->eclair->config, config_name, "x", &new_window->x) ||
      !eclair_config_get_prop_int(&new_window->eclair->config, config_name, "y", &new_window->y))
   {
      new_window->x = -1;
      new_window->y = -1;
   }

   if (eclair_config_get_prop_int(&new_window->eclair->config, config_name, "w", &window_w) &&
      eclair_config_get_prop_int(&new_window->eclair->config, config_name, "h", &window_h))
      eclair_window_resize(new_window, window_w, window_h, 1);
   else
      eclair_window_resize(new_window, new_window->min_width, new_window->min_height, 1);

   if (main_window || !eclair_config_get_prop_int(&new_window->eclair->config, config_name, "opened", &opened) || opened)
      eclair_window_open(new_window);
   free(config_name);

   return new_window;
}

//Delete the window
void eclair_window_del(Eclair_Window *window)
{
   char *config_name;
   int window_x, window_y, window_w, window_h;

   if (!window)
      return;

   config_name = malloc(strlen(window->window_name) + strlen("_window") + 1);
   sprintf(config_name, "%s_window", window->window_name);
   ecore_evas_geometry_get(window->ecore_window, &window_x, &window_y, &window_w, &window_h);
   eclair_config_set_prop_int(&window->eclair->config, config_name, "x", window_x);
   eclair_config_set_prop_int(&window->eclair->config, config_name, "y", window_y);
   eclair_config_set_prop_int(&window->eclair->config, config_name, "w", window_w);
   eclair_config_set_prop_int(&window->eclair->config, config_name, "h", window_h);
   eclair_config_set_prop_int(&window->eclair->config, config_name, "opened", ecore_evas_visibility_get(window->ecore_window));
   free(config_name);

   evas_object_del(window->draggies);
   evas_object_del(window->edje_object);
   ecore_evas_free(window->ecore_window);
}

//Add the default callbacks to the window
void eclair_window_add_default_callbacks(Eclair_Window *window, Eclair *eclair)
{
   char *signal_name;

   if (!eclair || !window || !window->edje_object)
      return;

   evas_object_focus_set(window->edje_object, 1);
   evas_object_event_callback_add(window->edje_object, EVAS_CALLBACK_KEY_DOWN, eclair_key_press_cb, eclair);
   ecore_evas_callback_delete_request_set(window->ecore_window, eclair_window_close_request_cb);
   edje_object_signal_callback_add(window->edje_object, "eclair_open", "*", eclair_open_cb, &eclair->dialogs_manager);
   edje_object_signal_callback_add(window->edje_object, "eclair_add_file", "*", eclair_open_cb, &eclair->dialogs_manager);
   edje_object_signal_callback_add(window->edje_object, "eclair_play", "*", eclair_play_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "eclair_pause", "*", eclair_pause_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "eclair_stop", "*", eclair_stop_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "eclair_prev", "*", eclair_prev_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "eclair_next", "*", eclair_next_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "eclair_playlist_load", "*", eclair_playlist_load_cb, &eclair->dialogs_manager);
   edje_object_signal_callback_add(window->edje_object, "eclair_playlist_save", "*", eclair_playlist_save_cb, &eclair->dialogs_manager);
   edje_object_signal_callback_add(window->edje_object, "eclair_remove_all", "*", eclair_remove_all_cb, &eclair->playlist);
   edje_object_signal_callback_add(window->edje_object, "eclair_remove_selected", "*", eclair_remove_selected_cb, &eclair->playlist);
   edje_object_signal_callback_add(window->edje_object, "eclair_remove_unselected", "*", eclair_remove_unselected_cb, &eclair->playlist);
   edje_object_signal_callback_add(window->edje_object, "eclair_select_inverse", "*", eclair_select_inverse_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "eclair_select_all", "*", eclair_select_all_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "eclair_select_none", "*", eclair_select_none_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "drag*", "progress_bar_drag", eclair_progress_bar_drag_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "drag", "volume_bar_drag", eclair_volume_bar_drag_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "drag", "playlist_scrollbar_drag", eclair_playlist_container_scrollbar_drag_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "eclair_playlist_scroll_down_start", "*", eclair_playlist_container_scroll_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "eclair_playlist_scroll_down_stop", "*", eclair_playlist_container_scroll_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "eclair_playlist_scroll_up_start", "*", eclair_playlist_container_scroll_cb, eclair);
   edje_object_signal_callback_add(window->edje_object, "eclair_playlist_scroll_up_stop", "*", eclair_playlist_container_scroll_cb, eclair);
   if (window->window_name)
   {
      signal_name = malloc(strlen("eclair_") + strlen(window->window_name) + strlen("_close") + 1);
      sprintf(signal_name, "eclair_%s_close", window->window_name);
      edje_object_signal_callback_add(window->edje_object, signal_name, "*", eclair_window_close_cb, window);
      free(signal_name); 
      signal_name = malloc(strlen("eclair_") + strlen(window->window_name) + strlen("_minimize") + 1);
      sprintf(signal_name, "eclair_%s_minimize", window->window_name);
      edje_object_signal_callback_add(window->edje_object, signal_name, "*", eclair_window_minimize_cb, window);
      free(signal_name);      
      if (window->resizable)
      {
         signal_name = malloc(strlen("eclair_") + strlen(window->window_name) + strlen("_resize_start") + 1);
         sprintf(signal_name, "eclair_%s_resize_start", window->window_name);
         edje_object_signal_callback_add(window->edje_object, signal_name, "", eclair_window_resize_start_cb, window);
         free(signal_name);
         signal_name = malloc(strlen("eclair_") + strlen(window->window_name) + strlen("_resize_stop") + 1);
         sprintf(signal_name, "eclair_%s_resize_stop", window->window_name);
         edje_object_signal_callback_add(window->edje_object, signal_name, "", eclair_window_resize_stop_cb, window);
         free(signal_name);
         evas_object_event_callback_add(window->edje_object, EVAS_CALLBACK_MOUSE_MOVE, eclair_window_mouse_move_cb, window);
      }
   }
   ecore_evas_data_set(window->ecore_window, "eclair_window", window);
   ecore_evas_callback_resize_set(window->ecore_window, eclair_window_resize_cb);
   edje_object_message_handler_set(window->edje_object, eclair_message_cb, eclair);
}

//Resize the window and its objects
void eclair_window_resize(Eclair_Window *window, Evas_Coord width, Evas_Coord height, Evas_Bool resize_window)
{
   if (!window)
      return;

   if (width < window->min_width)
      width = window->min_width;
   else if (width > window->max_width)
      width = window->max_width;
   if (height < window->min_height)
      height = window->min_height;
   else if (height > window->max_height)
      height = window->max_height;
   
   evas_object_resize(window->edje_object, width, height);
   evas_object_resize(window->draggies, width, height);
   if (resize_window)
      ecore_evas_resize(window->ecore_window, width, height);
}

//Show the window
void eclair_window_open(Eclair_Window *window)
{
   char *signal_name;

   if (!window)
      return;
   
   ecore_evas_show(window->ecore_window);
   if (window->x >= 0 && window->y >= 0)
      ecore_evas_move(window->ecore_window, window->x, window->y);
   signal_name = malloc(strlen("signal_") + strlen(window->window_name) + strlen("_opened") + 1);
   sprintf(signal_name, "signal_%s_opened", window->window_name);
   eclair_send_signal_to_all_windows(window->eclair, signal_name);
   free(signal_name);
}

//Quit eclair if the window is a main window, otherwise, just hide the window
void eclair_window_close(Eclair_Window *window)
{
   char *signal_name;

   if (!window)
      return;

   if (window->main_window)
      eclair_shutdown(window->eclair);
   else
   {
      ecore_evas_geometry_get(window->ecore_window, &window->x, &window->y, NULL, NULL);
      ecore_evas_hide(window->ecore_window);
      signal_name = malloc(strlen("signal_") + strlen(window->window_name) + strlen("_closed") + 1);
      sprintf(signal_name, "signal_%s_closed", window->window_name);
      eclair_send_signal_to_all_windows(window->eclair, signal_name);
      free(signal_name);
   }
}
