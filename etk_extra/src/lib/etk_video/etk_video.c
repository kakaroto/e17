/** @file etk_video.c */
#include <Emotion.h>
#include "etk_video.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>

/**
 * @addtogroup Etk_Video
 * @{
 */

enum Etk_Video_Property_Id
{
   ETK_VIDEO_FILE_PROPERTY,
   ETK_VIDEO_KEEP_ASPECT_PROPERTY,
};

static void _etk_video_constructor(Etk_Video *video);
static void _etk_video_destructor(Etk_Video *video);
static void _etk_video_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_video_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static Etk_Bool _etk_video_realize_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_video_unrealize_cb(Etk_Object *object, void *data);
static void _etk_video_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_video_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_video_load(Etk_Video *video);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Video
 * @return Returns the type of an Etk_Video
 */
Etk_Type *etk_video_type_get()
{
   static Etk_Type *video_type = NULL;

   if (!video_type)
   {
      video_type = etk_type_new("Etk_Video", ETK_WIDGET_TYPE, sizeof(Etk_Video),
         ETK_CONSTRUCTOR(_etk_video_constructor), ETK_DESTRUCTOR(_etk_video_destructor), NULL);

      etk_type_property_add(video_type, "video-file", ETK_VIDEO_FILE_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(video_type, "keep-aspect", ETK_VIDEO_KEEP_ASPECT_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));

      video_type->property_set = _etk_video_property_set;
      video_type->property_get = _etk_video_property_get;
   }

   return video_type;
}

/**
 * @brief Creates a new video
 * @return Returns the new video widget
 */
Etk_Widget *etk_video_new()
{
   return etk_widget_new(ETK_VIDEO_TYPE, NULL);
}

/**
 * @brief Creates a new video and loads the video from a video file
 * @param filename the name of the file to load
 * @return Returns the new video widget
 */
Etk_Widget *etk_video_new_from_file(const char *filename)
{
   return etk_widget_new(ETK_VIDEO_TYPE, "video-file", filename, NULL);
}

/**
 * @brief Loads the video from a file
 * @param video a video widget
 * @param filename the name of the file to load
 */
void etk_video_set_from_file(Etk_Video *video, const char *filename)
{

   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(video)) || (video->filename == filename))
      return;

   free(video->filename);
   video->filename = filename ? strdup(filename) : NULL;
   etk_object_notify(ETK_OBJECT(video), "video-file");

   _etk_video_load(video);

}

/**
 * @brief Gets the name of the file used for the video
 * @param video a video widget
 * @return Returns the name of the file used by the video (NULL on failure)
 */
const char *etk_video_file_get(Etk_Video *video)
{
   if (!video)
      return NULL;
   return video->filename;
}

/**
 * @brief Gets the native size of the video
 * @param video a video widget
 * @param width the location where to set the native width of the video
 * @param height the location where to set the native height of the video
 */
void etk_video_size_get(Etk_Video *video, int *width, int *height)
{
   if (!video)
   {
      if (width)
         *width = 0;
      if (height)
         *height = 0;
   }
   else
   {

      emotion_object_size_get(video->video_object, width, height);

   }
}

/**
 * @brief Plays or stops a video
 * @param video a video widget
 * @param play wether to play or stop (ETK_TRUE / ETK_FALSE)
 */
void etk_video_play_set(Etk_Video *video, Etk_Bool play)
{

   if (!video)
      return;

   if (!video->video_object && video->filename)
   {
      video->play_queued = ETK_TRUE;
      return;
   }

   emotion_object_play_set(video->video_object, play);

}

/**
 * @brief Gets wether a video is playing or not
 * @param video a video widget
 * @return Returns wether a video is playing or not (ETK_TRUE / ETK_FALSE)
 */
Etk_Bool etk_video_play_get(Etk_Video *video)
{

   if (!video || !video->video_object)
      return ETK_FALSE;

   return emotion_object_play_get(video->video_object);

}

/**
 * @brief Sets the position of the video in seconds
 * @param video a video widget
 * @param seconds the position of the video in seconds
 */
void etk_video_position_set(Etk_Video *video, double seconds)
{

   if (!video || !video->video_object)
      return;

   return emotion_object_position_set(video->video_object, seconds);
}

/**
 * @brief Gets the position of the video in seconds
 * @param video a video widget
 * @return Returns the position of the video in seconds
 */
double etk_video_position_get(Etk_Video *video)
{

   if (!video || !video->video_object)
      return ETK_FALSE;

   return emotion_object_position_get(video->video_object);
}

/**
 * @brief Gets the total play length of a video
 * @param video a video widget
 * @return Returns the total play length of the video
 */
double etk_video_play_length_get(Etk_Video *video)
{

   if (!video || !video->video_object)
      return 0;

   return emotion_object_play_length_get(video->video_object);
}

/**
 * @brief Sets the volume of the audio
 * @param video a video widget
 * @param volume the volume to set
 */
void etk_video_audio_volume_set(Etk_Video *video, double volume)
{

   if (!video || !video->video_object)
      return;

   emotion_object_audio_volume_set(video->video_object, volume);
}

/**
 * @brief Get the volume of the video
 * @param video a video widget
 * @return Returns the volume of the video
 */
double etk_video_audio_volume_get(Etk_Video *video)
{

   if (!video || !video->video_object)
      return 0;

   return emotion_object_audio_volume_get(video->video_object);
}

/**
 * @brief Set wether the volume of the video is muted or not
 * @param video a video widget
 * @param mute wether to mute or unmute the volume (ETK_TRUE / ETK_FALSE)
 */
void etk_video_audio_mute_set(Etk_Video *video, Etk_Bool mute)
{

   if (!video || !video->video_object)
      return;

   emotion_object_audio_mute_set(video->video_object, mute);
}

/**
 * @brief Gets wether the volume of the video is muted or not
 * @param video a video widget
 * @return Returns wether the volume of the video is muted or not
 */
Etk_Bool etk_video_audio_mute_get(Etk_Video *video)
{

   if (!video || !video->video_object)
      return 0;

   return emotion_object_audio_mute_get(video->video_object);
}

/**
 * @brief Sets if the video should keep its aspect ratio when it is resized
 * @param video a video widget
 * @param keep_aspect if keep_aspect == ETK_TRUE, the video will keep its aspect ratio when itis resized
 */
void etk_video_keep_aspect_set(Etk_Video *video, Etk_Bool keep_aspect)
{

   if (!video)
      return;

   video->keep_aspect = keep_aspect;
   etk_widget_size_recalc_queue(ETK_WIDGET(video));
   etk_object_notify(ETK_OBJECT(video), "keep-aspect");
}

/**
 * @brief Get whether the video keeps its aspect ratio when it is resized
 * @param video a video widget
 * @return Returns ETK_TRUE if the video keeps its aspect ratio when it is resized
 */
Etk_Bool etk_video_keep_aspect_get(Etk_Video *video)
{
   if (!video)
      return ETK_TRUE;
   return video->keep_aspect;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the video */
static void _etk_video_constructor(Etk_Video *video)
{

   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(video)))
      return;

   video->video_object = NULL;
   video->filename = NULL;
   video->keep_aspect = ETK_TRUE;
   video->play_queued = ETK_FALSE;

   widget->size_request = _etk_video_size_request;
   widget->size_allocate = _etk_video_size_allocate;

   etk_signal_connect("realized", ETK_OBJECT(video), ETK_CALLBACK(_etk_video_realize_cb), NULL);
   etk_signal_connect("unrealized", ETK_OBJECT(video), ETK_CALLBACK(_etk_video_unrealize_cb), NULL);

}

/* Destroys the video */
static void _etk_video_destructor(Etk_Video *video)
{
   if (!video)
      return;

   free(video->filename);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_video_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{

   Etk_Video *video;

   if (!(video = ETK_VIDEO(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_VIDEO_FILE_PROPERTY:
         etk_video_set_from_file(video, etk_property_value_string_get(value));
         break;
      case ETK_VIDEO_KEEP_ASPECT_PROPERTY:
         etk_video_keep_aspect_set(video, etk_property_value_bool_get(value));
         break;
      default:
         break;
   }

}

/* Gets the value of the property whose id is "property_id" */
static void _etk_video_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{

   Etk_Video *video;

   if (!(video = ETK_VIDEO(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_VIDEO_FILE_PROPERTY:
         etk_property_value_string_set(value, video->filename);
         break;
      case ETK_VIDEO_KEEP_ASPECT_PROPERTY:
         etk_property_value_bool_set(value, video->keep_aspect);
         break;
      default:
         break;
   }

}

/* Calculates the ideal size of the video */
static void _etk_video_size_request(Etk_Widget *widget, Etk_Size *size)
{

   Etk_Video *video;

   if (!(video = ETK_VIDEO(widget)) || !size)
      return;

   if (video->video_object)
   {
      emotion_object_size_get(video->video_object, &size->w, &size->h);
   }
   else
   {
      size->w = 0;
      size->h = 0;
   }

}

/* Resizes the video to the allocated size */
static void _etk_video_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{

   Etk_Video *video;

   if (!(video = ETK_VIDEO(widget)))
      return;

   if (!video->video_object)
      return;

   if (video->keep_aspect)
   {
      double aspect_ratio;
      int video_w, video_h;
      int new_size;

      emotion_object_size_get(video->video_object, &video_w, &video_h);

      if (video_w <= 0 || video_h <= 0)
      {
         video_w = 1;
         video_h = 1;
      }

      aspect_ratio = (double)video_w / (double)video_h;
      if (geometry.h * aspect_ratio > geometry.w)
      {
         new_size = geometry.w / aspect_ratio;
         geometry.y += (geometry.h - new_size) / 2;
         geometry.h = new_size;
      }
      else
      {
         new_size = geometry.h * aspect_ratio;
         geometry.x += (geometry.w - new_size) / 2;
         geometry.w = new_size;
      }
   }

   evas_object_move(video->video_object, geometry.x, geometry.y);
   evas_object_resize(video->video_object, geometry.w, geometry.h);

}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the video is realized */
static Etk_Bool _etk_video_realize_cb(Etk_Object *object, void *data)
{
   Etk_Video *video;

   if (!(video = ETK_VIDEO(object)))
      return ETK_TRUE;
   _etk_video_load(video);

   return ETK_TRUE;
}

/* Called when the video is unrealized */
static Etk_Bool _etk_video_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Video *video;

   if (!(video = ETK_VIDEO(object)))
      return ETK_TRUE;
   video->video_object = NULL;

   return ETK_TRUE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Load the video from the video or the edje file */
static void _etk_video_load(Etk_Video *video)
{

   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(video)))
      return;

   if (video->video_object)
   {
      etk_widget_member_object_del(widget, video->video_object);
      evas_object_del(video->video_object);
      video->video_object = NULL;
   }
   if (video->filename)
   {
      Evas *evas;

      if (!video->video_object && (evas = etk_widget_toplevel_evas_get(widget)))
      {
         /* TODO: FIXME: the video might not be realized here... */
         video->video_object = emotion_object_add(evas);
	 /* TODO: FIXME: we need to detect what engine emotion is using */
	 emotion_object_init(video->video_object, "xine");
         etk_widget_member_object_add(widget, video->video_object);
      }
      if (video->video_object)
      {
         const char *video_file;

	 video_file = emotion_object_file_get(video->video_object);
         if (!video_file || strcmp(video_file, video->filename) != 0)
         {
	    /* TODO: spit out error and hide object if this doesnt work */
	    emotion_object_file_set(video->video_object, video->filename);
	    evas_object_show(video->video_object);
         }
      }

      if (video->play_queued)
      {
	 emotion_object_play_set(video->video_object, 1);
	 video->play_queued = ETK_FALSE;
      }
   }

   etk_widget_size_recalc_queue(widget);

}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Video
 *
 * @image html widgets/image.png
 * This widget supports loading any video supported by your installation of emotion.
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Video
 *
 * \par Properties:
 * @prop_name "video_file": The video file (.avi, .mpg, ...) which the video is loaded from.
 * This file has to be supported by Emotion.
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "keep_aspect": Whether of not the video keeps its aspect ratio when it is resized
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 */
