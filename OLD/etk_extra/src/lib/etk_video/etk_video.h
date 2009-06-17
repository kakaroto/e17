/** @file etk_video.h */
#ifndef _ETK_VIDEO_H_
#define _ETK_VIDEO_H_

#include <Etk.h>
#include <Evas.h>

/**
 * @defgroup Etk_Video Etk_Video
 * @brief An Etk_Video is a widget that can load and display and control a video
 * @{
 */

/** Gets the type of an video */
#define ETK_VIDEO_TYPE        (etk_video_type_get())
/** Casts the object to an Etk_Video */
#define ETK_VIDEO(obj)        (ETK_OBJECT_CAST((obj), ETK_VIDEO_TYPE, Etk_Video))
/** Check if the object is an Etk_Video */
#define ETK_IS_VIDEO(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_VIDEO_TYPE))

typedef struct Etk_Video Etk_Video;

/**
 * @brief @widget A widget that can load and display an video
 * @structinfo
 */
struct Etk_Video
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Evas_Object *video_object;
   char *filename;
   
   Etk_Bool keep_aspect;
   Etk_Bool play_queued;
};

Etk_Type   *etk_video_type_get();
Etk_Widget *etk_video_new();
Etk_Widget *etk_video_new_from_file(const char *filename);

void        etk_video_set_from_file(Etk_Video *video, const char *filename);
const char *etk_video_file_get(Etk_Video *video);

Evas_Object *etk_video_object_get(Etk_Video *video);

void etk_video_play_set(Etk_Video *video, Etk_Bool play);
Etk_Bool etk_video_play_get(Etk_Video *video);

void etk_video_position_set(Etk_Video *video, double seconds);
double etk_video_position_get(Etk_Video *video);

double etk_video_play_length_get(Etk_Video *video);

void etk_video_audio_volume_set(Etk_Video *video, double volume);
double etk_video_audio_volume_get(Etk_Video *video);

void etk_video_audio_mute_set(Etk_Video *video, Etk_Bool mute);
Etk_Bool etk_video_audio_mute_get(Etk_Video *video);

void etk_video_size_get(Etk_Video *video, int *width, int *height);
  
void     etk_video_keep_aspect_set(Etk_Video *video, Etk_Bool keep_aspect);
Etk_Bool etk_video_keep_aspect_get(Etk_Video *video);

/** @} */

#endif
