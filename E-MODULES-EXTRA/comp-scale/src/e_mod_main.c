#include <e.h>
#include "e_mod_main.h"

//TODO
//~ handle add, delete border
//~ how to handle/disable possible desk show events
//  while scale is active?
//

/* #define DBG(...) */
#define DBG(...) printf(__VA_ARGS__)

#define SPACING 32
#define PLACE_RUNS  10000
#define GROW_RUNS   1000
#define SHRINK_RUNS 2000
#define GROW 6.0

static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);

static void _scale_conf_new(void);
static void _scale_conf_free(void);
static Config_Item *_scale_conf_item_get(const char *id);
static void _scale_gc_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _scale_gc_cb_menu_post(void *data, E_Menu *menu);
static void _scale_gc_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);

static void _scale_finish(void);
static void _scale_in(void);
static void _scale_out(void);

static Eina_Bool _scale_cb_mouse_down(void *data, int type, void *event);
static Eina_Bool _scale_cb_mouse_move(void *data, int type, void *event);

static void _scale_win_delorig(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _scale_win_cb_intercept_move(void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _scale_win_cb_intercept_resize(void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _scale_win_cb_intercept_color(void *data, Evas_Object *obj, int r, int g, int b, int a);
static void _scale_win_cb_intercept_layer(void *data, Evas_Object *obj, int l);

typedef struct _Item Item;
typedef struct _Slot Slot;

struct _Item
{
  Evas_Object *o, *o_win;
  E_Border *bd;
  E_Comp_Win *cw;
  double scale;
  int alpha;

  double x;
  double y;
  double w;
  double h;

  double mx;
  double my;

  /* border origin (is moved when scale on another desk) */
  double bd_x;
  double bd_y;

  /* current position to draw by compositor */
  int cur_x, cur_y, cur_w, cur_h;

  /* borders' desk distance to the current desk */
  int dx, dy;

  int overlaps;

  int in_slots;
};

struct _Slot
{
  Eina_List *items;
  int x, y, w, h;
  Item *it;
  double min;
};

static Ecore_X_Window input_win = 0;
static E_Msg_Handler *msg_handler = NULL;
static E_Action *act = NULL;

static Eina_List *items = NULL;
static Eina_List *popups = NULL;
static Eina_List *handlers = NULL;
static Ecore_Animator *scale_animator = NULL;
static Eina_Bool scale_state = EINA_FALSE;
static double start_time;
static E_Zone *zone = NULL;
static int max_x, max_y, min_x, min_y;
static int use_x, use_y, use_w, use_h;
static int max_width, max_height;
static int step_count;
static Item *background = NULL;
static Item *selected_item = NULL;
static E_Desk *current_desk = NULL;
static int show_all_desks = EINA_FALSE;
static int send_to_desk = EINA_FALSE;

static Eina_Bool
_scale_redraw(void *blah)
{
   Eina_List *l;
   Item *it;
   double scale, a, in, duration;

   if (show_all_desks)
     duration = scale_conf->desks_duration;
   else
     duration = scale_conf->scale_duration;

   if (scale_state)
     scale = (ecore_time_get() - start_time) / duration;
   else
     scale = 1.0 - (ecore_time_get() - start_time) / duration;

   if (scale > 1.0) scale = 1.0;
   if (scale < 0.0) scale = 0.0;

   in = log(10) * scale;
   in = 1.0 / exp(in*in);

   EINA_LIST_FOREACH(items, l, it)
     {
	it->cur_w = it->bd->w * in + it->w * (1.0 - in);
	it->cur_h = it->bd->h * in + it->h * (1.0 - in);
	it->cur_x = it->bd_x  * in + it->x * (1.0 - in);
	it->cur_y = it->bd_y  * in + it->y * (1.0 - in);

	evas_object_move(it->o, it->cur_x, it->cur_y);
	evas_object_resize(it->o, it->cur_w, it->cur_h);

	evas_object_move(it->o_win, it->cur_x, it->cur_y);
	evas_object_resize(it->o_win, it->cur_w, it->cur_h);

	a = 255.0 * (1.0 - in);
	evas_object_color_set(it->o, a, a, a, a);

	if (!scale_conf->fade_windows)
	  continue;

	if ((it->bd->desk != current_desk) && (selected_item != it))
	  {
	     double ax = it->cur_x - it->x;
	     double ay = it->cur_y - it->y;
	     double bx = it->bd_x  - it->x;
	     double by = it->bd_y  - it->y;

	     a = (1.0 - sqrt(ax*ax + ay*ay) / sqrt(bx*bx + by*by)) * 255;
	  }
	else
	  {
	     a = 255.0;
	  }

	it->alpha = a;
	evas_object_color_set(it->o_win, a, a, a, a);
     }

   if (scale_conf->fade_popups)
     {
	a = 255.0 * in;

	EINA_LIST_FOREACH(popups, l, it)
	  evas_object_color_set(it->o_win, a, a, a, a);
     }

   if (scale_conf->fade_desktop && background)
     {
	a = 255.0 * (0.5 + in/2.0);

	evas_object_color_set(background->o_win, a, a, a, 255);
     }

   e_manager_comp_evas_update(e_manager_current_get());

   if (scale < 1.0 && scale > 0.0)
     return 1;

   if (scale == 0.0)
     _scale_finish();

   scale_animator = NULL;
   return 0;
}

static void
_scale_in()
{
   start_time = ecore_time_get();
   scale_state = EINA_TRUE;

   if (!scale_animator)
     scale_animator = ecore_animator_add(_scale_redraw, NULL);
}

static void
_scale_out()
{
   double duration, now = ecore_time_get();

   if (show_all_desks)
     duration = scale_conf->desks_duration;
   else
     duration = scale_conf->scale_duration;

   if (now - start_time < duration)
     start_time = now - (duration - (now - start_time));
   else
     start_time = now;

   if (!scale_animator)
     scale_animator = ecore_animator_add(_scale_redraw, NULL);

   scale_state = EINA_FALSE;
}

static void
_scale_finish()
{
   Ecore_Event_Handler *handler;
   Item *it;
   E_Desk *desk;

   e_grabinput_release(input_win, input_win);
   ecore_x_window_free(input_win);
   input_win = 0;

   desk = e_desk_current_get(zone);

   if (selected_item &&  selected_item->bd->desk != desk)
     {
	if (send_to_desk)
	  {
	     e_border_desk_set(selected_item->bd, desk);
	  }
	else
	  {
	     int tmp = e_config->desk_flip_animate_mode;

	     desk = selected_item->bd->desk;

	     e_config->desk_flip_animate_mode = 0;
	     e_desk_show(desk);
	     e_config->desk_flip_animate_mode = tmp;
	  }
     }

   EINA_LIST_FREE(items, it)
     {
	evas_object_event_callback_del(it->o_win, EVAS_CALLBACK_DEL, _scale_win_delorig);
	evas_object_intercept_move_callback_del(it->o_win, _scale_win_cb_intercept_move);
	evas_object_intercept_resize_callback_del(it->o_win, _scale_win_cb_intercept_resize);
	evas_object_intercept_color_set_callback_del(it->o_win, _scale_win_cb_intercept_color);
	evas_object_intercept_layer_set_callback_del(it->o_win, _scale_win_cb_intercept_layer);

	/* evas_object_clip_unset(it->o_win); */
	/* edje_object_part_unswallow(it->o, it->o_win);  */
	evas_object_del(it->o);

	if (it->bd->desk != desk)
	  {
	     e_border_hide(it->bd, 2);
	     evas_object_hide(it->o_win);
	     evas_object_color_set(it->o_win, 255, 255, 255, 255);
	     evas_object_move(it->o_win, it->bd->x, it->bd->y);
	     evas_object_resize(it->o_win, it->cw->w, it->cw->h);
	  }

	e_object_unref(E_OBJECT(it->bd));
	E_FREE(it);
     }

   EINA_LIST_FREE(popups, it)
     {
	evas_object_event_callback_del(it->o_win, EVAS_CALLBACK_DEL, _scale_win_delorig);
	evas_object_color_set(it->o_win, 255, 255, 255, 255);
	E_FREE(it);
     }

   if (scale_conf->fade_desktop && background)
     {
	it = background;
	evas_object_event_callback_del(it->o_win, EVAS_CALLBACK_DEL, _scale_win_delorig);
	evas_object_color_set(it->o_win, 255, 255, 255, 255);
	E_FREE(it);
     }

   EINA_LIST_FREE(handlers, handler)
     ecore_event_handler_del(handler);

   e_msg_handler_del(msg_handler);
   msg_handler = NULL;
   zone = NULL;
   selected_item = NULL;
   current_desk = NULL;
   send_to_desk = EINA_FALSE;
   show_all_desks = EINA_FALSE;

   e_manager_comp_evas_update(e_manager_current_get());
}


static void
_scale_win_delorig(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Item *it = data;

   evas_object_event_callback_del(it->o_win, EVAS_CALLBACK_DEL, _scale_win_delorig);

   if (it->bd)
     {
	e_object_unref(E_OBJECT(it->bd));
	_scale_out();

	evas_object_intercept_move_callback_del(it->o_win, _scale_win_cb_intercept_move);
	evas_object_intercept_resize_callback_del(it->o_win, _scale_win_cb_intercept_resize);
	evas_object_intercept_color_set_callback_del(it->o_win, _scale_win_cb_intercept_color);
	evas_object_intercept_layer_set_callback_del(it->o_win, _scale_win_cb_intercept_layer);

	/* evas_object_clip_unset(it->o_win); */
	/* edje_object_part_unswallow(it->o, it->o_win); */
	evas_object_del(it->o);

	items = eina_list_remove(items, it);
     }
   else
     {
	popups = eina_list_remove(popups, it);
     }

   E_FREE(it);
}

static void
_scale_win_cb_intercept_move(void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Item *it = data;

   evas_object_move(obj, it->cur_x, it->cur_y);
}

static void
_scale_win_cb_intercept_resize(void *data, Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Item *it = data;

   evas_object_resize(obj, it->cur_w, it->cur_h);
}

static void
_scale_win_cb_intercept_color(void *data, Evas_Object *obj, int r, int g, int b, int a)
{
   Item *it = data;

   evas_object_color_set(obj, it->alpha, it->alpha, it->alpha, it->alpha);
}

static void
_scale_win_cb_intercept_layer(void *data, Evas_Object *obj, int l)
{
   Item *it = data;

   evas_object_layer_set(it->o_win, l);
   evas_object_stack_above(it->o, it->o_win);
}

static void
_scale_win_new(Evas *e, E_Manager *man, E_Manager_Comp_Source *src, E_Desk *desk)
{
   Item *it;

   if (!e_manager_comp_src_image_get(man, src)) return;

   E_Comp_Win *cw = (void*)src;

   if (!cw->bd)
     {
	if (cw->win == zone->container->bg_win)
	  {
	     if (scale_conf->fade_desktop)
	       {
		  it = E_NEW(Item, 1);
		  it->o_win = e_manager_comp_src_shadow_get(man, src);
		  evas_object_event_callback_add(it->o_win, EVAS_CALLBACK_DEL,
						 _scale_win_delorig, it);
		  background = it;
	       }
	  }
	else if (scale_conf->fade_popups)
	  {
	     it = E_NEW(Item, 1);
	     it->o_win = e_manager_comp_src_shadow_get(man, src);
	     evas_object_event_callback_add(it->o_win, EVAS_CALLBACK_DEL,
					    _scale_win_delorig, it);

	     popups = eina_list_append(popups, it);
	  }

   	return;
     }

   if (!cw->bd) return;

   if (cw->bd->zone != desk->zone)
     return;

   if ((!show_all_desks) && (cw->bd->desk != desk))
     return;

   if (cw->bd->iconic)
     return;

   it = E_NEW(Item, 1);
   it->scale = 1.0;

   it->dx = cw->bd->desk->x - desk->x;
   it->dy = cw->bd->desk->y - desk->y;

   it->x = cw->bd->x + it->dx * desk->zone->w;
   it->y = cw->bd->y + it->dy * desk->zone->h;
   it->w = cw->bd->w;
   it->h = cw->bd->h;

   it->bd_x = it->x;
   it->bd_y = it->y;

   it->cur_x = it->x;
   it->cur_y = it->y;
   it->cur_w = it->w;
   it->cur_h = it->h;

   e_object_ref(E_OBJECT(cw->bd));
   it->bd = cw->bd;
   it->cw = cw;

   it->o_win = e_manager_comp_src_shadow_get(man, src);
   it->o = edje_object_add(e);
   if (!e_theme_edje_object_set(it->o, "base/theme/modules/scale",
                                "modules/scale/win"))
     edje_object_file_set(it->o, scale_conf->theme_path, "modules/scale/win");

   evas_object_stack_above(it->o, it->o_win);
   /* edje_object_part_swallow(it->o, "e.swallow.win", it->o_win); */
   /* evas_object_clip_set(it->o_win, it->o);
    * evas_object_move(it->o, it->x, it->y); */

   evas_object_show(it->o);

   evas_object_event_callback_add(it->o_win, EVAS_CALLBACK_DEL, _scale_win_delorig, it);

   evas_object_intercept_move_callback_add(it->o_win, _scale_win_cb_intercept_move, it);
   evas_object_intercept_resize_callback_add(it->o_win, _scale_win_cb_intercept_resize, it);
   evas_object_intercept_color_set_callback_add(it->o_win, _scale_win_cb_intercept_color, it);
   evas_object_intercept_layer_set_callback_add(it->o_win, _scale_win_cb_intercept_layer, it);

   if (it->bd->desk != desk)
     {
	e_border_show(it->bd);
  	evas_object_move(it->o_win, it->x, it->y);
     }

   items = eina_list_append(items, it);
}

static int
_scale_grow()
{
   Item *it, *ot;
   Eina_List *l, *ll;

   int cont = 0;
   int overlap, off;
   double grow_l, grow_r, grow_d, grow_u;

   if (show_all_desks)
     off = scale_conf->desks_spacing;
   else
     off = scale_conf->spacing;

   /* double mean = 0; */
   /* EINA_LIST_FOREACH(items, l, it)
    *   mean += it->scale;
    *
    * mean /= (double) eina_list_count(items); */

   EINA_LIST_FOREACH(items, l, it)
     {
	overlap = 0;

	/* if (it->scale > mean)
	 *   continue; */

	if (it->w >= it->bd->w)
	  continue;
	if (it->h >= it->bd->h)
	  continue;

	if (it->bd->w > it->bd->h)
	  {
	     grow_l = grow_r = GROW;
	     grow_u = grow_d = GROW * (double)it->bd->h/(double)it->bd->w;
	  }
	else
	  {
	     grow_l = grow_r = GROW * (double)it->bd->w/(double)it->bd->h;
	     grow_u = grow_d = GROW;
	  }

	if (it->x - grow_l < min_x)
	  grow_l = 0;
	if (it->y - grow_u < min_y)
	  grow_u = 0;
	if (it->x + it->w + grow_r > max_x)
	  grow_r = 0;
	if (it->y + it->h + grow_d > max_y)
	  grow_d = 0;

	if ((grow_l + grow_r) == 0)
	  continue;
	if ((grow_u + grow_d) == 0)
	  continue;

	EINA_LIST_FOREACH(items, ll, ot)
	  {
	     if (it == ot)
	       continue;
	     if (grow_l && E_INTERSECTS(it->x - grow_l - off ,it->y,
					it->w + off*2, it->h + off*2,
					ot->x, ot->y, ot->w, ot->h))
	       grow_l = 0;

	     if (grow_r && E_INTERSECTS(it->x - off, it->y - off,
					it->w + grow_r + off*2, it->h + off*2,
					ot->x, ot->y, ot->w, ot->h))
	       grow_r = 0;

	     if ((grow_l == 0) && (grow_r == 0) && (overlap = 1))
	       break;

	     if (grow_u && E_INTERSECTS(it->x - off, it->y - off - grow_u,
					it->w + off*2, it->h + off*2,
					ot->x, ot->y, ot->w, ot->h))
	       grow_u = 0;

	     if (grow_d && E_INTERSECTS(it->x - off, it->y - off,
					it->w + off*2, it->h + grow_d + off*2,
					ot->x, ot->y, ot->w, ot->h))
	       grow_d = 0;

	     if ((grow_u == 0) && (grow_d == 0) && (overlap = 1))
	       break;
	  }

	if (overlap)
	  continue;

	if (it->bd->w > it->bd->h)
	  {
	     if ((grow_u > 0) && (grow_d > 0))
	       it->w += grow_l + grow_r;
	     else
	       it->w += MAX(grow_l, grow_r);

	     it->h = it->w * (double)it->bd->h / (double)it->bd->w;
	  }
	else
	  {
	     if ((grow_r > 0) && (grow_l > 0))
	       it->h += grow_u + grow_d;
	     else
	       it->h += MAX(grow_u, grow_d);

	     it->w = it->h * (double)it->bd->w / (double)it->bd->h;
	  }

	it->x -= grow_l;
	it->y -= grow_u;

	cont++;
     }

   return cont;
}

static void
_scale_displace(Item *it, Item *ot, int disp)
{
   /* cycle items with same center around to get even	distribution.
      dont try to understand this but these initial conditions
      are important.. you can have up to 9 maximized windows.
      if you can figure out more please tell me :) */

   if (disp % 8 == 0)
     {
	// 1.
	it->y -= 1;
	it->x -= 1;
	// 2.
	ot->x += 1;
	ot->y -= 1;
     }
   else if (disp % 8 == 1)
     {
	// 3.
	ot->y += 1;
     }
   else if (disp % 8 == 2)
     {
	// 4.
	ot->x -= 2;
	ot->y += 2;
     }
   else if (disp % 8 == 3)
     {
	// 5.
	ot->x += 2;
	ot->y += 2;
     }
   else if (disp % 8 == 4)
     {
	// 6.
	ot->x += 1;
	ot->y += 1;
     }
   else if (disp % 8 == 5)
     {
	// 7.
	ot->x -= 1;
	ot->y += 1;
     }
   else if (disp % 8 == 6)
     {
	// 8.
	ot->x -= 1;
	ot->y -= 1;
     }
   else if (disp % 8 == 7)
     {
	ot->x += 1;
	ot->y -= 1;
     }
}

static int
_scale_place(int offset)
{
   Item *it, *ot;
   Eina_List *l, *ll;
   int overlap = 0;
   int outside = 0;

   EINA_LIST_FOREACH(items, l, it)
     {
	it->mx = it->x;
	it->my = it->y;
     }

   int disp = 0;

   EINA_LIST_FOREACH(items, l, it)
     {
	EINA_LIST_FOREACH(items, ll, ot)
	  {
	     int w = it->w;
	     int h = it->h;

	     if (it == ot)
	       continue;

	     if (!E_INTERSECTS(it->x - offset, it->y - offset,
			       it->w + offset*2, it->h + offset*2,
			       ot->x, ot->y, ot->w, ot->h))
	       continue;

	     overlap += 1;

	     it->overlaps++;

	     if (it->x < ot->x)
	       w += it->x - ot->x;
	     if (w < 0) w = 0;

	     if (it->x + it->w > ot->x + ot->w)
	       w = ot->x + ot->w - it->x;

	     if (it->y < ot->y)
	       h += it->y - ot->y;
	     if (h < 0) h = 0;

	     if (it->y + it->h > ot->y + ot->h)
	       h = ot->y + ot->h - it->y;

	     double dist_y = (it->y + it->h/2) - (ot->y + ot->h/2);
	     double dist_x = (it->x + it->w/2) - (ot->x + ot->w/2);

	     if (dist_x == 0 && dist_y == 0)
	       {
		  _scale_displace(it, ot, disp);
		  disp++;
	       }
	     else if (w > h)
	       {
		  if (dist_y)
		    {
		       dist_y = (dist_y > 0 ? 2 : -2);
		       it->my += dist_y;
		    }
		  if (dist_x)
		    {
		       dist_x = (dist_x > 0 ? 1 : -1);
		       it->mx += dist_x;
		    }
	       }
	     else //if (w < h)
	       {
		  if (dist_y)
		    {
		       dist_y = (dist_y > 0 ? 1 : -1);
		       it->my += dist_y;
		    }
		  if (dist_x)
		    {
		       dist_x = (dist_x > 0 ? 2 : -2);
		       it->mx += dist_x;
		    }
	       }
	  }
     }

   EINA_LIST_FOREACH(items, l, it)
     {
   	it->x = it->mx;
   	it->y = it->my;

   	if (it->x < min_x)
	  {
	     outside = 1;
	     it->x = min_x;
	  }

   	if (it->y < min_y)
	  {
	     outside = 1;
	     it->y = min_y;
	  }

   	if (it->x + it->w > max_x)
	  {
	     outside = 1;
	     it->x = max_x - it->w;
	  }

   	if (it->y + it->h > max_y)
	  {
	     outside = 1;
	     it->y = max_y - it->h;
	  }
     }

   if (!(overlap || outside))
     return 0;

   if (outside && (step_count++ > 50))
     {
	double zone_diag = sqrt(zone->w * zone->h);
	double sw, sh;
	step_count = 0;
	EINA_LIST_FOREACH(items, l, it)
	  {
	     if (!it->overlaps)
	       continue;

	     if (it->scale <= 0.005)
	       continue;

	     it->scale -= it->scale *
	       (0.001 + (sqrt(it->bd->w * it->bd->h) / zone_diag) / 50.0);

	     sw = (double)it->bd->w * it->scale;
	     sh = (double)it->bd->h * it->scale;
	     it->x += (it->w - sw)/2.0;
	     it->y += (it->h - sh)/2.0;
	     it->w = sw;
	     it->h = sh;

	     it->overlaps = 0;
	  }
	return 1;
     }

   return overlap || outside;
}

static int
_scale_shrink()
{
   Eina_List *l, *ll;
   Item *it, *ot;
   int shrunk = 0;
   int off;
   double move_x;
   double move_y;

   int min_x = zone->w;
   int min_y = zone->h;
   int max_x = 0;
   int max_y = 0;

   if (show_all_desks)
     off = scale_conf->desks_spacing;
   else
     off = scale_conf->spacing;


   EINA_LIST_REVERSE_FOREACH(items, l, it)
     {
	if (show_all_desks)
	  {
	     move_x = ((it->x + it->w/2.0) - (max_x - min_x)/2) / 5.0;
	     move_y = ((it->y + it->h/2.0) - (max_y - min_y)/2) / 5.0;
	  }
	else
	  {
	     move_x = ((it->x + it->w/2.0) - (double)(it->bd->x + it->bd->w/2.0)) / 10.0;
	     move_y = ((it->y + it->h/2.0) - (double)(it->bd->y + it->bd->h/2.0)) / 10.0;
	  }

	if (!(move_y || move_x))
	  continue;

	EINA_LIST_FOREACH(items, ll, ot)
	  {
	     if (it == ot) continue;

	     while(move_x)
	       {
		  if (E_INTERSECTS(it->x - move_x, it->y, it->w, it->h,
				   ot->x - off, ot->y - off, ot->w + off*2, ot->h + off*2))
		    move_x = move_x / 2.0;
		  else break;
	       }

	     while(move_y)
	       {
		  if (E_INTERSECTS(it->x, it->y - move_y, it->w, it->h,
				   ot->x - off, ot->y - off, ot->w + off*2, ot->h + off*2))
		    move_y = move_y / 2.0;
		  else break;
	       }

	     if (!(move_y || move_x)) break;
	  }

	it->x -= move_x;
	it->y -= move_y;

	if (move_y > 1 || move_x > 1)
	  shrunk++;
     }
   printf("shrunk___ %d\n",shrunk);

   return shrunk;
}


/* TODO add slot item an calc distance only once */
static Slot *cur_slot = NULL;

static double
_slot_dist(const Item *it, const Slot *slot)
{
   double dx = it->x - slot->x;
   double dy = it->y - slot->y;
   double dw = (it->x + it->w) - (slot->x + slot->w);
   double dh = (it->y + it->h) - (slot->y + slot->h);

   return (sqrt(dx*dx + dy*dy + dw*dw + dh*dh));
}

static int
_cb_sort_nearest(const void *d1, const void *d2)
{
   const Item *it1 = d1;
   const Item *it2 = d2;

   return (_slot_dist(it1, cur_slot) < _slot_dist(it2, cur_slot)) ? -1 : 1;
}

static int
_scale_place_slotted()
{
   Eina_List *l, *ll, *slots = NULL;
   Slot *slot, *slot2;
   Item *it;
   int rows, cols, cnt, x, y, w, h;
   int fast = 0;
   int cont = 0;

   cnt = eina_list_count(items);

   rows = sqrt(cnt);
   cols = cnt/rows;

   if (cols*rows < cnt)
     cols += 1;

   if (cnt == 3)
     {
	cols = 2;
	rows = 2;
     }

   DBG("%d rows, %d cols -- cnt %d\n", rows, cols, cnt);

   double min_x, max_x, min_y, max_y;
   max_x = max_y = 0;
   min_x = min_y = 100000;

   EINA_LIST_FOREACH(items, l, it)
     {
   	if (it->x < min_x) min_x = it->x;
   	if (it->y < min_y) min_y = it->y;
   	if (it->x + it->w > max_x) max_x = it->x + it->w;
   	if (it->y + it->h > max_y) max_y = it->y + it->h;
     }

   w = (max_x - min_x) / cols;
   h = (max_y - min_y) / rows;
   l = items;

   for (y = 0; y < rows; y++)
     {
	for (x = 0; x < cols; x++)
	  {
	     slot = E_NEW(Slot, 1);
	     slot->x = min_x + x * w;
	     slot->y = min_y + y * h;
	     slot->w = w;
	     slot->h = h;
	     slot->items = eina_list_clone(items);
	     cur_slot = slot;

	     if (fast)
	       {
		  slot->it = eina_list_data_get(l);
		  if (l) l = l->next;
	       }
	     else
	       {
		  slot->items = eina_list_sort(slot->items, cnt, _cb_sort_nearest);
		  slot->it = eina_list_data_get(slot->items);
		  slot->items = eina_list_remove_list(slot->items, slot->items);
	       }

	     slot->min = _slot_dist(slot->it, slot);
	     slots = eina_list_append(slots, slot);

	     DBG("add slot: %dx%d,   \t%f -> %d:%d\n", slot->x, slot->y, slot->min,
		 (int)(slot->it->x), (int)(slot->it->y));
	  }
     }

   if (!fast)
     {
	cont = 1;
	EINA_LIST_FOREACH(items, l, it)
	  it->in_slots = cols * rows;
     }

   while (cont)
     {
	cont = 0;
	EINA_LIST_FOREACH(slots, l, slot)
	  {
	     EINA_LIST_FOREACH(slots, ll, slot2)
	       {
		  if (slot == slot2)
		    continue;

		  if (slot->it != slot2->it)
		    continue;

		  Item *it1 = eina_list_data_get(slot->items);
		  Item *it2 = eina_list_data_get(slot2->items);
		  if (it1 && it2)
		    {
		       double d1 = _slot_dist(it1, slot);
		       double d2 = _slot_dist(it2, slot2);

		       cont = 1;

		       DBG("%dx%d - compare:\n\ts1: %dx%d (%dx%d:%f),\n\ts2 %dx%d (%dx%d:%f)\n",
			   (int)slot->it->x, (int)slot->it->y,
			   slot->x, slot->y, (int)it1->x, (int)it1->y, d1,
			   slot2->x, slot2->y, (int)it2->x, (int)it2->y, d2);

		       if (slot->it->in_slots > 1 && slot->min + d1 >= slot2->min + d2)
			 {

			    slot->it->in_slots--;
			    slot->it = it1;
			    slot->min = d1;
			    slot->items = eina_list_remove_list(slot->items, slot->items);
			    break;
			 }
		    }
	       }
	  }
     }

   do {
        cont = 0;
	EINA_LIST_FOREACH(slots, l, slot)
	  {
	     EINA_LIST_FOREACH(l->next, ll, slot2)
	       {
		  double d1, d2;

		  d1 = _slot_dist(slot->it, slot) + _slot_dist(slot2->it, slot2);
		  d2 = _slot_dist(slot->it, slot2) + _slot_dist(slot2->it, slot);
		  if (d1 > d2)
		    {
		       it = slot->it;

		       slot->it = slot2->it;
		       slot2->it = it;
		       cont = 1;
		    }
	       }
	  }
     }
   while (fast && cont);

   EINA_LIST_FOREACH(slots, l, slot)
     {
	if (!slot->it)
	  continue;

	EINA_LIST_FOREACH(slots, ll, slot2)
	  {
	     if (slot == slot2)
	       continue;

	     if (slot->it != slot2->it)
	       continue;

	     if (_slot_dist(slot->it, slot) > _slot_dist(slot2->it, slot2))
	       slot->it = NULL;
	     else
	       slot2->it = NULL;
	  }
     }

   w = zone->w / cols;
   h = zone->h / rows;

   l = slots;

   for (y = 0; l && y < rows; y++)
     {
	for (x = 0; l &&  x < cols; x++)
	  {
	     slot = eina_list_data_get(l);

	     slot->x = x * w;
	     slot->y = y * h;
	     slot->w = w;
	     slot->h = h;

	     l = eina_list_next(l);
	  }
     }
   int spacing = scale_conf->spacing;

   EINA_LIST_FOREACH(slots, l, slot)
     {
	if (slot->it)
	  {
	     it = slot->it;

	     if (it->w > slot->w - spacing)
	       {
		  it->w = slot->w - spacing;
		  it->h = it->w * (double)it->bd->h / (double)it->bd->w;
	       }
	     if (it->h > slot->h - spacing)
	       {
		  it->h = slot->h - spacing;
		  it->w = it->h * (double)it->bd->w / (double)it->bd->h;
	       }
	     it->x = slot->x + (slot->w - it->w)/2.0;
	     it->y = slot->y + (slot->h - it->h)/2.0;
	     /* printf("place: %d:%d %dx%d -> %d:%d %dx%d\n",
	      * 	    (int)it->bd_x, (int)it->bd_y, (int)it->bd->w, (int)it->bd->h,
	      * 	    (int)it->x, (int)it->y, (int)it->w, (int)it->h); */
	  }

	EINA_LIST_FREE(slot->items, it);
	E_FREE(slot);
     }

   return 1;
}


static int
_cb_sort_center(const void *d1, const void *d2)
{
   const Item *it1 = d1;
   const Item *it2 = d2;

   double dx1 = ((it1->x + it1->w/2.0) - (double)(max_width/2));
   double dy1 = ((it1->y + it1->h/2.0) - (double)(max_height/2));

   double dx2 = ((it2->x + it2->w/2.0) - (double)(max_width/2));
   double dy2 = ((it2->y + it2->h/2.0) - (double)(max_height/2));

   return (sqrt(dx1*dx1 + dy1*dy1) > sqrt(dx2*dx2 + dy2*dy2)) ? -1 : 1;
}

static void
_scale_place_natural()
{
   Eina_List *l;
   int spacing, i = 0;
   Item *it;

   max_width  = zone->w;
   max_height = zone->h;

   if (show_all_desks)
     {
	max_width  = zone->h * zone->desk_x_count;
	max_height = zone->w * zone->desk_y_count;
     }

   items = eina_list_sort(items, eina_list_count(items), _cb_sort_center);

   if (show_all_desks)
     spacing = scale_conf->desks_spacing;
   else
     spacing = scale_conf->spacing;

   if (scale_conf->grow && (spacing < SPACING))
     spacing = SPACING;
   if (scale_conf->tight && (spacing < SPACING))
     spacing = SPACING;

   step_count = 0;

   while ((i++ < PLACE_RUNS) &&
	  (_scale_place(spacing) ||
	   (min_x < use_x) ||
	   (min_y < use_y) ||
	   (max_x > use_w) ||
	   (max_y > use_h)))
     {
	/* shrink region to visible region */
	if (min_x < use_x) min_x += 2;
	if (min_y < use_y) min_y += 2;
	if (min_x > use_x) min_x = use_x;
	if (min_y > use_y) min_y = use_y;

	if (max_x > use_w) max_x -= 2;
	if (max_y > use_h) max_y -= 2;
	if (max_x < use_w) max_x = use_w;
	if (max_y < use_h) max_y = use_h;

	if (!show_all_desks)
	  continue;

	/* move other desks windows into visible region */
	EINA_LIST_FOREACH(items, l, it)
	  {
	     if ((min_x < use_x) && (it->dx < 0) && it->x < 0) it->x += 4.0;
	     if ((min_y < use_y) && (it->dy < 0) && it->y < 0) it->y += 4.0;

	     if ((max_x > use_w) && (it->dx > 0) && it->x > zone->w) it->x -= 4.0;
	     if ((max_y > use_h) && (it->dy > 0) && it->y > zone->h) it->y -= 4.0;
	  }
     }

   DBG("place %d\n", i);
}

void
scale_run(E_Manager *man)
{
   Eina_List *l;
   E_Manager_Comp_Source *src;
   Evas *e;
   int i, spacing;
   Item *it;

   zone = e_util_zone_current_get(e_manager_current_get());
   current_desk = e_desk_current_get(zone);

   start_time = ecore_time_get();

   input_win = ecore_x_window_input_new(zone->container->win, 0, 0, 1, 1);
   ecore_x_window_show(input_win);
   if (!e_grabinput_get(input_win, 0, input_win))
     {
	ecore_x_window_free(input_win);
	input_win = 0;
	return;
     }

   handlers = eina_list_append
     (handlers, ecore_event_handler_add
      (ECORE_EVENT_MOUSE_BUTTON_DOWN, _scale_cb_mouse_down, NULL));

   handlers = eina_list_append
     (handlers, ecore_event_handler_add
      (ECORE_EVENT_MOUSE_MOVE, _scale_cb_mouse_move, NULL));

   e = e_manager_comp_evas_get(man);

   EINA_LIST_FOREACH((Eina_List *)e_manager_comp_src_list(man), l, src)
     {
	_scale_win_new(e, man, src, current_desk);
     }

   if ((eina_list_count(items) < 2) && (!show_all_desks))
     {
	_scale_finish();
	return;
     }

   if (show_all_desks)
     spacing = scale_conf->desks_spacing;
   else
     spacing = scale_conf->spacing;

   if (!scale_conf->fade_popups)
     {
	e_zone_useful_geometry_get(zone, &use_x, &use_y, &use_w, &use_h);
	use_x += spacing;
	use_y += spacing;
	use_w += use_x - spacing*2;
	use_h += use_y - spacing*2;
     }
   else
     {
	use_x = use_y = spacing;
	use_w = zone->w - spacing*2;
	use_h = zone->h - spacing*2;
     }

   min_x = -zone->w * zone->desk_x_current;
   min_y = -zone->h * zone->desk_y_current;
   max_x =  zone->w + zone->w * ((zone->desk_x_count - 1) - zone->desk_x_current);
   max_y =  zone->h + zone->h * ((zone->desk_y_count - 1) - zone->desk_y_current);

   if (show_all_desks)
     {
	if (scale_conf->desks_layout_mode)
	  _scale_place_slotted();
	else
	  _scale_place_natural();
     }
   else
     {
	if (scale_conf->layout_mode)
	  _scale_place_slotted();
	else
	  _scale_place_natural();
     }

   min_x = use_x;
   min_y = use_y;
   max_x = use_h;
   max_y = use_w;

   if (scale_conf->grow)
     {
	i = 0;
   	while (i++ < GROW_RUNS && _scale_grow());
	DBG("grow %d", i);
     }

   if (scale_conf->tight)
     {
	items = eina_list_sort(items, eina_list_count(items), _cb_sort_center);
	i = 0;
	while (i++ < SHRINK_RUNS && _scale_shrink());
	DBG("shrunk %d", i);
     }

   if (show_all_desks)
     {
	/* center and move windows near visible desk
	 * to make the sliding smoother */

	min_x = zone->w;
	min_y = zone->h;
	max_x = 0;
	max_y = 0;

	EINA_LIST_FOREACH(items, l, it)
	  {
	     if (it->x < min_x) min_x = it->x;
	     if (it->y < min_y) min_y = it->y;
	     if (it->x + it->w > max_x) max_x = it->x + it->w;
	     if (it->y + it->h > max_y) max_y = it->y + it->h;
	  }

	EINA_LIST_FOREACH(items, l, it)
	  {
	     it->x = it->x - min_x + ((use_x + use_w) - (max_x - min_x))/2;
	     it->y = it->y - min_y + ((use_y + use_h) - (max_y - min_y))/2;

	     if (it->dx > 0) it->bd_x =   zone->w + it->bd->x/4;
	     if (it->dy > 0) it->bd_y =   zone->h + it->bd->y/4;
	     if (it->dx < 0) it->bd_x = -(zone->w - it->bd->x)/4;
	     if (it->dy < 0) it->bd_y = -(zone->h + it->bd->y)/4;
	  }
     }

   DBG("time: %f\n", ecore_time_get() - start_time);

   _scale_in();
}


static Eina_Bool
_scale_cb_mouse_move(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Move *ev = event;
   Item *it;
   Eina_List *l;

   if (ev->window != input_win)
     return ECORE_CALLBACK_PASS_ON;

   if (!scale_state)
     return ECORE_CALLBACK_PASS_ON;

   EINA_LIST_FOREACH(items, l, it)
     if (E_INTERSECTS(ev->x, ev->y, 1, 1, it->x, it->y, it->w, it->h))
       break;

   if (selected_item && (it != selected_item))
     {
	edje_object_signal_emit(selected_item->o, "mouse,out", "e");
	selected_item = NULL;
     }

   if (it)
     {
	edje_object_signal_emit(it->o, "mouse,in", "e");
	selected_item = it;
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_scale_cb_mouse_down(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Button *ev = event;
   Item *it, *ot;
   Eina_List *l;

   if (ev->window != input_win)
     return ECORE_CALLBACK_PASS_ON;

   if (!scale_state)
     return ECORE_CALLBACK_PASS_ON;

   EINA_LIST_FOREACH(items, l, it)
     if (E_INTERSECTS(ev->x, ev->y, 1, 1, it->x, it->y, it->w, it->h))
       break;

   if (!it)
     {
	_scale_out();
	return ECORE_CALLBACK_PASS_ON;
     }

   e_border_raise(it->bd);


   if (it->bd->desk != e_desk_current_get(it->bd->zone))
     {
	if (ev->buttons == 1)
	  {
	     selected_item = it;
	     current_desk = it->bd->desk;

	     EINA_LIST_FOREACH(items, l, ot)
	       {
		  if (ot->bd->desk == it->bd->desk)
		    {
		       ot->bd_x = ot->bd->x;
		       ot->bd_y = ot->bd->y;
		    }
		  else
		    {
		       if (ot->dx > it->dx)
			 ot->bd_x = ot->bd->x + zone->w;
		       else if (ot->dx < it->dx)
			 ot->bd_x = ot->bd->x - zone->w;

		       if (ot->dy > it->dy)
			 ot->bd_y = ot->bd->y + zone->h;
		       else if (ot->dy < it->dy)
			 ot->bd_y = ot->bd->y - zone->h;
		    }
	       }
	  }
	else if (ev->buttons == 3)
	  {
	     send_to_desk = EINA_TRUE;
	     selected_item = it;
	     it->bd_x = it->bd->x;
	     it->bd_y = it->bd->y;
	  }
     }

   _scale_out();

   return ECORE_CALLBACK_PASS_ON;
}

static void
_scale_handler(void *data, const char *name, const char *info, int val,
	       E_Object *obj, void *msgdata)
{
   E_Manager *man = (E_Manager *)obj;
   E_Manager_Comp_Source *src = (E_Manager_Comp_Source *)msgdata;
   Evas *e;

   /* DBG("handler... '%s' '%s'\n", name, info); */
   if (strcmp(name, "comp.manager")) return;

   e = e_manager_comp_evas_get(man);
   if (!strcmp(info, "change.comp"))
     {
        if (!e) DBG("TTT: No comp manager\n");
        else DBG("TTT: comp canvas = %p\n", e);
     }
   else if (!strcmp(info, "resize.comp"))
     {
        DBG("%s: %p | %p\n", info, man, src);
     }
   else if (!strcmp(info, "add.src"))
     {
        /* DBG("%s: %p | %p\n", info, man, src); */
        _scale_win_new(e, man, src, e_desk_current_get(e_util_zone_current_get(man)));

     }
   else if (!strcmp(info, "del.src"))
     {
        DBG("%s: %p | %p\n", info, man, src);
     }
   else if (!strcmp(info, "config.src"))
     {

        DBG("%s: %p | %p\n", info, man, src);
     }
   /* else if (!strcmp(info, "visible.src"))
    *   {
    *      DBG("%s: %p | %p\n", info, man, src);
    *   } */
}

static void
_e_mod_action_cb_edge(E_Object *obj, const char *params, E_Event_Zone_Edge *ev)
{
   Eina_List *list, *l;
   E_Manager *man;

   msg_handler = e_msg_handler_add(_scale_handler, NULL);
   list = e_manager_list();

   if (params && !strcmp(params, "show_all_desks"))
     show_all_desks = EINA_TRUE;
   else
     show_all_desks = EINA_FALSE;

   EINA_LIST_FOREACH(list, l, man)
     {
        Evas *e = e_manager_comp_evas_get(man);
        if (e) scale_run(man);
     }
}

static void
_e_mod_action_cb(E_Object *obj, const char *params)
{
   Eina_List *list, *l;
   E_Manager *man;

   msg_handler = e_msg_handler_add(_scale_handler, NULL);
   list = e_manager_list();

   if (params && !strcmp(params, "show_all_desks"))
     show_all_desks = EINA_TRUE;
   else
     show_all_desks = EINA_FALSE;

   EINA_LIST_FOREACH(list, l, man)
     {
        Evas *e = e_manager_comp_evas_get(man);
        if (e) scale_run(man);
     }
}


/* Module and Gadcon stuff */

typedef struct _Instance Instance;

struct _Instance
{
  E_Gadcon_Client *gcc;
  Evas_Object *o_scale;
  E_Menu *menu;
  Config_Item *conf_item;
};

static int uuid = 0;
static Eina_List *instances = NULL;
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;
Config *scale_conf = NULL;

static const E_Gadcon_Client_Class _gc_class =
  {
    GADCON_CLIENT_CLASS_VERSION, "scale",
    {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon,
     _gc_id_new, NULL, NULL},
    E_GADCON_CLIENT_STYLE_PLAIN
  };

EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "Scale"};

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   snprintf(buf, sizeof(buf), "%s/e-module-scale.edj", m->dir);

   e_configure_registry_category_add("appearance", 80, D_("Look"),
                                     NULL, "preferences-look");
   e_configure_registry_item_add("appearance/comp-scale", 110, D_("Composite Scale Windows"),
                                 NULL, buf, e_int_config_scale_module);

   conf_item_edd = E_CONFIG_DD_NEW("Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, switch2, INT);

   conf_edd = E_CONFIG_DD_NEW("Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_VAL(D, T, version, INT);
   E_CONFIG_VAL(D, T, grow, UCHAR);
   E_CONFIG_VAL(D, T, tight, UCHAR);
   E_CONFIG_VAL(D, T, fade_popups, UCHAR);
   E_CONFIG_VAL(D, T, fade_desktop, UCHAR);
   E_CONFIG_VAL(D, T, fade_windows, UCHAR);
   E_CONFIG_VAL(D, T, fade_desktop, UCHAR);
   E_CONFIG_VAL(D, T, scale_duration, DOUBLE);
   E_CONFIG_VAL(D, T, spacing, DOUBLE);
   E_CONFIG_VAL(D, T, desks_duration, DOUBLE);
   E_CONFIG_VAL(D, T, desks_spacing, DOUBLE);
   E_CONFIG_VAL(D, T, layout_mode, INT);
   E_CONFIG_VAL(D, T, desks_layout_mode, INT);
   E_CONFIG_LIST(D, T, conf_items, conf_item_edd);

   scale_conf = e_config_domain_load("module.scale", conf_edd);
   if (scale_conf)
     {
	if (!e_util_module_config_check(D_("Scale Windows"),
					scale_conf->version,
					MOD_CONFIG_FILE_EPOCH,
					MOD_CONFIG_FILE_VERSION))
	  _scale_conf_free();
     }

   if (!scale_conf) _scale_conf_new();

   scale_conf->module = m;

   snprintf(buf, sizeof(buf), "%s/e-module-scale.edj",
            scale_conf->module->dir);
   scale_conf->theme_path = eina_stringshare_add(buf);

   e_gadcon_provider_register(&_gc_class);

   act = e_action_add("scale-windows");
   if (act)
     {
	act->func.go = _e_mod_action_cb;
	act->func.go_edge = _e_mod_action_cb_edge;
	e_action_predef_name_set(D_("Desktop"), D_("Scale Windows"), "scale-windows", "", NULL, 0);
	e_action_predef_name_set(D_("Desktop"), D_("Scale Windows / All Desktops"), "scale-windows", "show_all_desks", NULL, 0);
     }

   return m;
}


EAPI int
e_modapi_shutdown(E_Module *m)
{
   e_configure_registry_item_del("appearance/comp-scale");
   e_configure_registry_category_del("appearance");

   if (scale_conf->cfd) e_object_del(E_OBJECT(scale_conf->cfd));
   scale_conf->cfd = NULL;

   scale_conf->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);

   while (scale_conf->conf_items)
     {
        Config_Item *ci = NULL;
        ci = scale_conf->conf_items->data;

        scale_conf->conf_items =
          eina_list_remove_list(scale_conf->conf_items,
                                scale_conf->conf_items);

        if (ci->id) eina_stringshare_del(ci->id);
        E_FREE(ci);
     }

   eina_stringshare_del(scale_conf->theme_path);
   E_FREE(scale_conf);

   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);

   if (act)
     {
	e_action_predef_name_del(D_("Desktop"),
				 D_("Scale Windows"));
	e_action_del("scale-windows");
     }

   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save("module.scale", conf_edd, scale_conf);
   return 1;
}

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Instance *inst = NULL;

   inst = E_NEW(Instance, 1);
   inst->conf_item = _scale_conf_item_get(id);

   inst->o_scale = edje_object_add(gc->evas);

   if (!e_theme_edje_object_set(inst->o_scale, "base/theme/modules/scale",
                                "modules/scale/main"))
     edje_object_file_set(inst->o_scale, scale_conf->theme_path, "modules/scale/main");

   inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->o_scale);
   inst->gcc->data = inst;

   evas_object_event_callback_add(inst->o_scale, EVAS_CALLBACK_MOUSE_DOWN,
                                  _scale_gc_cb_mouse_down, inst);

   instances = eina_list_append(instances, inst);

   return inst->gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst = NULL;

   if (!(inst = gcc->data)) return;
   instances = eina_list_remove(instances, inst);

   if (inst->menu)
     {
        e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
        e_object_del(E_OBJECT(inst->menu));
        inst->menu = NULL;
     }
   if (inst->o_scale)
     {
        evas_object_event_callback_del(inst->o_scale, EVAS_CALLBACK_MOUSE_DOWN,
                                       _scale_gc_cb_mouse_down);
        evas_object_del(inst->o_scale);
     }
   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(E_Gadcon_Client_Class *client_class)
{
   return D_("Scale");
}

static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class)
{
   Config_Item *ci = NULL;

   ci = _scale_conf_item_get(NULL);
   return ci->id;
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas)
{
   Evas_Object *o = NULL;

   o = edje_object_add(evas);

   edje_object_file_set(o, scale_conf->theme_path, "icon");

   return o;
}

static void
_scale_conf_new(void)
{
   scale_conf = E_NEW(Config, 1);
   scale_conf->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) if ((scale_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

   IFMODCFG(0x0001);
   scale_conf->grow = 1;
   scale_conf->tight = 1;
   scale_conf->scale_duration = 0.4;
   scale_conf->spacing = 10;
   scale_conf->fade_windows = 1;
   scale_conf->fade_popups = 0;
   scale_conf->fade_desktop = 1;
   scale_conf->desks_duration = 0.6;
   scale_conf->desks_spacing = 44;
   scale_conf->layout_mode = 0;
   scale_conf->desks_layout_mode = 1;
   _scale_conf_item_get(NULL);
   IFMODCFGEND;

   scale_conf->version = MOD_CONFIG_FILE_VERSION;

   e_config_save_queue();
}

static void
_scale_conf_free(void)
{
   while (scale_conf->conf_items)
     {
        Config_Item *ci = NULL;

        ci = scale_conf->conf_items->data;
        scale_conf->conf_items =
          eina_list_remove_list(scale_conf->conf_items,
                                scale_conf->conf_items);
        if (ci->id) eina_stringshare_del(ci->id);
        E_FREE(ci);
     }

   E_FREE(scale_conf);
}

static Config_Item *
_scale_conf_item_get(const char *id)
{
   Eina_List *l = NULL;
   Config_Item *ci = NULL;
   char buf[128];

   if (!id)
     {
        snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, ++uuid);
        id = buf;
     }
   else
     {
        uuid++;
        for (l = scale_conf->conf_items; l; l = l->next)
          {
             if (!(ci = l->data)) continue;
             if ((ci->id) && (!strcmp(ci->id, id))) return ci;
          }
     }
   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->switch2 = 0;
   scale_conf->conf_items = eina_list_append(scale_conf->conf_items, ci);
   return ci;
}

static void
_scale_gc_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event)
{
   Instance *inst = NULL;
   Evas_Event_Mouse_Down *ev;
   E_Zone *zone = NULL;
   E_Menu_Item *mi = NULL;
   int x, y;

   if (!(inst = data)) return;
   ev = event;

   if (ev->button == 1)
     {
	_e_mod_action_cb(NULL, NULL);
     }
   else if (ev->button == 2)
     {
	_e_mod_action_cb(NULL, "show_all_desks");
     }
   else if ((ev->button == 3) && (!inst->menu))
     {
        E_Menu *ma, *mg;

        zone = e_util_zone_current_get(e_manager_current_get());

        ma = e_menu_new();
        e_menu_post_deactivate_callback_set(ma, _scale_gc_cb_menu_post, inst);
        inst->menu = ma;

        mg = e_menu_new();

        mi = e_menu_item_new(mg);
        e_menu_item_label_set(mi, D_("Settings"));
        e_util_menu_item_theme_icon_set(mi, "preferences-system");
        e_menu_item_callback_set(mi, _scale_gc_cb_menu_configure, NULL);

        e_gadcon_client_util_menu_items_append(inst->gcc, ma, mg, 0);
        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y,
                                          NULL, NULL);

        e_menu_activate_mouse(ma, zone, (x + ev->output.x),
                              (y + ev->output.y), 1, 1,
                              E_MENU_POP_DIRECTION_AUTO, ev->timestamp);
        evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
                                 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

static void
_scale_gc_cb_menu_post(void *data, E_Menu *menu)
{
   Instance *inst = NULL;

   if (!(inst = data)) return;
   if (!inst->menu) return;
   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

/* call configure from popup */
static void
_scale_gc_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi)
{
   if (!scale_conf) return;
   if (scale_conf->cfd) return;
   e_int_config_scale_module(mn->zone->container, NULL);
}

