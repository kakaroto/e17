#include "erss.h"
#include "parse_config.h"      /* rc */
#include "tooltip.h"           /* Erss_Tooltip, erss_tooltip_new() */



typedef enum {
  ERSS_GAI_SUCC=0,
  ERSS_GAI_FAIL=-1,
  ERSS_GAI_NOFEED=-2,
  ERSS_GAI_NOITEM=-3
} erss_gai_error;



Evas        *evas=NULL;
Ecore_Evas  *ee=NULL;
int          world_x,
             world_y;
Evas_Object *bg=NULL,
            *cont=NULL,
            *tid=NULL;






int erss_set_time (void *data)
{
	Erss_Feed *f=(Erss_Feed *)data;
	/* these commented out bits stopped being used when we stopped showing
	 * the clock in the window - left in case they are ever wanted again
	 */
	/* char      *str; */
	char       text[100];

	/* str = erss_time_format (); */
	if (f->last_time)
		snprintf (text, sizeof (text), "Last update: %s", f->last_time);

	edje_object_part_text_set (tid, "clock", text);

	/* free (str); */

	return TRUE;
}






static void erss_window_move (Ecore_Evas *ee)
{
	int x, y, w, h;
	Evas_Object *o = NULL;

	ecore_evas_geometry_get (ee, &x, &y, &w, &h);

	if((o = evas_object_name_find(ecore_evas_get(ee), "root_background")))
		esmart_trans_x11_freshen(o, x, y, w, h);

	world_x = x;
	world_y = y;

}
	
void erss_window_resize(Ecore_Evas *ee)
{
	int x, y, w, h;
	Evas_Object *o = NULL;

	ecore_evas_geometry_get(ee, &x, &y, &w, &h);

	if((o = evas_object_name_find(ecore_evas_get(ee), "root_background")))
	{
		evas_object_resize(o, w, h);
		esmart_trans_x11_freshen(o, x, y, w, h);
	}
	
	if((o = evas_object_name_find(ecore_evas_get(ee), "background")))
	{
		evas_object_resize(o, w, h);
	}

	if((o = evas_object_name_find(ecore_evas_get(ee), "container")))
		evas_object_resize(o, w, h);
}






static void erss_mouse_click_item (void *data, Evas_Object *o,
				   const char *sig, const char *src)
{
	char *url = data;
	char  c[1024];

	if (!rc->browser) {
		fprintf (stderr, "%s error: you have not defined any browser in your config file setting /usr/bin/mozilla as default\n", PACKAGE);
		rc->browser = strdup ("mozilla");
	}
	
	snprintf (c, sizeof (c), "%s \"%s\"", rc->browser, url);
	ecore_exe_run (c, NULL);
}

static void erss_mouse_in_cursor_change (void *data, Evas *e, Evas_Object *obj,
					 void *event_info)
{
	Ecore_X_Window win;
	Ecore_X_Cursor c;
	Erss_Feed *f = (Erss_Feed *) data;

	win = ecore_evas_software_x11_window_get(ee);
	if ((f != NULL) && (f->cfg->item_url != NULL)) {
		c = ecore_x_cursor_shape_get(ECORE_X_CURSOR_HAND2);
		ecore_x_window_cursor_set(win, c);
	}
}

static void erss_mouse_out_cursor_change (void *data, Evas *e,
					  Evas_Object *obj, void *event_info)
{
	Ecore_X_Window win;
  
	win = ecore_evas_software_x11_window_get(ee);
	ecore_x_window_cursor_set(win, 0);
}






static erss_gai_error erss_gui_item_new(Erss_Feed *f,Erss_Article *item) {
  if(!f)
    return ERSS_GAI_NOFEED;
  if(!item)
    return ERSS_GAI_NOITEM;

/* fprintf(stderr, "%s: %p -- %s\n", __FUNCTION__, item, item->title); */

  if(item->title) {
    item->obj = edje_object_add (evas);
    edje_object_file_set (item->obj, f->cfg->theme, "erss_item");
    evas_object_show (item->obj);

    evas_object_event_callback_add (item->obj,
				    EVAS_CALLBACK_MOUSE_IN, erss_mouse_in_cursor_change, f);
    evas_object_event_callback_add (item->obj,
				    EVAS_CALLBACK_MOUSE_OUT, erss_mouse_out_cursor_change, f);

    esmart_container_element_append(cont, item->obj);
    edje_object_part_text_set (item->obj, "article", item->title);
  }

  if(item->url) {
    edje_object_signal_callback_add (item->obj, "exec*", "*",
				     erss_mouse_click_item, item->url);
    edje_object_signal_emit (item->obj, "mouse,in", "article");
    edje_object_signal_emit (item->obj, "mouse,out", "article");
  }

  if (item->description && item->obj)
    erss_tooltip_for(item);

  return ERSS_GAI_SUCC;
}



int erss_gui_items_add(Erss_Feed *f) {
  int c=0;

  if (!f)
    return -1;

  if (f->list) {
    Erss_Article *item = ecore_list_first_goto (f->list);
    while ((item = ecore_list_next(f->list))) {
      if(erss_gui_item_new(f,item)==ERSS_GAI_SUCC)
	c++;
    }
  }
  return c;
}



static Erss_Article *erss_gui_item_dst(Erss_Article **i) {
	Erss_Article *item;


	if((i != NULL) && ((item=*i) != NULL)) {
		*i = NULL;

		if (item->obj)
			esmart_container_element_destroy (cont, item->obj);

		if (item->title)
			free (item->title);

		if (item->url)
			free (item->url);

		if (item->description)
			free (item->description);

		free (item);
	}

	return NULL;
}



int erss_gui_items_drop(Ecore_List **l) {
	Ecore_List     *list;
	Erss_Article *item;

	if ((l == NULL) || ((list=*l) == NULL))
		return FALSE;

	item = ecore_list_first_goto (list);
	while ((item = ecore_list_next(list))) {
		if (item->description && item->obj)
			erss_tooltip_free(item);
		erss_gui_item_dst(&item);
	}

	ecore_list_remove (list);

	return TRUE;
}






int erss_gui_init (char *winname, Erss_Config *cfg) {
	Ecore_X_Window  win;
	Evas_Object    *header;
	char            title[PATH_MAX];
	int             x,y,w,h;
	int             height,width;

	ecore_x_init (NULL);

	if (!ecore_evas_init ())
		return -1;

	width = 300;
	height = 16 * cfg->num_stories;

	printf("stories in %s: %d\n",winname,cfg->num_stories);

	if (cfg->header) height += 26;
	if (cfg->clock)  height += 26;

	ee = ecore_evas_software_x11_new (NULL, 0, 0, 0, width, height);
	win = ecore_evas_software_x11_window_get(ee);
	
	if (!ee)
		return -1;

	ecore_evas_borderless_set (ee, cfg->borderless);
	ecore_evas_name_class_set (ee, "Erss", "Erss");
	snprintf(title, PATH_MAX, "erss - %s", winname);
	ecore_evas_title_set (ee, title);
	ecore_evas_layer_set (ee, 2);
	ecore_evas_show (ee);
	
	/* ecore_x_window_lower(win); */

	if (cfg->x != 0 && cfg->y != 0)
		ecore_evas_move (ee, cfg->x, cfg->y);

	evas = ecore_evas_get (ee);

	evas_font_path_append (evas, PACKAGE_DATA_DIR"/fonts/");

	ecore_evas_geometry_get (ee, &x, &y, &w, &h);
	
	bg = esmart_trans_x11_new (evas);
	evas_object_move (bg, 0, 0);
	evas_object_layer_set (bg, -5);
	evas_object_resize (bg, w, h);
	evas_object_name_set(bg, "root_background");
	evas_object_show (bg);

	bg = evas_object_rectangle_add(evas);
	evas_object_move (bg, 0, 0);
	evas_object_layer_set (bg, -6);
	evas_object_resize (bg, w, h);
	evas_object_color_set(bg, 255, 255, 255, 0);
	evas_object_name_set(bg, "background");
	evas_object_show (bg);

	ecore_evas_callback_move_set (ee, erss_window_move);
	ecore_evas_callback_resize_set(ee, erss_window_resize);

	cont = esmart_container_new(evas);
	evas_object_move(cont, 0, 0);
	evas_object_resize(cont, width, height);
	evas_object_layer_set(cont, 0);
	evas_object_name_set(cont, "container");
	evas_object_show(cont);
	esmart_container_padding_set(cont, 10, 10, 10, 10);
	esmart_container_spacing_set(cont, 5);
	esmart_container_direction_set(cont, 1);
	esmart_container_fill_policy_set(cont,
			CONTAINER_FILL_POLICY_FILL);

	edje_init();

	if (cfg->header) {
		header = edje_object_add (evas);
		edje_object_file_set (header, cfg->theme, "erss");
		edje_object_part_text_set (header, "header", cfg->header);
		evas_object_show (header);

		evas_object_event_callback_add (header,
						EVAS_CALLBACK_MOUSE_IN, erss_mouse_in_cursor_change, NULL);
		evas_object_event_callback_add (header,
						EVAS_CALLBACK_MOUSE_OUT, erss_mouse_out_cursor_change, NULL);

		edje_object_signal_callback_add (header, "exec*", "*",
						 erss_mouse_click_item, cfg->hostname);
		edje_object_signal_emit (header, "mouse,in", "article");
		edje_object_signal_emit (header, "mouse,out", "article");

		esmart_container_element_append(cont, header);
	}

	if ((rc->clock==1)||((cfg->clock==1)&&(rc->clock!=0))) {
		tid = edje_object_add (evas);
		edje_object_file_set (tid, cfg->theme, "erss_clock");
		edje_object_part_text_set (tid, "clock", "");
		evas_object_show (tid);

		esmart_container_element_append(cont, tid);
	}
	return 0;
}



int erss_gui_exit (void) {
	ecore_evas_shutdown ();
	ecore_x_shutdown ();
	return 0;
}
