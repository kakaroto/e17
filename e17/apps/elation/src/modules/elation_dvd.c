#include "Elation.h"

#include <dvdnav/dvdnav.h>
#include <dvdnav/nav_read.h>

/* external module symbols. the rest is private */
void *init(Elation_Module *em);

typedef struct _Elation_Module_Private Elation_Module_Private;

struct _Elation_Module_Private
{
   Evas_Object *video;
};

static void shutdown(Elation_Module *em);
static void resize(Elation_Module *em);
static void hide(Elation_Module *em);
static void show(Elation_Module *em);

static void frame_decode_cb(void *data, Evas_Object *obj, void *event_info);
static void frame_resize_cb(void *data, Evas_Object *obj, void *event_info);
static void length_change_cb(void *data, Evas_Object *obj, void *event_info);

void *
init(Elation_Module *em)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   Evas_Coord w, h;
   
   em = calloc(1, sizeof(Elation_Module));
   if (!em) return NULL;
   em->info = ei;
   pr = calloc(1, sizeof(Elation_Module_Private));
   if (!pr)
     {
	free(em);
	return NULL;
     }
   em->data = pr;
   
   evas_viewport_get(em->info->evas, NULL, NULL, &w, &h);
   pr->video = elation_object_add(em->info->evas);

   evas_object_smart_callback_add(pr->video, "frame_decode", frame_decode_cb, em);
   evas_object_smart_callback_add(pr->video, "frame_resize", frame_resize_cb, em);
   evas_object_smart_callback_add(pr->video, "length_change",length_change_cb, em);
   
   elation_object_file_set(pr->video, "dvd://");
   evas_object_move(pr->video, 0, 0);
   evas_object_resize(pr->video, w, h);
   elation_object_smooth_scale_set(pr->video, 1);
   elation_object_play_set(pr->video, 1);
   
   return em;
}

static void
shutdown(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_del(pr->video);
   free(pr);
   free(em);
}

static void
resize(Elation_Module *em)
{
   Elation_Module_Private *pr;
   Evas_Coord w, h;
   
   pr = em->data;
   evas_viewport_get(em->info->evas, NULL, NULL, &w, &h);
   evas_object_move(pr->video, 0, 0);
   evas_object_resize(pr->video, w, h);
}

static void
hide(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_hide(pr->video);
}

static void
show(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   evas_object_show(pr->video);
}

static void
dvd_info_get(Elation_Module *em)
{
   dvdnav_t *dvd;
   int title_num = 0;
   int i;
   char *str = NULL;
   
   if (dvdnav_open(&dvd, "/dev/dvd") == DVDNAV_STATUS_ERR) return;
   dvdnav_get_title_string(dvd, &str);
   printf("TITLE: %s\n", str);
   // strdup str as its an internal string in the dvd handle
   dvdnav_get_number_of_titles(dvd, &title_num);
   for (i = 0; i < title_num; i++)
     {
	int part_num;
	
	dvdnav_get_number_of_parts(dvd, i, &part_num);
     }
   dvdnav_close(dvd);
}

static void
frame_decode_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   double pos, len;
   
   em = data;
   pos = elation_object_position_get(obj);
   len = elation_object_play_length_get(obj);
}

static void
frame_resize_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   int iw, ih;
   double ratio;
   
   em = data;
   elation_object_size_get(obj, &iw, &ih);
   ratio = elation_object_ratio_get(obj);
   if (ratio > 0.0) iw = ih * ratio;
}

static void
length_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elation_Module *em;
   double pos, len;
                
   em = data;
   pos = elation_object_position_get(obj);
   len = elation_object_play_length_get(obj);
}
