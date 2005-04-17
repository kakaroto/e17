#include "eclair.h"
#include "../config.h"
#include <Emotion.h>
#include <Esmart/Esmart_Draggies.h>
#include <Esmart/Esmart_Container.h>
#include "eclair_args.h"
#include "eclair_utils.h"
#include "eclair_callbacks.h"
#include "eclair_subtitles.h"
#include <tag_c.h>

static void _eclair_gui_create_window(Eclair *eclair);
static void _eclair_video_create_window(Eclair *eclair);
static void *_eclair_meta_tag_thread(void *param);

//Initialize eclair
Evas_Bool eclair_init(Eclair *eclair, int *argc, char *argv[])
{
   Evas_List *filenames, *l;
   filenames = NULL;

   if (!eclair)
      return 0;

   ecore_evas_init();
   edje_init();
   gtk_init(argc, &argv);

   eclair->video_window = NULL;
   eclair->video_object = NULL;
   eclair->black_background = NULL;
   eclair->subtitles_object = NULL;
   eclair->gui_window = NULL;
   eclair->gui_object = NULL;
   eclair->gui_draggies = NULL;
   eclair->playlist_container = NULL;
   eclair->playlist_entry_height = -1;
   eclair->file_chooser_widget = NULL;
   eclair->state = ECLAIR_STOP;
   eclair->seek_to_pos = -1.0;
   eclair->dont_update_progressbar = 0;
   eclair->file_chooser_th_created = 0;
   eclair->video_engine = ECLAIR_SOFTWARE;
   eclair->gui_engine = ECLAIR_SOFTWARE;
   eclair->meta_tag_files_to_scan = NULL;

   if (!eclair_args_parse(eclair, *argc, argv, &filenames))
      return 0;

   _eclair_gui_create_window(eclair);
   _eclair_video_create_window(eclair);
   eclair_playlist_init(eclair, &eclair->playlist);
   eclair_current_file_set(eclair, NULL);
   eclair_subtitles_init(&eclair->subtitles);
   
   pthread_cond_init(&eclair->meta_tag_cond, NULL);
   pthread_mutex_init(&eclair->meta_tag_mutex, NULL);
   pthread_create(&eclair->meta_tag_thread, NULL, _eclair_meta_tag_thread, eclair);

   for (l = filenames; l; l = l->next)
      eclair_playlist_add_media_file(&eclair->playlist, (char *)l->data);
   evas_list_free(filenames);

   //TODO
   edje_object_part_drag_value_set(eclair->gui_object, "volume_bar_drag", emotion_object_audio_volume_get(eclair->video_object), 0);

   return 1;
}

//Shutdown eclair and the EFL
void eclair_shutdown(Eclair *eclair)
{
   if (eclair)
   {
      eclair_playlist_empty(&eclair->playlist);
      eclair_subtitles_free(&eclair->subtitles);
   }

   ecore_main_loop_quit();

   edje_shutdown();
   ecore_evas_shutdown();
}

//Update text objects, progress bar...
//Called when an new frame is decoded
void eclair_update(Eclair *eclair)
{
   double progress_rate;
   char time_elapsed[10] = "";
   double position, length, time_to_display, x;

   if (!eclair)
      return;
   if (!eclair->video_object || !eclair->gui_object)
      return;

   position = emotion_object_position_get(eclair->video_object);
   length = emotion_object_play_length_get(eclair->video_object);

   //Update time text
   edje_object_part_drag_value_get(eclair->gui_object, "progress_bar_drag", &x, NULL);
   time_to_display = x * length;
   eclair_utils_second_to_string(time_to_display, length, time_elapsed);
   edje_object_part_text_set(eclair->gui_object, "time_elapsed", time_elapsed);

   //Update progress bar
   progress_rate = position / length;
   if (eclair->dont_update_progressbar)
   {
      eclair->dont_update_progressbar = 0.0;
      if (eclair->seek_to_pos >= 0.0)
         edje_object_part_drag_value_set(eclair->gui_object, "progress_bar_drag", eclair->seek_to_pos / length, 0.0);
   }
   else
   {
      if (eclair->seek_to_pos >= 0.0)
      {
         edje_object_part_drag_value_set(eclair->gui_object, "progress_bar_drag", eclair->seek_to_pos / length, 0.0);
         eclair->seek_to_pos = -1.0;
      }
      else
         edje_object_part_drag_value_set(eclair->gui_object, "progress_bar_drag", progress_rate, 0.0);
   }

   //Display subtitles
   eclair_subtitles_display_current_subtitle(&eclair->subtitles, position, eclair->subtitles_object);
}

//Set the file as current
void eclair_current_file_set(Eclair *eclair, const Eclair_Playlist_Media_File *file)
{   
   char *window_title;
   char *artist_title_string;
   const char *filename;

   if (!eclair)
      return;
   
   if (eclair->gui_object)
   {
      if (file)
      {
         if ((artist_title_string = eclair_utils_mediafile_to_artist_title_string(file)))
         {
            edje_object_part_text_set(eclair->gui_object, "current_media_name", artist_title_string);
            free(artist_title_string);
         }
         else if ((filename = eclair_utils_path_to_filename(file->path)))
            edje_object_part_text_set(eclair->gui_object, "current_media_name", filename);
         else
            edje_object_part_text_set(eclair->gui_object, "current_media_name", "No media opened");
      }
      else
         edje_object_part_text_set(eclair->gui_object, "current_media_name", "No media opened");
   }

   if (eclair->video_window)
   {
      if (file)
      {
         if (file->path)
         {
            window_title = (char *)malloc(strlen(file->path) + strlen("eclair: ") + 1);
            strcpy(window_title, "eclair: ");
            strcat(window_title, file->path);
            ecore_evas_title_set(eclair->video_window, window_title);
            free(window_title);
         }
         else
            ecore_evas_title_set(eclair->video_window, "eclair");
      }
      else
         ecore_evas_title_set(eclair->video_window, "eclair");
   }
}

//Set media progress rate
void eclair_progress_rate_set(Eclair *eclair, double progress_rate)
{
   if (!eclair)
      return;
   if (!eclair->video_object)
      return;

   emotion_object_position_set(eclair->video_object, emotion_object_play_length_get(eclair->video_object) * progress_rate);
}

//Set the scroll percent of the playlist container
void eclair_playlist_container_scroll_percent_set(Eclair *eclair, double percent)
{
   if (!eclair)
      return;
   if (!eclair->playlist_container)
      return;

   esmart_container_scroll_percent_set(eclair->playlist_container, percent);
   if (eclair->gui_object)
      edje_object_part_drag_value_set(eclair->gui_object, "playlist_scrollbar_button", 0, percent);
}

//Scroll the playlist
void eclair_playlist_container_scroll(Eclair *eclair, int num_entries)
{
   double percent;
   Evas_List *entries_list;
   Evas_Coord container_height;
   float hidden_items;
   
   if (!eclair || !eclair->playlist_container || !eclair->gui_object || eclair->playlist_entry_height <= 0)
      return;

   entries_list = esmart_container_elements_get(eclair->playlist_container);
   if (!entries_list)
      return;

   evas_object_geometry_get(eclair->playlist_container, NULL, NULL, NULL, &container_height);
   hidden_items = entries_list->count - ((float)container_height / eclair->playlist_entry_height);
   percent = esmart_container_scroll_percent_get(eclair->playlist_container);
   if (hidden_items > 0)
      percent += num_entries / hidden_items;
   if (percent > 1.0)
      percent = 1.0;
   if (percent < 0.0)
      percent = 0.0;
   eclair_playlist_container_scroll_percent_set(eclair, percent);
}

//Thread for file selection
//Open a file selection dialog and add the files selected 
void *eclair_file_chooser_thread(void *param)
{
   Eclair *eclair = (Eclair *)param;
   GSList *filenames, *l;

   if (!eclair)
      return NULL;

   pthread_mutex_lock(&eclair->file_chooser_mutex);

   eclair->file_chooser_widget = gtk_file_chooser_dialog_new("Open files...", NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(eclair->file_chooser_widget), GTK_RESPONSE_ACCEPT);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(eclair->file_chooser_widget), 1);
	gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(eclair->file_chooser_widget), 0);

   gtk_widget_show(eclair->file_chooser_widget);

   if (gtk_dialog_run(GTK_DIALOG(eclair->file_chooser_widget)) == GTK_RESPONSE_ACCEPT)
   {
      filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(eclair->file_chooser_widget));
      for (l = filenames; l; l = l->next)
      {
         if (l->data == NULL)
            continue;

         eclair_playlist_add_media_file(&eclair->playlist, (char *)l->data);
		}

      g_slist_foreach(filenames, (GFunc)g_free, NULL);
      g_slist_free(filenames);
   }

   gtk_widget_destroy(eclair->file_chooser_widget);

   while (gtk_events_pending())
	  gtk_main_iteration();

   eclair->file_chooser_widget = NULL;
   eclair->file_chooser_th_created = 0;
   
   return NULL;
}

//Play a new file
void eclair_play_file(Eclair *eclair, const char *path)
{
   int video_width, video_height;

   if (!eclair || !path)
      return;
   if (!eclair->video_window || !eclair->video_object)
      return;

   ecore_evas_show(eclair->video_window);

   emotion_object_file_set(eclair->video_object, path);
   emotion_object_play_set(eclair->video_object, 0);
   eclair_progress_rate_set(eclair, 0.0);
   eclair->state = ECLAIR_PAUSE;
   eclair_play(eclair);

   eclair_subtitles_load_from_media_file(&eclair->subtitles, path);

   emotion_object_size_get(eclair->video_object, &video_width, &video_height);
   ecore_evas_resize(eclair->video_window, video_width, video_height);
   eclair_video_window_resize_cb(eclair->video_window);
}

//Play the active file from the playlist
void eclair_play_current(Eclair *eclair)
{
   Eclair_Playlist_Media_File *current_media_file;

   if (!eclair)
      return;

   if (!(current_media_file = eclair_playlist_current_media_file(&eclair->playlist)))
   {
      eclair_stop(eclair);
      return;
   }

   eclair_play_file(eclair, current_media_file->path);
}

//Play the file which is before the active file 
void eclair_play_prev(Eclair *eclair)
{
   if (!eclair)
      return;
   
   eclair_playlist_prev_as_current(&eclair->playlist);
   eclair_play_current(eclair);
}

//Play the file which is after the active file
void eclair_play_next(Eclair *eclair)
{
   if (!eclair)
      return;
   
   eclair_playlist_next_as_current(&eclair->playlist);
   eclair_play_current(eclair);
}

//Pause the playback
void eclair_pause(Eclair *eclair)
{
   if (!eclair)
      return;
   if (eclair->state != ECLAIR_PLAYING)
      return;

   emotion_object_play_set(eclair->video_object, 0);
   edje_object_signal_emit(eclair->gui_object, "signal_pause", "eclair_bin");
   eclair->state = ECLAIR_PAUSE;
}

//Play the current file if state is STOP or resume if state is PAUSE
void eclair_play(Eclair *eclair)
{
   if (!eclair)
      return;
   if (!eclair->video_window)
      return;

   if (eclair->state == ECLAIR_PAUSE)
   {
      if (!eclair->video_object)
         return;

      emotion_object_play_set(eclair->video_object, 1);
      edje_object_signal_emit(eclair->gui_object, "signal_play", "eclair_bin");
      eclair->state = ECLAIR_PLAYING;
   }
   else if (eclair->state == ECLAIR_STOP)
      eclair_play_current(eclair);
}

//Stop the playback and hide the video window
void eclair_stop(Eclair *eclair)
{
   if (!eclair)
      return;

   ecore_evas_hide(eclair->video_window);

   if (eclair->video_object)
   {
      emotion_object_play_set(eclair->video_object, 0);
      eclair_progress_rate_set(eclair, 0.0);
   }
   
   if (eclair->gui_object)
      edje_object_part_text_set(eclair->gui_object, "time_elapsed", "0:00");

   edje_object_signal_emit(eclair->gui_object, "signal_stop", "eclair_bin");
   eclair->state = ECLAIR_STOP;
}

//Read the meta tags of the files stocked in meta_tag_files_to_scan
void *_eclair_meta_tag_thread(void *param)
{
   Eclair *eclair = (Eclair *)param;
   Evas_List *l, *next;
   Eclair_Playlist_Media_File *current_file;

   if (!eclair)
      return NULL;

   pthread_mutex_lock(&eclair->meta_tag_mutex);

   //TODO: delete thread, counter?
   for (;;)
   {
      pthread_cond_wait(&eclair->meta_tag_cond, &eclair->meta_tag_mutex);
      for (l = eclair->meta_tag_files_to_scan; l; l = next)
      {
         next = l->next;
         if ((current_file = (Eclair_Playlist_Media_File *)l->data))
         {
            TagLib_File *tag_file;
            TagLib_Tag *tag;
            const TagLib_AudioProperties *tag_audio_props;

            if (!current_file->path)
               continue;
            if (!(tag_file = taglib_file_new(current_file->path)))
               continue;
            if (!(tag = taglib_file_tag(tag_file)))
            {
               taglib_file_free(tag_file);
               continue;
            }
            current_file->artist = strdup(taglib_tag_artist(tag));
            current_file->title = strdup(taglib_tag_title(tag));
            current_file->album = strdup(taglib_tag_album(tag));
            current_file->genre = strdup(taglib_tag_genre(tag));
            current_file->comment = strdup(taglib_tag_comment(tag));
            current_file->year = taglib_tag_year(tag);
            current_file->track = taglib_tag_track(tag);

            if (!(tag_audio_props = taglib_file_audioproperties(tag_file)))
            {
               taglib_file_free(tag_file);
               continue;
            }
            current_file->length = taglib_audioproperties_length(tag_audio_props);

            taglib_file_free(tag_file);

            eclair_playlist_media_file_entry_update(current_file);
         }
         eclair->meta_tag_files_to_scan = evas_list_remove_list(eclair->meta_tag_files_to_scan, l);
         taglib_tag_free_strings();
      }
   }

   return NULL;
}

//Create the gui window and load the interface
static void _eclair_gui_create_window(Eclair *eclair)
{
   Evas *evas;
   Evas_Coord gui_width, gui_height;

   if (!eclair)
      return;

   if (eclair->gui_engine == ECLAIR_GL)
      eclair->gui_window = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 32, 32);
   else
      eclair->gui_window = ecore_evas_software_x11_new(NULL, 0, 0, 0, 32, 32);
   ecore_evas_title_set(eclair->gui_window, "eclair");
   ecore_evas_name_class_set(eclair->gui_window, "eclair", "eclair");
   ecore_evas_borderless_set(eclair->gui_window, 1);
   ecore_evas_shaped_set(eclair->gui_window, 1);

   evas = ecore_evas_get(eclair->gui_window);
   eclair->gui_object = edje_object_add(evas);
   edje_object_file_set(eclair->gui_object, PACKAGE_DATA_DIR "/themes/default.edj", "eclair_body");
   edje_object_size_min_get(eclair->gui_object, &gui_width, &gui_height);
   evas_object_resize(eclair->gui_object, (int)gui_width, (int)gui_height);
   evas_object_show(eclair->gui_object);
   ecore_evas_resize(eclair->gui_window, (int)gui_width, (int)gui_height);

   eclair->gui_draggies = esmart_draggies_new(eclair->gui_window);
   esmart_draggies_button_set(eclair->gui_draggies, 1);
   evas_object_move(eclair->gui_draggies, 0, 0);
   evas_object_resize (eclair->gui_draggies, gui_width, gui_height);
   evas_object_layer_set (eclair->gui_draggies, -1);
   evas_object_show(eclair->gui_draggies);

   if (edje_object_part_exists(eclair->gui_object, "playlist_container"))
   {
      eclair->playlist_container = esmart_container_new(evas_object_evas_get(eclair->gui_object)); 
      esmart_container_direction_set(eclair->playlist_container, CONTAINER_DIRECTION_VERTICAL);
      esmart_container_fill_policy_set(eclair->playlist_container, CONTAINER_FILL_POLICY_FILL_X);
      esmart_container_spacing_set(eclair->playlist_container, 0);
      esmart_container_padding_set(eclair->playlist_container, 0, 0, 0, 0);
      edje_object_part_swallow(eclair->gui_object, "playlist_container", eclair->playlist_container);
      evas_object_event_callback_add(eclair->playlist_container, EVAS_CALLBACK_MOUSE_WHEEL, eclair_gui_playlist_container_wheel_cb, eclair);
      evas_object_show(eclair->playlist_container);
   }

   evas_object_focus_set(eclair->gui_object, 1);
   evas_object_event_callback_add(eclair->gui_object, EVAS_CALLBACK_KEY_DOWN, eclair_key_press_cb, eclair);
   edje_object_signal_callback_add(eclair->gui_object, "eclair_close", "*", eclair_gui_close_cb, eclair);
   edje_object_signal_callback_add(eclair->gui_object, "eclair_minimize", "*", eclair_gui_minimize_cb, eclair);
   edje_object_signal_callback_add(eclair->gui_object, "eclair_open", "*", eclair_gui_open_cb, eclair);
   edje_object_signal_callback_add(eclair->gui_object, "eclair_play", "*", eclair_gui_play_cb, eclair);
   edje_object_signal_callback_add(eclair->gui_object, "eclair_pause", "*", eclair_gui_pause_cb, eclair);
   edje_object_signal_callback_add(eclair->gui_object, "eclair_stop", "*", eclair_gui_stop_cb, eclair);  
   edje_object_signal_callback_add(eclair->gui_object, "eclair_prev", "*", eclair_gui_prev_cb, eclair);  
   edje_object_signal_callback_add(eclair->gui_object, "eclair_next", "*", eclair_gui_next_cb, eclair);  
   edje_object_signal_callback_add(eclair->gui_object, "drag,stop", "progress_bar_drag", eclair_gui_progress_bar_drag_cb, eclair);
   edje_object_signal_callback_add(eclair->gui_object, "drag", "playlist_scrollbar_button", eclair_gui_playlist_scrollbar_button_drag_cb, eclair);
   edje_object_signal_callback_add(eclair->gui_object, "drag", "volume_bar_drag", eclair_gui_volume_bar_cb, eclair);

   ecore_evas_show(eclair->gui_window);
}

//Create the video window and object
static void _eclair_video_create_window(Eclair *eclair)
{
   Evas *evas;

   if (!eclair)
      return;

   if (eclair->video_engine == ECLAIR_GL)
      eclair->video_window = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 32, 32);
   else
      eclair->video_window = ecore_evas_software_x11_new(NULL, 0, 0, 0, 32, 32);
   ecore_evas_title_set(eclair->video_window, "eclair");
   ecore_evas_name_class_set(eclair->video_window, "eclair", "eclair");
   ecore_evas_data_set(eclair->video_window, "eclair", eclair);
   ecore_evas_callback_resize_set(eclair->video_window, eclair_video_window_resize_cb);
   ecore_evas_callback_delete_request_set(eclair->video_window, eclair_video_window_close_cb);
   ecore_evas_hide(eclair->video_window);
   
   evas = ecore_evas_get(eclair->video_window);
   eclair->video_object = emotion_object_add(evas);
   evas_object_show(eclair->video_object);
   evas_object_layer_set(eclair->video_object, 1);

   eclair->black_background = evas_object_rectangle_add(evas);
   evas_object_color_set(eclair->black_background, 0, 0, 0, 255);
   evas_object_show(eclair->black_background);
   evas_object_layer_set(eclair->black_background, 0);

   eclair->subtitles_object = evas_object_textblock_add(evas);
   evas_font_path_append(evas, PACKAGE_DATA_DIR "/fonts/");
   evas_object_layer_set(eclair->subtitles_object, 2);
   evas_object_show(eclair->subtitles_object);

   evas_object_focus_set(eclair->video_object, 1);
   evas_object_event_callback_add(eclair->video_object, EVAS_CALLBACK_KEY_DOWN, eclair_key_press_cb, eclair);
   evas_object_smart_callback_add(eclair->video_object, "frame_decode", eclair_video_frame_decode_cb, eclair);
   evas_object_smart_callback_add(eclair->video_object, "playback_finished", eclair_video_playback_finished_cb, eclair);   
}

int main(int argc, char *argv[])
{
   Eclair eclair;

   if (!eclair_init(&eclair, &argc, argv))
      return 1;
   
   ecore_main_loop_begin();

   return 0;
}
