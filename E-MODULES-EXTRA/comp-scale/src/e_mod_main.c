#include <e.h>
#include "e_mod_main.h"

//  TODO
//~ handle add, delete border


#define DBG(...)
/* #define DBG(...) printf(__VA_ARGS__) */


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

/* static void _scale_win_movorig(void *data, Evas *e, Evas_Object *obj, void *event_info);
 * static void _scale_win_reszorig(void *data, Evas *e, Evas_Object *obj, void *event_info); */
static void _scale_win_delorig(void *data, Evas *e, Evas_Object *obj, void *event_info);

static void _scale_finish(void);
static void _scale_in(void);
static void _scale_out(void);
static Eina_Bool _scale_cb_mouse_down(void *data, int type, void *event);

static void _scale_win_cb_intercept_move(void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _scale_win_cb_intercept_resize(void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y);

static E_Action *act = NULL;

typedef struct _Item Item;

typedef struct _E_Comp_Win E_Comp_Win;

struct _Item
{
  Evas_Object *o;
  E_Border *bd;
  E_Comp_Win *cw;
  double scale;

  double x;
  double y;
  double w;
  double h;

  double mx;
  double my;

  double bd_x;
  double bd_y;

  int cur_x, cur_y, cur_w, cur_h;
  
  int overlaps;
};

static Eina_List *items = NULL;
static Eina_List *popups = NULL;
static Eina_List *handlers = NULL;
static Ecore_Animator *scale_animator = NULL;
static Eina_Bool scale_state = EINA_FALSE;
static double start_time;
static double current_scaling = 0;

static int max_x;
static int max_y;
static int min_x;
static int min_y;
static int width, height;
static int step_count;

static Ecore_X_Window input_win = 0;
static E_Msg_Handler *msg_handler = NULL;

struct _E_Comp_Win
{
  EINA_INLIST;

  void			*c; // parent compositor
  Ecore_X_Window	 win; // raw window - for menus etc.
  E_Border		*bd; // if its a border - later
  E_Popup		*pop; // if its a popup - later
  E_Menu		*menu; // if it is a menu - later
  int			 x, y, w, h; // geometry
  struct {
    int			 x, y, w, h; // hidden geometry (used when its unmapped and re-instated on map)
  } hidden;
  int			 pw, ph; // pixmap w/h
  int			 border; // border width
  Ecore_X_Pixmap	 pixmap; // the compositing pixmap
  Ecore_X_Damage	 damage; // damage region
  Ecore_X_Visual	 vis; // window visual
  int			 depth; // window depth
  Evas_Object		*obj; // composite object
  Evas_Object		*shobj; // shadow object
  Eina_List		*obj_mirror; // extra mirror objects
  Ecore_X_Image		*xim; // x image - software fallback
  void			*up; // update handler
  E_Object_Delfn	*dfn; // delete function handle for objects being tracked
  Ecore_X_Sync_Counter	 counter; // sync counter for syncronised drawing
  Ecore_Timer		*update_timeout; // max time between damage and "done" event
  Ecore_Timer		*ready_timeout; // max time on show (new window draw) to wait for window contents to be ready if sync protocol not handled. this is fallback.
  int			 dmg_updates; // num of damage event updates since a redirect
  Ecore_X_Rectangle	*rects; // shape rects... if shaped :(
  int			 rects_num; // num rects above

  Ecore_X_Pixmap        cache_pixmap; // the cached pixmap (1/nth the dimensions)
  int                   cache_w, cache_h; // cached pixmap size
  int                   update_count; // how many updates have happened to this win
  double                last_visible_time; // last time window was visible
  double                last_draw_time; // last time window was damaged

  int                   pending_count; // pending event count

  char                 *title, *name, *clas, *role; // fetched for override-redirect windowa
  Ecore_X_Window_Type   primary_type; // fetched for override-redirect windowa

  Eina_Bool             delete_pending : 1; // delete pendig
  Eina_Bool             hidden_override : 1; // hidden override

  Eina_Bool             animating : 1; // it's busy animating - defer hides/dels
  Eina_Bool             force : 1; // force del/hide even if animating
  Eina_Bool             defer_hide : 1; // flag to get hide to work on deferred hide
  Eina_Bool             delete_me : 1; // delete me!

  Eina_Bool             visible : 1; // is visible
  Eina_Bool             input_only : 1; // is input_only
  Eina_Bool             override : 1; // is override-redirect
  Eina_Bool             argb : 1; // is argb
  Eina_Bool             shaped : 1; // is shaped
  Eina_Bool             update : 1; // has updates to fetch
  Eina_Bool             redirected : 1; // has updates to fetch
  Eina_Bool             shape_changed : 1; // shape changed
  Eina_Bool             native : 1; // native
  Eina_Bool             drawme : 1; // drawme flag fo syncing rendering
  Eina_Bool             invalid : 1; // invalid depth used - just use as marker
  Eina_Bool             nocomp : 1; // nocomp applied
  Eina_Bool             needpix : 1; // need new pixmap
  Eina_Bool             needxim : 1; // need new xim
  Eina_Bool             real_hid : 1; // last hide was a real window unmap
  Eina_Bool             inhash : 1; // is in the windows hash
  Eina_Bool             show_ready : 1; // is this window ready for its first show
};


#define GROW 6.0

static int
_grow()
{
   Item *it, *ot;
   Eina_List *l, *ll;

   int cont = 0;
   int overlap;
   double grow_l, grow_r, grow_d, grow_u;
   double mean = 0;
   int off = scale_conf->spacing;
   
   EINA_LIST_FOREACH(items, l, it)
     mean += it->scale;
   
   mean /= ((double) eina_list_count(items) /* + 0.5 */);

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
	     if (grow_l && E_INTERSECTS(it->x - grow_l, it->y, it->w, it->h,
					ot->x - off, ot->y - off, ot->w + grow_l + off*2, ot->h + off*2))
	       grow_l = 0;

	     if (grow_r && E_INTERSECTS(it->x, it->y, it->w + grow_r, it->h,
					ot->x - grow_r - off, ot->y - off, ot->w + off*2, ot->h + off*2))
	       grow_r = 0;

	     if ((grow_l == 0) && (grow_r == 0) && (overlap = 1))
	       break;

	     if (grow_u && E_INTERSECTS(it->x, it->y - grow_u, it->w, it->h,
					ot->x - off, ot->y - off, ot->w + off*2, ot->h + grow_u + off*2))
	       grow_u = 0;

	     if (grow_d && E_INTERSECTS(it->x, it->y, it->w, it->h + grow_d,
					ot->x - off, ot->y - grow_d - off, ot->w + off*2, ot->h + off*2))
	       grow_d = 0;

	     if ((grow_u == 0) && (grow_d == 0) && (overlap = 1))
	       break;
	  }

	if (overlap)
	  continue;

	if (it->bd->w > it->bd->h)
	  {
	     it->w += ((grow_u > 0) && (grow_d > 0)) ? grow_l + grow_r : MAX(grow_l, grow_r);
	     it->h = it->w * (double)it->bd->h / (double)it->bd->w;
	  }
	else
	  {
	     it->h += ((grow_r > 0) && (grow_l > 0)) ? grow_u + grow_d : MAX(grow_u, grow_d);
	     it->w = it->h * (double)it->bd->w / (double)it->bd->h;
	  }

	it->x -= grow_l;
	it->y -= grow_u;

	cont++;
     }

   return cont;
}

static int
_place(int offset)
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

	     if (!E_INTERSECTS(it->x - offset, it->y - offset, it->w + offset*2, it->h + offset*2,
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

	     /* cycle items with same center around to get even	distribution.
		dont try to understand this but these initial conditions
		are important.. you can have up to 9 maximized windows.
		if you can figure out more please tell me :) */
	     if (dist_x == 0 && dist_y == 0)
	       {
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

   if (!overlap)
     return 0;

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

   if (outside && (step_count++ > 50))
     {
	double zone_diag = sqrt(width * height);
	double sw, sh;
	step_count = 0;
	EINA_LIST_FOREACH(items, l, it)
	  {
	     if (!it->overlaps)
	       continue;
	     
	     if (it->scale <= 0.005)
	       continue;

	     it->scale -= it->scale * (0.001 + (sqrt(it->bd->w * it->bd->h) / zone_diag) / 50.0);
		  
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

#if 0
static void
_bounding_box_get()
{
   Eina_List *l;
   double x, y, w, h;
   Item *it;
   EINA_LIST_FOREACH(items, l, it)
     {
	
     }
}

static int
_cb_sort_down(const void *d1, const void *d2)
{
   const Item *it1 = d1;
   const Item *it2 = d2;
   return (it1->y < it2->y) ? -1 : 1;
}

static int
_cb_sort_right(const void *d1, const void *d2)
{
   const Item *it1 = d1;
   const Item *it2 = d2;
   return (it1->x < it2->x) ? -1 : 1;
}

static int
_shrink()
{
   Eina_List *l, *ll;
   Item *it, *ot;
   int last_offset, offset;
   
   items = eina_list_sort(items, eina_list_count(items), _cb_sort_down); 
   
   EINA_LIST_REVERSE_FOREACH(items, l, it)
     {
	last_offset = offset = 100;
	
	while (last_offset > 10)
	  {
	     if (it->y + it->h + last_offset > height)
	       {
		  last_offset = height - (it->y + it->h);
		  continue;
	       }
	     
	     offset = last_offset;
	     
	     EINA_LIST_FOREACH(l->next, ll, ot)
	       {
		  if (!E_INTERSECTS(it->x, it->y + offset, it->w, it->h,
				    ot->x, ot->y, ot->w, ot->h))
		    continue;

		  last_offset = offset / 2;
		  offset = 0;
		  break;
	       }
	     it->y += offset;
	  }
     }
   items = eina_list_sort(items, eina_list_count(items), _cb_sort_right); 
   
   EINA_LIST_REVERSE_FOREACH(items, l, it)
     {
	last_offset = offset = 100;
	
	while (last_offset > 10)
	  {
	     if (it->x + it->w + last_offset > width)
	       {
		  last_offset = width - (it->x + it->w);
		  continue;
	       }
	     
	     offset = last_offset;
	     
	     EINA_LIST_FOREACH(l->next, ll, ot)
	       {
		  if (!E_INTERSECTS(it->x + offset, it->y, it->w, it->h,
				    ot->x, ot->y, ot->w, ot->h))
		    continue;

		  last_offset = offset / 2;

		  offset = 0;
		  break;
	       }
	     it->x += offset;
	  }
     }
}
#endif

static int
_shrink()
{
   Eina_List *l, *ll;
   Item *it, *ot;
   int shrunk = 0;
   int off = scale_conf->spacing;
   
   EINA_LIST_REVERSE_FOREACH(items, l, it)
     {
	double move_x = ((it->x + it->w/2.0) - (double)(it->bd->x + it->bd->w/2.0)) / 10.0;
	double move_y = ((it->y + it->h/2.0) - (double)(it->bd->y + it->bd->h/2.0)) / 10.0;

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

	if (move_y || move_x)
	  shrunk = 1;
     }

   return shrunk;
}

static Eina_Bool
_redraw(void *blah)
{
   Eina_List *l;
   Item *it;
   double scale;
   
   if (scale_state)
     scale = (ecore_time_get() - start_time) / scale_conf->scale_duration;
   else
     scale = 1.0 - (ecore_time_get() - start_time) / scale_conf->scale_duration;

   if (scale > 1.0) scale = 1.0;
   if (scale < 0.0) scale = 0.0;
   
   current_scaling = scale;

   EINA_LIST_FOREACH(items, l, it)
     {
	/* it->cw->x = it->bd_x * (1.0 - scale) + it->x * scale;
	 * it->cw->y = it->bd_y * (1.0 - scale) + it->y * scale; */

	it->cur_w = it->bd->w * (1.0 - scale) + it->w * scale;
	it->cur_h = it->bd->h * (1.0 - scale) + it->h * scale;
	it->cur_x = it->bd_x * (1.0 - scale) + it->x * scale;
	it->cur_y = it->bd_y * (1.0 - scale) + it->y * scale;

	/* printf("resize %d %d\n", it->cur_w, it->cur_h); */

	evas_object_move(it->o, it->cur_x, it->cur_y);
	evas_object_resize(it->o, it->cur_w, it->cur_h);
     }

   double a = 255.0 * (1.0 - scale);
   
   EINA_LIST_FOREACH(popups, l, it)
     evas_object_color_set(it->o, a, a, a, a);

   /* e = e_manager_comp_evas_get(e_manager_current_get()); */
   
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
     scale_animator = ecore_animator_add(_redraw, NULL);
}

static void
_scale_out()
{
   double now = ecore_time_get();
   
   if (now - start_time < scale_conf->scale_duration)
     start_time = now - (scale_conf->scale_duration - (now - start_time));
   else
     start_time = now;

   if (!scale_animator)
     scale_animator = ecore_animator_add(_redraw, NULL);

   scale_state = EINA_FALSE;
}

static void
_scale_finish()
{
   Ecore_Event_Handler *handler;
   Item *it;
   e_grabinput_release(input_win, input_win);
   ecore_x_window_free(input_win);
   input_win = 0;

   E_Zone *zone = e_util_zone_current_get(e_manager_current_get());
   E_Desk *desk = e_desk_current_get(zone);

   EINA_LIST_FREE(items, it)
     {
	if (it->bd->desk != desk)
	  e_border_hide(it->bd, 2);

	evas_object_event_callback_del(it->o, EVAS_CALLBACK_DEL, _scale_win_delorig);

	evas_object_intercept_move_callback_del(it->o, _scale_win_cb_intercept_move);
	evas_object_intercept_resize_callback_del(it->o, _scale_win_cb_intercept_resize); 

	E_FREE(it);
     }

   EINA_LIST_FREE(popups, it)
     {
	evas_object_event_callback_del(it->o, EVAS_CALLBACK_DEL, _scale_win_delorig);
	E_FREE(it);
     }

   EINA_LIST_FREE(handlers, handler)
     ecore_event_handler_del(handler);

   e_msg_handler_del(msg_handler);
   msg_handler = NULL;

   e_manager_comp_evas_update(e_manager_current_get());
}

static void
_scale_win_delorig(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Item *it = data;

   items = eina_list_remove(items, it);
   popups = eina_list_remove(popups, it);

   evas_object_intercept_move_callback_del(it->o, _scale_win_cb_intercept_move);
   evas_object_intercept_resize_callback_del(it->o, _scale_win_cb_intercept_resize); 

   evas_object_event_callback_del(it->o, EVAS_CALLBACK_DEL, _scale_win_delorig);

   if (it->bd)
     _scale_out();
   
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

   e_manager_comp_evas_update(e_manager_current_get());
}

static void
_scale_win_new(Evas *e, E_Manager *man, E_Manager_Comp_Source *src, E_Desk *desk)
{
   Item *it;

   if (!e_manager_comp_src_image_get(man, src)) return;

   E_Comp_Win *cw = (void*)src;

   if (cw->pop)
     {
   	it = E_NEW(Item, 1);
   	it->o = e_manager_comp_src_shadow_get(man, src);
	evas_object_event_callback_add(it->o, EVAS_CALLBACK_DEL, _scale_win_delorig, it);

   	popups = eina_list_append(popups, it);
   	return;
     }
   
   if (!cw->bd) return;
   
   if (cw->bd->desk != desk)
     return;

   if (cw->bd->iconic)
     return;

   it = E_NEW(Item, 1);
   it->scale = 1.0;
   it->x = cw->bd->x + (cw->bd->desk->x - desk->x) * width;
   it->y = cw->bd->y + (cw->bd->desk->y - desk->y) * height;

   it->bd_x = it->x;
   it->bd_y = it->y;

   it->w = cw->bd->w;
   it->h = cw->bd->h;

   it->cur_x = it->x;
   it->cur_y = it->y;
   it->cur_w = it->w;
   it->cur_h = it->h;

   it->bd = cw->bd;
   it->cw = cw;
   it->o = e_manager_comp_src_shadow_get(man, src);
   
   evas_object_event_callback_add(it->o, EVAS_CALLBACK_DEL, _scale_win_delorig, it);

   evas_object_intercept_move_callback_add(it->o, _scale_win_cb_intercept_move, it);
   evas_object_intercept_resize_callback_add(it->o, _scale_win_cb_intercept_resize, it); 

   if (it->bd->desk != desk)
     {
	e_border_show(it->bd);
  	evas_object_move(it->o, it->x, it->y);
     }

   items = eina_list_append(items, it);
}

static int
_cb_sort_center(const void *d1, const void *d2)
{
   const Item *it1 = d1;
   const Item *it2 = d2;

   double dx1 = ((it1->x + it1->w/2.0) - (double)(width/2));
   double dy1 = ((it1->y + it1->h/2.0) - (double)(height/2));

   double dx2 = ((it2->x + it2->w/2.0) - (double)(width/2));
   double dy2 = ((it2->y + it2->h/2.0) - (double)(height/2));
   
   return (sqrt(dx1*dx1 + dy1*dy1) < sqrt(dx2*dx2 + dy2*dy2)) ? -1 : 1;
}

void
scale_run(E_Manager *man)
{
   Eina_List *list, *l;
   E_Manager_Comp_Source *src;
   Evas *e;
   int i, spacing;

   E_Zone *zone = e_util_zone_current_get(e_manager_current_get());
   E_Desk *desk = e_desk_current_get(zone);

   input_win = ecore_x_window_input_new(zone->container->win, 0, 0, 1, 1);
   ecore_x_window_show(input_win);
   if (!e_grabinput_get(input_win, 0, input_win))
     {
	ecore_x_window_free(input_win);
	input_win = 0;
	return;
     }
   e_zone_useful_geometry_get(zone, &min_x, &min_y, &width, &height);

   handlers = eina_list_append
     (handlers, ecore_event_handler_add
      (ECORE_EVENT_MOUSE_BUTTON_DOWN, _scale_cb_mouse_down, NULL));

   e = e_manager_comp_evas_get(man);

   list = (Eina_List *)e_manager_comp_src_list(man);
   EINA_LIST_FOREACH(list, l, src)
     _scale_win_new(e, man, src, desk);

   items = eina_list_sort(items, eina_list_count(items), _cb_sort_center); 

   max_x = width;
   max_y = height;

   start_time = ecore_time_get();
   
   step_count = 0;
   i = 0;
   spacing = scale_conf->spacing;
   
   if (scale_conf->grow && (spacing < 48)) spacing = 48;
   if (scale_conf->tight && (spacing < 48)) spacing = 48;

   while (i++ < 10000 && _place(spacing));

   DBG("place %d\n", i);

   if (i == 1)
     {
	_scale_finish();
	return;
     }

   if (scale_conf->grow)
     {	
	i = 0;
   	while (i++ < 1000 && _grow());
	DBG("grow %d", i);
     }

   if (scale_conf->tight)
     {
	i = 0;
	while (i++ < 1000 && _shrink());
	DBG("shrunk %d", i);
     }
   
   DBG("time: %f\n", ecore_time_get() - start_time);

   _scale_in();   
}


static Eina_Bool
_scale_cb_mouse_down(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Button *ev;
   Item *it;
   Eina_List *l;

   ev = event;
   if (ev->window != input_win)
     return ECORE_CALLBACK_PASS_ON;

   if (!scale_state)
     return ECORE_CALLBACK_PASS_ON;
   
   EINA_LIST_FOREACH(items, l, it)
     {
	if (E_INTERSECTS(ev->x, ev->y, 1, 1, it->x, it->y, it->w, it->h))
	  {
	     e_border_raise(it->bd);
	     break;
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

   DBG("handler... '%s' '%s'\n", name, info);
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
   else if (!strcmp(info, "visible.src"))
     {
        DBG("%s: %p | %p\n", info, man, src);
     }
}

static void
_e_mod_action_cb_edge(E_Object *obj, const char *params, E_Event_Zone_Edge *ev)
{
   Eina_List *list, *l;
   E_Manager *man;

   msg_handler = e_msg_handler_add(_scale_handler, NULL);
   list = e_manager_list();
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
   E_CONFIG_VAL(D, T, scale_duration, DOUBLE);
   E_CONFIG_VAL(D, T, spacing, DOUBLE);
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

   e_gadcon_provider_register(&_gc_class);

   act = e_action_add("scale-windows");
   if (act)
     {
	act->func.go = _e_mod_action_cb;
	act->func.go_edge = _e_mod_action_cb_edge;
	e_action_predef_name_set(D_("Desktop"), D_("Scale Windows"), "scale-windows", "", NULL, 0);
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
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/e-module-scale.edj",
            scale_conf->module->dir);

   inst = E_NEW(Instance, 1);
   inst->conf_item = _scale_conf_item_get(id);

   inst->o_scale = edje_object_add(gc->evas);

   if (!e_theme_edje_object_set(inst->o_scale, "base/theme/modules/scale",
                                "modules/scale/main"))
     edje_object_file_set(inst->o_scale, buf, "modules/scale/main");

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
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/e-module-scale.edj", scale_conf->module->dir);

   o = edje_object_add(evas);

   edje_object_file_set(o, buf, "icon");

   return o;
}

static void
_scale_conf_new(void)
{

   scale_conf = E_NEW(Config, 1);
   scale_conf->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) if ((scale_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

   IFMODCFG(0x008d);
   scale_conf->grow = 0;
   scale_conf->tight = 1;
   scale_conf->scale_duration = 0.3;
   scale_conf->spacing = 32;
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
	_e_mod_action_cb_edge(NULL, NULL, NULL);
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
