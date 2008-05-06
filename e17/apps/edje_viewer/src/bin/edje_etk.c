/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>

#include <Edje.h>
#include <Etk.h>

#include "edje_etk.h"
#include "etk_gui.h"
#include "conf.h"

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

#define DAT PACKAGE_DATA_DIR"/"

#define FREE(ptr) do { if(ptr) { free(ptr); ptr = NULL; }} while (0);

static void edje_move_resize(Demo_Edje *de, Evas_Coord xx, Evas_Coord yy,
      Evas_Coord ww, Evas_Coord hh);
static void top_down_cb(void *data, Evas *e, Evas_Object *obj, 
	void *event_info);
static void top_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void top_move_cb(void *data, Evas *e, Evas_Object *obj,
	void *event_info);
static void bottom_down_cb(void *data, Evas *e, Evas_Object *obj, 
	void *event_info);
static void bottom_up_cb(void *data, Evas *e, Evas_Object *obj,
	void *event_info);
static void bottom_move_cb(void *data, Evas *e, Evas_Object *obj, 
	void *event_info);
static void signal_cb(void *data, Evas_Object *o, const char *sig,
	const char *src);
static void message_cb(void *data, Evas_Object *obj, Edje_Message_Type type,
	int id, void *msg);

Evas_Object *o_bg;
Evas_Object *o_shadow;
Evas_List *visible_elements = NULL;

Evas_List *visible_elements_get() {
  return visible_elements;
}

void bg_setup(Etk_Canvas *canvas)
{
   Evas_Object *o;
   Evas *evas;
   Evas_Coord x, y, w, h;

   evas = etk_widget_toplevel_evas_get(ETK_WIDGET(canvas));
   if (!evas) return;

   etk_widget_geometry_get(ETK_WIDGET(canvas), &x, &y, &w, &h);

   o = evas_object_image_add(evas);
   evas_object_move(o, x, y);
   evas_object_resize(o, w, h);
   evas_object_layer_set(o, -999);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_image_file_set(o,
	 DAT"data/images/bg.png", NULL);
   evas_object_image_fill_set(o, 0, 0, 128, 128);
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);

   o_bg = o;
   etk_canvas_object_add(canvas, o);

   o = evas_object_image_add(evas);
   evas_object_move(o, x, y);
   evas_object_resize(o, w, h);
   evas_object_layer_set(o, -999);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_image_file_set(o,
	 DAT"data/images/shadow.png", NULL);
   evas_object_image_smooth_scale_set(o, 0);
   evas_object_image_fill_set(o, 0, 0, w, h);
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);
   
   o_shadow = o;
   etk_canvas_object_add(canvas, o);
}

void canvas_resize_cb(Etk_Object *canvas, const char *property_name __UNUSED__, void *data __UNUSED__)
{
   Evas *evas;
   Evas_Coord x, y, w, h;
   Evas_Coord xx, yy, ww, hh;

   evas = etk_widget_toplevel_evas_get(ETK_WIDGET(canvas));
   if (!evas) return;

   etk_widget_geometry_get(ETK_WIDGET(canvas), &x, &y, &w, &h);
   xx = 10 + x;
   yy = 10 + y;
   ww = w/3 - 40;
   hh = h/3 - 50;

   evas_object_resize(o_bg, w, h);
   evas_object_move(o_bg, x, y);
   evas_object_resize(o_shadow, w, h);
   evas_object_move(o_shadow, x, y);
   evas_object_image_fill_set(o_shadow, 0, 0, w, h);
}

void list_entries(const char *file, Etk_Tree *tree,
      Etk_Canvas *canvas __UNUSED__)
{
   Evas_List *entries;
   Evas_List *collections = NULL;
   Etk_Tree_Col *col1;
   Etk_Tree_Row *row;
   Etk_Bool sort_parts;

   entries = edje_file_collection_list(file);
   col1 = etk_tree_nth_col_get(tree, 0);

   if (entries)
     {
	Evas_List *l;

	etk_tree_clear(tree);

	for (l = entries; l; l = l->next)
	  {
	     char *name;
	     Collection *co;

	     name = l->data;
	     co = calloc(1, sizeof(Collection));
	     collections = evas_list_append(collections, co);
	     co->file = strdup(file);
	     co->part = strdup(name);
	     co->de = NULL;

	     row = etk_tree_row_append(tree, NULL, col1, name, NULL);
	     etk_tree_row_data_set(row, co);
	  }
	edje_file_collection_list_free(entries);
	edje_viewer_config_recent_set(file);
	edje_viewer_config_last_set(file);
	sort_parts = edje_viewer_config_sort_parts_get();
	if (sort_parts)
	  etk_tree_col_sort_set(col1, gui_part_col_sort_cb, NULL);
     }
}

Demo_Edje *edje_part_create(Etk_Tree *output, Etk_Canvas *canvas, 
      const char *file, char *name)
{
   Evas_Object *o;
   Demo_Edje *de;
   Evas_Coord xx, yy, ww, hh;
   Evas_Coord cx, cy, cw, ch;
   Evas *evas;

   evas = etk_widget_toplevel_evas_get(ETK_WIDGET(canvas));
   if (!evas) return NULL;

   de = calloc(1, sizeof(Demo_Edje));
   de->name = strdup(name);
   etk_widget_geometry_get(ETK_WIDGET(canvas), &cx, &cy, &cw, &ch);
   xx = 10 + cx;
   yy = 10 + cy;
   ww = cw/3 - 40;
   hh = ch/3 - 50;

   o = evas_object_image_add(evas);
   etk_canvas_object_add(canvas, o);
   evas_object_image_file_set(o, DAT"data/images/border.png", NULL);
   evas_object_image_smooth_scale_set(o, 0);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_image_border_set(o, 26, 26, 26, 26);
   evas_object_image_fill_set(o, 0, 0, ww, hh);
   evas_object_pass_events_set(o, 1);
   de->image = o;

   o = evas_object_rectangle_add(evas);
   etk_canvas_object_add(canvas, o);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, top_down_cb, de);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP,   top_up_cb, de);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, top_move_cb, de);

   de->top = o;

   o = evas_object_rectangle_add(evas);
   etk_canvas_object_add(canvas, o);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, 
	 bottom_down_cb, de);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP,  
	 bottom_up_cb, de);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, 
	 bottom_move_cb, de);
   de->bottom = o;

   o = evas_object_rectangle_add(evas);
   etk_canvas_object_add(canvas, o);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
	 bottom_down_cb, de);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP,  
	 bottom_up_cb, de);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE,
	 bottom_move_cb, de);
   de->left = 0;

   o = evas_object_rectangle_add(evas);
   etk_canvas_object_add(canvas, o);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
	 bottom_down_cb, de);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP,  
	 bottom_up_cb, de);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, 
	 bottom_move_cb, de);

   de->right = o;

   o = evas_object_rectangle_add(evas);
   etk_canvas_object_add(canvas, o);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_pass_events_set(o, 1);
   de->title_clip = o;

   o = evas_object_text_add(evas);
   etk_canvas_object_add(canvas, o);
   evas_object_color_set(o, 0, 0, 0, 255);
   evas_object_text_text_set(o, name);
   evas_object_text_font_set(o, "Vera", 10);
   evas_object_pass_events_set(o, 1);
   evas_object_clip_set(o, de->title_clip);
   de->title = o;

   o = edje_object_add(evas);
   etk_canvas_object_add(canvas, o);
   edje_object_message_handler_set(o, message_cb, output);
   edje_object_signal_callback_add(o, "*", "*", signal_cb, output);
   edje_object_file_set(o, file, name);
   edje_object_part_drag_size_set(o, "dragable", 0.01, 0.5);
   edje_object_part_drag_step_set(o, "dragable", 0.1, 0.1);
   edje_object_part_drag_page_set(o, "dragable", 0.2, 0.2);
   de->edje = o;

   de->first_run = ETK_TRUE;

   return de;
}

void edje_part_show(Etk_Canvas *canvas, Demo_Edje *de)
{
   Evas_Coord x, y, w, h, xx, yy, ww, hh;

   etk_widget_geometry_get(ETK_WIDGET(canvas), &x, &y, &w, &h);
   if (de->first_run)
     {
	xx = 10 + x;
	yy = 10 + y;
	ww = w/3 - 40;
	hh = h/3 - 50;
	de->cx = x;
	de->cy = y;
	de->cw = w;
	de->ch = h;

	edje_move_resize(de, xx, yy, ww, hh);
	de->first_run = ETK_FALSE;
     }

   evas_object_show(de->image);
   evas_object_show(de->top);
   evas_object_show(de->bottom);
   evas_object_show(de->left);
   evas_object_show(de->right);
   evas_object_show(de->title_clip);
   evas_object_show(de->title);
   evas_object_show(de->edje);

   edje_part_resize(de);
   visible_elements = evas_list_append(visible_elements, de);
}

void edje_part_hide(Demo_Edje *de)
{
   evas_object_hide(de->edje);
   evas_object_hide(de->left);
   evas_object_hide(de->right);
   evas_object_hide(de->top);
   evas_object_hide(de->bottom);
   evas_object_hide(de->image);
   evas_object_hide(de->title_clip);
   evas_object_hide(de->title);

   visible_elements = evas_list_remove(visible_elements, de);
}

void edje_part_resize(Demo_Edje *de)
{
   Evas_Coord x, y, w, h, tw, th;
   Evas_Coord minw, minh;

   evas_object_geometry_get(de->edje, &x, &y, &w, &h);
   x -= 10;
   y -= 20;
   w += 20;
   h += 30;
   minw = 20 + de->minw;
   minh = 30 + de->minh;
   evas_object_move(de->left, x, y + 20);
   evas_object_resize(de->left, 10, h - 30);
   evas_object_move(de->right, x + w - 10, y + 20);
   evas_object_resize(de->right, 10, h - 30);
   evas_object_move(de->top, x, y);
   evas_object_resize(de->top, w, 20);
   evas_object_move(de->bottom, x, y + (h - 10));
   evas_object_resize(de->bottom, w, 10);
   evas_object_move(de->title_clip, x + 20, y);
   evas_object_resize(de->title_clip, w - 40, 20);
   evas_object_geometry_get(de->title, NULL, NULL, &tw, &th);
   evas_object_move(de->title, x + ((w - tw) / 2), y + 4 + ((16 - th) / 2));
   evas_object_move(de->image, x, y);
   evas_object_resize(de->image, w, h);
   evas_object_image_fill_set(de->image, 0, 0, w, h);
   evas_object_move(de->edje, x + 10, y + 20);
   evas_object_resize(de->edje, w - 20, h - 30);
}

static void edje_move_resize(Demo_Edje *de, Evas_Coord xx, Evas_Coord yy,
      Evas_Coord ww, Evas_Coord hh)
{
   Evas_Coord tw, th, ew, eh;

   evas_object_move(de->image, xx, yy);
   evas_object_resize(de->image, ww, hh);
   evas_object_show(de->image);

   evas_object_move(de->top, xx, yy);
   evas_object_resize(de->top, ww, 20);
   evas_object_show(de->top);

   evas_object_move(de->bottom, xx, yy + hh - 10);
   evas_object_resize(de->bottom, ww, 10);
   evas_object_show(de->bottom);

   evas_object_move(de->left, xx, yy + 20);
   evas_object_resize(de->left, 10, hh - 20 - 10);
   evas_object_show(de->left);

   evas_object_move(de->right, xx + ww - 10, yy + 20);
   evas_object_resize(de->right, 10, hh - 20 - 10);
   evas_object_show(de->right);

   evas_object_move(de->title_clip, xx + 20, yy);
   evas_object_resize(de->title_clip, ww - 20 - 20, 20);
   evas_object_show(de->title_clip);

   evas_object_geometry_get(de->title, NULL, NULL, &tw, &th);
   evas_object_move(de->title, xx + ((ww - tw) / 2), yy + 4 + ((16 - th) / 2));
   evas_object_show(de->title);

   ew = ww;
   eh = hh;
   edje_object_size_min_get(de->edje, &(de->minw), &(de->minh));
   if (ww < de->minw) ew = de->minw;
   if (hh < de->minh) eh = de->minh;
   edje_object_size_max_get(de->edje, &(de->maxw), &(de->maxh));
   if (de->maxw > 0)
     {
	if (ww > de->maxw) ew = de->maxw;
     }
   if (de->maxh > 0)
     {
	if (hh > de->maxh) eh = de->maxh;
     }
   evas_object_move(de->edje, xx + 10, yy + 20);
   evas_object_resize(de->edje, ew, eh);
   evas_object_show(de->edje);

}

static void bottom_down_cb
(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
    Demo_Edje *de;
    Evas_Event_Mouse_Down *ev;
    Evas_Coord x, y, w, h;
    int hdir, vdir;

    de = data;
    ev = event_info;
    de->down_bottom = 1;
    evas_object_geometry_get(de->edje, &x, &y, &w, &h);
    hdir = 1;
    vdir = 1;
    x -= 10;
    y -= 20;
    w += 20;
    h += 30;
    if ((ev->canvas.x - x) < (w / 2)) hdir = 0;
    if ((ev->canvas.y - y) < (h / 2)) vdir = 0;
    de->hdir = hdir;
    de->vdir = vdir;
}

static void top_down_cb(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
    Demo_Edje *de;

    de = data;
    de->down_top = 1;
    evas_object_raise(de->image);
    evas_object_raise(de->top);
    evas_object_raise(de->bottom);
    evas_object_raise(de->left);
    evas_object_raise(de->right);
    evas_object_raise(de->title_clip);
    evas_object_raise(de->title);
    evas_object_raise(de->edje);
}

static void top_up_cb(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
    Demo_Edje *de;

    de = data;
    de->down_top = 0;
}

static void top_move_cb(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
    Demo_Edje *de;
    Evas_Event_Mouse_Move *ev;

    de = data;
    ev = event_info;
    if (de->down_top)
    {
	Evas_Coord x, y;

	evas_object_geometry_get(de->left, &x, &y, NULL, NULL);
	evas_object_move(de->left, 
		x + ev->cur.canvas.x - ev->prev.canvas.x,
		y + ev->cur.canvas.y - ev->prev.canvas.y);
	evas_object_geometry_get(de->right, &x, &y, NULL, NULL);
	evas_object_move(de->right, 
		x + ev->cur.canvas.x - ev->prev.canvas.x,
		y + ev->cur.canvas.y - ev->prev.canvas.y);
	evas_object_geometry_get(de->top, &x, &y, NULL, NULL);
	evas_object_move(de->top, 
		x + ev->cur.canvas.x - ev->prev.canvas.x,
		y + ev->cur.canvas.y - ev->prev.canvas.y);
	evas_object_geometry_get(de->bottom, &x, &y, NULL, NULL);
	evas_object_move(de->bottom, 
		x + ev->cur.canvas.x - ev->prev.canvas.x,
		y + ev->cur.canvas.y - ev->prev.canvas.y);
	evas_object_geometry_get(de->title_clip, &x, &y, NULL, NULL);
	evas_object_move(de->title_clip, 
		x + ev->cur.canvas.x - ev->prev.canvas.x,
		y + ev->cur.canvas.y - ev->prev.canvas.y);
	evas_object_geometry_get(de->title, &x, &y, NULL, NULL);
	evas_object_move(de->title, 
		x + ev->cur.canvas.x - ev->prev.canvas.x,
		y + ev->cur.canvas.y - ev->prev.canvas.y);
	evas_object_geometry_get(de->image, &x, &y, NULL, NULL);
	evas_object_move(de->image, 
		x + ev->cur.canvas.x - ev->prev.canvas.x,
		y + ev->cur.canvas.y - ev->prev.canvas.y);
	evas_object_geometry_get(de->edje, &x, &y, NULL, NULL);
	evas_object_move(de->edje, 
		x + ev->cur.canvas.x - ev->prev.canvas.x,
		y + ev->cur.canvas.y - ev->prev.canvas.y);
    }
}

static void bottom_up_cb
(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
    Demo_Edje *de;

    de = data;
    de->down_bottom = 0;
}

static void bottom_move_cb
(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
    Demo_Edje *de;
    Evas_Event_Mouse_Move *ev;

    de = data;
    ev = event_info;
    if (de->down_bottom)
    {
	Evas_Coord x, y, w, h;
	Evas_Coord minw, minh;
	Evas_Coord maxw, maxh;
	int hdir, vdir;

	evas_object_geometry_get(de->edje, &x, &y, &w, &h);
	hdir = de->hdir;
	vdir = de->vdir;
	x -= 10;
	y -= 20;
	w += 20;
	h += 30;
	minw = 20 + de->minw;
	minh = 30 + de->minh;
	maxw = 20 + de->maxw;
	maxh = 30 + de->maxh;

	if (hdir > 0)
	{
	    w += ev->cur.canvas.x - ev->prev.canvas.x;
	    if (w < minw) w = minw;
	    else if (w > maxw) w = maxw;
	}
	else
	{
	    w -= ev->cur.canvas.x - ev->prev.canvas.x;
	    x += ev->cur.canvas.x - ev->prev.canvas.x;
	    if (w < minw)
	    {
		x += w - minw;
		w = minw;
	    }
	    if (w > maxw)
	    {
		x -= w - maxw;
		w = maxw;
	    }
	}
	if (vdir > 0)
	{
	    h += ev->cur.canvas.y - ev->prev.canvas.y;
	    if (h < minh) h = minh;
	    if (h > maxh) h = maxh;
	}
	else
	{
	    h -= ev->cur.canvas.y - ev->prev.canvas.y;
	    y += ev->cur.canvas.y - ev->prev.canvas.y;
	    if (h < minh)
	    {
		y += h - minh;
		h = minh;
	    }
	    if (h > maxh)
	    {
		y -= h - maxh;
		h = maxh;
	    }
	}
	evas_object_move(de->edje, x + 10, y + 20);
	evas_object_resize(de->edje, w - 20, h - 30);
	edje_part_resize(de);
    }
}

static void signal_cb 
(void *data, Evas_Object *o, const char *sig, const char *src)
{
   Etk_Tree *output;
   Etk_Tree_Col *col;
   Etk_Tree_Row *row;
   int count;
   char *str;

   if (!(output = data)) return;
   
   col = etk_tree_nth_col_get(output, 0);
   count = output->total_rows;
   if (count > 5000)
     {
	row = etk_tree_first_row_get(output);
	etk_tree_row_delete(row);
     }
   
   str = calloc(1024, sizeof(char));
   snprintf(str, 1024, "CALLBACK for \"%s\" \"%s\"", sig, src);


   row = etk_tree_row_append(output, NULL, col, str, NULL);
   etk_tree_row_scroll_to(row, ETK_TRUE);
   etk_tree_row_select(row);
   if (!strcmp(sig, "drag"))
     {
	double x, y;

	count = output->total_rows;
	if (count > 5000)
	  {
	     row = etk_tree_first_row_get(output);
	     etk_tree_row_delete(row);
	  }

	edje_object_part_drag_value_get(o, src, &x, &y);
	snprintf(str, 1024, "Drag %3.3f %3.3f", x, y);
	etk_tree_row_append(output, NULL, col, str, NULL);
	etk_tree_row_scroll_to(row, ETK_TRUE);
	etk_tree_row_select(row);
     }

   FREE(str);
}


static void message_cb
(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg)
{
   Etk_Tree *output;
   Etk_Tree_Col *col;
   Etk_Tree_Row *row;
   int count;
   char *str;

   if (!(output = data)) return;
   
   col = etk_tree_nth_col_get(output, 0);
   count = output->total_rows;
   if (count > 5000)
     {
	row = etk_tree_first_row_get(output);
	etk_tree_row_delete(row);
     }
   
   str = calloc(1024, sizeof(char));
   snprintf(str, 1024, "MESSAGE for %p from script type %i id %i", obj, type, id);

   row = etk_tree_row_append(output, NULL, col, str, NULL);
   etk_tree_row_scroll_to(row, ETK_TRUE);
   etk_tree_row_select(row);
   if (type == EDJE_MESSAGE_STRING)
     {
	Edje_Message_String *emsg;

	count = output->total_rows;
	if (count > 5000)
	  {
	     row = etk_tree_first_row_get(output);
	     etk_tree_row_delete(row);
	  }

	emsg = (Edje_Message_String *)msg;
	snprintf(str, 1024, "STWING: \"%s\"\n", emsg->str);
	row = etk_tree_row_append(output, NULL, col, str, NULL);
	etk_tree_row_scroll_to(row, ETK_TRUE);
	etk_tree_row_select(row);
     }
   edje_object_message_send(obj, EDJE_MESSAGE_NONE, 12345, NULL);

   FREE(str);
}
