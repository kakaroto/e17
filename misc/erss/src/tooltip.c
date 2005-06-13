#include "erss.h"
#include "parse_config.h"       /* rc */
#include "gui.h"                /* erss_window_resize() */
#include "tooltip.h"            /* Erss_Tooltip */



typedef struct _erss_tooltip {
   Evas           *evas;
   Ecore_Evas     *ee;
   Ecore_X_Window  win;
   Evas_Object    *bg;
   Evas_Object    *etox;
   int             x,y;
   Ecore_Timer    *timer;
} Erss_Tooltip;






static void erss_window_move_tooltip (Ecore_Evas * ee)
{
	int x, y, w, h;
	Evas_Object *o = NULL;
	
	ecore_evas_geometry_get (ee, &x, &y, &w, &h);

	if((o = evas_object_name_find(ecore_evas_get(ee), "root_background")))
			esmart_trans_x11_freshen(o, x, y, w, h);
}

static Erss_Tooltip *erss_tooltip_new (char *description)
{
	Erss_Tooltip *tt;
	int x, y, w, h;
	Evas_Coord ew, eh;

	if(description == NULL)
		return NULL;
	
	tt = malloc (sizeof (Erss_Tooltip));
	memset (tt, 0, sizeof (Erss_Tooltip));

	tt->ee = ecore_evas_software_x11_new (NULL, 0, 0, 0, 250, 80);
	ecore_evas_override_set(tt->ee, 1);
	ecore_evas_borderless_set (tt->ee, TRUE);
	tt->win = ecore_evas_software_x11_window_get(ee);
	ecore_x_netwm_window_type_set (tt->win, ECORE_X_WINDOW_TYPE_UTILITY);
	ecore_evas_geometry_get (tt->ee, &x, &y, &w, &h);

	tt->evas = ecore_evas_get (tt->ee);
	evas_font_path_append (tt->evas, PACKAGE_DATA_DIR"/fonts/");

	tt->bg = esmart_trans_x11_new (tt->evas);
	evas_object_move (tt->bg, 0, 0);
	evas_object_layer_set (tt->bg, -5);
	evas_object_resize (tt->bg, w, h);
	evas_object_name_set(tt->bg, "root_background");
	evas_object_show (tt->bg);
	
	tt->bg = evas_object_rectangle_add(tt->evas);
	evas_object_move (tt->bg, 0, 0);
	evas_object_layer_set (tt->bg, -6);
	evas_object_resize (tt->bg, w, h);
	evas_object_color_set(tt->bg, 255, 255, 255, 20);
	evas_object_name_set(tt->bg, "background");
	evas_object_show (tt->bg);

	tt->etox = etox_new_all(tt->evas, x + 5, y + 5, w - 10 , h - 10, 
			255, ETOX_ALIGN_LEFT);
	etox_context_set_wrap_marker(etox_get_context(tt->etox), NULL, NULL);
	etox_context_set_align(etox_get_context(tt->etox), ETOX_ALIGN_LEFT);
	etox_context_set_font(etox_get_context(tt->etox), "Vera", 10);
	etox_context_set_style(etox_get_context(tt->etox), "shadow");
	etox_context_set_color(etox_get_context(tt->etox), 225, 225, 225, 255);
	etox_set_soft_wrap(tt->etox, 1);
	etox_set_word_wrap(tt->etox, 1);
	etox_set_alpha(tt->etox, 255);
	evas_object_layer_set(tt->etox, 1000);
	etox_set_text (tt->etox, description);
	evas_object_show (tt->etox);
	evas_object_geometry_get(tt->etox, NULL, NULL, &ew, &eh);

	if (eh > ew) {
		double scale = (double)ew / (double)eh;
		ew = ew / scale;
		eh *= scale;
		evas_object_resize(tt->etox, ew, eh);
		evas_object_geometry_get(tt->etox, NULL, NULL, &ew, &eh);
	}

	ecore_evas_callback_move_set (tt->ee, erss_window_move_tooltip);
	ecore_evas_callback_resize_set(tt->ee, erss_window_resize);
	ecore_evas_resize(tt->ee, ew + 10, eh + 10);

	return tt;
}

static void erss_tooltip_hide (Erss_Tooltip *tt)
{
	ecore_evas_hide (tt->ee);

	if (tt->timer) 
		ecore_timer_del (tt->timer);
}

static void erss_tooltip_show (Erss_Tooltip *tt)
{
	tt->timer = NULL;

	ecore_evas_move (tt->ee, world_x + tt->x - 10, world_y + tt->y - 10);
	ecore_evas_show (tt->ee);
}


static int erss_tooltip_timer (void *data)
{
	Erss_Tooltip *tt = data;

	erss_tooltip_show (tt);

	return FALSE;
}

static void erss_tooltip_mouse_in (void *data, Evas *e, Evas_Object *obj, 
		void *event_info) 
{
	Evas_Event_Mouse_In *ev = event_info;
	Erss_Tooltip *tt = data;

	tt->x = ev->output.x;
	tt->y = ev->output.y;

	tt->timer = ecore_timer_add (rc->tooltip_delay, erss_tooltip_timer, tt);
}

static void erss_tooltip_mouse_out (void *data, Evas *e, Evas_Object *obj, 
		void *event_info) 
{
	Erss_Tooltip *tt = data;
	if (tt)
		erss_tooltip_hide (tt);
} 

static void erss_tooltip_mouse_clicked (void *data, Evas *e, Evas_Object *obj, 
		void *event_info) 
{
	Erss_Tooltip *tt = data;
	if (tt)
		erss_tooltip_hide (tt);
} 

int erss_tooltip_for(Erss_Article *item) {
    Erss_Tooltip *tt;

    if(item && (tt = erss_tooltip_new (item->description))) {
      evas_object_event_callback_add (item->obj,
				      EVAS_CALLBACK_MOUSE_IN, erss_tooltip_mouse_in, tt);
      evas_object_event_callback_add (item->obj,
				      EVAS_CALLBACK_MOUSE_OUT, erss_tooltip_mouse_out, tt);
      evas_object_event_callback_add (tt->etox,
				      EVAS_CALLBACK_MOUSE_UP, erss_tooltip_mouse_clicked, tt);
      evas_object_event_callback_add (tt->bg,
				      EVAS_CALLBACK_MOUSE_UP, erss_tooltip_mouse_clicked, tt);
      return TRUE;
    }
    return FALSE;
}

void erss_tooltip_free(Erss_Article *item) {
    Erss_Tooltip *tt;

    /* get rid of our callbacks */
    evas_object_event_callback_del (item->obj,
				    EVAS_CALLBACK_MOUSE_IN, erss_tooltip_mouse_in);
    tt = evas_object_event_callback_del (item->obj,
				    EVAS_CALLBACK_MOUSE_OUT, erss_tooltip_mouse_out);
    if (tt==NULL) {
	return;
    }
    evas_object_event_callback_del (tt->etox,
				    EVAS_CALLBACK_MOUSE_UP, erss_tooltip_mouse_clicked);
    evas_object_event_callback_del (tt->bg,
				    EVAS_CALLBACK_MOUSE_UP, erss_tooltip_mouse_clicked);

    /* then lose the ecore_evas */
    ecore_evas_free(tt->ee);
    ecore_x_window_del(tt->win);
    /* finally free the damn thing */
    memset(tt,0,sizeof(Erss_Tooltip));
    free(tt);
}
