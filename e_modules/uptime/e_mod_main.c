#include <e.h>
#include <linux/kernel.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

static Uptime *_uptime_init                   (E_Module *m);
static void _uptime_shutdown               (Uptime *n);
static void _uptime_config_menu_new        (Uptime *n);
static int  _uptime_face_init              (Uptime_Face *cf);
static void _uptime_face_menu_new          (Uptime_Face *cf);
static void _uptime_face_enable            (Uptime_Face *cf);
static void _uptime_face_disable           (Uptime_Face *cf);
static void _uptime_face_free              (Uptime_Face *cf);
static void _uptime_face_cb_gmc_change     (void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static void _uptime_face_cb_mouse_down     (void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _uptime_face_cb_menu_edit      (void *data, E_Menu *mn, E_Menu_Item *mi);
static void _uptime_face_cb_menu_configure (void *data, E_Menu *mn, E_Menu_Item *mi);
static int  _uptime_face_update_values     (void *data);

static int uptime_count;

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
     "Uptime"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   Uptime *c;
   
   c = _uptime_init(m);
   if (!c)
     return NULL;
   
   m->config_menu = c->config_menu;
   return c;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   Uptime *c;
   
   c = m->data;
   if (!c)
     return 0;
   
   if (m->config_menu) 
     {
	e_menu_deactivate(m->config_menu);
	e_object_del(E_OBJECT(m->config_menu));
	m->config_menu = NULL;
     }
   if (c->cfd) 
     {
	e_object_del(E_OBJECT(c->cfd));
	c->cfd = NULL;
     }
   _uptime_shutdown(c);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m) 
{
   Uptime *c;
   
   c = m->data;
   if (!c)
     return 0;
   e_config_domain_save("module.uptime", c->conf_edd, c->conf);
   return 1;
}

EAPI int
e_modapi_info(E_Module *m) 
{
   m->icon_file = strdup(PACKAGE_DATA_DIR"/module_icon.png");
   return 1;
}

EAPI int
e_modapi_about(E_Module *m) 
{
   e_module_dialog_show(_("Enlightenment Uptime Monitor Module"),
			_("This module is used to monitor uptime."));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m) 
{
   Uptime *c;
   E_Container *con;
   
   c = m->data;
   if (!c)
     return 0;
   if (!c->face)
     return 0;
   
   con = e_container_current_get(e_manager_current_get());
   if (c->face->con == con)
     _configure_uptime_module(con, c);
   
   return 1;
}

static Uptime *
_uptime_init(E_Module *m) 
{
   Uptime *c;
   E_Menu_Item *mi;
   Evas_List *mans, *l, *l2;
   
   c = E_NEW(Uptime, 1);
   if (!c)
     return NULL;
   
   c->conf_edd = E_CONFIG_DD_NEW("Uptime_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D c->conf_edd
   E_CONFIG_VAL(D, T, check_interval, INT);
   
   c->conf = e_config_domain_load("module.uptime", c->conf_edd);
   if (!c->conf) 
     {
	c->conf = E_NEW(Config, 1);
	c->conf->check_interval = 1;
     }
   E_CONFIG_LIMIT(c->conf->check_interval, 0, 60);
   
   _uptime_config_menu_new(c);
   
   mans = e_manager_list();
   for (l = mans; l; l = l->next) 
     {
	E_Manager *man;
	
	man = l->data;
	for (l2 = man->containers; l2; l2 = l2->next) 
	  {
	     E_Container *con;
	     Uptime_Face *cf;
	     
	     con = l2->data;
	     cf = E_NEW(Uptime_Face, 1);
	     if (cf) 
	       {
		  cf->conf_face_edd = E_CONFIG_DD_NEW("Uptime_Face_Config", Config_Face);
		  #undef T
		  #undef D
		  #define T Config_Face
		  #define D cf->conf_face_edd
		  E_CONFIG_VAL(D, T, enabled, UCHAR);
		  
		  c->face = cf;
		  cf->uptime = c;		  
		  cf->con = con;
		  cf->evas = con->bg_evas;
		  
		  cf->conf = E_NEW(Config_Face, 1);
		  cf->conf->enabled = 1;
		  
		  if (!_uptime_face_init(cf))
		    return NULL;
		  
		  _uptime_face_menu_new(cf);
		  
		  mi = e_menu_item_new(c->config_menu);
		  e_menu_item_label_set(mi, _("Configuration"));
		  e_menu_item_callback_set(mi, _uptime_face_cb_menu_configure, cf);
		  
		  mi = e_menu_item_new(c->config_menu);
		  e_menu_item_label_set(mi, con->name);
		  e_menu_item_submenu_set(mi, cf->menu);
		 
		  if (!cf->conf->enabled)
		    _uptime_face_disable(cf);
		  else
		    _uptime_face_enable(cf);
	       }
	  }
     }
   return c;
}

static void
_uptime_shutdown(Uptime *c) 
{
   _uptime_face_free(c->face);
   
   E_FREE(c->conf);
   E_CONFIG_DD_FREE(c->conf_edd);
   E_FREE(c);
}

static void
_uptime_config_menu_new(Uptime *c) 
{
   E_Menu *mn;
   
   mn = e_menu_new();
   c->config_menu = mn;
}

static int
_uptime_face_init(Uptime_Face *cf) 
{
   Evas_Object *o;
   char buf[4096];
   
   evas_event_freeze(cf->evas);
   
   o = edje_object_add(cf->evas);
   cf->uptime_obj = o;
   
   if (!e_theme_edje_object_set(o, "base/theme/modules/uptime", 
				"modules/uptime/main")) 
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/uptime.edj");	
	edje_object_file_set(o, buf, "modules/uptime/main");
     }
   
   evas_object_show(o);

   o = evas_object_rectangle_add(cf->evas);
   cf->event_obj = o;
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
				  _uptime_face_cb_mouse_down, cf);
   evas_object_show(o);
   
   cf->gmc = e_gadman_client_new(cf->con->gadman);
   e_gadman_client_domain_set(cf->gmc, "module.uptime", uptime_count++);
   e_gadman_client_policy_set(cf->gmc,
			      E_GADMAN_POLICY_ANYWHERE |
			      E_GADMAN_POLICY_HMOVE |
			      E_GADMAN_POLICY_VMOVE);
   e_gadman_client_min_size_set(cf->gmc, 4, 4);
   e_gadman_client_max_size_set(cf->gmc, 160, 20);
   e_gadman_client_auto_size_set(cf->gmc, 160, 20);
   e_gadman_client_align_set(cf->gmc, 1.0, 1.0);
   e_gadman_client_resize(cf->gmc, 160, 20);
   e_gadman_client_change_func_set(cf->gmc, _uptime_face_cb_gmc_change, cf);
   e_gadman_client_load(cf->gmc);
   evas_event_thaw(cf->evas);

   cf->monitor = ecore_timer_add((double)cf->uptime->conf->check_interval, _uptime_face_update_values, cf);
   return 1;
}

static void
_uptime_face_menu_new(Uptime_Face *cf) 
{
   E_Menu *mn;
   E_Menu_Item *mi;
   
   mn = e_menu_new();
   cf->menu = mn;
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");         
   e_menu_item_callback_set(mi, _uptime_face_cb_menu_configure, cf);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/gadgets");         
   e_menu_item_callback_set(mi, _uptime_face_cb_menu_edit, cf);
}

static void
_uptime_face_enable(Uptime_Face *cf) 
{
   cf->conf->enabled = 1;
   e_config_save_queue();
   evas_object_show(cf->uptime_obj);
   /* evas_object_show(cf->chart_obj); */
   evas_object_show(cf->event_obj);
}

static void
_uptime_face_disable(Uptime_Face *cf) 
{
   cf->conf->enabled = 0;
   e_config_save_queue();
   evas_object_hide(cf->event_obj);
   /* evas_object_hide(cf->chart_obj); */
   evas_object_hide(cf->uptime_obj);
}

static void 
_uptime_face_free(Uptime_Face *cf) 
{
   if (cf->monitor)
     ecore_timer_del(cf->monitor);
   if (cf->menu)
     e_object_del(E_OBJECT(cf->menu));
   if (cf->event_obj)
     evas_object_del(cf->event_obj);
   if (cf->uptime_obj)
     evas_object_del(cf->uptime_obj);

   /*
   if (cf->chart_obj)
     evas_object_del(cf->chart_obj);
    */
   
   if (cf->gmc) 
     {
	e_gadman_client_save(cf->gmc);
	e_object_del(E_OBJECT(cf->gmc));
     }
   
   E_FREE(cf->conf);
   E_FREE(cf);
   uptime_count--;
}

static void 
_uptime_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change) 
{
   Uptime_Face *cf;
   Evas_Coord x, y, w, h;
   
   cf = data;
   switch (change) 
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	e_gadman_client_geometry_get(cf->gmc, &x, &y, &w, &h);
	evas_object_move(cf->uptime_obj, x, y);
	/* evas_object_move(cf->chart_obj, x, y); */
	evas_object_move(cf->event_obj, x, y);
	evas_object_resize(cf->uptime_obj, w, h);
	/* evas_object_resize(cf->chart_obj, w, h); */
	evas_object_resize(cf->event_obj, w, h);
	break;
      case E_GADMAN_CHANGE_RAISE:
	evas_object_raise(cf->uptime_obj);
	/* evas_object_raise(cf->chart_obj); */
	evas_object_raise(cf->event_obj);
	break;
      default:
	break;
     }   
}

static void 
_uptime_face_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info) 
{
   Uptime_Face *cf;
   Evas_Event_Mouse_Down *ev;
   
   ev = event_info;
   cf = data;
   if (ev->button == 3) 
     {
	e_menu_activate_mouse(cf->menu, e_zone_current_get(cf->con),
			      ev->output.x, ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	e_util_container_fake_mouse_up_all_later(cf->con);
     }
}

static void 
_uptime_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Uptime_Face *cf;
   
   cf = data;
   e_gadman_mode_set(cf->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void 
_uptime_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Uptime_Face *cf;

   cf = data;
   _configure_uptime_module(cf->con, cf->uptime);
}

static int 
_uptime_face_update_values(void *data) 
{
   Uptime_Face *cf;
   char u_date_time[256];
   struct sysinfo s_info;
   long minute = 60;
   long hour = minute * 60;
   long day = hour * 24;
  
   cf = data;
   sysinfo(&s_info);
   sprintf(u_date_time, "uptime: %ld days, %ld:%02ld:%02ld",
	   s_info.uptime / day, (s_info.uptime % day) / hour,
	   (s_info.uptime % hour) / minute, s_info.uptime % minute);
   edje_object_part_text_set(cf->uptime_obj, "uptime", u_date_time);   
   return 1;
}
