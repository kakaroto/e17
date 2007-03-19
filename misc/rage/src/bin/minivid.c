#include "main.h"

static void minivid_free_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void minivid_obj_frame_resize_cb(void *data, Evas_Object *obj, void *event_info);
static void minivid_obj_stopped_cb(void *data, Evas_Object *obj, void *event_info);

Evas_Object *
minivid_add(char *module, char *file, int audio)
{
   Evas_Object *o, *ov;

   /* get a frame design from the edje theme */
   o = edje_object_add(evas);
   edje_object_file_set(o, theme, "video_container_mini");
   /* creat video object */
   ov = emotion_object_add(evas); 
   if (!emotion_object_init(ov, module))
     {
	printf("ERROR! - cannot init video module %s\n", module);
	evas_object_del(o);
	return NULL;
     }
   /* set callback on free of edje design we will return - if it is freed
    * call the callback to clean up the video obj */
   evas_object_event_callback_add(o, EVAS_CALLBACK_FREE, minivid_free_cb, ov);
   /* callbacks for video events we need to handle */
   evas_object_smart_callback_add(ov, "frame_resize", minivid_obj_frame_resize_cb, o);
   evas_object_smart_callback_add(ov, "decode_stop", minivid_obj_stopped_cb, o);
   /* set the file to play */
   emotion_object_file_set(ov, file);
   /* start playing */
   emotion_object_play_set(ov, 1);
   /* if audio is on - then play audio too */
   if (audio)
     {
	emotion_object_audio_mute_set(ov, 0);
	emotion_object_audio_volume_set(ov, 1.0);
     }
   else
     emotion_object_audio_mute_set(ov, 1);
   /* call the resize callback to handle a swallow into the edje design */
   minivid_obj_frame_resize_cb(o, ov, NULL);
   /* show out objects */
   evas_object_show(o);
   evas_object_show(ov);
   edje_object_signal_emit(o, "active", "on");
   return o;
}

void
minivid_del(Evas_Object *obj)
{
   edje_object_signal_emit(obj, "active", "off");
   evas_object_del(obj);
}

/***/
/* on free of the parent edje - delete the swallowed video */
static void
minivid_free_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   evas_object_del(data);
}

/* if the video changes size during playback - handle this */
static void
minivid_obj_frame_resize_cb(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Coord w, h;
   int iw, ih;
   double ratio;

   /* get size and ratio and calculate a desired output ratio */
   emotion_object_size_get(obj, &iw, &ih);
   if ((iw <= 1) || (ih <= 1))
     {
	w = 640;
	h = 480;
     }
   else
     {
	ratio = emotion_object_ratio_get(obj);
	if (ratio > 0.0) iw = (ih * ratio) + 0.5;
	else ratio = (double)iw / (double)ih;
	w = 10240 * ratio;
	h = 10240;
     }
/* this would make the video fill the swallowed region all the way to the edge
 * without padding, but that means some video may be cut off */
/*   edje_extern_object_aspect_set(ov, EDJE_ASPECT_CONTROL_NEITHER, w, h);*/
/* this makes the video fit with padding of blank pixels to its swallowed
 * region - so you get "ltterbox" effects */
   edje_extern_object_aspect_set(obj, EDJE_ASPECT_CONTROL_BOTH, w, h);
   /* actuslly re-pack with new options */
   edje_object_part_swallow(data, "video", obj);
}

/* when the video stops - play again from the start */
static void
minivid_obj_stopped_cb(void *data, Evas_Object *obj, void *event_info)
{
   emotion_object_position_set(obj, 0.0);
   emotion_object_play_set(obj, 1);
}
