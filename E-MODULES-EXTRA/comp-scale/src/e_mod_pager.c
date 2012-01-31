#include <e.h>
#include "e_mod_main.h"

#define OFFSET 2.5

typedef struct _Item Item;

struct _Item
{
  Evas_Object *o, *o_win;
  E_Border *bd;
  E_Desk *desk;
  E_Manager_Comp_Source *src;
  E_Manager *man;

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

  int mouse_down;
  int moved;
};

static Eina_Bool _pager_cb_mouse_down(void *data, int type, void *event);
static Eina_Bool _pager_cb_mouse_up(void *data, int type, void *event);
static Eina_Bool _pager_cb_mouse_move(void *data, int type, void *event);

static void _pager_win_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _pager_win_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _pager_win_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _pager_win_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _pager_win_cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _pager_win_cb_delorig(void *data, Evas *e, Evas_Object *obj, void *event_info);

static Item *_pager_win_new(Evas *e, E_Manager *man, E_Manager_Comp_Source *src);
static void _pager_win_del(Item *it);

static void _pager_finish();
static void _pager_handler(void *data, const char *name, const char *info, int val, E_Object *obj, void *msgdata);


static Ecore_X_Window input_win = 0;
static E_Msg_Handler *msg_handler = NULL;

static Eina_List *items = NULL;
static Eina_List *desks = NULL;
static Eina_List *popups = NULL;

#define SCALE_STATE_OUT  0
#define SCALE_STATE_IN   1
#define SCALE_STATE_HOLD 2

static Eina_List *handlers = NULL;
static Ecore_Animator *scale_animator = NULL;
static int scale_state = SCALE_STATE_OUT;
static double start_time;
static E_Zone *zone = NULL;
static Evas_Object *bg_over = NULL;
static Item *selected_item = NULL;
static E_Desk *previous_desk = NULL;
static E_Desk *current_desk = NULL;
static E_Desk *initial_desk = NULL;
static int min_x, min_y, max_x, max_y;
static double desk_w, desk_h;
static double zone_w, zone_h, zone_x, zone_y;
static double zoom = 0.0;
static int mouse_activated = 0;
static int mouse_x, mouse_y;
static Evas_Object *zone_clip = NULL;
static int smooth = 0;

static void
_pager_place_desks(double scale)
{
   int x, y;
   Eina_List *l = desks;
   int a = (1.0 - scale) * 255.0;

   for (y = 0; y < zone->desk_y_count; y++)
     {
	for (x = 0; x < zone->desk_x_count; x++)
	  {
	     double x1 = (x - initial_desk->x) * zone_w;
	     double y1 = (y - initial_desk->y) * zone_h;
	     double x2 = min_x + x * desk_w;
	     double y2 = min_y + y * desk_h;
	     double cur_x = (scale * x1) + (1.0 - scale) * x2;
	     double cur_y = (scale * y1) + (1.0 - scale) * y2;

	     Evas_Object *o = eina_list_data_get(l);

	     int w = (scale * (zone->w)) + (1.0 - scale) * desk_w - OFFSET;
	     int h = (scale * (zone->h)) + (1.0 - scale) * desk_h - OFFSET;

	     evas_object_move(o, cur_x, cur_y);
	     evas_object_resize(o, w, h);

	     if (scale_conf->pager_fade_desktop)
	       evas_object_color_set(o, a, a, a, a);

	     l = eina_list_next(l);
	  }
     }
}

static void
_pager_place_windows(double scale)
{
   Eina_List *l;
   Item *it;

   EINA_LIST_FOREACH(items, l, it)
     {
	if (!it->o) continue;
	it->cur_x = it->bd_x * scale + it->x * (1.0 - scale);
	it->cur_y = it->bd_y * scale + it->y * (1.0 - scale);
	it->cur_w = (double)(it->bd_x + it->bd->w) * scale + (it->x + it->w) * (1.0 - scale) - it->cur_x;
	it->cur_h = (double)(it->bd_y + it->bd->h) * scale + (it->y + it->h) * (1.0 - scale) - it->cur_y;

	evas_object_move(it->o, it->cur_x, it->cur_y);
	evas_object_resize(it->o, it->cur_w, it->cur_h);
     }
}

static Eina_Bool
_pager_redraw(void *data)
{
   double in;
   Eina_Bool finish = EINA_FALSE;
   Eina_List *l;
   Item *it;


   if (scale_state == SCALE_STATE_HOLD)
     {
	e_manager_comp_evas_update(e_manager_current_get());
	return ECORE_CALLBACK_RENEW;
     }

   in = (ecore_loop_time_get() - start_time) / scale_conf->pager_duration;

   if (in >= 1.0)
     {
	if (scale_state == SCALE_STATE_IN)
	  {
	     in = 0.0;
	     scale_state = SCALE_STATE_HOLD;
	  }
	else
	  {
	     in = 1.0;
	     finish = EINA_TRUE;
	  }
     }
   else if (scale_state)
     {
	in = log(14) * in;
	in = 1.0 / exp(in*in);
     }
   else
     {
	in = log(14) * (1.0 - in);
	in = 1.0 / exp(in*in);
     }

   _pager_place_desks(in);
   _pager_place_windows(in);

   if (scale_conf->pager_fade_windows)
     {
	EINA_LIST_FOREACH(items, l, it)
	  {
	     double a = 255.0;

	     if (!it->o) continue;

	     if ((it->desk != current_desk) && (it->desk != previous_desk))
	       a = 255.0 * (1.0 - in);

	     evas_object_color_set(it->o, a, a, a, a);
	  }
     }

   double a = 255.0 * in;

   if (scale_conf->pager_fade_popups)
     {
	EINA_LIST_FOREACH(popups, l, it)
	  evas_object_color_set(it->o_win, a, a, a, a);
     }

   EINA_LIST_FOREACH(items, l, it)
     {
	if (it->o) continue;
	evas_object_color_set(it->o_win, a, a, a, a);
     }

   if (scale_conf->pager_fade_desktop)
     {
   	double a = (1.0 - in) * 155.0;
   	evas_object_color_set(bg_over, 0, 0, 0, a);
     }

   /* e_manager_comp_evas_update(e_manager_current_get()); */

   if (finish)
     {
	scale_animator = NULL;
	_pager_finish();
	return ECORE_CALLBACK_CANCEL;
     }

   return ECORE_CALLBACK_RENEW;
}

static void
_pager_in()
{
   start_time = ecore_loop_time_get();
   scale_state = SCALE_STATE_IN;

   if (!scale_animator)
     scale_animator = ecore_animator_add(_pager_redraw, NULL);
}
static void
_pager_out()
{
   Item *it;
   Eina_List *l;
   double duration, now;

   now = ecore_loop_time_get();
   duration = scale_conf->pager_duration;

   if (now - start_time < duration)
     start_time = now - (duration - (now - start_time));
   else
     start_time = now;

   if (!scale_animator)
     scale_animator = ecore_animator_add(_pager_redraw, NULL);

   EINA_LIST_FOREACH(items, l, it)
     {
	edje_object_signal_emit(it->o, "hide", "e");
	/* edje_object_signal_emit(it->o, "mouse,in", "e"); */
	it->bd_x = it->bd->x + (it->desk->x - current_desk->x) * zone_w;
	it->bd_y = it->bd->y + (it->desk->y - current_desk->y) * zone_h;
     }

   initial_desk = current_desk;
   scale_state = SCALE_STATE_OUT;
}

static void
_pager_finish()
{
   Ecore_Event_Handler *handler;
   Item *it;
   E_Desk *desk;
   Evas_Object *o, *oo;
   Eina_List *l;

   desk = e_desk_current_get(zone);

   EINA_LIST_FOREACH(items, l, it)
     {
	if (it->desk == it->bd->desk)
	  continue;

	/* dont hide border - hack */
	//if (it->desk == current_desk)
        // it->bd->visible = 0;

	e_border_desk_set(it->bd, it->desk);

	// if (it->desk == current_desk)
	//  it->bd->visible = 1;
     }

   if ((current_desk) && (desk != current_desk))
     {
	/* dont do flip animation */
	int tmp = e_config->desk_flip_animate_mode;
	e_config->desk_flip_animate_mode = 0;
	e_desk_show(current_desk);
	e_config->desk_flip_animate_mode = tmp;
     }

   EINA_LIST_FREE(items, it)
     _pager_win_del(it);

   EINA_LIST_FREE(popups, it)
     _pager_win_del(it);

   EINA_LIST_FREE(desks, o)
     {
	oo = edje_object_part_swallow_get(o, "e.swallow.desk");
	if (oo) evas_object_del(oo);

	evas_object_del(o);
     }

   EINA_LIST_FREE(handlers, handler)
     ecore_event_handler_del(handler);

   e_grabinput_release(input_win, input_win);
   ecore_x_window_free(input_win);
   input_win = 0;

   if (scale_animator)
     ecore_animator_del(scale_animator);

   scale_animator = NULL;
   evas_object_del(bg_over);
   evas_object_del(zone_clip);
   bg_over = NULL;
   zone_clip = NULL;

   e_msg_handler_del(msg_handler);
   msg_handler = NULL;
   zone = NULL;
   selected_item = NULL;
   current_desk = NULL;
   previous_desk = NULL;
}

static E_Desk *
_pager_desk_at_xy_get(double x, double y)
{
   x = ((x * zoom) - (min_x * zoom)) / (double)zone_w;
   y = ((y * zoom) - (min_y * zoom)) / (double)zone_h;
   if (x > zone->desk_x_count - 1) x = zone->desk_x_count - 1;
   if (y > zone->desk_y_count - 1) y = zone->desk_y_count - 1;

   return e_desk_at_xy_get(zone, x, y);
}

static void
_pager_desk_select(E_Desk *desk)
{
   Evas_Object *o_desk;

   if (current_desk && (desk != current_desk))
     {
	o_desk = eina_list_nth(desks, current_desk->y * zone->desk_x_count + current_desk->x);
	edje_object_signal_emit(o_desk, "unfocused", "e");
     }

   previous_desk = current_desk;
   current_desk = desk;

   o_desk = eina_list_nth(desks, desk->y * zone->desk_x_count + desk->x);
   edje_object_signal_emit(o_desk, "focused", "e");
}

static void
_pager_win_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Item *it = data;
   Evas_Event_Mouse_Down *ev = event_info;

   if (scale_state == SCALE_STATE_OUT)
     return;

   if ((ev->button == 2) || (ev->flags & EVAS_BUTTON_DOUBLE_CLICK))
     {
	selected_item = it;

	if (current_desk != it->desk)
	  _pager_desk_select(it->desk);

	_pager_out();
	return;
     }
   else if (ev->button == 1)
     {
	e_border_raise(it->bd);

	evas_object_raise(it->o);

	if (current_desk != it->desk)
	  _pager_desk_select(it->desk);
     }

   it->mx = ev->canvas.x;
   it->my = ev->canvas.y;
   it->mouse_down = EINA_TRUE;
}

static void
_pager_win_final_position_set(Item *it)
{
   /* move to desk position */
   it->x = it->desk->x * desk_w;
   it->y = it->desk->y * desk_h;
   /* center desks*/
   it->x += min_x + 1.0; /* XXX get offset from desk theme */
   it->y += min_y + 1.0;
   /* move window to relative position on desk */
   it->x += (double)(it->bd->x - zone_x) * (1.0 / zoom);
   it->y += (double)(it->bd->y - zone_y) * (1.0 / zoom);
}

static void
_pager_win_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Item *it = data;
   int x = it->bd->x;
   int y = it->bd->y;

   if (!it->mouse_down)
     return;

   it->mouse_down = EINA_FALSE;

   if (scale_state == SCALE_STATE_OUT)
     return;

   if (!it->moved)
     return;

   if (x + it->bd->w > zone_x + zone_w) x = zone_x + zone_w - it->bd->w;
   if (y + it->bd->h > zone_y + zone_h) y = zone_y + zone_h - it->bd->h;
   if (x < zone->x) x = zone_x;
   if (y < zone->y) y = zone_y;

   e_border_move(it->bd, x, y);

   _pager_win_final_position_set(it);

   it->cur_x = it->x;
   it->cur_y = it->y;

   evas_object_move(it->o, it->cur_x, it->cur_y);
}

static void
_pager_win_cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev = event_info;
   Item *it = data;
   int x, y;
   E_Desk *desk, *desk2;

   if (!it || !it->mouse_down)
     return;

   if (abs(ev->cur.canvas.x - it->mx) +
       abs(ev->cur.canvas.y - it->my) < 15)
     return;

   it->moved = EINA_TRUE;

   x = it->x + (ev->cur.canvas.x - ev->prev.canvas.x);
   y = it->y + (ev->cur.canvas.y - ev->prev.canvas.y);

   if (x < min_x)
     x = min_x;
   else if (x + it->w > max_x)
     x = max_x - it->w;

   if (y < min_y)
     y = min_y;
   else if (y + it->h > max_y)
     y = max_y - it->h;

   desk = _pager_desk_at_xy_get(ev->cur.canvas.x, ev->cur.canvas.y);
   if (!desk) return;

   if ((!it->bd->fullscreen) && (!it->bd->maximized))
     {
	it->x = x;
	it->y = y;
	it->cur_x = x;
	it->cur_y = y;

	it->bd_x = ((it->x * zoom) - min_x * zoom);
	it->bd_y = ((it->y * zoom) - min_y * zoom);

	x = (int)(it->bd_x) % (int)zone_w;
	y = (int)(it->bd_y) % (int)zone_h;

	if ((desk2 = _pager_desk_at_xy_get(it->x, it->y)))
	  {
	     x += (desk2->x - desk->x) * zone_w + zone_x;
	     y += (desk2->y - desk->y) * zone_h + zone_y;
	  }

	if (current_desk != desk)
	  _pager_desk_select(desk);

	e_border_move(it->bd, x, y);

	it->desk = desk;
     }
   else if (desk && (desk != it->desk))
     {
	it->desk = desk;

	_pager_desk_select(desk);
	_pager_win_final_position_set(it);

	it->cur_x = it->x;
	it->cur_y = it->y;
     }

   evas_object_move(it->o, it->cur_x, it->cur_y);
}

static void
_pager_win_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Item *it = data;

   if (!mouse_activated)
     return;

   if (scale_state == SCALE_STATE_OUT)
     return;

   mouse_activated = 0;

   if (selected_item && (it != selected_item))
     {
	edje_object_signal_emit(selected_item->o, "mouse,out", "e");
	selected_item = NULL;
     }

   if (it != selected_item)
     {
	edje_object_signal_emit(it->o, "mouse,in", "e");
	selected_item = it;
     }
}

static void
_pager_win_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Item *it = data;

   if (!mouse_activated)
     return;

   if (selected_item == it)
     {
	edje_object_signal_emit(it->o, "mouse,out", "e");
	selected_item = NULL;
     }
}

static void
_pager_win_cb_delorig(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Item *it = data;

   if (it->bd)
     items = eina_list_remove(items, it);
   else
     popups = eina_list_remove(popups, it);

   _pager_win_del(it);
}

static void
_pager_win_del(Item *it)
{
   evas_object_event_callback_del(it->o_win, EVAS_CALLBACK_DEL,
				  _pager_win_cb_delorig);

   if (it->bd && !it->o)
     {
	e_manager_comp_src_hidden_set(it->man, it->src, EINA_FALSE);

	e_object_unref(E_OBJECT(it->bd));
     }
   else if (it->bd)
     {
	evas_object_event_callback_del(it->o, EVAS_CALLBACK_MOUSE_IN,
				       _pager_win_cb_mouse_in);

	evas_object_event_callback_del(it->o, EVAS_CALLBACK_MOUSE_OUT,
				       _pager_win_cb_mouse_out);

	evas_object_event_callback_del(it->o, EVAS_CALLBACK_MOUSE_DOWN,
				       _pager_win_cb_mouse_down);

	evas_object_event_callback_del(it->o, EVAS_CALLBACK_MOUSE_UP,
				       _pager_win_cb_mouse_up);

	evas_object_event_callback_del(it->o, EVAS_CALLBACK_MOUSE_MOVE,
				       _pager_win_cb_mouse_move);


	if ((it->bd->desk != current_desk) && (!it->bd->sticky))
	  e_border_hide(it->bd, 2);

	e_manager_comp_src_hidden_set(it->man, it->src, EINA_FALSE);

	evas_object_del(it->o_win);
	evas_object_del(it->o);

	e_object_unref(E_OBJECT(it->bd));
     }
   else
     {
	evas_object_color_set(it->o_win, 255, 255, 255, 255);
     }

   E_FREE(it);
}

static Item *
_pager_win_new(Evas *e, E_Manager *man, E_Manager_Comp_Source *src)
{
   Item *it;
   Evas_Object *o, *sh_obj, *obj;
   E_Border *bd;
   
   sh_obj = e_manager_comp_src_shadow_get(man, src);
   if (!sh_obj) return NULL;

   obj = e_manager_comp_src_image_get(man, src);
   if (!obj) return NULL;

   bd = e_manager_comp_src_border_get(man, src);
   
   if (!bd)
     {
        Ecore_X_Window win = e_manager_comp_src_window_get(man, src);
        
	if (win == zone->container->bg_win)
	  {
	     smooth = evas_object_image_smooth_scale_get(obj);

	     o = evas_object_rectangle_add(e);
	     evas_object_move(o, zone->x, zone->y);
	     evas_object_resize(o, zone->w, zone->h);
	     evas_object_show(o);
	     bg_over = o;

	     evas_object_stack_above(bg_over, sh_obj);

	     if (scale_conf->pager_fade_desktop)
	       evas_object_color_set(bg_over, 0, 0, 0, 0);
	     else
	       evas_object_color_set(bg_over, 0, 0, 0, 255);

	     return NULL;
	  }
	else if (scale_conf->pager_fade_popups)
	  {
             E_Popup *pop = e_manager_comp_src_popup_get(man, src);
             
	     if ((pop) && (pop->zone != zone))
	       return NULL;

	     it = E_NEW(Item, 1);
	     it->man = man;
	     it->o_win = sh_obj;
	     it->src = src;
	     evas_object_event_callback_add(it->o_win, EVAS_CALLBACK_DEL,
					    _pager_win_cb_delorig, it);

	     popups = eina_list_append(popups, it);
	     return it;
	  }
	return NULL;
     }

   if (bd->zone != zone)
     return NULL;

   if (bd->iconic)
     return NULL;

   it = E_NEW(Item, 1);
   it->bd = bd;
   it->desk = it->bd->desk;
   it->man = man;
   it->src = src;
   e_object_ref(E_OBJECT(it->bd));

   e_manager_comp_src_hidden_set(man, src, EINA_TRUE);

   items = eina_list_append(items, it);

   evas_object_event_callback_add(it->o_win, EVAS_CALLBACK_DEL,
				  _pager_win_cb_delorig, it);

   if ((it->bd->client.netwm.state.skip_pager) || (e_mod_border_ignore(it->bd)))
     return NULL;

   it->o_win = e_manager_comp_src_image_mirror_add(man, src);
   /* it->o_win = evas_object_image_filled_add(e);
    * o = e_manager_comp_src_image_get(man, src);
    * evas_object_image_source_set(it->o_win, o); */
   /* evas_object_show(o); */

   it->o = edje_object_add(e);
   if (!e_theme_edje_object_set(it->o, "base/theme/modules/scale",
				"modules/scale/win"))
     edje_object_file_set(it->o, scale_conf->theme_path,
			  "modules/scale/win");

   evas_object_stack_above(it->o, it->o_win);
   evas_object_show(it->o);
   edje_object_part_swallow(it->o, "e.swallow.win", it->o_win);
   evas_object_clip_set(it->o, zone_clip);

   evas_object_event_callback_add(it->o, EVAS_CALLBACK_MOUSE_IN,
				  _pager_win_cb_mouse_in, it);

   evas_object_event_callback_add(it->o, EVAS_CALLBACK_MOUSE_OUT,
				  _pager_win_cb_mouse_out, it);

   evas_object_event_callback_add(it->o, EVAS_CALLBACK_MOUSE_DOWN,
				  _pager_win_cb_mouse_down, it);

   evas_object_event_callback_add(it->o, EVAS_CALLBACK_MOUSE_UP,
				  _pager_win_cb_mouse_up, it);

   evas_object_event_callback_add(it->o, EVAS_CALLBACK_MOUSE_MOVE,
				  _pager_win_cb_mouse_move, it);

   it->dx = it->desk->x - current_desk->x;
   it->dy = it->desk->y - current_desk->y;

   it->x = it->bd->x + it->dx * zone_w;
   it->y = it->bd->y + it->dy * zone_h;

   it->bd_x = it->x;
   it->bd_y = it->y;

   it->cur_x = it->x;
   it->cur_y = it->y;
   it->cur_w = it->bd->w;
   it->cur_h = it->bd->h;

   if (it->desk != current_desk)
     e_border_show(it->bd);

   _pager_win_final_position_set(it);

   it->w = (double)it->bd->w / zoom - OFFSET*2.0;
   it->h = (double)it->bd->h / zoom - OFFSET*2.0;

   edje_object_part_text_set(it->o, "e.text.label", e_border_name_get(it->bd));
   edje_object_signal_emit(it->o, "show", "e");

   if (it->bd != e_border_focused_get())
     edje_object_signal_emit(it->o, "mouse,out", "e");
   else
     {
	edje_object_signal_emit(it->o, "mouse,in", "e");
	selected_item = it;
     }

   if (scale_state)
     _pager_redraw(NULL);

   return it;
}

static Eina_Bool
_pager_cb_mouse_move(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Move *ev = event;

   if (ev->window != input_win)
     return ECORE_CALLBACK_PASS_ON;

   if (scale_state == SCALE_STATE_OUT)
     return ECORE_CALLBACK_PASS_ON;

   if (mouse_x < 0)
     {
	mouse_x = ev->x;
	mouse_y = ev->y;
	return ECORE_CALLBACK_PASS_ON;
     }
   if ((mouse_x == ev->x) || (mouse_y == ev->y))
     return ECORE_CALLBACK_PASS_ON;
   mouse_activated = 1;

   evas_event_feed_mouse_move((Evas *) data, ev->x, ev->y,
			      ev->timestamp, NULL);

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_pager_cb_mouse_up(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Button *ev = event;
   Evas_Button_Flags flags = EVAS_BUTTON_NONE;

   if (ev->window != input_win)
     return ECORE_CALLBACK_PASS_ON;

   if (scale_state == SCALE_STATE_OUT)
     return ECORE_CALLBACK_PASS_ON;

   evas_event_feed_mouse_up((Evas *)data, ev->buttons,
			    flags, ev->timestamp, NULL);

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_pager_cb_mouse_down(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Button *ev = event;
   Evas_Button_Flags flags = EVAS_BUTTON_NONE;
   Item *it;
   Eina_List *l;

   if (ev->window != input_win)
     return ECORE_CALLBACK_PASS_ON;

   if (scale_state == SCALE_STATE_OUT)
     return ECORE_CALLBACK_PASS_ON;

   EINA_LIST_FOREACH(items, l, it)
     if (E_INTERSECTS(ev->x, ev->y, 1, 1,
		      it->x, it->y, it->w, it->h))
       break;

   if (!it)
     {
	if (E_INSIDE(ev->x, ev->y, min_x, min_y,
		     (max_x - min_x) - OFFSET*2,
		     (max_y - min_y) - OFFSET*2))
	  {
	     E_Desk *desk = _pager_desk_at_xy_get(ev->x, ev->y);
	     if (desk)
	       _pager_desk_select(desk);
	  }

	_pager_out();
	return ECORE_CALLBACK_PASS_ON;
     }

   if (ev->double_click) flags |= EVAS_BUTTON_DOUBLE_CLICK;
   if (ev->triple_click) flags |= EVAS_BUTTON_TRIPLE_CLICK;
   evas_event_feed_mouse_down((Evas *)data, ev->buttons, flags,
			      ev->timestamp, NULL);

   return ECORE_CALLBACK_PASS_ON;
}

static void
_pager_switch(const char *params)
{
   int max_x,max_y, desk_x, desk_y;
   int x = 0, y = 0;
   E_Desk *desk;

   if (!params[0])
     {
	_pager_out();
	return;
     }

   e_zone_desk_count_get(zone, &max_x, &max_y);

   if (!strcmp(params, "_left"))
     x = -1;
   else if (!strcmp(params, "_right"))
     x = 1;
   else if (!strcmp(params, "_up"))
     y = -1;
   else if (!strcmp(params, "_down"))
     y = 1;
   else if (!strcmp(params, "_next"))
     {
	x = 1;
	if (current_desk->x == max_x - 1)
	  y = 1;
     }
   else if (!strcmp(params, "_prev"))
     {
	x = -1;
	if (current_desk->x == 0)
	  y = -1;
     }

   desk_x = current_desk->x + x;
   desk_y = current_desk->y + y;

   if (desk_x < 0)
     desk_x = max_x - 1;
   else if (desk_x >= max_x)
     desk_x = 0;

   if (desk_y < 0)
     desk_y = max_y - 1;
   else if (desk_y >= max_y)
     desk_y = 0;

   desk = e_desk_at_xy_get(zone, desk_x, desk_y);
   if (desk) _pager_desk_select(desk);

   /* if (!scale_state)
    *   {
    *     Eina_List *l;
    *     Item *it;
    *
    *     EINA_LIST_FOREACH(items, l, it)
    * 	 evas_object_color_set(it->o, 255.0, 255.0, 255.0, 255.0);
    *   } */
}

static Eina_Bool
_pager_cb_key_down(void *data, int type, void *event)
{
  Ecore_Event_Key *ev = event;

   if (ev->window != input_win)
     return ECORE_CALLBACK_PASS_ON;

   if (!strcmp(ev->key, "Up"))
     _pager_switch("_up");
   else if (!strcmp(ev->key, "Down"))
     _pager_switch("_down");
   else if (!strcmp(ev->key, "Left"))
     _pager_switch("_left");
   else if (!strcmp(ev->key, "Right"))
     _pager_switch("_right");
   else if (!strcmp(ev->key, "p"))
     _pager_switch("_prev");
   else if (!strcmp(ev->key, "n"))
     _pager_switch("_next");
   else if (!strcmp(ev->key, "Return"))
     _pager_out();
   else if (!strcmp(ev->key, "space"))
     _pager_out();
   else if (!strcmp(ev->key, "Escape"))
     {
	_pager_desk_select(e_desk_current_get(zone));
	_pager_out();
     }
   else
     {
	E_Action *act;
	Eina_List *l;
	E_Config_Binding_Key *bind;
	E_Binding_Modifier mod;

	for (l = e_config->key_bindings; l; l = l->next)
	  {
	     bind = l->data;
	     mod = 0;

	     if (bind->action && strcmp(bind->action, "scale-windows")) continue;
	     if (!bind->params || strncmp(bind->params, "go_pager", 8)) continue;

	     if (ev->modifiers & ECORE_EVENT_MODIFIER_SHIFT)
               mod |= E_BINDING_MODIFIER_SHIFT;
	     if (ev->modifiers & ECORE_EVENT_MODIFIER_CTRL)
               mod |= E_BINDING_MODIFIER_CTRL;
	     if (ev->modifiers & ECORE_EVENT_MODIFIER_ALT)
               mod |= E_BINDING_MODIFIER_ALT;
	     if (ev->modifiers & ECORE_EVENT_MODIFIER_WIN)
               mod |= E_BINDING_MODIFIER_WIN;

	     if (bind->key && (!strcmp(bind->key, ev->keyname)) &&
		 ((bind->modifiers == mod) || (bind->any_mod)))
	       {
		  if (!(act = e_action_find(bind->action))) continue;
		  if (act->func.go_key)
		    act->func.go_key(E_OBJECT(zone), bind->params, ev);
		  else if (act->func.go)
		    act->func.go(E_OBJECT(zone), bind->params);
	       }
	  }
     }
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_pager_cb_key_up(void *data, int type, void *event)
{
  Ecore_Event_Key *ev = event;

  if (ev->window != input_win)
    return ECORE_CALLBACK_PASS_ON;

  if (scale_state == SCALE_STATE_OUT)
    return ECORE_CALLBACK_PASS_ON;

  if (!e_mod_hold_modifier_check(event))
    _pager_out();

  return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_pager_run(E_Manager *man)
{
   Evas *e;
   E_Manager_Comp_Source *src;
   Ecore_Event_Handler *h;
   const char *file, *group;
   Eina_List *l;
   int x, y;
   Item *it;


   mouse_activated = 0;
   mouse_x = -1;
   mouse_y = -1;

   if (input_win) return EINA_FALSE;

   e = e_manager_comp_evas_get(man);
   if (!e) return EINA_FALSE;

   zone = e_util_zone_current_get(man);
   if (!zone)
     return EINA_FALSE;

   initial_desk = current_desk = e_desk_current_get(zone);
   if (!current_desk)
     return EINA_FALSE;

   input_win = ecore_x_window_input_new(zone->container->win,
					0, 0, 1, 1);
   ecore_x_window_show(input_win);

   if (!e_grabinput_get(input_win, 0, input_win))
     {
	ecore_x_window_free(input_win);
	input_win = 0;
	return EINA_FALSE;
     }

   msg_handler = e_msg_handler_add(_pager_handler, NULL);

   h = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN,
			       _pager_cb_mouse_down, e);
   handlers = eina_list_append(handlers, h);
   h = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP,
			       _pager_cb_mouse_up, e);
   handlers = eina_list_append(handlers, h);
   h = ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE,
			       _pager_cb_mouse_move, e);
   handlers = eina_list_append(handlers, h);
   h = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
			       _pager_cb_key_down, e);
   handlers = eina_list_append(handlers, h);
   h = ecore_event_handler_add(ECORE_EVENT_KEY_UP,
			       _pager_cb_key_up, e);
   handlers = eina_list_append(handlers, h);

   zoom = zone->desk_x_count;
   if (zoom < zone->desk_y_count)
     zoom = zone->desk_y_count;

   if (scale_conf->pager_keep_shelves)
     {
	int x, y, w, h;
	e_zone_useful_geometry_get(zone, &x, &y, &w, &h);
	zone_w = w;
	zone_h = h;
	zone_x = zone->x + x;
	zone_y = zone->y + y;
     }
   else
     {
	zone_w = zone->w;
	zone_h = zone->h;
	zone_x = zone->x;
	zone_y = zone->y;
     }

   desk_w = (zone_w / zoom);
   desk_h = (zone_h / zoom);

   min_x = OFFSET + zone_x + (zoom - zone->desk_x_count) * desk_w / 2.0;
   min_y = OFFSET + zone_y + (zoom - zone->desk_y_count) * desk_h / 2.0;

   max_x = min_x + (zone->desk_x_count * desk_w);
   max_y = min_y + (zone->desk_y_count * desk_h);

   zone_clip = evas_object_rectangle_add(e);
   evas_object_move(zone_clip, zone_x, zone_y);
   evas_object_resize(zone_clip, zone_w, zone_h);
   evas_object_show(zone_clip);

   EINA_LIST_FOREACH((Eina_List *)e_manager_comp_src_list(man), l, src)
     _pager_win_new(e, man, src);

   edje_object_file_get(zone->bg_object, &file, &group);

   for (y = 0; y < zone->desk_y_count; y++)
     {
	for (x = 0; x < zone->desk_x_count; x++)
	  {
	     Evas_Object *o, *oo;

	     o = edje_object_add(e);

	     if (!e_theme_edje_object_set(o, "base/theme/modules/scale", "modules/scale/desk"))
	       edje_object_file_set(o, scale_conf->theme_path, "modules/scale/desk");

	     evas_object_stack_above(o, bg_over);
	     evas_object_show(o);

	     desks = eina_list_append(desks, o);

	     if ((x != zone->desk_x_current) || (y != zone->desk_y_current))
	       edje_object_signal_emit(o, "unfocused", "e");
	     evas_object_clip_set(o, zone_clip);

	     if (edje_object_part_exists(o, "e.swallow.desk"))
	       {
		  oo = edje_object_add(e);
		  edje_object_file_set(oo, file, group);
		  evas_object_resize(oo, zone->w, zone->h);
		  edje_extern_object_max_size_set(oo, 65536, 65536);
		  edje_extern_object_min_size_set(oo, 0, 0);

	     	  /* oo = evas_object_image_filled_add(e);
	     	   * evas_object_image_source_set(oo, bg_proxy);
	     	   * evas_object_image_smooth_scale_set(oo, smooth); */

		  /* printf("add %p\n", oo); */
	     	  edje_object_part_swallow(o, "e.swallow.desk", oo);
	       }
	  }
     }

   _pager_place_desks(1.0);
   _pager_place_windows(1.0);

   EINA_LIST_FOREACH(items, l, it)
     {
	evas_object_image_smooth_scale_set(it->o_win, smooth);

	if ((scale_conf->pager_fade_windows) && (it->bd->desk != current_desk))
	  evas_object_color_set(it->o, 0, 0, 0, 0);
     }

   evas_event_feed_mouse_in(e, ecore_x_current_time_get(), NULL);
   evas_event_feed_mouse_move(e, -1000000, -1000000,
                              ecore_x_current_time_get(), NULL);

   return EINA_TRUE;
}

Eina_Bool
pager_run(E_Manager *man, const char *params, int init_method)
{
   Eina_Bool ret = EINA_FALSE;

   if (scale_state == SCALE_STATE_OUT)
     {
	if (input_win)
	  return ret;

	if (scale_animator)
	  {
	     ecore_animator_del(scale_animator);
	     scale_animator = NULL;
	  }

	ret = _pager_run(man);

	if (ret)
	  {
	    if (strlen(params) > 8)
	      _pager_switch(params+8);

	    _pager_in();
	  }
     }
   else
     {
	_pager_switch(params+8);
     }

   return ret;
}

static void
_pager_handler(void *data, const char *name, const char *info, int val,
	       E_Object *obj, void *msgdata)
{
  E_Manager *man = (E_Manager *)obj;
  E_Manager_Comp_Source *src = (E_Manager_Comp_Source *)msgdata;
  Evas *e;

  /* if (!scale_state) return; */

  if (strcmp(name, "comp.manager")) return;

  DBG("handler... '%s' '%s'\n", name, info);

  /* XXX disabled for now. */
  /* return; */

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
      DBG("%s: %p | %p\n", info, man, src);
      e_manager_comp_src_hidden_set(man, src, EINA_TRUE);
    }
  else if (!strcmp(info, "del.src"))
    {
      DBG("%s: %p | %p\n", info, man, src);
    }
  else if (!strcmp(info, "config.src"))
    {

      DBG("%s: %p | %p\n", info, man, src);
    }
  else if (!strcmp(info, "visibility.src"))
    {
      DBG("%s: %p | %p\n", info, man, src);
      /* _pager_win_new(e, man, src);       */
    }
}
