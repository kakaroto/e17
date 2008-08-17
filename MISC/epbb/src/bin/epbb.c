/***************************************************************************
 * Corey Donohoe : http://atmos.org/
 * File: epbb.c
 * Date: January 21, 2004
 *
 * The edje + ecore glue for epbb.  Take different state changes we receive
 * from the pbbuttons ipc subsystem and pipe them into our edje
 **************************************************************************/
#include<Evas.h>
#include<Ecore.h>
#include<Ecore_X.h>
#include<Ecore_Evas.h>
#include<Edje.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<limits.h>
#include "epbb.h"
#include "../config.h"

#define UN(ptr) ptr = 0

static void epbb_init(void);

static void
_window_cb(void *data, Evas_Object * o, const char *sig, const char *src)
{
   Epbb *e = NULL;

   if ((e = (Epbb *) data))
   {
      if (!strcmp(sig, "window,show"))
         ecore_evas_show(e->ee);
      else if (!strcmp(sig, "window,hide"))
         ecore_evas_hide(e->ee);
   }
}

Epbb *
epbb_new(Ecore_Evas * ee)
{
   Evas_Coord w, h;
   char buf[PATH_MAX];
   Epbb *result = NULL;

   if ((result = malloc(sizeof(Epbb))))
   {
      memset(result, 0, sizeof(Epbb));
      result->ee = ee;

      epbb_init();
      result->obj = edje_object_add(ecore_evas_get(ee));
      snprintf(buf, PATH_MAX, "%s/.e/apps/epbb/fonts", getenv("HOME"));
      evas_font_path_append(ecore_evas_get(ee), buf);

      snprintf(buf, PATH_MAX, "%s/.e/apps/epbb/epbb.eet", getenv("HOME"));
      if (!edje_object_file_set(result->obj, buf, "Epbb"))
      {
         if (!edje_object_file_set
             (result->obj, PACKAGE_DATA_DIR "/themes/default.eet", "Epbb"))
         {
            evas_object_del(result->obj);
            result->obj = NULL;
            free(result);
            return (NULL);
         }
      }
      /* add our object */
      evas_object_move(result->obj, 0, 0);
      evas_object_resize(result->obj, w, h);
      evas_object_layer_set(result->obj, 0);
      evas_object_name_set(result->obj, "edje");
      evas_object_show(result->obj);

      edje_object_signal_callback_add(result->obj, "window,*", "", _window_cb,
                                      result);
      /* fix our ecore_evas to do the theme's bidding */
      edje_object_size_max_get(result->obj, &w, &h);
      ecore_evas_size_min_set(ee, (int) w, (int) h);
      edje_object_size_min_get(result->obj, &w, &h);
      ecore_evas_size_min_set(ee, (int) w, (int) h);
      ecore_evas_resize(ee, (int) w, (int) h);
   }
   return (result);
}

void
epbb_free(Epbb * e)
{
   if (e)
   {
      if (e->obj)
         evas_object_del(e->obj);
      if (e->ee)
         ecore_evas_free(e->ee);
      free(e);
   }
}

static void
epbb_progress_bar_percent_set(Epbb * e, double percent)
{
   double dragx, dragy;

   if (edje_object_part_exists(e->obj, "Status"))
   {
      edje_object_part_drag_value_get(e->obj, "Status", &dragx, &dragy);
      edje_object_part_drag_value_set(e->obj, "Status", percent, dragy);
   }
}

static void
epbb_status_text_set(Epbb * e, char *buf)
{
   if (edje_object_part_exists(e->obj, "Message"))
   {
      edje_object_part_text_set(e->obj, "Message", buf);
   }
}

void
epbb_warning_sleep_set(Epbb * e, int val)
{
   char buf[120];

   edje_object_signal_emit(e->obj, "pbb,warning,sleep", "");
   snprintf(buf, 120, "Sleeping in %d Minutes.", val / 60);
   epbb_status_text_set(e, buf);
   if (!ecore_evas_visibility_get(e->ee))
      ecore_evas_show(e->ee);
}

void
epbb_warning_battery_set(Epbb * e, int level)
{
   switch (level)
   {
     case 1:
        edje_object_signal_emit(e->obj, "pbb,warning,battery,1", "");
        break;
     case 2:
        edje_object_signal_emit(e->obj, "pbb,warning,battery,2", "");
        break;
     case 3:
        edje_object_signal_emit(e->obj, "pbb,warning,battery,3", "");
        break;
     default:
        break;
   }
}

void
epbb_mute_set(Epbb * e)
{
   char buf[120];

   edje_object_signal_emit(e->obj, "pbb,audio,mute", "");
   epbb_progress_bar_percent_set(e, 0.0);
   snprintf(buf, 120, "%d%%", 0);
   epbb_status_text_set(e, buf);
}

void
epbb_volume_set(Epbb * e, double val)
{
   char buf[120];

   edje_object_signal_emit(e->obj, "pbb,audio,volume", "");
   epbb_progress_bar_percent_set(e, val);
   snprintf(buf, 120, "%0.0f%%", val * 100);
   epbb_status_text_set(e, buf);
}

void
epbb_brightness_set(Epbb * e, double val)
{
   char buf[120];

   edje_object_signal_emit(e->obj, "pbb,display,brightness", "");
   epbb_progress_bar_percent_set(e, val);
   snprintf(buf, 120, "%0.0f%%", 100 * (double) val);
   epbb_status_text_set(e, buf);
}

void
epbb_battery_remaining_set(Epbb * e, int seconds)
{
   char buf[120];

   if (e)
   {
      if (!e->charging)
      {
         snprintf(buf, 120, "%d Minutes Remaining", seconds / 60);
      }
      else
      {
         snprintf(buf, 120, "Charging...");
      }
      edje_object_signal_emit(e->obj, "pbb,battery,changed", "");
      epbb_status_text_set(e, buf);
#if 0
      fprintf(stderr, "%s\n", buf);
#endif
   }
}

void
epbb_battery_source_set(Epbb * e, int val)
{
   if (e)
   {
      if (e->charging != val)
      {
         if (val)
            edje_object_signal_emit(e->obj, "pbb,ac,on", "");
         else
            edje_object_signal_emit(e->obj, "pbb,ac,off", "");
         e->charging = val;
      }
   }
}

static void
epbb_init(void)
{
   int i, count;
   char buf[PATH_MAX];
   struct stat status;
   char *dirs[] = { ".e", ".e/apps", ".e/apps/epbb", ".e/apps/epbb/fonts" };
   count = sizeof(dirs) / sizeof(char *);

   for (i = 0; i < count; i++)
   {
      snprintf(buf, PATH_MAX, "%s/%s", getenv("HOME"), dirs[i]);
      if (!stat(buf, &status))
         continue;
      mkdir(buf, S_IRUSR | S_IWUSR | S_IXUSR);
   }
}
