#include<Evas.h>
#include<Ecore.h>
#include<Ecore_X.h>
#include<Ecore_Evas.h>
#include<Edje.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<limits.h>
#include "epbb.h"
#include "../config.h"

#define UN(ptr) ptr = 0

static void
_window_cb(void *data, Evas_Object *o, const char *sig, const char *src)
{
    Epbb *e = NULL;

    if((e = (Epbb*)data))
    {
	if(!strcmp(sig, "window,show"))
	    ecore_evas_show(e->ee);
	else if(!strcmp(sig, "window,hide"))
	    ecore_evas_hide(e->ee);
    }
}

Epbb*
epbb_new(Ecore_Evas *ee)
{
    Evas_Coord w, h;
    Epbb *result = NULL;
    if((result = malloc(sizeof(Epbb))))
    {
	memset(result, 0, sizeof(Epbb));
	result->ee = ee;

	result->obj = edje_object_add(ecore_evas_get(ee));
	if(edje_object_file_set(result->obj, PACKAGE_DATA_DIR"/themes/default.eet", "Epbb"))
	{
	    /* add our object */
	    evas_object_move(result->obj, 0, 0);
	    evas_object_resize(result->obj, w, h);
	    evas_object_layer_set(result->obj, 0);
	    evas_object_name_set(result->obj, "edje");
	    evas_object_show(result->obj);
	    
	    edje_object_signal_callback_add(result->obj, "window,*", "", _window_cb, result);
	    /* fix our ecore_evas to do the theme's bidding */
	    edje_object_size_max_get(result->obj, &w, &h);
	    ecore_evas_size_min_set(ee, (int)w, (int)h);
	    edje_object_size_min_get(result->obj, &w, &h);
	    ecore_evas_size_min_set(ee, (int)w, (int)h);
	    ecore_evas_resize(ee, (int)w, (int)h);    
	}
	else
	{
	    evas_object_del(result->obj);
	    result->obj = NULL;
	}
    }
    return(result);
}

void
epbb_free(Epbb *e)
{
    if(e)
    {
	if(e->timer) ecore_timer_del(e->timer);
	if(e->obj) evas_object_del(e->obj);
	if(e->ee) ecore_evas_free(e->ee);
	free(e);
    }
}

static void
epbb_progress_bar_percent_set(Epbb *e, double percent)
{
    double dragx, dragy;
    if(edje_object_part_exists(e->obj, "Status"))
    {
	edje_object_part_drag_value_get(e->obj, "Status", &dragx, &dragy);
	edje_object_part_drag_value_set(e->obj, "Status", percent, dragy);
    }
}

static void
epbb_status_text_set(Epbb *e, char *buf)
{
    if(edje_object_part_exists(e->obj, "Message"))
    {
	edje_object_part_text_set(e->obj, "Message", buf);
    }
}

void
epbb_warning_sleep_set(Epbb *e, int val)
{
    char buf[120];
    
    edje_object_signal_emit(e->obj, "SLEEP_WARNING", "");
    snprintf(buf, 120, "Sleeping in %d Minutes.", val/60);
    epbb_status_text_set(e, buf);
    if(!ecore_evas_visibility_get(e->ee))
	ecore_evas_show(e->ee);
}

void
epbb_warning_battery_set(Epbb *e, int val)
{
    char buf[120];
    
    edje_object_signal_emit(e->obj, "BATTERY_WARNING", "");
    snprintf(buf, 120, "%d Minutes Left", val/60);
    epbb_status_text_set(e, buf);
}

void
epbb_mute_set(Epbb *e)
{
    char buf[120];
    
    edje_object_signal_emit(e->obj, "MUTED", "");
    epbb_progress_bar_percent_set(e, 0.0);
    snprintf(buf, 120, "%d%%", 0);
    epbb_status_text_set(e, buf);
}
void
epbb_volume_set(Epbb *e, int val)
{
    char buf[120];
    edje_object_signal_emit(e->obj, "VOLUME", "");
    epbb_progress_bar_percent_set(e, (double)val/(double)100);
    snprintf(buf, 120, "%d%%", val);
    epbb_status_text_set(e, buf);
}
void
epbb_brightness_set(Epbb *e, int val)
{
    char buf[120];
    
    edje_object_signal_emit(e->obj, "BRIGHTNESS", "");
    epbb_progress_bar_percent_set(e, (double)val/(double)15);
    snprintf(buf, 120, "%0.0f%%", 100 * ((double)val/15.0));
    epbb_status_text_set(e, buf);
}

void
epbb_battery_remaining_set(Epbb *e, int mins)
{
#if 0
    UN(mins);
#endif
}

void
epbb_battery_source_set(Epbb *e, int val)
{
#if 0
    static int battery_state = 0;
    if(val == battery_state) return;
    //set_battery_state(val);
#endif
}
