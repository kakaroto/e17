#include "main.h"

static Evas_Object *o_video = NULL;
static Evas_Object *o_video_bg = NULL;
static double jump = 0.0;
static Ecore_Timer *jump_reset_timer = NULL;
static Ecore_Job *video_stopped_job = NULL;
static Ecore_Timer *_hide_timer = NULL;

static int video_menu_bg_hide_tmer_cb(void *data);
static void video_resize(void);
static int video_jump_reset_timer_cb(void *data);
static void video_obj_frame_decode_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_frame_resize_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_length_change_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_stopped_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_channels_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_title_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_progress_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_ref_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_button_num_cb(void *data, Evas_Object *obj, void *event_info);
static void video_obj_button_cb(void *data, Evas_Object *obj, void *event_info);
static void video_stopped_job_cb(void *data);

void
video_init(char *module, char *file, char *swallow)
{
   Evas_Object *o;

   if (o_video_bg) return;
   o = edje_object_add(evas);
   edje_object_file_set(o, theme, "video_container");
   o_video_bg = o;
   
   o = emotion_object_add(evas); 
   if (!emotion_object_init(o, module))
     {
	printf("ERROR!\n");
     }
   
   evas_object_smart_callback_add(o, "frame_decode", video_obj_frame_decode_cb, NULL);
   evas_object_smart_callback_add(o, "frame_resize", video_obj_frame_resize_cb, NULL);
   evas_object_smart_callback_add(o, "length_change", video_obj_length_change_cb, NULL);
   
   evas_object_smart_callback_add(o, "decode_stop", video_obj_stopped_cb, NULL);
   evas_object_smart_callback_add(o, "channels_change", video_obj_channels_cb, NULL);
   evas_object_smart_callback_add(o, "title_change", video_obj_title_cb, NULL);
   evas_object_smart_callback_add(o, "progress_change", video_obj_progress_cb, NULL);
   evas_object_smart_callback_add(o, "ref_change", video_obj_ref_cb, NULL);
   evas_object_smart_callback_add(o, "button_num_change", video_obj_button_num_cb, NULL);
   evas_object_smart_callback_add(o, "button_change", video_obj_button_cb, NULL);
   o_video = o;
   
   emotion_object_file_set(o_video, file);
   emotion_object_play_set(o_video, 1);
   emotion_object_audio_mute_set(o_video, 0);
   emotion_object_audio_volume_set(o_video, 1.0);
   layout_swallow(swallow, o_video_bg);

   edje_extern_object_aspect_set(o_video, EDJE_ASPECT_CONTROL_BOTH, 640, 480);
   edje_object_part_swallow(o_video_bg, "video", o_video);

   video_resize();
   
   evas_object_show(o_video);
   evas_object_show(o_video_bg);

   _hide_timer = ecore_timer_add(10.0, video_menu_bg_hide_tmer_cb, NULL);
   
   /* FIXME: add this video to recently played list */
}
 
void
video_shutdown(void)
{
   if (!o_video_bg) return;
   if (!_hide_timer)
     {
	menu_show();
	background_show();
	mini_pause_set(0);
     }
   else
     {
	ecore_timer_del(_hide_timer);
	_hide_timer = NULL;
     }
   
   evas_object_del(o_video_bg);
   evas_object_del(o_video);
   if (video_stopped_job) ecore_job_del(video_stopped_job);
   o_video = NULL;
   o_video_bg = NULL;
   video_stopped_job = NULL;
}
 
/* ESC   INS   PSE
 * 1     2     3
 * 4     5     6
 * 7     8     9
 * END   0     BSP
 *
 *     t   g
 * HME         m
 *       UP
 * LFT   RET   RGT
 *       DWN
 * r           spc
 *     s   p
 * 
 * BLF BRT PER l
 * PRI NEX COM k
 * TAB q   w   n
 */

void
video_key(Evas_Event_Key_Down *ev)
{
   
   if      (!strcmp(ev->keyname, "bracketleft"))
     {
	/* FIXME: play info display */
	double p;
	
	p = emotion_object_position_get(o_video);
	if (jump >= 0.0) jump = -7.5;
	else jump *= 2.0;
	if (jump < -120.0) jump = -120.0;
	if (jump_reset_timer)
	  ecore_timer_del(jump_reset_timer);
	jump_reset_timer = ecore_timer_add(2.0, 
					   video_jump_reset_timer_cb,
					   NULL);
	emotion_object_position_set(o_video, p + jump);
	edje_object_signal_emit(o_video_bg, "active", "");
     }
   else if (!strcmp(ev->keyname, "bracketright"))
     {
	/* FIXME: play info display */
	double p;
	
	p = emotion_object_position_get(o_video);
	if (jump <= 0.0) jump = 7.5;
	else jump *= 2.0;
	if (jump > 120.0) jump = 120.0;
	if (jump_reset_timer)
	  ecore_timer_del(jump_reset_timer);
	jump_reset_timer = ecore_timer_add(2.0, 
					   video_jump_reset_timer_cb,
					   NULL);
	emotion_object_position_set(o_video, p + jump);
	edje_object_signal_emit(o_video_bg, "active", "");
     }
   else if (!strcmp(ev->keyname, "p"))
     {
	/* FIXME: play info display end */
	jump = 0.0;
	emotion_object_play_set(o_video, 1);
	edje_object_signal_emit(o_video_bg, "active", "");
     }
   else if (!strcmp(ev->keyname, "space"))
     {
	/* FIXME: play info display */
	jump = 0.0;
	if (emotion_object_play_get(o_video))
	  emotion_object_play_set(o_video, 0);
	else
	  emotion_object_play_set(o_video, 1);
	edje_object_signal_emit(o_video_bg, "active", "");
     }
   else if (!strcmp(ev->keyname, "Up"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_UP);
   else if (!strcmp(ev->keyname, "Down"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_DOWN);
   else if (!strcmp(ev->keyname, "Left"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_LEFT);
   else if (!strcmp(ev->keyname, "Right"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_RIGHT);
   else if (!strcmp(ev->keyname, "Return"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_SELECT);
   else if (!strcmp(ev->keyname, "n"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_MENU1);
   else if (!strcmp(ev->keyname, "Prior"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_PREV);
   else if (!strcmp(ev->keyname, "Next"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_NEXT);
   else if (!strcmp(ev->keyname, "0"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_0);
   else if (!strcmp(ev->keyname, "1"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_1);
   else if (!strcmp(ev->keyname, "2"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_2);
   else if (!strcmp(ev->keyname, "3"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_3);
   else if (!strcmp(ev->keyname, "4"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_4);
   else if (!strcmp(ev->keyname, "5"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_5);
   else if (!strcmp(ev->keyname, "6"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_6);
   else if (!strcmp(ev->keyname, "7"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_7);
   else if (!strcmp(ev->keyname, "8"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_8);
   else if (!strcmp(ev->keyname, "9"))
     emotion_object_event_simple_send(o_video, EMOTION_EVENT_9);
   else if (!strcmp(ev->keyname, "k"))
     {
	/* FIXME: volume display */
	emotion_object_audio_volume_set(o_video, emotion_object_audio_volume_get(o_video) - 0.1);
     }
   else if (!strcmp(ev->keyname, "l"))
     {
	/* FIXME: volume display */
	emotion_object_audio_volume_set(o_video, emotion_object_audio_volume_get(o_video) + 0.1);
     }
   else if (!strcmp(ev->keyname, "m"))
     {
	/* FIXME: volume display */
	if (emotion_object_audio_mute_get(o_video))
	  emotion_object_audio_mute_set(o_video, 0);
	else
	  emotion_object_audio_mute_set(o_video, 1);
     }
   else if (!strcmp(ev->keyname, "s"))
     {
	/* FIXME: save position for this video */
	main_mode_pop();
	jump = 0.0;
	video_shutdown();
     }
   else if (!strcmp(ev->keyname, "Home"))
     {
	/* FIXME: pop up menu for options etc. */
     }
   else if (!strcmp(ev->keyname, "Insert"))
     {
	/* FIXME: program ? */
     }
   else if (!strcmp(ev->keyname, "Pause"))
     {
	/* FIXME: standby ? */
     }
   else if (!strcmp(ev->keyname, "End"))
     {
	/* FIXME: input ? */
     }
   else if (!strcmp(ev->keyname, "BackSpace"))
     {
	/* FIXME: catv/clear ? */
     }
   else if (!strcmp(ev->keyname, "t"))
     {
	/* FIXME: tool ? */
     }
   else if (!strcmp(ev->keyname, "g"))
     {
	/* FIXME: tv guide ? */
     }
   else if (!strcmp(ev->keyname, "period"))
     {
	/* FIXME: ch up ? */
     }
   else if (!strcmp(ev->keyname, "comma"))
     {
	/* FIXME: ch down ? */
     }
   else if (!strcmp(ev->keyname, "Tab"))
     {
	/* FIXME: photo ? */
     }
   else if (!strcmp(ev->keyname, "q"))
     {
	/* FIXME: repeat ? */
     }
   else if (!strcmp(ev->keyname, "w"))
     {
	/* FIXME: rotate ? (zoom mode)  */
     }
   else if (!strcmp(ev->keyname, "n"))
     {
	/* FIXME: play info display toggle */
     }
}


/***/
static int
video_menu_bg_hide_tmer_cb(void *data)
{
   background_hide();
   menu_hide();
   mini_pause_set(1);
   _hide_timer = NULL;
   return 0;
}

static void
video_resize(void)
{
   Evas_Coord w, h;
   int iw, ih;
   double ratio;

   emotion_object_size_get(o_video, &iw, &ih);
   if ((iw == 0) || (ih == 0)) return;
   ratio = emotion_object_ratio_get(o_video);
   if (ratio > 0.0) iw = (ih * ratio) + 0.5;
   else ratio = (double)iw / (double)ih;
   w = 10240 * ratio;
   h = 10240;
   // fit so there is no blank space
//   edje_extern_object_aspect_set(o_video, EDJE_ASPECT_CONTROL_NEITHER, w, h);
   // fit and pad with blank
   edje_extern_object_aspect_set(o_video, EDJE_ASPECT_CONTROL_BOTH, w, h);
   edje_object_part_swallow(o_video_bg, "video", o_video);
}

static int
video_jump_reset_timer_cb(void *data)
{
   if (jump > 0.0) jump = 10.0;
   else if (jump < 0.0) jump = -10.0;
   jump_reset_timer = NULL;
   return 0;
};

static void
video_obj_frame_decode_cb(void *data, Evas_Object *obj, void *event_info)
{
   double pos, len;
   char buf[256];
   int ph, pm, ps, pf, lh, lm, ls;

   pos = emotion_object_position_get(obj);
   len = emotion_object_play_length_get(obj);
   lh = len / 3600;
   lm = len / 60 - (lh * 60);
   ls = len - ((lh * 3600) + (lm * 60));
   ph = pos / 3600;
   pm = pos / 60 - (ph * 60);
   ps = pos - ((ph * 3600) + (pm * 60));
   pf = pos * 100 - (ps * 100) - (pm * 60 * 100) - (ph * 60 * 60 * 100);
   snprintf(buf, sizeof(buf), "%i:%02i:%02i.%02i / %i:%02i:%02i",
	    ph, pm, ps, pf, lh, lm, ls);
   edje_object_part_text_set(o_video_bg, "position", buf);
}

static void
video_obj_frame_resize_cb(void *data, Evas_Object *obj, void *event_info)
{
   video_resize();
}

static void
video_obj_length_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   double pos, len;
   char buf[256];
   int ph, pm, ps, pf, lh, lm, ls;

   pos = emotion_object_position_get(obj);
   len = emotion_object_play_length_get(obj);
   lh = len / 3600;
   lm = len / 60 - (lh * 60);
   ls = len - (lm * 60);
   ph = pos / 3600;
   pm = pos / 60 - (ph * 60);
   ps = pos - (pm * 60);
   pf = pos * 100 - (ps * 100) - (pm * 60 * 100) - (ph * 60 * 60 * 100);
   snprintf(buf, sizeof(buf), "%i:%02i:%02i.%02i / %i:%02i:%02i",
	    ph, pm, ps, pf, lh, lm, ls);
   edje_object_part_text_set(o_video_bg, "position", buf);
   edje_object_signal_emit(o_video_bg, "active", "");
}

static void
video_obj_stopped_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("video stopped!\n"); 
   if (!video_stopped_job)
     video_stopped_job = ecore_job_add(video_stopped_job_cb, data);
}

static void
video_obj_channels_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("channels changed: [AUD %i][VID %i][SPU %i]\n",
	  emotion_object_audio_channel_count(obj),
	  emotion_object_video_channel_count(obj),
	  emotion_object_spu_channel_count(obj));
}

static void
video_obj_title_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("video title to: \"%s\"\n", emotion_object_title_get(obj));
}

static void
video_obj_progress_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("progress: \"%s\" %3.3f\n", 
	  emotion_object_progress_info_get(obj),
	  emotion_object_progress_status_get(obj));
}

static void
video_obj_ref_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("video ref to: \"%s\" %i\n",
	  emotion_object_ref_file_get(obj),
	  emotion_object_ref_num_get(obj));
}

static void
video_obj_button_num_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("video spu buttons to: %i\n",
	  emotion_object_spu_button_count_get(obj));
}

static void
video_obj_button_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("video selected spu button: %i\n",
	  emotion_object_spu_button_get(obj));
}

static void
video_stopped_job_cb(void *data)
{
   main_mode_pop();
   video_shutdown();
}
