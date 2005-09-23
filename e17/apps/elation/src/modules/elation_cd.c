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
   char *device;
   int track;
   int track_num;
   Evas_Object *video;
   Evas_Object *overlay;
};

static void shutdown(Elation_Module *em);
static void resize(Elation_Module *em);
static void show(Elation_Module *em);
static void hide(Elation_Module *em);
static void focus(Elation_Module *em);
static void unfocus(Elation_Module *em);
static void action(Elation_Module *em, int action);

static void frame_decode_cb(void *data, Evas_Object *obj, void *event_info);
static void length_change_cb(void *data, Evas_Object *obj, void *event_info);
static void decode_stop_cb(void *data, Evas_Object *obj, void *event_info);
static void title_change_cb(void *data, Evas_Object *obj, void *event_info);
static void progress_change_cb(void *data, Evas_Object *obj, void *event_info);
static void channels_change_cb(void *data, Evas_Object *obj, void *event_info);
static void key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void new_track(Elation_Module *em, int track);
    
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
   
   em->data = pr;

   pr->device = strdup("/dev/cdrom");
   
   pr->video = emotion_object_add(em->info->evas);
   evas_object_event_callback_add(pr->video, EVAS_CALLBACK_KEY_DOWN, key_down_cb, em);
   
   evas_object_smart_callback_add(pr->video, "frame_decode", frame_decode_cb, em);
   evas_object_smart_callback_add(pr->video, "length_change",length_change_cb, em);
   evas_object_smart_callback_add(pr->video, "decode_stop", decode_stop_cb, em);
   
   evas_object_smart_callback_add(pr->video, "title_change", title_change_cb, em);
   evas_object_smart_callback_add(pr->video, "progress_change", progress_change_cb, em);
   evas_object_smart_callback_add(pr->video, "channels_change", channels_change_cb, em);

     {
	int fd;
	
	fd = open(pr->device, O_RDONLY | O_NONBLOCK);
	if (fd >= 0)
	  {
	     int ret;
	     struct cdrom_tochdr hd;
	     
	     ret = ioctl(fd, CDROMREADTOCHDR, &hd);
	     if (ret == 0)
	       {
		  int i;
		  
		  pr->track_num = hd.cdth_trk1;
		  printf("hd->cdth_trk0 = %i, hd->cdth_trk1 = %i\n",
			 (int)hd.cdth_trk0, (int)hd.cdth_trk1);
		  for (i = 1; i <= hd.cdth_trk1; i++)
		    {
		       struct cdrom_tocentry ent;
		       
		       ent.cdte_track = i;
		       ent.cdte_format = CDROM_MSF;
		       ret = ioctl(fd, CDROMREADTOCENTRY, &ent);
		       if (ret == 0)
			 {
			    int sec;
			    
			    sec = ent.cdte_addr.msf.minute * 60;
			    sec += ent.cdte_addr.msf.second;
			    printf("TRACK %i = %i sec [f: %i]\n", i, sec,
				   (int)ent.cdte_addr.msf.frame);
			 }
		    }
	       }
	     close(fd);
	  }
     }
   
   pr->overlay = edje_object_add(em->info->evas);
   edje_object_file_set(pr->overlay, PACKAGE_DATA_DIR"/data/theme.edj", "cd");
   edje_object_signal_emit(pr->overlay, "media", "1");
			   
   new_track(em, 0);
   return pr;
}

static void
shutdown(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_del(pr->video);
   evas_object_del(pr->overlay);
   free(pr);
}

static void
resize(Elation_Module *em)
{
   Elation_Module_Private *pr;
   Evas_Coord w, h;
   
   pr = em->data;
   evas_output_viewport_get(em->info->evas, NULL, NULL, &w, &h);
   
   evas_object_move(pr->video, 0, 0);
   evas_object_resize(pr->video, w, h);
   
   evas_object_move(pr->overlay, 0, 0);
   evas_object_resize(pr->overlay, w, h);
}

static void
show(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_show(pr->overlay);
}

static void
hide(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_hide(pr->overlay);
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
	new_track(em, pr->track + 1);
	break;
      case ELATION_ACT_PREV:
	new_track(em, pr->track - 1);
	break;
      case ELATION_ACT_SELECT:
	if (emotion_object_play_get(pr->video))
	  emotion_object_play_set(pr->video, 0);
	else
	  emotion_object_play_set(pr->video, 1);
	break;
      case ELATION_ACT_EXIT:
	em->info->func.action_broadcast(ELATION_ACT_DISK_EJECT);
	em->shutdown(em);
	break;
      case ELATION_ACT_UP:
	break;
      case ELATION_ACT_DOWN:
	break;
      case ELATION_ACT_LEFT:
	  {
	     double pos;
	     
	     pos = emotion_object_position_get(pr->video);
	     emotion_object_position_set(pr->video, pos - 30.0);
	  }
	break;
      case ELATION_ACT_RIGHT:
	  {
	     double pos;
	     
	     pos = emotion_object_position_get(pr->video);
	     emotion_object_position_set(pr->video, pos + 30.0);
	  }
	break;
      case ELATION_ACT_MENU:
	break;
      case ELATION_ACT_INFO:
	break;
      case ELATION_ACT_INPUT:
	break;
      case ELATION_ACT_PLAY:
	emotion_object_play_set(pr->video, 1);
	break;
      case ELATION_ACT_PAUSE:
	if (emotion_object_play_get(pr->video))
	  emotion_object_play_set(pr->video, 0);
	else
	  emotion_object_play_set(pr->video, 1);
	break;
      case ELATION_ACT_STOP:
	emotion_object_play_set(pr->video, 0);
	emotion_object_position_set(pr->video, 0.0);
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
//   printf("EL decode %3.3f / %3.3f\n", pos, len);
}

static void
length_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   double pos, len;
                
   em = data;
   pos = emotion_object_position_get(obj);
   len = emotion_object_play_length_get(obj);
   printf("EL length change %3.3f / %3.3f\n", pos, len);
}

static void
decode_stop_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   
   em = data;
   pr = em->data;
   printf("EL video stop\n");
//   emotion_object_position_set(pr->video, 0.0);
//   emotion_object_play_set(pr->video, 1);
   new_track(em, pr->track + 1);
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

static void
new_track(Elation_Module *em, int track)
{
   Elation_Module_Private *pr;
   char buf[256];
   
   pr = em->data;
   pr->track = track;
   while (pr->track >= pr->track_num) pr->track -= pr->track_num;
   while (pr->track < 0) pr->track += pr->track_num;
   
   snprintf(buf, sizeof(buf), "cdda:/%i", pr->track + 1);
   emotion_object_file_set(pr->video, buf);
   emotion_object_play_set(pr->video, 1);
   snprintf(buf, sizeof(buf), "%i", pr->track + 1);
   edje_object_part_text_set(pr->overlay,
			     "track_number",
			     buf);
   edje_object_part_text_set(pr->overlay,
			     "track_title",
			     emotion_object_meta_info_get(pr->video, EMOTION_META_INFO_TRACK_TITLE));
   edje_object_part_text_set(pr->overlay,
			     "track_artist",
			     emotion_object_meta_info_get(pr->video, EMOTION_META_INFO_TRACK_ARTIST));
   edje_object_part_text_set(pr->overlay,
			     "track_album",
			     emotion_object_meta_info_get(pr->video, EMOTION_META_INFO_TRACK_ALBUM));
   printf("track %i, %s - %s - %s\n", 
	  pr->track,
	  emotion_object_meta_info_get(pr->video, EMOTION_META_INFO_TRACK_ARTIST),
	  emotion_object_meta_info_get(pr->video, EMOTION_META_INFO_TRACK_ALBUM),
	  emotion_object_meta_info_get(pr->video, EMOTION_META_INFO_TRACK_TITLE)
	  );
}
