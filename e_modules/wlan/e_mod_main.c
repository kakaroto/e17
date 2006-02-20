#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

static Wlan *_wlan_init(E_Module *m);
static void _wlan_shutdown(Wlan *n);
static void _wlan_config_menu_new(Wlan *n);

static int  _wlan_face_init(Wlan_Face *nf);
static void _wlan_face_menu_new(Wlan_Face *nf);
static void _wlan_face_enable(Wlan_Face *nf);
static void _wlan_face_disable(Wlan_Face *nf);
static void _wlan_face_free(Wlan_Face *nf);
static void _wlan_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static void _wlan_face_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _wlan_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi);
static void _wlan_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);
static int  _wlan_face_update_values(void *data);

static int wlan_count;

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
     "Wlan"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   Wlan *n;
   
   n = _wlan_init(m);
   if (!n)
     return NULL;
   
   m->config_menu = n->config_menu;
   return n;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   Wlan *n;
   
   n = m->data;
   if (!n)
     return 0;
   
   if (m->config_menu) 
     {
	e_menu_deactivate(m->config_menu);
	e_object_del(E_OBJECT(m->config_menu));
	m->config_menu = NULL;
     }
   if (n->cfd) 
     {
	e_object_del(E_OBJECT(n->cfd));
	n->cfd = NULL;
     }
   _wlan_shutdown(n);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m) 
{
   Wlan *n;
   
   n = m->data;
   if (!n)
     return 0;
   e_config_domain_save("module.wlan", n->conf_edd, n->conf);
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
   e_module_dialog_show(_("Enlightenment WLan Monitor Module"),
			_("This module is used to monitor a wlan device."));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m) 
{
   Wlan *n;
   E_Container *con;
   
   n = m->data;
   if (!n)
     return 0;
   if (!n->face)
     return 0;
   
   con = e_container_current_get(e_manager_current_get());
   if (n->face->con == con)
     _configure_wlan_module(con, n);
   
   return 1;
}

static Wlan *
_wlan_init(E_Module *m) 
{
   Wlan *n;
   E_Menu_Item *mi;
   Evas_List *mans, *l, *l2;
   
   n = E_NEW(Wlan, 1);
   if (!n)
     return NULL;
   
   n->conf_edd = E_CONFIG_DD_NEW("Wlan_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D n->conf_edd
   E_CONFIG_VAL(D, T, device, STR);
   E_CONFIG_VAL(D, T, check_interval, INT);
   
   n->conf = e_config_domain_load("module.wlan", n->conf_edd);
   if (!n->conf) 
     {
	n->conf = E_NEW(Config, 1);
	n->conf->device = (char *)evas_stringshare_add("eth0");
	n->conf->check_interval = 30;
     }
   E_CONFIG_LIMIT(n->conf->check_interval, 0, 60);
   
   _wlan_config_menu_new(n);
   
   mans = e_manager_list();
   for (l = mans; l; l = l->next) 
     {
	E_Manager *man;
	
	man = l->data;
	for (l2 = man->containers; l2; l2 = l2->next) 
	  {
	     E_Container *con;
	     Wlan_Face *nf;
	     
	     con = l2->data;
	     nf = E_NEW(Wlan_Face, 1);
	     if (nf) 
	       {
		  nf->conf_face_edd = E_CONFIG_DD_NEW("Wlan_Face_Config", Config_Face);
		  #undef T
		  #undef D
		  #define T Config_Face
		  #define D nf->conf_face_edd
		  E_CONFIG_VAL(D, T, enabled, UCHAR);
		  
		  n->face = nf;
		  nf->wlan = n;		  
		  nf->con = con;
		  nf->evas = con->bg_evas;
		  
		  nf->conf = E_NEW(Config_Face, 1);
		  nf->conf->enabled = 1;
		  
		  if (!_wlan_face_init(nf))
		    return NULL;
		  
		  _wlan_face_menu_new(nf);
		  
		  mi = e_menu_item_new(n->config_menu);
		  e_menu_item_label_set(mi, _("Configuration"));
		  e_menu_item_callback_set(mi, _wlan_face_cb_menu_configure, nf);
		  
		  mi = e_menu_item_new(n->config_menu);
		  e_menu_item_label_set(mi, con->name);
		  e_menu_item_submenu_set(mi, nf->menu);
		 
		  if (!nf->conf->enabled)
		    _wlan_face_disable(nf);
		  else
		    _wlan_face_enable(nf);
	       }
	  }
     }
   return n;
}

static void
_wlan_shutdown(Wlan *n) 
{
   _wlan_face_free(n->face);

   if (n->conf->device)
     evas_stringshare_del(n->conf->device);
   
   E_FREE(n->conf);
   E_CONFIG_DD_FREE(n->conf_edd);
   E_FREE(n);
}

static void
_wlan_config_menu_new(Wlan *n) 
{
   E_Menu *mn;
   
   mn = e_menu_new();
   n->config_menu = mn;
}

static int
_wlan_face_init(Wlan_Face *nf) 
{
   Evas_Object *o;
   char buf[4096];
   
   evas_event_freeze(nf->evas);
   
   o = edje_object_add(nf->evas);
   nf->wlan_obj = o;
   
   if (!e_theme_edje_object_set(o, "base/theme/modules/wlan", 
				"modules/wlan/main")) 
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/wlan.edj");	
	edje_object_file_set(o, buf, "modules/wlan/main");
     }
   
   evas_object_show(o);

   /*
   o = evas_object_rectangle_add(nf->evas);
   nf->chart_obj = o;
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 0);
   evas_object_color_set(o, 0, 0, 0, 255);
   evas_object_show(o);
   */
   
   o = evas_object_rectangle_add(nf->evas);
   nf->event_obj = o;
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
				  _wlan_face_cb_mouse_down, nf);
   evas_object_show(o);
   
   nf->gmc = e_gadman_client_new(nf->con->gadman);
   e_gadman_client_domain_set(nf->gmc, "module.wlan", wlan_count++);
   e_gadman_client_policy_set(nf->gmc,
			      E_GADMAN_POLICY_ANYWHERE |
			      E_GADMAN_POLICY_HMOVE |
			      E_GADMAN_POLICY_HSIZE |
			      E_GADMAN_POLICY_VMOVE |
			      E_GADMAN_POLICY_VSIZE);
   e_gadman_client_min_size_set(nf->gmc, 4, 4);
   e_gadman_client_max_size_set(nf->gmc, 128, 128);
   e_gadman_client_auto_size_set(nf->gmc, 40, 40);
   e_gadman_client_align_set(nf->gmc, 1.0, 1.0);
   e_gadman_client_aspect_set(nf->gmc, 1.0, 1.0);
   e_gadman_client_resize(nf->gmc, 40, 40);
   e_gadman_client_change_func_set(nf->gmc, _wlan_face_cb_gmc_change, nf);
   e_gadman_client_load(nf->gmc);
   evas_event_thaw(nf->evas);

   nf->monitor = ecore_timer_add((double)nf->wlan->conf->check_interval, _wlan_face_update_values, nf);
   return 1;
}

static void
_wlan_face_menu_new(Wlan_Face *nf) 
{
   E_Menu *mn;
   E_Menu_Item *mi;
   
   mn = e_menu_new();
   nf->menu = mn;
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_menu_item_callback_set(mi, _wlan_face_cb_menu_configure, nf);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_menu_item_callback_set(mi, _wlan_face_cb_menu_edit, nf);
}

static void
_wlan_face_enable(Wlan_Face *nf) 
{
   nf->conf->enabled = 1;
   e_config_save_queue();
   evas_object_show(nf->wlan_obj);
   /* evas_object_show(nf->chart_obj); */
   evas_object_show(nf->event_obj);
}

static void
_wlan_face_disable(Wlan_Face *nf) 
{
   nf->conf->enabled = 0;
   e_config_save_queue();
   evas_object_hide(nf->event_obj);
   /* evas_object_hide(nf->chart_obj); */
   evas_object_hide(nf->wlan_obj);
}

static void 
_wlan_face_free(Wlan_Face *nf) 
{
   if (nf->monitor)
     ecore_timer_del(nf->monitor);
   if (nf->menu)
     e_object_del(E_OBJECT(nf->menu));
   if (nf->event_obj)
     evas_object_del(nf->event_obj);
   if (nf->wlan_obj)
     evas_object_del(nf->wlan_obj);

   /*
   if (nf->chart_obj)
     evas_object_del(nf->chart_obj);
    */
   
   if (nf->gmc) 
     {
	e_gadman_client_save(nf->gmc);
	e_object_del(E_OBJECT(nf->gmc));
     }
   
   E_FREE(nf->conf);
   E_FREE(nf);
   wlan_count--;
}

static void 
_wlan_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change) 
{
   Wlan_Face *nf;
   Evas_Coord x, y, w, h;
   
   nf = data;
   switch (change) 
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	e_gadman_client_geometry_get(nf->gmc, &x, &y, &w, &h);
	evas_object_move(nf->wlan_obj, x, y);
	/* evas_object_move(nf->chart_obj, x, y); */
	evas_object_move(nf->event_obj, x, y);
	evas_object_resize(nf->wlan_obj, w, h);
	/* evas_object_resize(nf->chart_obj, w, h); */
	evas_object_resize(nf->event_obj, w, h);
	break;
      case E_GADMAN_CHANGE_RAISE:
	evas_object_raise(nf->wlan_obj);
	/* evas_object_raise(nf->chart_obj); */
	evas_object_raise(nf->event_obj);
	break;
      default:
	break;
     }   
}

static void 
_wlan_face_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info) 
{
   Wlan_Face *nf;
   Evas_Event_Mouse_Down *ev;
   
   ev = event_info;
   nf = data;
   if (ev->button == 3) 
     {
	e_menu_activate_mouse(nf->menu, e_zone_current_get(nf->con),
			      ev->output.x, ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	e_util_container_fake_mouse_up_all_later(nf->con);
     }
}

static void 
_wlan_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Wlan_Face *nf;
   
   nf = data;
   e_gadman_mode_set(nf->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void 
_wlan_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Wlan_Face *nf;

   nf = data;
   _configure_wlan_module(nf->con, nf->wlan);
}

static int 
_wlan_face_update_values(void *data) 
{
   Wlan_Face *nf;
   int new_status, new_link, new_level, new_noise;
   unsigned int dummy;
   char iface[64];
   char buf[256];
   FILE *stat;
   int found_dev = 0;
   int wlan_status = 0;
   int wlan_link = 0;
   int wlan_level = 0;
   int wlan_noise = 0;
   
   stat = fopen("/proc/net/wireless", "r");
   if (!stat)
      return 1;

   while (fgets(buf, 256, stat))
     {
        int i = 0;

        /* remove : */
        for (; buf[i] != 0; i++)
	  if (buf[i] == ':' || buf[i] == '.')
	    buf[i] = ' ';

        if (sscanf(buf, "%s %u %u %u %u %u %u %u %u %u %u",
                   iface, &wlan_status, &wlan_link, &wlan_level,
                   &wlan_noise, &dummy, &dummy, &dummy, &dummy,
                   &dummy, &dummy) < 11)
	  continue;

        if (!strcmp(iface, nf->wlan->conf->device))
          {
             found_dev = 1;
             break;
          }
     }
   fclose(stat);
   
   if (!found_dev)
     return 1;

   /* Update the modules text */
   Edje_Message_Int_Set *msg;
   
   msg = malloc(sizeof(Edje_Message_Int_Set) + 1 * sizeof(int));
   msg->count = 1;
   msg->val[0] = wlan_link;
   edje_object_message_send(nf->wlan_obj, EDJE_MESSAGE_INT_SET, 1, msg);
   free(msg);

   return 1;
}
