#include "main.h"

static Evas_Object *o_dvb = NULL;
static Evas_Object *o_dvb_bg = NULL;
static double jump = 0.0;
static Ecore_Timer *jump_reset_timer = NULL;
static Ecore_Job *dvb_stopped_job = NULL;
static Ecore_Timer *_hide_timer = NULL;
static int channel = 0;
static char *mdl = NULL;
static char *swal = NULL;

static int dvb_menu_bg_hide_tmer_cb(void *data);
static void dvb_resize(void);
static int dvb_jump_reset_timer_cb(void *data);
static void dvb_obj_frame_decode_cb(void *data, Evas_Object *obj, void *event_info);
static void dvb_obj_frame_resize_cb(void *data, Evas_Object *obj, void *event_info);
static void dvb_obj_length_change_cb(void *data, Evas_Object *obj, void *event_info);
static void dvb_obj_stopped_cb(void *data, Evas_Object *obj, void *event_info);
static void dvb_obj_channels_cb(void *data, Evas_Object *obj, void *event_info);
static void dvb_obj_title_cb(void *data, Evas_Object *obj, void *event_info);
static void dvb_obj_progress_cb(void *data, Evas_Object *obj, void *event_info);
static void dvb_obj_ref_cb(void *data, Evas_Object *obj, void *event_info);
static void dvb_obj_button_num_cb(void *data, Evas_Object *obj, void *event_info);
static void dvb_obj_button_cb(void *data, Evas_Object *obj, void *event_info);
static void dvb_stopped_job_cb(void *data);

void
dvb_init(char *module, char *file, char *swallow)
{
   Evas_Object *o;

   if (o_dvb_bg) return;
   if (mdl) free(mdl);
   mdl = strdup(module);
   if (swal) free(swal);
   swal = strdup(swallow);
   o = edje_object_add(evas);
   edje_object_file_set(o, theme, "video_container");
   o_dvb_bg = o;
   
   o = emotion_object_add(evas); 
   if (!emotion_object_init(o, module))
     {
	printf("ERROR!\n");
     }
   
   evas_object_smart_callback_add(o, "frame_decode", dvb_obj_frame_decode_cb, NULL);
   evas_object_smart_callback_add(o, "frame_resize", dvb_obj_frame_resize_cb, NULL);
   evas_object_smart_callback_add(o, "length_change", dvb_obj_length_change_cb, NULL);
   
   evas_object_smart_callback_add(o, "decode_stop", dvb_obj_stopped_cb, NULL);
   evas_object_smart_callback_add(o, "channels_change", dvb_obj_channels_cb, NULL);
   evas_object_smart_callback_add(o, "title_change", dvb_obj_title_cb, NULL);
   evas_object_smart_callback_add(o, "progress_change", dvb_obj_progress_cb, NULL);
   evas_object_smart_callback_add(o, "ref_change", dvb_obj_ref_cb, NULL);
   evas_object_smart_callback_add(o, "button_num_change", dvb_obj_button_num_cb, NULL);
   evas_object_smart_callback_add(o, "button_change", dvb_obj_button_cb, NULL);
   o_dvb = o;

   channel = 0;
   emotion_object_file_set(o_dvb, "dvb://0");
   emotion_object_play_set(o_dvb, 1);
   emotion_object_audio_mute_set(o_dvb, 0);
   emotion_object_audio_volume_set(o_dvb, 1.0);
   layout_swallow(swallow, o_dvb_bg);

   edje_extern_object_aspect_set(o_dvb, EDJE_ASPECT_CONTROL_BOTH, 640, 480);
   edje_object_part_swallow(o_dvb_bg, "video", o_dvb);

   dvb_resize();
   
   evas_object_show(o_dvb);
   evas_object_show(o_dvb_bg);

   _hide_timer = ecore_timer_add(10.0, dvb_menu_bg_hide_tmer_cb, NULL);
   
   /* FIXME: add this video to recently played list */
}
 
void
dvb_shutdown(void)
{
   if (!o_dvb_bg) return;
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
   
   evas_object_del(o_dvb_bg);
   evas_object_del(o_dvb);
   if (dvb_stopped_job) ecore_job_del(dvb_stopped_job);
   o_dvb = NULL;
   o_dvb_bg = NULL;
   dvb_stopped_job = NULL;
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
dvb_key(Evas_Event_Key_Down *ev)
{
   
   if      (!strcmp(ev->keyname, "bracketleft"))
     {
	edje_object_signal_emit(o_dvb_bg, "active", "");
     }
   else if (!strcmp(ev->keyname, "bracketright"))
     {
	edje_object_signal_emit(o_dvb_bg, "active", "");
     }
   else if (!strcmp(ev->keyname, "p"))
     {
	/* FIXME: play info display end */
	jump = 0.0;
	emotion_object_play_set(o_dvb, 1);
	edje_object_signal_emit(o_dvb_bg, "active", "");
     }
   else if (!strcmp(ev->keyname, "space"))
     {
	/* FIXME: play info display */
	jump = 0.0;
	if (emotion_object_play_get(o_dvb))
	  emotion_object_play_set(o_dvb, 0);
	else
	  emotion_object_play_set(o_dvb, 1);
	edje_object_signal_emit(o_dvb_bg, "active", "");
     }
   else if (!strcmp(ev->keyname, "Up"))
     {
        channel++;
          {
             char buf[256];
             snprintf(buf, sizeof(buf), "dvb://%i", channel);
             emotion_object_file_set(o_dvb, buf);
          }
     }
   else if (!strcmp(ev->keyname, "Down"))
     {
        channel--;
        if (channel < 0)
          {
             channel = 0;
          }
        else
          {
             char buf[256];
             snprintf(buf, sizeof(buf), "dvb://%i", channel);
             emotion_object_file_set(o_dvb, buf);
          }
     }
   else if (!strcmp(ev->keyname, "Left"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_LEFT);
   else if (!strcmp(ev->keyname, "Right"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_RIGHT);
   else if (!strcmp(ev->keyname, "Return"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_SELECT);
   else if (!strcmp(ev->keyname, "n"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_MENU1);
   else if (!strcmp(ev->keyname, "Prior"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_PREV);
   else if (!strcmp(ev->keyname, "Next"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_NEXT);
   else if (!strcmp(ev->keyname, "0"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_0);
   else if (!strcmp(ev->keyname, "1"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_1);
   else if (!strcmp(ev->keyname, "2"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_2);
   else if (!strcmp(ev->keyname, "3"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_3);
   else if (!strcmp(ev->keyname, "4"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_4);
   else if (!strcmp(ev->keyname, "5"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_5);
   else if (!strcmp(ev->keyname, "6"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_6);
   else if (!strcmp(ev->keyname, "7"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_7);
   else if (!strcmp(ev->keyname, "8"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_8);
   else if (!strcmp(ev->keyname, "9"))
     emotion_object_event_simple_send(o_dvb, EMOTION_EVENT_9);
   else if (!strcmp(ev->keyname, "k"))
     {
	/* FIXME: volume display */
	emotion_object_audio_volume_set(o_dvb, emotion_object_audio_volume_get(o_dvb) - 0.1);
     }
   else if (!strcmp(ev->keyname, "l"))
     {
	/* FIXME: volume display */
	emotion_object_audio_volume_set(o_dvb, emotion_object_audio_volume_get(o_dvb) + 0.1);
     }
   else if (!strcmp(ev->keyname, "m"))
     {
	/* FIXME: volume display */
	if (emotion_object_audio_mute_get(o_dvb))
	  emotion_object_audio_mute_set(o_dvb, 0);
	else
	  emotion_object_audio_mute_set(o_dvb, 1);
     }
   else if (!strcmp(ev->keyname, "s"))
     {
	/* FIXME: save position for this video */
	main_mode_pop();
	jump = 0.0;
	dvb_shutdown();
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
dvb_menu_bg_hide_tmer_cb(void *data)
{
   background_hide();
   menu_hide();
   mini_pause_set(1);
   _hide_timer = NULL;
   return 0;
}

static void
dvb_resize(void)
{
   Evas_Coord w, h;
   int iw, ih;
   double ratio;

   emotion_object_size_get(o_dvb, &iw, &ih);
   if ((iw == 0) || (ih == 0)) return;
   ratio = emotion_object_ratio_get(o_dvb);
   if (ratio > 0.0) iw = (ih * ratio) + 0.5;
   else ratio = (double)iw / (double)ih;
   w = 10240 * ratio;
   h = 10240;
   // fit so there is no blank space
//   edje_extern_object_aspect_set(o_dvb, EDJE_ASPECT_CONTROL_NEITHER, w, h);
   // fit and pad with blank
   edje_extern_object_aspect_set(o_dvb, EDJE_ASPECT_CONTROL_BOTH, w, h);
   edje_object_part_swallow(o_dvb_bg, "video", o_dvb);
}

static int
dvb_jump_reset_timer_cb(void *data)
{
   if (jump > 0.0) jump = 10.0;
   else if (jump < 0.0) jump = -10.0;
   jump_reset_timer = NULL;
   return 0;
};

static void
dvb_obj_frame_decode_cb(void *data, Evas_Object *obj, void *event_info)
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
//   edje_object_part_text_set(o_dvb_bg, "position", buf);
}

static void
dvb_obj_frame_resize_cb(void *data, Evas_Object *obj, void *event_info)
{
   dvb_resize();
}

static void
dvb_obj_length_change_cb(void *data, Evas_Object *obj, void *event_info)
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
//   edje_object_part_text_set(o_dvb_bg, "position", buf);
//   edje_object_signal_emit(o_dvb_bg, "active", "");
}

static void
dvb_obj_stopped_cb(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *o;
   char buf[256];
   
   printf("dvb stopped!\n"); 
   
   o = emotion_object_add(evas_object_evas_get(obj)); 
   if (!emotion_object_init(o, mdl))
     {
	printf("ERROR!\n");
     }

   evas_object_del(o_dvb);
   
   evas_object_smart_callback_add(o, "frame_decode", dvb_obj_frame_decode_cb, NULL);
   evas_object_smart_callback_add(o, "frame_resize", dvb_obj_frame_resize_cb, NULL);
   evas_object_smart_callback_add(o, "length_change", dvb_obj_length_change_cb, NULL);
   
   evas_object_smart_callback_add(o, "decode_stop", dvb_obj_stopped_cb, NULL);
   evas_object_smart_callback_add(o, "channels_change", dvb_obj_channels_cb, NULL);
   evas_object_smart_callback_add(o, "title_change", dvb_obj_title_cb, NULL);
   evas_object_smart_callback_add(o, "progress_change", dvb_obj_progress_cb, NULL);
   evas_object_smart_callback_add(o, "ref_change", dvb_obj_ref_cb, NULL);
   evas_object_smart_callback_add(o, "button_num_change", dvb_obj_button_num_cb, NULL);
   evas_object_smart_callback_add(o, "button_change", dvb_obj_button_cb, NULL);
   o_dvb = o;

   snprintf(buf, sizeof(buf), "dvb://%i", channel);
   emotion_object_file_set(o_dvb, buf);
   emotion_object_play_set(o_dvb, 1);
   emotion_object_audio_mute_set(o_dvb, 0);
   emotion_object_audio_volume_set(o_dvb, 1.0);
   layout_swallow(swal, o_dvb_bg);

   edje_extern_object_aspect_set(o_dvb, EDJE_ASPECT_CONTROL_BOTH, 640, 480);
   edje_object_part_swallow(o_dvb_bg, "video", o_dvb);

   dvb_resize();
   
   evas_object_show(o_dvb);
//   if (!dvb_stopped_job)
//     dvb_stopped_job = ecore_job_add(dvb_stopped_job_cb, data);
}

static void
dvb_obj_channels_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("channels changed: [AUD %i][VID %i][SPU %i]\n",
	  emotion_object_audio_channel_count(obj),
	  emotion_object_video_channel_count(obj),
	  emotion_object_spu_channel_count(obj));
}

static void
dvb_obj_title_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("dvb title to: \"%s\"\n", emotion_object_title_get(obj));
   edje_object_part_text_set(o_dvb_bg, "position", emotion_object_title_get(obj));
   edje_object_signal_emit(o_dvb_bg, "active", "");
}

static void
dvb_obj_progress_cb(void *data, Evas_Object *obj, void *event_info)
{
   char buf[256];
   printf("progress: \"%s\" %3.3f\n", 
	  emotion_object_progress_info_get(obj),
	  emotion_object_progress_status_get(obj));
}

static void
dvb_obj_ref_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("dvb ref to: \"%s\" %i\n",
	  emotion_object_ref_file_get(obj),
	  emotion_object_ref_num_get(obj));
}

static void
dvb_obj_button_num_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("dvb spu buttons to: %i\n",
	  emotion_object_spu_button_count_get(obj));
}

static void
dvb_obj_button_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("dvb selected spu button: %i\n",
	  emotion_object_spu_button_get(obj));
}

static void
dvb_stopped_job_cb(void *data)
{
   main_mode_pop();
   dvb_shutdown();
}
