#include "Elation.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>

/* external module symbols. the rest is private */
void *init(Elation_Module *em);

/********************/
typedef struct _Elation_Module_Private Elation_Module_Private;

struct _Elation_Module_Private
{
   Evas_Object *background1;
   Evas_Object *background2;
   Evas_Object *video;
   
   double       media_fade_in_start;
   Ecore_Timer *media_fade_in_timer;
   
   unsigned char menu_visible : 1;
};

static void shutdown(Elation_Module *em);
static void resize(Elation_Module *em);
static void show(Elation_Module *em);
static void hide(Elation_Module *em);
static void focus(Elation_Module *em);
static void unfocus(Elation_Module *em);
static void action(Elation_Module *em, int action);

static void frame_decode_cb(void *data, Evas_Object *obj, void *event_info);
static void frame_resize_cb(void *data, Evas_Object *obj, void *event_info);
static void length_change_cb(void *data, Evas_Object *obj, void *event_info);
static void decode_stop_cb(void *data, Evas_Object *obj, void *event_info);
static void button_num_change_cb(void *data, Evas_Object *obj, void *event_info);
static void title_change_cb(void *data, Evas_Object *obj, void *event_info);
static void progress_change_cb(void *data, Evas_Object *obj, void *event_info);
static void channels_change_cb(void *data, Evas_Object *obj, void *event_info);
static void ref_change_cb(void *data, Evas_Object *obj, void *event_info);
static void button_change_cb(void *data, Evas_Object *obj, void *event_info);
static void key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static int  media_play_timer_cb(void *data);
static int  media_fade_in_timer_cb(void *data);

void *
init(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = calloc(1, sizeof(Elation_Module_Private));
   if (!pr) return NULL;
   
   /* set up module methods */
   em->shutdown = shutdown;
   em->resize = resize;
   em->show = show;
   em->hide = hide;
   em->focus = focus;
   em->unfocus = unfocus;
   em->action = action;

   pr->background1 = evas_object_rectangle_add(em->info->evas);
   evas_object_color_set(pr->background1, 0, 0, 0, 255);
   pr->background2 = evas_object_rectangle_add(em->info->evas);
   evas_object_color_set(pr->background2, 0, 0, 0, 255);
   
   pr->video = emotion_object_add(em->info->evas);
   evas_object_event_callback_add(pr->video, EVAS_CALLBACK_KEY_DOWN, key_down_cb, em);
   
   evas_object_smart_callback_add(pr->video, "frame_decode", frame_decode_cb, em);
   evas_object_smart_callback_add(pr->video, "frame_resize", frame_resize_cb, em);
   evas_object_smart_callback_add(pr->video, "length_change",length_change_cb, em);
   evas_object_smart_callback_add(pr->video, "decode_stop", decode_stop_cb, em);
   evas_object_smart_callback_add(pr->video, "button_num_change", button_num_change_cb, em);
   
   evas_object_smart_callback_add(pr->video, "title_change", title_change_cb, em);
   evas_object_smart_callback_add(pr->video, "progress_change", progress_change_cb, em);
   evas_object_smart_callback_add(pr->video, "channels_change", channels_change_cb, em);
   evas_object_smart_callback_add(pr->video, "ref_change", ref_change_cb, em);
   evas_object_smart_callback_add(pr->video, "button_change", button_change_cb, em);
   
   emotion_object_smooth_scale_set(pr->video, 1);
   
   emotion_object_file_set(pr->video, "vcd://");
   emotion_object_play_set(pr->video, 1);
   
   pr->media_fade_in_timer = ecore_timer_add(1.0 / 30.0, media_fade_in_timer_cb, em);
   pr->media_fade_in_start = ecore_time_get();
   
   return pr;
}

static void
shutdown(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_del(pr->background1);
   evas_object_del(pr->background2);
   evas_object_del(pr->video);
   if (pr->media_fade_in_timer) ecore_timer_del(pr->media_fade_in_timer);
   free(pr);
}

static void
resize(Elation_Module *em)
{
   Elation_Module_Private *pr;
   Evas_Coord x, y, w, h, ww, hh;
   double ratio;
   int vw, vh;
   
   pr = em->data;
   evas_output_viewport_get(em->info->evas, NULL, NULL, &w, &h);
   
   ww = w;
   hh = h;
   
   emotion_object_size_get(pr->video, &vw, &vh);
   ratio = emotion_object_ratio_get(pr->video);
   if (ratio > 0.0)
     {
	x = 0;
	y = (h - (w / ratio)) / 2;
	if (y < 0)
	  {
	     y = 0;
	     x = (w - (h * ratio)) / 2;
	     w = h * ratio;
	  }
	else
	  h = w / ratio;
	evas_object_move(pr->video, x, y);
	evas_object_resize(pr->video, w, h);
     }
   else
     {
	if (vh > 1)
	  {
	     ratio = (double)vw / (double)vh;
	     x = 0;
	     y = (h - (w / ratio)) / 2;
	     if (y < 0)
	       {
		  y = 0;
		  x = (w - (h * ratio)) / 2;
		  w = h * ratio;
	       }
	     else
	       h = w / ratio;
	  }
	evas_object_move(pr->video, 0, 0);
	evas_object_resize(pr->video, w, h);
     }
   
   if (w == ww)
     {
	evas_object_move(pr->background1, 0, 0);
	evas_object_resize(pr->background1, ww, y);
	evas_object_move(pr->background2, 0, y + h);
	evas_object_resize(pr->background2, ww, hh - h - y);
     }
   else
     {
	evas_object_move(pr->background1, 0, 0);
	evas_object_resize(pr->background1, x, hh);
	evas_object_move(pr->background2, x + w, 0);
	evas_object_resize(pr->background2, ww - w - x, hh);
     }
}

static void
show(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_show(pr->background1);
   evas_object_show(pr->background2);
   evas_object_show(pr->video);
}

static void
hide(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_hide(pr->background1);
   evas_object_hide(pr->background2);
   evas_object_hide(pr->video);
}

static void
focus(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_focus_set(pr->video, 1);
}

static void
unfocus(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_focus_set(pr->video, 0);
}
   
static void
action(Elation_Module *em, int action)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   switch (action)
     {
      case ELATION_ACT_NEXT:
	if (pr->menu_visible)
	  {
	  }
	else
	  emotion_object_event_simple_send(pr->video, EMOTION_EVENT_NEXT);
	break;
      case ELATION_ACT_PREV:
	if (pr->menu_visible)
	  {
	  }
	else
	  emotion_object_event_simple_send(pr->video, EMOTION_EVENT_PREV);
	break;
      case ELATION_ACT_SELECT:
	if (pr->menu_visible)
	  emotion_object_event_simple_send(pr->video, EMOTION_EVENT_SELECT);
	else
	  {
	     if (emotion_object_play_get(pr->video))
	       emotion_object_play_set(pr->video, 0);
	     else
	       emotion_object_play_set(pr->video, 1);
	  }
	break;
      case ELATION_ACT_EXIT:
	  {
	     em->info->func.action_broadcast(ELATION_ACT_DISK_EJECT);
	     em->shutdown(em);
	  }
	break;
      case ELATION_ACT_UP:
	if (pr->menu_visible)
	  emotion_object_event_simple_send(pr->video, EMOTION_EVENT_UP);
	else
	  {
	     // FIXME: bring up config menu
	  }
	break;
      case ELATION_ACT_DOWN:
	if (pr->menu_visible)
	  emotion_object_event_simple_send(pr->video, EMOTION_EVENT_DOWN);
	else
	  {
	     // FIXME: bring up config menu
	  }
	break;
      case ELATION_ACT_LEFT:
	if (pr->menu_visible)
	  emotion_object_event_simple_send(pr->video, EMOTION_EVENT_LEFT);
	else
	  {
	     double pos;
	     
	     pos = emotion_object_position_get(pr->video);
	     emotion_object_position_set(pr->video, pos - 30.0);
	  }
	break;
      case ELATION_ACT_RIGHT:
	if (pr->menu_visible)
	  emotion_object_event_simple_send(pr->video, EMOTION_EVENT_RIGHT);
	else
	  {
	     double pos;
	     
	     pos = emotion_object_position_get(pr->video);
	     emotion_object_position_set(pr->video, pos + 30.0);
	  }
	break;
      case ELATION_ACT_MENU:
	if (pr->menu_visible)
	  {
	     // FIXME: bring up config menu
	  }
	else
	  emotion_object_event_simple_send(pr->video, EMOTION_EVENT_MENU1);
	break;
      case ELATION_ACT_INFO:
	if (pr->menu_visible)
	  {
	     // FIXME: bring up config menu
	  }
	else
	  {
	  }
	break;
      case ELATION_ACT_INPUT:
	if (pr->menu_visible)
	  {
	     // FIXME: bring up config menu
	  }
	else
	  {
	  }
	break;
      case ELATION_ACT_PLAY:
	if (pr->menu_visible)
	  {
	     emotion_object_event_simple_send(pr->video, EMOTION_EVENT_NEXT);
	  }
	else
	  {
	     emotion_object_play_set(pr->video, 1);
	  }
	break;
      case ELATION_ACT_PAUSE:
	if (pr->menu_visible)
	  {
	  }
	else
	  {
	     if (emotion_object_play_get(pr->video))
	       emotion_object_play_set(pr->video, 0);
	     else
	       emotion_object_play_set(pr->video, 1);
	  }
	break;
      case ELATION_ACT_STOP:
	if (pr->menu_visible)
	  {
	  }
	else
	  {
	     emotion_object_play_set(pr->video, 0);
	     emotion_object_position_set(pr->video, 0.0);
	  }
	break;
      default:
	break;
     }
}

/*** private stuff ***/

static void
frame_decode_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   double pos, len;
   
   em = data;
   pos = emotion_object_position_get(obj);
   len = emotion_object_play_length_get(obj);
}

static void
frame_resize_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   
   em = data;
   em->resize(em);
}

static void
length_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   double pos, len;
                
   em = data;
   pos = emotion_object_position_get(obj);
   len = emotion_object_play_length_get(obj);
}

static void
decode_stop_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   
   em = data;
   pr = em->data;
//   emotion_object_position_set(pr->video, 0.0);
//   emotion_object_play_set(pr->video, 1);
}
   
static void
button_num_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   
   em = data;
   pr = em->data;
   if (emotion_object_spu_button_count_get(pr->video) > 0)
     pr->menu_visible = 1;
   else
     pr->menu_visible = 0;
}

static void
title_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   
   em = data;
   pr = em->data;
   printf("EL video title to: \"%s\"\n", emotion_object_title_get(pr->video));
}

static void
progress_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   
   em = data;
   pr = em->data;
   printf("EL progress: \"%s\" %3.3f\n",
	  emotion_object_progress_info_get(pr->video),
	  emotion_object_progress_status_get(pr->video));
}

static void
channels_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   
   em = data;
   pr = em->data;
   printf("EL channels changed: [AUD %i][VID %i][SPU %i]\n",
	  emotion_object_audio_channel_count(pr->video),
	  emotion_object_video_channel_count(pr->video),
	  emotion_object_spu_channel_count(pr->video));
}

static void
ref_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   
   em = data;
   pr = em->data;
   printf("EL video ref to: \"%s\" %i\n",
	  emotion_object_ref_file_get(pr->video),
	  emotion_object_ref_num_get(pr->video));
}

static void
button_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   
   em = data;
   pr = em->data;
   printf("EL video selected spu button: %i\n",
	  emotion_object_spu_button_get(pr->video));
}

static void
key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev;
   Elation_Module *em;
   Elation_Module_Private *pr;
   int action = ELATION_ACT_NONE;
   
   ev = (Evas_Event_Key_Down *)event_info;
   em = data;
   pr = em->data;
   
   /* translator */
   /* FIXME: create proper translator sys later to amke remote config easy */
   
   if (!strcmp(ev->keyname, "Escape"))      action = ELATION_ACT_EXIT;
   else if (!strcmp(ev->keyname, "Up"))     action = ELATION_ACT_UP;
   else if (!strcmp(ev->keyname, "Down"))   action = ELATION_ACT_DOWN;
   else if (!strcmp(ev->keyname, "Left"))   action = ELATION_ACT_LEFT;
   else if (!strcmp(ev->keyname, "Right"))  action = ELATION_ACT_RIGHT;
   else if (!strcmp(ev->keyname, "Return")) action = ELATION_ACT_SELECT;
   else if (!strcmp(ev->keyname, "Prior"))  action = ELATION_ACT_PREV;
   else if (!strcmp(ev->keyname, "Next"))   action = ELATION_ACT_NEXT;
   else if (!strcmp(ev->keyname, "m"))      action = ELATION_ACT_MENU;
   else if (!strcmp(ev->keyname, "i"))      action = ELATION_ACT_INFO;
   else if (!strcmp(ev->keyname, "o"))      action = ELATION_ACT_INPUT;
   else if (!strcmp(ev->keyname, "p"))      action = ELATION_ACT_PLAY;
   else if (!strcmp(ev->keyname, "a"))      action = ELATION_ACT_PAUSE;
   else if (!strcmp(ev->keyname, "s"))      action = ELATION_ACT_STOP;
   else if (!strcmp(ev->keyname, "k"))      action = ELATION_ACT_SKIP;
   em->action(em, action);
}

static int
media_fade_in_timer_cb(void *data)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   int a;
   double t;
   
   em = data;
   pr = em->data;
   t = ecore_time_get() - pr->media_fade_in_start;
   a = (t * 255) / 10.0;
   if (a > 255) a = 255;
   evas_object_color_set(pr->background1, 0, 0, 0, a);
   evas_object_color_set(pr->background2, 0, 0, 0, a);
   evas_object_color_set(pr->video, 255, 255, 255, a);
   if (a < 255) return 1;
   pr->media_fade_in_timer = NULL;
   return 0;
}
