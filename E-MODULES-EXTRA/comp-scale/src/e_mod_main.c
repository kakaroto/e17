#include <e.h>
#include "e_mod_main.h"

/* Local Function Prototypes */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);

static void _scale_conf_new(void);
static void _scale_conf_free(void);
static Eina_Bool _scale_conf_timer(void *data);
static Config_Item *_scale_conf_item_get(const char *id);
static void _scale_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _scale_cb_menu_post(void *data, E_Menu *menu);
static void _scale_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);

static void movorig(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void reszorig(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void delorig(void *data, Evas *e, Evas_Object *obj, void *event_info);


static E_Action *act = NULL;

typedef struct _Item Item;

typedef struct _E_Comp_Win E_Comp_Win;

struct _Item
{
  Evas_Object *o;
  E_Border *bd;
  E_Comp_Win *cw;
  double factor;
  
  int x;
  int y;
  int w;
  int h;  

  int mx;
  int my;
};

static Eina_List *items = NULL;
static Eina_List *handlers = NULL;
static Ecore_Timer *timer = NULL;

static int max_x;
static int max_y;
static int min_x;
static int min_y;
static int width, height;
static int cnt = 0;
static double factor;

static double advance = 0.0;
static double zoom = 0.00;
static Ecore_X_Window input_win = 0;
static E_Msg_Handler *msg_handler = NULL;
/* static Evas *comp_evas = NULL; */

struct _E_Comp_Win
{
  EINA_INLIST;
   
  void                 *c; // parent compositor
  Ecore_X_Window        win; // raw window - for menus etc.
  E_Border             *bd; // if its a border - later
  E_Popup              *pop; // if its a popup - later
  E_Menu               *menu; // if it is a menu - later
  int                   x, y, w, h; // geometry
  struct {
    int                x, y, w, h; // hidden geometry (used when its unmapped and re-instated on map)
  } hidden;
  int                   pw, ph; // pixmap w/h
  int                   border; // border width
  Ecore_X_Pixmap        pixmap; // the compositing pixmap
  Ecore_X_Damage        damage; // damage region
  Ecore_X_Visual        vis; // window visual
  int                   depth; // window depth
  Evas_Object          *obj; // composite object
  Evas_Object          *shobj; // shadow object
  Eina_List            *obj_mirror; // extra mirror objects
  Ecore_X_Image        *xim; // x image - software fallback
  void             *up; // update handler
  E_Object_Delfn       *dfn; // delete function handle for objects being tracked
  Ecore_X_Sync_Counter  counter; // sync counter for syncronised drawing
  Ecore_Timer          *update_timeout; // max time between damage and "done" event
  Ecore_Timer          *ready_timeout; // max time on show (new window draw) to wait for window contents to be ready if sync protocol not handled. this is fallback.
  int                   dmg_updates; // num of damage event updates since a redirect
  Ecore_X_Rectangle    *rects; // shape rects... if shaped :(
  int                   rects_num; // num rects above
   
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

static int
_grow()
{
   Item *it, *ot;
   Eina_List *l, *ll;

   int cont = 0;
   int overlap;
   int grow_l, grow_r, grow_d, grow_u;
   
   EINA_LIST_FOREACH(items, l, it)
     {
	if (it->w >= it->bd->w)
	  continue;
	if (it->h >= it->bd->h)
	  continue;

	overlap = 0;
	grow_l = grow_r = 2;
	grow_u = grow_d = 2.0 * (double)it->bd->h/(double)it->bd->w;
		
	if (it->x - grow_l < 0)
	  grow_l = 0;
	if (it->y - grow_u < 0)
	  grow_u = 0;
	if (it->x + it->w + grow_r > width)
	  grow_r = 0;
	if (it->y + it->h + grow_d > height)
	  grow_d = 0;
	
	if (!(grow_l || grow_r))
	  continue;
	if (!(grow_u || grow_d))
	  continue;
	
	EINA_LIST_FOREACH(items, ll, ot)
	  {
	     if (it == ot)
	       continue;

	     if (grow_l &&
		 E_INTERSECTS(it->x-grow_l*2, it->y, it->w, it->h,
			      ot->x, ot->y, ot->w, ot->h))
	       grow_l = 0;

	     if (grow_u &&
		 E_INTERSECTS(it->x, it->y-grow_u*2, it->w, it->h,
			      ot->x, ot->y, ot->w, ot->h))
	       grow_u = 0;

	     if (grow_r &&
		 E_INTERSECTS(it->x, it->y, it->w+grow_r*2, it->h,
			      ot->x, ot->y, ot->w, ot->h))
	       grow_r = 0;

	     if (grow_d &&
		 E_INTERSECTS(it->x, it->y, it->w, it->h+grow_d*2,
			      ot->x, ot->y, ot->w, ot->h))
	       grow_d = 0;

	     if (!((grow_l || grow_r) && (grow_u || grow_d)))
	       {
		  overlap = 1;
		  break;
	       }
	  }

	if (!overlap)
	  {		  
	     cont++;
	     it->w += (grow_u && grow_d) ? 4 : 2;
	     it->h = it->w * (double)it->bd->h/(double)it->bd->w;
	     it->x -= grow_l;
	     it->y -= grow_u;
	  }
     }

   return cont;
}

static int
_place()
{
   Item *it, *ot;
   Eina_List *l, *ll;
   int overlap = 0;
   int outside = 0;

   cnt++;

   EINA_LIST_FOREACH(items, l, it)
     {	
	it->mx = it->x;
	it->my = it->y;
     }
   
   EINA_LIST_FOREACH(items, l, it)
     {	
	EINA_LIST_FOREACH(l->next, ll, ot)
	  {
	     int w = it->w;
	     int h = it->h;

	     if (!E_INTERSECTS(it->x-2, it->y-2, it->w+4, it->h+4, ot->x, ot->y, ot->w, ot->h))
	       continue;

	     overlap += 1;

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

	     int dist_y = (it->y + it->h/2) - (ot->y + ot->h/2);
	     int dist_x = (it->x + it->w/2) - (ot->x + ot->w/2);	     

	     if (dist_x == 0 && dist_y == 0)
	       {
		  ot->x +=1;
		  ot->y +=1;
	       }
	     
	     if (w > h)
	       {

		  if (dist_y)
		    {
		       dist_y = (dist_y > 0 ? 2 : -2);
		       it->my += dist_y;
		       ot->my -= dist_y;
		    }
		  if (dist_x)
		    {		       
		       dist_x = (dist_x > 0 ? 1 : -1);
		       it->mx += dist_x;
		       ot->mx -= dist_x;
		    }
	       }
	     else //if (w < h)
	       {
		  if (dist_y)
		    {
		       dist_y = (dist_y > 0 ? 1 : -1);
		       it->my += dist_y;
		       ot->my -= dist_y;
		    }
		  if (dist_x)
		    {
		       dist_x = (dist_x > 0 ? 2 : -2);
		       it->mx += dist_x;
		       ot->mx -= dist_x;
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
   	
   	if (it->x < 0)
	  {
	     outside = 1;
	     it->x = 0;
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

   if (outside)
     {	
	
	if (cnt > 20)
	  {
	     /* printf("resize %f\n", factor); */
	     cnt = 0;
	     
	     factor -= 0.02;
	     
	     EINA_LIST_FOREACH(items, l, it)
	       {
		  it->w = it->bd->w * factor;
		  it->h = it->bd->h * factor;

	       }
	  }
	
	return 1;
     }

   return overlap;
}

static void
_finish()
{
   Ecore_Event_Handler *handler;
   /* Eina_List *l; */
   Item *it;

   e_grabinput_release(input_win, input_win);
   ecore_x_window_free(input_win);
   input_win = 0;

   EINA_LIST_FREE(items, it)
     {
	evas_object_event_callback_del(it->o, EVAS_CALLBACK_MOVE, movorig); 
	evas_object_event_callback_del(it->o, EVAS_CALLBACK_RESIZE, reszorig); 
	evas_object_event_callback_del(it->o, EVAS_CALLBACK_DEL, delorig); 
	E_FREE(it);
     }
   
   EINA_LIST_FREE(handlers, handler)
     ecore_event_handler_del(handler);

   e_msg_handler_del(msg_handler);
   msg_handler = NULL;
   
   e_manager_comp_evas_update(e_manager_current_get());
}

static Eina_Bool
_redraw(void *blah)
{
   Eina_List *l;
   Item *it;

   advance += zoom;

   /* evas_event_freeze(comp_evas);  */
   if (advance <= 1.0 && advance >= 0.0)
     {
	EINA_LIST_FOREACH(items, l, it)
	  {
	     it->cw->x = it->bd->x * (1.0 - advance) + it->x * advance;
	     it->cw->y = it->bd->y * (1.0 - advance) + it->y * advance;

	     double scaling = (double)(it->bd->w * (1.0 - advance) + it->w * advance) / (double)(it->cw->pw);
	     
	     evas_object_move(it->o,
	     		      it->bd->x * (1.0 - advance) + it->x * advance,
	     		      it->bd->y * (1.0 - advance) + it->y * advance);
	     
	     evas_object_resize(it->o,
	     			it->cw->pw * scaling,
	     			it->cw->ph * scaling);
	  }
     }
   /* evas_event_thaw(comp_evas); */
   
   e_manager_comp_evas_update(e_manager_current_get());
   
   if (advance <= 1.0 && advance >= 0.0)
     return 1;

   if (advance <= 0.0)
     _finish();
   
   timer = NULL;
   return 0;
}


static void
delorig(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   /* Eina_List *l; */
   Item *it = data;

   items = eina_list_remove(items, it);
   
   evas_object_event_callback_del(it->o, EVAS_CALLBACK_MOVE, movorig); 
   evas_object_event_callback_del(it->o, EVAS_CALLBACK_RESIZE, reszorig); 
   evas_object_event_callback_del(it->o, EVAS_CALLBACK_DEL, delorig); 
   E_FREE(it);
   if (!timer)
     timer = ecore_timer_add(0.01, _redraw, NULL);

   zoom = -0.10;
}

static void
movorig(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   /* Item *it = data; */
   /* printf("move\n"); */
}

// FIXME 
static void
reszorig(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Item *it = data;
   int w, h;
   double adv;
   evas_object_geometry_get(obj, NULL, NULL, &w, &h);

   if (advance > 1.0)
     adv = 1.0;
   else if (advance < 0.0) 
     adv = 0.0;
   else
     adv = advance;
   
   double scaling = (double)(it->bd->w * (1.0 - adv) + it->w * adv) / (double)(it->cw->pw);

   if (!((it->cw->pw * scaling == w) && (it->cw->ph * scaling == h)))
     {
	evas_object_resize(it->o,
			it->cw->pw * scaling,
			it->cw->ph * scaling);

     }
}

static void
newwin(Evas *e, E_Manager *man, E_Manager_Comp_Source *src, E_Desk *desk)
{
   Evas_Object *o;
   Item *it;
   
   if (!e_manager_comp_src_image_get(man, src)) return;

   E_Comp_Win *cw = (void*)src;
   if (!cw->bd) return;

   if (cw->bd->desk != desk)
     return;

   it = E_NEW(Item, 1);
   
   o = e_manager_comp_src_shadow_get(man, src);

   /* evas_object_event_callback_add(o, EVAS_CALLBACK_MOVE, movorig, it); */
   evas_object_event_callback_add(o, EVAS_CALLBACK_RESIZE, reszorig, it);
   evas_object_event_callback_add(o, EVAS_CALLBACK_DEL, delorig, it);

   /* it->x = cw->bd->x;
    * it->y = cw->bd->y; */
   it->x = cw->bd->x + (cw->bd->desk->x - desk->x) * width;
   it->y = cw->bd->y + (cw->bd->desk->y - desk->y) * height;
   it->w = cw->bd->w;
   it->h = cw->bd->h;
   it->o = o;
   it->bd = cw->bd;
   it->cw = cw;

   /* if (cw->bd->desk != desk)
    *   {
    * 	cw->visible = EINA_TRUE;
    * 	evas_object_show(o);
    * 	evas_object_move(o, it->x, it->y);
    *   } */
 
   /* 
    * e_manager_comp_src_hidden_set(man, src, EINA_FALSE);
    * evas_object_show(o); */
   
   items = eina_list_append(items, it);
}

static Eina_Bool
_cb_mouse_down(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Button *ev;

   ev = event;
   if (ev->window != input_win) return ECORE_CALLBACK_PASS_ON;

   if (!timer)
     timer = ecore_timer_add(0.01, _redraw, NULL);

   zoom = -0.10;

   Item *it;
   Eina_List *l;
   
   EINA_LIST_FOREACH(items, l, it)
     {
	if (E_INTERSECTS(ev->x, ev->y, 1, 1, it->x, it->y, it->w, it->h))
	  {
	     e_border_raise(it->bd);
	     break;
	  }	
     }
   
   return ECORE_CALLBACK_PASS_ON;
}

static void
setup(E_Manager *man)
{
   Eina_List *list, *l;
   E_Manager_Comp_Source *src;
   Evas *e;
   int i = 0;

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
      (ECORE_EVENT_MOUSE_BUTTON_DOWN, _cb_mouse_down, NULL));

   e = e_manager_comp_evas_get(man);
   /* comp_evas = e; */
   list = (Eina_List *)e_manager_comp_src_list(man);
   EINA_LIST_FOREACH(list, l, src)
     {
        newwin(e, man, src, desk);
     }

   max_x = width;
   max_y = height;
 
   factor = 1.0;
   
   while (i++ < 10000 && _place());
   printf("place %d\n", i);

   if (i == 1) 
     {
	_finish();
	return;
     }
      
   i = 0;
   while (i++ < 10000 && _grow())
     {
   	int k = 0;
   	cnt = 0;
   	
   	while (k++ < 10 && _place());
     }

   printf("grow %d\n", i);
   
   advance = 0.0;
   zoom = 0.1;
   
   if (!timer)
     timer = ecore_timer_add(0.01, _redraw, NULL);
}

static void
handler(void *data, const char *name, const char *info, int val, 
        E_Object *obj, void *msgdata)
{
   E_Manager *man = (E_Manager *)obj;
   E_Manager_Comp_Source *src = (E_Manager_Comp_Source *)msgdata;
   Evas *e;

   printf("handler... '%s' '%s'\n", name, info);
   if (strcmp(name, "comp.manager")) return;
   
   e = e_manager_comp_evas_get(man);
   if (!strcmp(info, "change.comp"))
     {
        if (!e) printf("TTT: No comp manager\n");
        else printf("TTT: comp canvas = %p\n", e);
        if (e) setup(man);
     }
   else if (!strcmp(info, "resize.comp"))
     {
        printf("%s: %p | %p\n", info, man, src);
     }
   else if (!strcmp(info, "add.src"))
     {
        printf("%s: %p | %p\n", info, man, src);
        newwin(e, man, src, e_desk_current_get(e_util_zone_current_get(man)));
     }
   else if (!strcmp(info, "del.src"))
     {
        printf("%s: %p | %p\n", info, man, src);
     }
   else if (!strcmp(info, "config.src"))
     {
	
        printf("%s: %p | %p\n", info, man, src);
     }
   else if (!strcmp(info, "visible.src"))
     {
        printf("%s: %p | %p\n", info, man, src);
     }
}

/* Local Structures */
typedef struct _Instance Instance;
struct _Instance 
{
  /* An instance of our item (module) with its elements */

  /* pointer to this gadget's container */
  E_Gadcon_Client *gcc;

  /* evas_object used to display */
  Evas_Object *o_scale;

  /* popup anyone ? */
  E_Menu *menu;

  /* Config_Item structure. Every gadget should have one :) */
  Config_Item *conf_item;
};

/* Local Variables */
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

/* We set the version and the name, check e_mod_main.h for more details */
EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "Scale"};

/*
 * Module Functions
 */

static void
_e_mod_action_cb_edge(E_Object *obj, const char *params, E_Event_Zone_Edge *ev)
{
   Eina_List *list, *l;
   E_Manager *man;

   msg_handler = e_msg_handler_add(handler, NULL);
   list = e_manager_list();
   EINA_LIST_FOREACH(list, l, man)
     {
        Evas *e = e_manager_comp_evas_get(man);
        if (e) setup(man);
     }
}


/* Function called when the module is initialized */
EAPI void *
e_modapi_init(E_Module *m) 
{
   char buf[4096];

   /* Location of message catalogs for localization */
   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   /* Location of theme to load for this module */
   snprintf(buf, sizeof(buf), "%s/e-module-scale.edj", m->dir);


   /* Display this Modules config info in the main Config Panel */

   /* starts with a category, create it if not already exists */
   e_configure_registry_category_add("advanced", 80, "Advanced", 
                                     NULL, "preferences-advanced");
   /* add right-side item */
   e_configure_registry_item_add("advanced/scale", 110, D_("Scale"), 
                                 NULL, buf, e_int_config_scale_module);

   /* Define EET Data Storage for the config file */
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
   E_CONFIG_VAL(D, T, switch1, UCHAR); /* our var from header */
   E_CONFIG_LIST(D, T, conf_items, conf_item_edd); /* the list */

   /* Tell E to find any existing module data. First run ? */
   scale_conf = e_config_domain_load("module.scale", conf_edd);
   if (scale_conf) 
     {
        /* Check config version */
        if ((scale_conf->version >> 16) < MOD_CONFIG_FILE_EPOCH) 
          {
             /* config too old */
	     _scale_conf_free();
	     ecore_timer_add(1.0, _scale_conf_timer,
			     D_("Scale Module Configuration data needed "
				"upgrading. Your old configuration<br> has been"
				" wiped and a new set of defaults initialized. "
				"This<br>will happen regularly during "
				"development, so don't report a<br>bug. "
				"This simply means the module needs "
				"new configuration<br>data by default for "
				"usable functionality that your old<br>"
				"configuration simply lacks. This new set of "
				"defaults will fix<br>that by adding it in. "
				"You can re-configure things now to your<br>"
				"liking. Sorry for the inconvenience.<br>"));
          }

        /* Ardvarks */
        else if (scale_conf->version > MOD_CONFIG_FILE_VERSION) 
          {
             /* config too new...wtf ? */
             _scale_conf_free();
	     ecore_timer_add(1.0, _scale_conf_timer, 
			     D_("Your Scale Module configuration is NEWER "
				"than the module version. This is "
				"very<br>strange. This should not happen unless"
				" you downgraded<br>the module or "
				"copied the configuration from a place where"
				"<br>a newer version of the module "
				"was running. This is bad and<br>as a "
				"precaution your configuration has been now "
				"restored to<br>defaults. Sorry for the "
				"inconvenience.<br>"));
          }
     }

   /* if we don't have a config yet, or it got erased above, 
    * then create a default one */
   if (!scale_conf) _scale_conf_new();

   /* create a link from the modules config to the module
    * this is not written */
   scale_conf->module = m;

   /* Tell any gadget containers (shelves, etc) that we provide a module
    * for the user to enjoy */
   e_gadcon_provider_register(&_gc_class);

   act = e_action_add("scale");
   if (act)
     {
	/* act->func.go = _e_mod_action_cb; */
	act->func.go_edge = _e_mod_action_cb_edge;
	e_action_predef_name_set
	  (D_("Desktop"),
	   D_("Scale Windows"),
	   "scale", "", NULL, 0);
     }

   /* Give E the module */
   return m;
}

/*
 * Function to unload the module
 */
EAPI int 
e_modapi_shutdown(E_Module *m) 
{
   /* Unregister the config dialog from the main panel */
   e_configure_registry_item_del("advanced/scale");

   /* Remove the config panel category if we can. E will tell us.
      category stays if other items using it */
   e_configure_registry_category_del("advanced");

   /* Kill the config dialog */
   if (scale_conf->cfd) e_object_del(E_OBJECT(scale_conf->cfd));
   scale_conf->cfd = NULL;

   /* Tell E the module is now unloaded. Gets removed from shelves, etc. */
   scale_conf->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);

   /* Cleanup our item list */
   while (scale_conf->conf_items) 
     {
        Config_Item *ci = NULL;

        /* Grab an item from the list */
        ci = scale_conf->conf_items->data;

        /* remove it */
        scale_conf->conf_items = 
          eina_list_remove_list(scale_conf->conf_items, 
                                scale_conf->conf_items);

        /* cleanup stringshares */
        if (ci->id) eina_stringshare_del(ci->id);

        /* keep the planet green */
        E_FREE(ci);
     }

   /* Cleanup the main config structure */
   E_FREE(scale_conf);

   /* Clean EET */
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);

   if (act)
     {
	e_action_predef_name_del(D_("Desktop"),
				 D_("Scale Windows"));
	e_action_del("scale");
     }

   return 1;
}

/*
 * Function to Save the modules config
 */ 
EAPI int 
e_modapi_save(E_Module *m) 
{
   e_config_domain_save("module.scale", conf_edd, scale_conf);
   return 1;
}

/* Local Functions */

/* Called when Gadget Controller (gadcon) says to appear in scene */
static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   Instance *inst = NULL;
   char buf[4096];

   /* theme file */
   snprintf(buf, sizeof(buf), "%s/e-module-scale.edj", 
            scale_conf->module->dir);

   /* New visual instance, any config ? */
   inst = E_NEW(Instance, 1);
   inst->conf_item = _scale_conf_item_get(id);

   /* create on-screen object */
   inst->o_scale = edje_object_add(gc->evas);
   /* we have a theme ? */
   if (!e_theme_edje_object_set(inst->o_scale, "base/theme/modules/scale", 
                                "modules/scale/main"))
     edje_object_file_set(inst->o_scale, buf, "modules/scale/main");

   /* Start loading our module on screen via container */
   inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->o_scale);
   inst->gcc->data = inst;

   /* hook a mouse down. we want/have a popup menu, right ? */
   evas_object_event_callback_add(inst->o_scale, EVAS_CALLBACK_MOUSE_DOWN, 
                                  _scale_cb_mouse_down, inst);

   /* add to list of running instances so we can cleanup later */
   instances = eina_list_append(instances, inst);

   /* return the Gadget_Container Client */
   return inst->gcc;
}

/* Called when Gadget_Container says stop */
static void 
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst = NULL;

   if (!(inst = gcc->data)) return;
   instances = eina_list_remove(instances, inst);

   /* kill popup menu */
   if (inst->menu) 
     {
        e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
        e_object_del(E_OBJECT(inst->menu));
        inst->menu = NULL;
     }
   /* delete the visual */
   if (inst->o_scale) 
     {
        /* remove mouse down callback hook */
        evas_object_event_callback_del(inst->o_scale, EVAS_CALLBACK_MOUSE_DOWN, 
                                       _scale_cb_mouse_down);
        evas_object_del(inst->o_scale);
     }
   E_FREE(inst);
}

/* For when container says we are changing position */
static void 
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient) 
{
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

/* Gadget/Module label, name for our module */
static char *
_gc_label(E_Gadcon_Client_Class *client_class) 
{
   return D_("Scale");
}

/* so E can keep a unique instance per-container */
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

   /* theme */
   snprintf(buf, sizeof(buf), "%s/e-module-scale.edj", scale_conf->module->dir);

   /* create icon object */
   o = edje_object_add(evas);

   /* load icon from theme */
   edje_object_file_set(o, buf, "icon");

   return o;
}

/* new module needs a new config :), or config too old and we need one anyway */
static void 
_scale_conf_new(void) 
{
   /* Config_Item *ci = NULL; */
   /* char buf[128]; */

   scale_conf = E_NEW(Config, 1);
   scale_conf->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) if ((scale_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

   /* setup defaults */
   IFMODCFG(0x008d);
   scale_conf->switch1 = 1;
   _scale_conf_item_get(NULL);
   IFMODCFGEND;

   /* update the version */
   scale_conf->version = MOD_CONFIG_FILE_VERSION;

   /* setup limits on the config properties here (if needed) */

   /* save the config to disk */
   e_config_save_queue();
}

/* This is called when we need to cleanup the actual configuration,
 * for example when our configuration is too old */
static void 
_scale_conf_free(void) 
{
   /* cleanup any stringshares here */
   while (scale_conf->conf_items) 
     {
        Config_Item *ci = NULL;

        ci = scale_conf->conf_items->data;
        scale_conf->conf_items = 
          eina_list_remove_list(scale_conf->conf_items, 
                                scale_conf->conf_items);
        /* EPA */
        if (ci->id) eina_stringshare_del(ci->id);
        E_FREE(ci);
     }

   E_FREE(scale_conf);
}

/* timer for the config oops dialog (old configuration needs update) */
static Eina_Bool 
_scale_conf_timer(void *data) 
{
   e_util_dialog_internal( D_("Scale Windows Configuration Updated"), data);
   return EINA_FALSE;
}

/* function to search for any Config_Item struct for this Item
 * create if needed */
static Config_Item *
_scale_conf_item_get(const char *id) 
{
   Eina_List *l = NULL;
   Config_Item *ci = NULL;
   char buf[128];

   if (!id) 
     {
        /* nothing passed, return a new id */
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

/* Pants On */
static void 
_scale_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event) 
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

        /* grab current zone */
        zone = e_util_zone_current_get(e_manager_current_get());

        /* create popup menu */
        ma = e_menu_new();
        e_menu_post_deactivate_callback_set(ma, _scale_cb_menu_post, inst);
        inst->menu = ma;

        mg = e_menu_new();

        mi = e_menu_item_new(mg);
        e_menu_item_label_set(mi, D_("Settings"));
        e_util_menu_item_theme_icon_set(mi, "preferences-system");
        e_menu_item_callback_set(mi, _scale_cb_menu_configure, NULL);

        /* Each Gadget Client has a utility menu from the Container */
        e_gadcon_client_util_menu_items_append(inst->gcc, ma, mg, 0);
        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, 
                                          NULL, NULL);

        /* show the menu relative to gadgets position */
        e_menu_activate_mouse(ma, zone, (x + ev->output.x), 
                              (y + ev->output.y), 1, 1, 
                              E_MENU_POP_DIRECTION_AUTO, ev->timestamp);
        evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button, 
                                 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

/* popup menu closing, cleanup */
static void 
_scale_cb_menu_post(void *data, E_Menu *menu) 
{
   Instance *inst = NULL;

   if (!(inst = data)) return;
   if (!inst->menu) return;
   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

/* call configure from popup */
static void 
_scale_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   if (!scale_conf) return;
   if (scale_conf->cfd) return;
   e_int_config_scale_module(mn->zone->container, NULL);
}
