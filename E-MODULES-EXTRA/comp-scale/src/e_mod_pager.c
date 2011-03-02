#include <e.h>
#include "e_mod_main.h"

#define OFFSET 2.5

typedef struct _Item Item;

struct _Item
{
  Evas_Object *o, *o_win;
  E_Border *bd;
  E_Desk *desk;
  E_Comp_Win *cw;
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

  int mouse_down;
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
static void _pager_win_cb_intercept_move(void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _pager_win_cb_intercept_resize(void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _pager_win_cb_intercept_color(void *data, Evas_Object *obj, int r, int g, int b, int a);
static void _pager_win_cb_intercept_stack_above(void *data, Evas_Object *obj, Evas_Object *above);

static void _pager_win_new(Evas *e, E_Manager *man, E_Manager_Comp_Source *src);
static void _pager_win_del(Item *it);

static void _pager_finish();
static void _pager_handler(void *data, const char *name, const char *info, int val, E_Object *obj, void *msgdata);



static Ecore_X_Window input_win = 0;
static E_Msg_Handler *msg_handler = NULL;

static Eina_List *items = NULL;
static Eina_List *desks = NULL;
static Eina_List *popups = NULL;

static Eina_List *handlers = NULL;
static Ecore_Animator *scale_animator = NULL;
static Eina_Bool scale_state = EINA_FALSE;
static double start_time;
static E_Zone *zone = NULL;
static Item *background = NULL;
static Item *selected_item = NULL;
static E_Desk *previous_desk = NULL;
static E_Desk *current_desk = NULL;
static int min_x, min_y, max_x, max_y;
static double desk_w, desk_h;
static double zoom = 0.0;

static void
_pager_place_desks(double scale)
{
   Evas_Object *o;
   int x, y;
   Eina_List *l = desks;

   int a = (1.0 - scale) * 255.0;

   for (y = 0; y < zone->desk_y_count; y++)
     {
	for (x = 0; x < zone->desk_x_count; x++)
	  {
	     o = eina_list_data_get(l);

	     evas_object_move(o,
			      (scale * (x - current_desk->x) * zone->w) +
			      (1.0 - scale) * (min_x + x * desk_w),
			      (scale * (y - current_desk->y) * zone->h) +
			      (1.0 - scale) * (min_y + y * desk_h));
	     evas_object_resize(o,
				(scale * zone->w) +
				(1.0 - scale) * (desk_w - OFFSET),
				(scale * zone->h) +
				(1.0 - scale) * (desk_h - OFFSET));

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
	it->cur_w = it->bd->w * scale + it->w * (1.0 - scale);
	it->cur_h = it->bd->h * scale + it->h * (1.0 - scale);
	it->cur_x = it->bd_x  * scale + it->x * (1.0 - scale);
	it->cur_y = it->bd_y  * scale + it->y * (1.0 - scale);

	evas_object_move(it->o, it->cur_x, it->cur_y);
	evas_object_resize(it->o, it->cur_w, it->cur_h);
     }
}

static Eina_Bool
_pager_redraw(void *data)
{
   Eina_List *l;
   Item *it;
   double scale, a, in, duration;

   duration = scale_conf->pager_duration;
   scale = (ecore_time_get() - start_time) / duration;

   if (!scale_state)
     scale = 1.0 - scale;

   if (scale > 1.0) scale = 1.0;
   if (scale < 0.0) scale = 0.0;

   in = log(10) * scale;
   in = 1.0 / exp(in*in);

   if (in > 1.0) in = 1.0;
   if (in < 0.0) in = 0.0;

   _pager_place_desks(in);
   _pager_place_windows(in);

   if (scale_conf->pager_fade_windows)
     {
	EINA_LIST_FOREACH(items, l, it)
	  {
	    if ((it->desk != current_desk) &&
		(it->desk != previous_desk))
	       {
		  double ax = it->cur_x - it->x;
		  double ay = it->cur_y - it->y;
		  double bx = it->bd_x  - it->x;
		  double by = it->bd_y  - it->y;

		  a = (1.0 - sqrt(ax*ax + ay*ay) /
		       sqrt(bx*bx + by*by)) * 220;
	       }
	     else
	       {
		 //a = 220.0 + (35.0 * in);
		 a = 255.0;
	       }

	     it->alpha = a;
	     evas_object_color_set(it->o_win, a, a, a, a);
	  }
     }

   if (scale_conf->pager_fade_popups)
     {
	a = 255.0 * in;

	EINA_LIST_FOREACH(popups, l, it)
	  evas_object_color_set(it->o_win, a, a, a, a);
     }

   if (scale_conf->pager_fade_desktop && background)
     {
	a = 255.0 * (0.5 + in/2.0);

	evas_object_color_set(background->o_win, a, a, a, 255);
     }

   if (scale < 1.0 && scale > 0.0)
     return 1;

   if (scale == 0.0)
     _pager_finish();
   else
     {
	_pager_place_windows(0.0);
	_pager_place_desks(0.0);
     }
   scale_animator = NULL;
   return 0;
}

static void
_pager_in()
{
   start_time = ecore_time_get();
   scale_state = EINA_TRUE;

   if (!scale_animator)
     scale_animator = ecore_animator_add(_pager_redraw, NULL);
}
static void
_pager_out()
{
   Item *it;
   Eina_List *l;
   double duration, now;

   now = ecore_time_get();
   duration = scale_conf->pager_duration;

   if (now - start_time < duration)
     start_time = now - (duration - (now - start_time));
   else
     start_time = now;

   if (!scale_animator)
     scale_animator = ecore_animator_add(_pager_redraw, NULL);

   if (selected_item)
     edje_object_signal_emit(selected_item->o, "hide", "e");

   EINA_LIST_FOREACH(items, l, it)
     {
	it->bd_x = it->bd->x + (it->desk->x - current_desk->x) * zone->w;
	it->bd_y = it->bd->y + (it->desk->y - current_desk->y) * zone->h;
     }

   scale_state = EINA_FALSE;
}

static void
_pager_finish()
{
   Ecore_Event_Handler *handler;
   Item *it;
   E_Desk *desk;
   Eina_List *l;
   Evas_Object *o;

   e_grabinput_release(input_win, input_win);
   ecore_x_window_free(input_win);
   input_win = 0;

   desk = e_desk_current_get(zone);

   EINA_LIST_FOREACH(items, l, it)
     {
	if (it->desk == it->bd->desk)
	  continue;

	/* dont hide border */
	if (it->desk == current_desk)
	  it->bd->visible = 0;

	e_border_desk_set(it->bd, it->desk);

	if (it->desk == current_desk)
	  it->bd->visible = 1;
     }

   if (current_desk && (desk != current_desk))
     {
	/* dont do flip animation */
	int tmp = e_config->desk_flip_animate_mode;
	e_config->desk_flip_animate_mode = 0;
	e_desk_show(current_desk);
	e_config->desk_flip_animate_mode = tmp;
     }

   EINA_LIST_FOREACH(items, l, it)
     {
	it->bd_x = it->bd->x;
	it->bd_y = it->bd->y;
     }

   _pager_place_windows(1.0);

   EINA_LIST_FREE(items, it)
     {
	if (it->bd->desk != current_desk)
	  {
	     e_border_hide(it->bd, 2);
	     evas_object_hide(it->o_win);
	  }
	_pager_win_del(it);
     }

   EINA_LIST_FREE(desks, o)
     evas_object_del(o);

   EINA_LIST_FREE(popups, it)
     _pager_win_del(it);

   if (background)
     _pager_win_del(background);


   EINA_LIST_FREE(handlers, handler)
     ecore_event_handler_del(handler);

   /* XXX fix stacking */
   E_Comp_Win *cw, *prev = NULL;
   Eina_List *list = (Eina_List *)e_manager_comp_src_list(e_manager_current_get());
   EINA_LIST_FOREACH(list, l, cw)
     {
   	if (!cw->shobj) continue;
   	if (prev) evas_object_stack_above(cw->shobj, prev->shobj);
   	prev = cw;
     }

   e_msg_handler_del(msg_handler);
   msg_handler = NULL;
   zone = NULL;
   selected_item = NULL;
   current_desk = NULL;
   previous_desk = NULL;
   background = NULL;
}

static E_Desk *
_pager_desk_at_xy_get(double x, double y)
{
   x = ((x * zoom) - (min_x * zoom)) / (double)zone->w;
   y = ((y * zoom) - (min_y * zoom)) / (double)zone->h;
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

   if (!scale_state)
     return;

   if ((ev->button == 2) || (ev->flags & EVAS_BUTTON_DOUBLE_CLICK))
     {
	selected_item = it;

	_pager_desk_select(it->desk);

	_pager_out();
	return;
     }
   else if (ev->button == 1)
     {
	e_border_raise(it->bd);

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
   it->x += (double)it->bd->x * (1.0 / zoom);
   it->y += (double)it->bd->y * (1.0 / zoom);
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

   if (!scale_state)
     return;

   if (x + it->bd->w > zone->w) x = zone->w - it->bd->w;
   if (y + it->bd->h > zone->h) y = zone->h - it->bd->h;
   if (x < 0) x = 0;
   if (y < 0) y = 0;

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

	x = (int)(it->bd_x) % zone->w;
	y = (int)(it->bd_y) % zone->h;

	if ((desk2 = _pager_desk_at_xy_get(it->x, it->y)))
	  {
	     x += (desk2->x - desk->x) * zone->w;
	     y += (desk2->y - desk->y) * zone->h;
	  }

	if (current_desk != desk)
	  _pager_desk_select(desk);

	e_border_move(it->bd, x, y);

	it->desk = desk;
     }
   else if (desk && (desk != it->desk))
     {

	it->desk = desk;

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

   if (selected_item && (it != selected_item))
     {
	edje_object_signal_emit(selected_item->o, "mouse,out", "e");
	selected_item = NULL;
     }

   edje_object_signal_emit(it->o, "mouse,in", "e");
   selected_item = it;
}

static void
_pager_win_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   /* Item *it = data; */

   if (selected_item)
     {
	edje_object_signal_emit(selected_item->o, "mouse,out", "e");
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
_pager_win_cb_intercept_stack_above(void *data, Evas_Object *obj, Evas_Object *above)
{
   Item *ot, *it = data;
   Eina_List *l;

   EINA_LIST_FOREACH(items, l, ot)
     if (ot->o_win == above) break;

   if (ot)
     evas_object_stack_above(it->o, ot->o);
}

static void
_pager_win_cb_intercept_move(void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Item *it = data;
   evas_object_move(obj, it->cur_x, it->cur_y);
}

static void
_pager_win_cb_intercept_resize(void *data, Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Item *it = data;
   evas_object_resize(obj, it->cur_w, it->cur_h);
}

static void
_pager_win_cb_intercept_color(void *data, Evas_Object *obj, int r, int g, int b, int a)
{
   Item *it = data;
   evas_object_color_set(obj, it->alpha, it->alpha, it->alpha, it->alpha);
}

static void
_pager_win_del(Item *it)
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

   evas_object_event_callback_del(it->o_win, EVAS_CALLBACK_DEL,
				  _pager_win_cb_delorig);

   evas_object_intercept_move_callback_del
     (it->o_win, _pager_win_cb_intercept_move);

   evas_object_intercept_resize_callback_del
     (it->o_win, _pager_win_cb_intercept_resize);

   evas_object_intercept_color_set_callback_del
     (it->o_win, _pager_win_cb_intercept_color);

   evas_object_intercept_stack_above_callback_del
     (it->o_win, _pager_win_cb_intercept_stack_above);

   if (it->bd)
     {
	it->alpha = 255;
	evas_object_color_set(it->o_win, 255, 255, 255, 255);
	edje_object_part_unswallow(it->o, it->o_win);
	evas_object_stack_above(it->o_win, it->o);
	e_object_unref(E_OBJECT(it->bd));
     }

   evas_object_del(it->o);

   E_FREE(it);
}

static void
_pager_win_new(Evas *e, E_Manager *man, E_Manager_Comp_Source *src)
{
   Item *it;
   E_Comp_Win *cw = (void*)src;
   Evas_Object *o;

   if (!e_manager_comp_src_image_get(man, src))
     return;

   if (!cw->bd)
     {
	o = e_manager_comp_src_shadow_get(man, src);
	if (!o) return;

	if (cw->win == zone->container->bg_win)
	  {
	     it = E_NEW(Item, 1);
	     it->o_win = o;
	     evas_object_event_callback_add
	       (it->o_win, EVAS_CALLBACK_DEL,
		_pager_win_cb_delorig, it);
	     background = it;
	  }
	else if (scale_conf->pager_fade_popups)
	  {
	     it = E_NEW(Item, 1);
	     it->o_win = o;
	     evas_object_event_callback_add
	       (it->o_win, EVAS_CALLBACK_DEL,
		_pager_win_cb_delorig, it);

	     popups = eina_list_append(popups, it);
	  }
   	return;
     }

   if (cw->bd->zone != zone)
     return;

   if (cw->bd->iconic)
     return;

   it = E_NEW(Item, 1);
   it->bd = cw->bd;
   it->desk = it->bd->desk;
   it->cw = cw;
   it->alpha = 255.0;
   e_object_ref(E_OBJECT(it->bd));

   it->o_win = e_manager_comp_src_shadow_get(man, src);
   it->o = edje_object_add(e);
   if (!e_theme_edje_object_set(it->o, "base/theme/modules/scale",
				"modules/scale/win"))
     edje_object_file_set(it->o, scale_conf->theme_path,
			  "modules/scale/win");

   evas_object_stack_above(it->o, it->o_win);
   evas_object_show(it->o);
   edje_object_part_swallow(it->o, "e.swallow.win", it->o_win);

   evas_object_event_callback_add(it->o_win, EVAS_CALLBACK_DEL,
				  _pager_win_cb_delorig, it);

   evas_object_intercept_move_callback_add
     (it->o_win, _pager_win_cb_intercept_move, it);

   evas_object_intercept_resize_callback_add
     (it->o_win, _pager_win_cb_intercept_resize, it);

   evas_object_intercept_color_set_callback_add
     (it->o_win, _pager_win_cb_intercept_color, it);

   evas_object_intercept_stack_above_callback_add
     (it->o_win, _pager_win_cb_intercept_stack_above, it);

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

   it->x = it->bd->x + it->dx * zone->w;
   it->y = it->bd->y + it->dy * zone->h;

   it->bd_x = it->x;
   it->bd_y = it->y;

   it->cur_x = it->x;
   it->cur_y = it->y;
   it->cur_w = it->bd->w;
   it->cur_h = it->bd->h;

   if (it->desk != current_desk)
     {
	e_border_show(it->bd);
	evas_object_move(it->o, it->cur_x, it->cur_y);

	if (scale_conf->pager_fade_windows)
	  {
	     it->alpha = 0.0;
	     evas_object_color_set(it->o_win, 0, 0, 0, 0);
	  }
     }

   _pager_win_final_position_set(it);
   evas_object_color_set(it->o, 255, 255, 255, 255);
   it->w = it->bd->w / zoom - OFFSET*2.0;
   it->h = it->bd->h / zoom - OFFSET*2.0;

   items = eina_list_append(items, it);

   edje_object_signal_emit(it->o, "show", "e");

   if (scale_state)
     _pager_redraw(NULL);
}

static Eina_Bool
_pager_cb_mouse_move(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Move *ev = event;

   evas_event_feed_mouse_move((Evas *) data, ev->x, ev->y,
			      ev->timestamp, NULL);

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_pager_cb_mouse_up(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Button *ev = event;
   Evas_Button_Flags flags = EVAS_BUTTON_NONE;

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

   if (!scale_state)
     return ECORE_CALLBACK_PASS_ON;

   EINA_LIST_FOREACH(items, l, it)
     if (E_INTERSECTS(ev->x, ev->y, 1, 1,
		      it->x, it->y, it->w, it->h))
       break;

   if (!it)
     {
	E_Desk *desk = _pager_desk_at_xy_get(ev->x, ev->y);
	if (desk)
	  _pager_desk_select(desk);

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
   printf("switch %s\n", params);

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

   printf("selec %d %d\n", desk_x, desk_y);

   desk = e_desk_at_xy_get(zone, desk_x, desk_y);
   if (desk) _pager_desk_select(desk);

   if (!scale_state)
     {
       Eina_List *l;
       Item *it;

       EINA_LIST_FOREACH(items, l, it)
	 {
	   it->alpha = 255.0;
	   evas_object_color_set(it->o_win, 255.0, 255.0, 255.0, 255.0);
	 }
     }
}

static Eina_Bool
_pager_cb_key_down(void *data, int type, void *event)
{
  Ecore_Event_Key *ev = event;

   if (ev->window != input_win)
     return ECORE_CALLBACK_PASS_ON;
   printf("%s\n", ev->key);

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

	     if (bind->action && strcmp(bind->action, "scale-windows")) continue;
	     if (!bind->params || strncmp(bind->params, "go_pager", 8)) continue;
	     printf("found action\n");

	     mod = 0;

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

  if (!scale_state)
    return ECORE_CALLBACK_PASS_ON;

  if (!e_mod_hold_modifier_check(event))
    _pager_out();

  return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_pager_run(E_Manager *man)
{
   Eina_List *l;
   E_Manager_Comp_Source *src;
   Ecore_Event_Handler *h;
   Evas *e;

   if (input_win) return EINA_FALSE;

   e = e_manager_comp_evas_get(man);
   if (!e) return EINA_FALSE;

   zone = e_util_zone_current_get(man);
   current_desk = e_desk_current_get(zone);

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

   desk_w = (zone->w / zoom);
   desk_h = (zone->h / zoom);

   min_x = OFFSET + (zoom - zone->desk_x_count) * desk_w / 2.0;
   min_y = OFFSET + (zoom - zone->desk_y_count) * desk_h / 2.0;

   max_x = min_x + (zone->desk_x_count * desk_w);
   max_y = min_y + (zone->desk_y_count * desk_h);

   EINA_LIST_FOREACH((Eina_List *)e_manager_comp_src_list(man), l, src)
     _pager_win_new(e, man, src);

   if (background)
     {
	Evas_Object *o;
	int x, y;

	for (y = 0; y < zone->desk_y_count; y++)
	  {
	     for (x = 0; x < zone->desk_x_count; x++)
	       {
		  o = edje_object_add(e);
		  if (!e_theme_edje_object_set(o, "base/theme/modules/scale",
					       "modules/scale/desk"))
		    edje_object_file_set(o, scale_conf->theme_path,
					 "modules/scale/desk");
		  evas_object_stack_above(o, background->o_win);
		  evas_object_show(o);

		  desks = eina_list_append(desks, o);

		  if ((x != zone->desk_x_current) || (y != zone->desk_y_current))
		    edje_object_signal_emit(o, "unfocused", "e");
	       }
	  }
	_pager_place_desks(1.0);
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

   printf("run: %s\n", params);

   if (scale_state)
     {
	_pager_switch(params+8);
     }
   else if (!scale_state)
     {
	if (input_win)
	  return ret;

	ret = _pager_run(man);

	if (ret)
	  {
	    if (strlen(params) > 8)
	      _pager_switch(params+8);

	    _pager_in();
	  }
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
      _pager_win_new(e, man, src);
    }
  else if (!strcmp(info, "del.src"))
    {
      DBG("%s: %p | %p\n", info, man, src);
    }
  else if (!strcmp(info, "config.src"))
    {

      DBG("%s: %p | %p\n", info, man, src);
    }
  else if (!strcmp(info, "visible.src"))
    {
      DBG("%s: %p | %p\n", info, man, src);
    }
}
