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
   Evas_Object *overlay;
   
   Ecore_Timer *media_play_timer;
   
   int           media_type;
   unsigned char have_media : 1;
};

static void shutdown(Elation_Module *em);
static void resize(Elation_Module *em);
static void show(Elation_Module *em);
static void hide(Elation_Module *em);
static void focus(Elation_Module *em);
static void unfocus(Elation_Module *em);
static void action(Elation_Module *em, int action);

static int  media_play_timer_cb(void *data);

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

   pr->overlay = edje_object_add(em->info->evas);
   edje_object_file_set(pr->overlay, PACKAGE_DATA_DIR"/data/theme.edj", "media");
   edje_object_signal_emit(pr->overlay, "media", "0");
   
   return pr;
}

static void
shutdown(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_del(pr->overlay);
   if (pr->media_play_timer) ecore_timer_del(pr->media_play_timer);
   free(pr);
}

static void
resize(Elation_Module *em)
{
   Elation_Module_Private *pr;
   Evas_Coord w, h;
   
   pr = em->data;
   evas_output_viewport_get(em->info->evas, NULL, NULL, &w, &h);
   
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
   evas_object_focus_set(pr->overlay, 1);
}

static void
unfocus(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_focus_set(pr->overlay, 0);
}
   
static void
action(Elation_Module *em, int action)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   switch (action)
     {
      case ELATION_ACT_DISK_IN:
	if (!pr->have_media)
	  {
	     printf("disk in\n");
	     pr->have_media = 1;
	  }
	break;
      case ELATION_ACT_DISK_TYPE_DVD:
	if (pr->have_media)
	  {
	     printf("disk type: dvd\n");
	     pr->media_type = action;
	     edje_object_signal_emit(pr->overlay, "media", "1");
	     pr->media_play_timer = ecore_timer_add(2.0, media_play_timer_cb, em);
	  }
	break;
      case ELATION_ACT_DISK_TYPE_VCD:
	if (pr->have_media)
	  {
	     printf("disk type: vcd\n");
	     pr->media_type = action;
	     edje_object_signal_emit(pr->overlay, "media", "1");
	     pr->media_play_timer = ecore_timer_add(2.0, media_play_timer_cb, em);
	  }
	break;
      case ELATION_ACT_DISK_TYPE_SVCD:
	if (pr->have_media)
	  {
	     printf("disk type: svcd\n");
	     pr->media_type = action;
	     edje_object_signal_emit(pr->overlay, "media", "1");
	     pr->media_play_timer = ecore_timer_add(2.0, media_play_timer_cb, em);
	  }
	break;
      case ELATION_ACT_DISK_TYPE_AUDIO:
	if (pr->have_media)
	  {
	     printf("disk type: audio\n");
	     pr->media_type = action;
	     edje_object_signal_emit(pr->overlay, "media", "1");
	     pr->media_play_timer = ecore_timer_add(2.0, media_play_timer_cb, em);
	  }
	break;
      case ELATION_ACT_DISK_OUT:
	if (pr->have_media)
	  {
	     printf("disk out\n");
	     edje_object_signal_emit(pr->overlay, "media", "0");
	     pr->have_media = 0;
	  }
	break;
      case ELATION_ACT_NONE:
      default:
	break;
     }
}

/*** private stuff ***/
static int
media_play_timer_cb(void *data)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   
   em = data;
   pr = em->data;
   pr->media_play_timer = NULL;
   if (!pr->have_media) return 0;
   
   edje_object_signal_emit(pr->overlay, "media", "ok");
   em->resize(em);

   if ((pr->have_media) && (pr->media_type == ELATION_ACT_DISK_TYPE_DVD))
     {
	Elation_Module *new_em;
	
	new_em = em->info->func.module_open(em->info, em, "dvd");
	evas_object_raise(pr->overlay);
	if (new_em)
	  {
	     new_em->resize(new_em);
	     new_em->show(new_em);
	     new_em->focus(new_em);
	  }
     }
   else if ((pr->have_media) && (pr->media_type == ELATION_ACT_DISK_TYPE_VCD))
     {
	Elation_Module *new_em;
	
	new_em = em->info->func.module_open(em->info, em, "vcd");
	evas_object_raise(pr->overlay);
	if (new_em)
	  {
	     new_em->resize(new_em);
	     new_em->show(new_em);
	     new_em->focus(new_em);
	  }
     }
   else if ((pr->have_media) && (pr->media_type == ELATION_ACT_DISK_TYPE_SVCD))
     {
	Elation_Module *new_em;
	
	new_em = em->info->func.module_open(em->info, em, "vcd");
	evas_object_raise(pr->overlay);
	if (new_em)
	  {
	     new_em->resize(new_em);
	     new_em->show(new_em);
	     new_em->focus(new_em);
	  }
     }
   else if ((pr->have_media) && (pr->media_type == ELATION_ACT_DISK_TYPE_AUDIO))
     {
	Elation_Module *new_em;
	
	new_em = em->info->func.module_open(em->info, em, "cd");
	evas_object_raise(pr->overlay);
	if (new_em)
	  {
	     new_em->resize(new_em);
	     new_em->show(new_em);
	     new_em->focus(new_em);
	  }
     }
   else
     {
	em->info->func.action_broadcast(ELATION_ACT_DISK_EJECT);
     }
   return 0;
}
