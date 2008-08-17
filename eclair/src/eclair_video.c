#include "eclair_video.h"
#include "../config.h"
#include <Emotion.h>
#include "eclair.h"
#include "eclair_callbacks.h"
#include "eclair_window.h"

static void *_eclair_video_create_emotion_object_thread(void *param);
static void _eclair_video_frame_decode_cb(void *data, Evas_Object *obj, void *event_info);
static void _eclair_video_playback_finished_cb(void *data, Evas_Object *obj, void *event_info);
static void _eclair_video_frame_resize_change_cb(void *data, Evas_Object *obj, void *event_info);
static void _eclair_video_audio_level_change_cb(void *data, Evas_Object *obj, void *event_info);
static void _eclair_video_audio_level_change_cb(void *data, Evas_Object *obj, void *event_info);
static void _eclair_video_window_resize_cb(Ecore_Evas *window);
static void _eclair_video_window_close_cb(Ecore_Evas *window);

//Initialize video vars
void eclair_video_init(Eclair_Video *video, Eclair *eclair)
{
   if (!video)
      return;

   video->video_window = NULL;
   video->video_object = NULL;
   video->black_background = NULL;
   video->subtitles_object = NULL;
   video->previous_video_width = 0;
   video->previous_video_height = 0;
   video->previous_video_ratio = 0.0;
   video->eclair = eclair;
}

//Shutdown the video module
void eclair_video_shutdown(Eclair_Video *video)
{
   if (!video)
      return;

   fprintf(stderr, "Eclair: Debug: Destroying \"create emotion object\" thread\n");
   pthread_join(video->video_create_thread, NULL);
   fprintf(stderr, "Eclair: Debug: \"create emotion object\" thread destroyed\n");
}
//Create the video window and object
void eclair_create_video_window(Eclair_Video *video, Eclair_Engine engine)
{
   Evas *evas;

   if (!video)
      return;

   if (engine == ECLAIR_GL)
      video->video_window = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 0, 0);
   else
      video->video_window = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
   ecore_evas_title_set(video->video_window, "eclair");
   ecore_evas_name_class_set(video->video_window, "eclair", "Eclair");
   ecore_evas_data_set(video->video_window, "eclair_video", video);
   ecore_evas_callback_resize_set(video->video_window, _eclair_video_window_resize_cb);
   ecore_evas_callback_delete_request_set(video->video_window, _eclair_video_window_close_cb);
   ecore_evas_hide(video->video_window);

   evas = ecore_evas_get(video->video_window);
   video->black_background = evas_object_rectangle_add(evas);
   evas_object_color_set(video->black_background, 0, 0, 0, 255);
   evas_object_layer_set(video->black_background, 0);
   evas_object_show(video->black_background);

   video->subtitles_object = evas_object_textblock_add(evas);
   evas_font_path_append(evas, PACKAGE_DATA_DIR "/fonts/");
   evas_object_layer_set(video->subtitles_object, 2);
   evas_object_show(video->subtitles_object);

   pthread_create(&video->video_create_thread, NULL, _eclair_video_create_emotion_object_thread, video);
}

//Initialize the video object
static void *_eclair_video_create_emotion_object_thread(void *param)
{
   Eclair_Video *video;
   Eclair *eclair;
   Evas *evas;
   Evas_Object *new_video_object;
   int result = 0;

   if (!(video = param) || !(eclair = video->eclair))
      return NULL;

   //TODO: make functions to display whatever we want on the media bar
   if (eclair->gui_window)
      edje_object_part_text_set(eclair->gui_window->edje_object, "current_media_name", "Initializing - Please wait...");

   evas = ecore_evas_get(video->video_window);
   new_video_object = emotion_object_add(evas);
   evas_object_layer_set(new_video_object, 1);
   evas_object_show(new_video_object);

   switch (eclair->video_module)
   {
      case ECLAIR_VIDEO_GSTREAMER:
         result |= emotion_object_init(new_video_object, "gstreamer");
         break;
      case ECLAIR_VIDEO_XINE:
      default:
         result |= emotion_object_init(new_video_object, "xine");
         break;
   }
   if (!result)
   {
      evas_object_del(new_video_object);
      return NULL;
   }

   evas_object_focus_set(new_video_object, 1);
   evas_object_event_callback_add(new_video_object, EVAS_CALLBACK_KEY_DOWN, eclair_key_press_cb, eclair);
   evas_object_smart_callback_add(new_video_object, "frame_decode", _eclair_video_frame_decode_cb, video);
   evas_object_smart_callback_add(new_video_object, "playback_finished", _eclair_video_playback_finished_cb, video);
   evas_object_smart_callback_add(new_video_object, "audio_level_change", _eclair_video_audio_level_change_cb, video);
   evas_object_smart_callback_add(new_video_object, "frame_resize", _eclair_video_frame_resize_change_cb, video);

   eclair_audio_level_set(eclair, emotion_object_audio_volume_get(new_video_object));
   video->video_object = new_video_object;
   eclair_update_current_file_info(eclair, 0);

   if (eclair->start_playing)
      eclair_play_current(eclair);

   return NULL;
}

//Resize the video object and the black background object according to the size of the window
void eclair_video_window_resize(Eclair_Video *video)
{
   Evas_Coord window_width, window_height, video_width, video_height, X, Y;
   double ratio;

   if (!video || !video->video_window)
      return;

   evas_output_viewport_get(ecore_evas_get(video->video_window), NULL, NULL, &window_width, &window_height);

   if (video->video_object)
   {
      ratio = emotion_object_ratio_get(video->video_object);
      if (ratio > 0.0)
      {
         if (window_width / ratio > window_height)
         {
            video_height = window_height;
            video_width = video_height * ratio;
         }
         else
         {
            video_width = window_width;
            video_height = video_width / ratio;
         }

         X = (window_width - video_width) / 2.0;
         Y = (window_height - video_height) / 2.0;
      }
      else
      {
         video_width = window_width;
         video_height = window_height;
         X = Y = 0;
      }
      evas_object_move(video->video_object, X, Y);
      evas_object_resize(video->video_object, video_width, video_height);
   }
   if (video->black_background)
   {
      evas_object_move(video->black_background, 0, 0);
      evas_object_resize(video->black_background, window_width, window_height);
   }
}


//------------------------------
// Callbacks
//------------------------------

//Called when a new frame is decoded
static void _eclair_video_frame_decode_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eclair_Video *video;

   if ((video = data))
      eclair_update(video->eclair);
}

//Called when the current media playback is finished
static void _eclair_video_playback_finished_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eclair_Video *video;

   if ((video = data))
      eclair_play_next(video->eclair);
}

//Called when the video has to be resized
static void _eclair_video_frame_resize_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eclair_Video *video;
   int w, h;
   double ratio;

   if (!(video = data) || !video->video_window || !video->video_object)
      return;

   emotion_object_size_get(video->video_object, &w, &h);
   ratio = emotion_object_ratio_get(video->video_object);

   if (video->previous_video_width == w && video->previous_video_height == h && video->previous_video_ratio == ratio)
      return;

   if (ratio > 0.0)
      ecore_evas_resize(video->video_window, h * ratio, h);
   else
      ecore_evas_resize(video->video_window, w, h);

   video->previous_video_width = w;
   video->previous_video_height = h;
   video->previous_video_ratio = ratio;
}

//Called when the audio volume is changed by an external application
static void _eclair_video_audio_level_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eclair_Video *video;
   Eclair *eclair;

   if ((video = data) && (eclair = video->eclair) && video->video_object)
      eclair_audio_level_set(eclair, emotion_object_audio_volume_get(video->video_object));
}

//Called when the video window is resized
static void _eclair_video_window_resize_cb(Ecore_Evas *window)
{
   Eclair_Video *video;

   if ((video = ecore_evas_data_get(window, "eclair_video")))
      eclair_video_window_resize(video);
}

//Called when the video window is closed
static void _eclair_video_window_close_cb(Ecore_Evas *window)
{
   Eclair_Video *video;

   if ((video = ecore_evas_data_get(window, "eclair_video")))
      eclair_stop(video->eclair);
}
