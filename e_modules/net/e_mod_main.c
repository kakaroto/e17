#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

static Net *_net_init(E_Module *m);
static void _net_shutdown(Net *n);
static void _net_config_menu_new(Net *n);

static int  _net_face_init(Net_Face *nf);
static void _net_face_menu_new(Net_Face *nf);
static void _net_face_enable(Net_Face *nf);
static void _net_face_disable(Net_Face *nf);
static void _net_face_free(Net_Face *nf);
static void _net_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static void _net_face_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _net_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi);
static void _net_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);

static int  _net_face_update_values(void *data);
static void _net_face_graph_values(Net_Face *nf, int in, int out);

static int net_count;

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
     "Net"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   Net *n;
   
   n = _net_init(m);
   if (!n)
     return NULL;
   
   m->config_menu = n->config_menu;
   return n;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   Net *n;
   
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
   _net_shutdown(n);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m) 
{
   Net *n;
   
   n = m->data;
   if (!n)
     return 0;
   e_config_domain_save("module.net", n->conf_edd, n->conf);
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
   e_module_dialog_show(_("Enlightenment Network Monitor Module"),
			_("This module is used to monitor a network device."));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m) 
{
   Net *n;
   E_Container *con;
   
   n = m->data;
   if (!n)
     return 0;
   if (!n->face)
     return 0;
   
   con = e_container_current_get(e_manager_current_get());
   if (n->face->con == con)
     _configure_net_module(con, n);
   
   return 1;
}

static Net *
_net_init(E_Module *m) 
{
   Net *n;
   E_Menu_Item *mi;
   Evas_List *mans, *l, *l2;
   
   n = E_NEW(Net, 1);
   if (!n)
     return NULL;
   
   n->conf_edd = E_CONFIG_DD_NEW("Net_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D n->conf_edd
   E_CONFIG_VAL(D, T, device, STR);
   E_CONFIG_VAL(D, T, check_interval, INT);
   
   n->conf = e_config_domain_load("module.net", n->conf_edd);
   if (!n->conf) 
     {
	n->conf = E_NEW(Config, 1);
	n->conf->device = (char *)evas_stringshare_add("eth0");
	n->conf->check_interval = 30;
     }
   E_CONFIG_LIMIT(n->conf->check_interval, 0, 60);
   
   _net_config_menu_new(n);
   
   mans = e_manager_list();
   for (l = mans; l; l = l->next) 
     {
	E_Manager *man;
	
	man = l->data;
	for (l2 = man->containers; l2; l2 = l2->next) 
	  {
	     E_Container *con;
	     Net_Face *nf;
	     
	     con = l2->data;
	     nf = E_NEW(Net_Face, 1);
	     if (nf) 
	       {
		  nf->conf_face_edd = E_CONFIG_DD_NEW("Net_Face_Config", Config_Face);
		  #undef T
		  #undef D
		  #define T Config_Face
		  #define D nf->conf_face_edd
		  E_CONFIG_VAL(D, T, enabled, UCHAR);
		  
		  n->face = nf;
		  nf->net = n;		  
		  nf->con = con;
		  nf->evas = con->bg_evas;
		  
		  nf->conf = E_NEW(Config_Face, 1);
		  nf->conf->enabled = 1;
		  
		  if (!_net_face_init(nf))
		    return NULL;
		  
		  _net_face_menu_new(nf);
		  
		  mi = e_menu_item_new(n->config_menu);
		  e_menu_item_label_set(mi, _("Configuration"));
		  e_menu_item_callback_set(mi, _net_face_cb_menu_configure, nf);
		  
		  mi = e_menu_item_new(n->config_menu);
		  e_menu_item_label_set(mi, con->name);
		  e_menu_item_submenu_set(mi, nf->menu);
		 
		  if (!nf->conf->enabled)
		    _net_face_disable(nf);
		  else
		    _net_face_enable(nf);
	       }
	  }
     }
   return n;
}

static void
_net_shutdown(Net *n) 
{
   _net_face_free(n->face);

   if (n->conf->device)
     evas_stringshare_del(n->conf->device);
   
   E_FREE(n->conf);
   E_CONFIG_DD_FREE(n->conf_edd);
   E_FREE(n);
}

static void
_net_config_menu_new(Net *n) 
{
   E_Menu *mn;
   
   mn = e_menu_new();
   n->config_menu = mn;
}

static int
_net_face_init(Net_Face *nf) 
{
   Evas_Object *o;
   char buf[4096];
   
   evas_event_freeze(nf->evas);
   
   o = edje_object_add(nf->evas);
   nf->net_obj = o;
   
   if (!e_theme_edje_object_set(o, "base/theme/modules/net", 
				"modules/net/main")) 
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/net.edj");	
	edje_object_file_set(o, buf, "modules/net/main");
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
				  _net_face_cb_mouse_down, nf);
   evas_object_show(o);
   
   nf->gmc = e_gadman_client_new(nf->con->gadman);
   e_gadman_client_domain_set(nf->gmc, "module.net", net_count++);
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
   e_gadman_client_change_func_set(nf->gmc, _net_face_cb_gmc_change, nf);
   e_gadman_client_load(nf->gmc);
   evas_event_thaw(nf->evas);

   nf->monitor = ecore_timer_add((double)nf->net->conf->check_interval, _net_face_update_values, nf);
   return 1;
}

static void
_net_face_menu_new(Net_Face *nf) 
{
   E_Menu *mn;
   E_Menu_Item *mi;
   
   mn = e_menu_new();
   nf->menu = mn;
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_menu_item_callback_set(mi, _net_face_cb_menu_configure, nf);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_menu_item_callback_set(mi, _net_face_cb_menu_edit, nf);
}

static void
_net_face_enable(Net_Face *nf) 
{
   nf->conf->enabled = 1;
   e_config_save_queue();
   evas_object_show(nf->net_obj);
   /* evas_object_show(nf->chart_obj); */
   evas_object_show(nf->event_obj);
}

static void
_net_face_disable(Net_Face *nf) 
{
   nf->conf->enabled = 0;
   e_config_save_queue();
   evas_object_hide(nf->event_obj);
   /* evas_object_hide(nf->chart_obj); */
   evas_object_hide(nf->net_obj);
}

static void 
_net_face_free(Net_Face *nf) 
{
   if (nf->monitor)
     ecore_timer_del(nf->monitor);
   if (nf->menu)
     e_object_del(E_OBJECT(nf->menu));
   if (nf->event_obj)
     evas_object_del(nf->event_obj);
   if (nf->net_obj)
     evas_object_del(nf->net_obj);

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
   net_count--;
}

static void 
_net_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change) 
{
   Net_Face *nf;
   Evas_Coord x, y, w, h;
   
   nf = data;
   switch (change) 
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	e_gadman_client_geometry_get(nf->gmc, &x, &y, &w, &h);
	evas_object_move(nf->net_obj, x, y);
	/* evas_object_move(nf->chart_obj, x, y); */
	evas_object_move(nf->event_obj, x, y);
	evas_object_resize(nf->net_obj, w, h);
	/* evas_object_resize(nf->chart_obj, w, h); */
	evas_object_resize(nf->event_obj, w, h);
	break;
      case E_GADMAN_CHANGE_RAISE:
	evas_object_raise(nf->net_obj);
	/* evas_object_raise(nf->chart_obj); */
	evas_object_raise(nf->event_obj);
	break;
      default:
	break;
     }   
}

static void 
_net_face_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info) 
{
   Net_Face *nf;
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
_net_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Net_Face *nf;
   
   nf = data;
   e_gadman_mode_set(nf->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void 
_net_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Net_Face *nf;

   nf = data;
   _configure_net_module(nf->con, nf->net);
}

static int 
_net_face_update_values(void *data) 
{
   Net_Face *nf;
   FILE *stat;
   char dev[64];
   char buf[256];
   unsigned long in, out, dummy;
   static unsigned long old_in, old_out;
   int found;
   
   nf = data;
   
   stat = fopen("/proc/net/dev", "r");
   if (!stat)
     return 1;

   found = 0;   
   while (fgets(buf, 256, stat)) 
     {
	int i = 0;
	for (; buf[i] != 0; i++) 
	  {
	     if (buf[i] == ':')
	       buf[i] = ' ';
	  }
        if (sscanf(buf, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu "
                   "%lu %lu %lu %lu\n", dev, &in, &dummy, &dummy,
                   &dummy, &dummy, &dummy, &dummy, &dummy, &out, &dummy,
                   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy) < 17)
           continue;
	if (!strcmp(dev, strdup(nf->net->conf->device))) 
	  {
	     found = 1;
	     break;
	  }
     }
   fclose(stat);
   
   if (!found) 
     return 1;
   
   if (in != old_in)
     old_in = in;
   else
     in = 0;
   
   if (out != old_out)
     old_out = out;
   else
     out = 0;

   /* Graph values */
   /* _net_face_graph_values(nf, in, out); */
   
   /* Update the modules text */
   Edje_Message_Int_Set *msg;
   
   msg = malloc(sizeof(Edje_Message_Int_Set) + 1 * sizeof(int));
   msg->count = 2;
   msg->val[0] = in;
   msg->val[1] = out;   
   edje_object_message_send(nf->net_obj, EDJE_MESSAGE_INT_SET, 1, msg);
   free(msg);

   return 1;
}

static void 
_net_face_graph_values(Net_Face *nf, int in, int out) 
{
   Evas_Object *o;
   int x, y, w, h;
   double factor = 0.0;
   int in_val, out_val;
   
   if (!nf)
     return;
   
   evas_object_geometry_get(nf->net_obj, &x, &y, &w, &h);
   
   /* Graph Values */
   factor = ((double)h / (double)100);   
   in_val = (int)((double)in * factor);
   out_val = (int)((double)out * factor);

   if (in_val > 100)
     in_val = 100;
   if (out_val > 100)
     out_val = 100;
   
   o = evas_object_line_add(nf->evas);
   evas_object_clip_set(o, nf->chart_obj);
   evas_object_layer_set(o, 2);
   evas_object_line_xy_set(o, x + w, y, x + w, y + in_val);
   evas_object_color_set(o, 255, 0, 0, 128);
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);
}
