#include "main.h"

static int status_volume_add(void *data, int type, void *ev);
static int status_volume_del(void *data, int type, void *ev);
static int status_volume_type_add(void *data, int type, void *ev);
static int status_volume_type_del(void *data, int type, void *ev);
static int status_volume_scan_start(void *data, int type, void *ev);
static int status_volume_scan_stop(void *data, int type, void *ev);
static int status_volume_scan_go(void *data, int type, void *ev);

static Evas_Object *o_busy = NULL;
static Evas_Object *o_status = NULL;
static int busy_count = 0;
static int type_count = 0;
static int have_video = 0;
static int have_audio = 0;
static int have_photo = 0;

void
status_init(void)
{
   ecore_event_handler_add(VOLUME_ADD, status_volume_add, NULL);
   ecore_event_handler_add(VOLUME_DEL, status_volume_del, NULL);
   ecore_event_handler_add(VOLUME_TYPE_ADD, status_volume_type_add, NULL);
   ecore_event_handler_add(VOLUME_TYPE_DEL, status_volume_type_del, NULL);
   ecore_event_handler_add(VOLUME_SCAN_START, status_volume_scan_start, NULL);
   ecore_event_handler_add(VOLUME_SCAN_STOP, status_volume_scan_stop, NULL);
   ecore_event_handler_add(VOLUME_SCAN_GO, status_volume_scan_go, NULL);

   o_busy = edje_object_add(evas);
   edje_object_file_set(o_busy, theme, "busy");
   layout_swallow("busy", o_busy);
   evas_object_show(o_busy);

   o_status = edje_object_add(evas);
   edje_object_file_set(o_status, theme, "status");
   layout_swallow("status", o_status);
   evas_object_show(o_status);
}

static int
status_volume_add(void *data, int type, void *ev)
{
   return 1;
}

static int
status_volume_del(void *data, int type, void *ev)
{
   return 1;
}

static int
status_volume_type_add(void *data, int type, void *ev)
{
   char buf[256];
   int p;
   
   type_count++;
   p = have_video;
   have_video = volume_type_num_get("video");
   if ((p == 0) && (have_video > 0)) menu_item_enabled_set("Main", "Videos", 1);
   p = have_audio;
   have_audio = volume_type_num_get("audio");
   if ((p == 0) && (have_audio > 0)) menu_item_enabled_set("Main", "Music", 1);
   p = have_photo;
   have_photo = volume_type_num_get("photo");
   if ((p == 0) && (have_photo > 0)) menu_item_enabled_set("Main", "Photos", 1);
   
   snprintf(buf, sizeof(buf), "%i", have_video);
   edje_object_part_text_set(o_status, "video_text", buf);
   snprintf(buf, sizeof(buf), "%i", have_audio);
   edje_object_part_text_set(o_status, "audio_text", buf);
   snprintf(buf, sizeof(buf), "%i", have_photo);
   edje_object_part_text_set(o_status, "photo_text", buf);
   return 1;
}

static int
status_volume_type_del(void *data, int type, void *ev)
{
   char buf[256];
   int p;
   
   type_count++;
   p = have_video;
   have_video = volume_type_num_get("video");
   if ((p > 0) && (have_video == 0)) menu_item_enabled_set("Main", "Videos", 0);
   p = have_audio;
   have_audio = volume_type_num_get("audio");
   if ((p > 0) && (have_audio == 0)) menu_item_enabled_set("Main", "Music", 0);
   p = have_photo;
   have_photo = volume_type_num_get("photo");
   if ((p > 0) && (have_photo == 0)) menu_item_enabled_set("Main", "Photos", 0);
   
   snprintf(buf, sizeof(buf), "%i", have_video);
   edje_object_part_text_set(o_status, "video_text", buf);
   snprintf(buf, sizeof(buf), "%i", have_audio);
   edje_object_part_text_set(o_status, "audio_text", buf);
   snprintf(buf, sizeof(buf), "%i", have_photo);
   edje_object_part_text_set(o_status, "photo_text", buf);
   return 1;
}

static int
status_volume_scan_start(void *data, int type, void *ev)
{
   busy_count++;
   if (busy_count == 1)
     {
	edje_object_signal_emit(o_status, "busy", "start");
	edje_object_signal_emit(o_busy, "busy", "start");
     }
   return 1;
}

static int
status_volume_scan_stop(void *data, int type, void *ev)
{
   busy_count--;
   if (busy_count == 0)
     {
	edje_object_signal_emit(o_busy, "busy", "stop");
	edje_object_signal_emit(o_status, "busy", "stop");
     }
   return 1;
}

static int
status_volume_scan_go(void *data, int type, void *ev)
{
   return 1;
}
