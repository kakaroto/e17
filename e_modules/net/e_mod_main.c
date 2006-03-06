#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

static Net      *_net_init(E_Module *m);
static void      _net_shutdown(Net *n);
static void      _net_config_menu_new(Net *n);

static Net_Face *_net_face_init(Net *n, E_Container *con);
static void      _net_face_menu_new(Net_Face *nf);
static void      _net_face_enable(Net_Face *nf);
static void      _net_face_disable(Net_Face *nf);
static void      _net_face_free(Net_Face *nf);
static void      _net_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static void      _net_face_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void      _net_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi);
static void      _net_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);
static int       _net_face_update_values(void *data);
static void      _net_face_graph_values(Net_Face *nf, int tx_val, int rx_val);
static void      _net_face_graph_clear(Net_Face *nf);

static int net_count;

static E_Config_DD *conf_edd;
static E_Config_DD *conf_face_edd;

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
   e_config_domain_save("module.net", conf_edd, n->conf);
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
   Evas_List *l;
   E_Container *con;
   
   n = m->data;
   if (!n)
     return 0;
   if (!n->faces)
     return 0;
   
   for (l = n->faces; l; l = l->next) 
     {
	Net_Face *nf;
	
	nf = l->data;
	if (!nf) 
	  continue;
	
	con = e_container_current_get(e_manager_current_get());
	if (nf->con == con) 
	  {
	     _configure_net_module(nf);
	     break;
	  }
     }
   return 1;
}

static Net *
_net_init(E_Module *m) 
{
   Net *n;
   E_Menu_Item *mi;
   Evas_List *mans, *l, *l2, *fl;
   
   n = E_NEW(Net, 1);
   if (!n)
     return NULL;
   
   conf_face_edd = E_CONFIG_DD_NEW("Net_Config_Face", Config_Face);
   #undef T
   #undef D
   #define T Config_Face
   #define D conf_face_edd
   E_CONFIG_VAL(D, T, enabled, UCHAR);
   E_CONFIG_VAL(D, T, device, STR);
   E_CONFIG_VAL(D, T, check_interval, INT);
   E_CONFIG_VAL(D, T, show_text, INT);
   E_CONFIG_VAL(D, T, show_graph, INT);
   
   conf_edd = E_CONFIG_DD_NEW("Net_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D conf_edd
   E_CONFIG_LIST(D, T, faces, conf_face_edd);
   
   n->conf = e_config_domain_load("module.net", conf_edd);
   if (!n->conf) 
     n->conf = E_NEW(Config, 1);
   
   _net_config_menu_new(n);
   
   mans = e_manager_list();
   fl = n->conf->faces;
   for (l = mans; l; l = l->next) 
     {
	E_Manager *man;
	
	man = l->data;
	for (l2 = man->containers; l2; l2 = l2->next) 
	  {
	     E_Container *con;
	     Net_Face *nf;
	     
	     con = l2->data;
	     nf = _net_face_init(n, con);
	     if (nf) 
	       {
		  if (!fl)
		    {
		       nf->conf = E_NEW(Config_Face, 1);
		       nf->conf->enabled = 1;
		       nf->conf->device = (char *)evas_stringshare_add("eth0");
		       nf->conf->check_interval = 30;
		       nf->conf->show_text = 1;
		       nf->conf->show_graph = 1;
		       n->conf->faces = evas_list_append(n->conf->faces, nf->conf);
		    }
		  else 
		    {
		       nf->conf = fl->data;
		       fl = fl->next;
		    }
		  E_CONFIG_LIMIT(nf->conf->check_interval, 0, 60);
		  E_CONFIG_LIMIT(nf->conf->show_text, 0, 1);
		  E_CONFIG_LIMIT(nf->conf->show_graph, 0, 1);
		  
		  nf->monitor = ecore_timer_add((double)nf->conf->check_interval, _net_face_update_values, nf);   
		  
		  _net_face_menu_new(nf);
		  
		  mi = e_menu_item_new(n->config_menu);
		  e_menu_item_label_set(mi, _("Configuration"));
		  e_menu_item_callback_set(mi, _net_face_cb_menu_configure, nf);
		  
		  mi = e_menu_item_new(n->config_menu);
		  e_menu_item_label_set(mi, con->name);
		  e_menu_item_submenu_set(mi, nf->menu);
		 
		  if (!nf->conf->enabled)
		    _net_face_disable(nf);
	       }
	  }
     }
   return n;
}

static void
_net_shutdown(Net *n) 
{
   E_CONFIG_DD_FREE(conf_edd);
   E_CONFIG_DD_FREE(conf_face_edd);

   while (n->faces)
     _net_face_free(n->faces->data);

   e_object_del(E_OBJECT(n->config_menu));
   evas_list_free(n->conf->faces);
   
   E_FREE(n->conf);
   E_FREE(n);
}

static void
_net_config_menu_new(Net *n) 
{
   E_Menu *mn;
   
   mn = e_menu_new();
   n->config_menu = mn;
}

static Net_Face *
_net_face_init(Net *n, E_Container *con) 
{
   Net_Face *nf;
   Evas_Object *o;
   char buf[4096];

   nf = E_NEW(Net_Face, 1);
   if (!nf)
     return NULL;   
   nf->net = n;
   n->faces = evas_list_append(n->faces, nf);

   nf->con = con;
   e_object_ref(E_OBJECT(con));
   nf->evas = con->bg_evas;
   
   evas_event_freeze(nf->evas);
   
   o = edje_object_add(nf->evas);
   nf->net_obj = o;
   
   if (!e_theme_edje_object_set(o, "base/theme/modules/net", "modules/net/main")) 
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/net.edj");	
	edje_object_file_set(o, buf, "modules/net/main");
     }   
   evas_object_show(o);

   o = edje_object_add(nf->evas);
   nf->chart_obj = o;
   evas_object_layer_set(o, 1);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_show(o);

   o = edje_object_add(nf->evas);
   nf->rtxt_obj = o;
   if (!e_theme_edje_object_set(o, "base/theme/modules/net", "modules/net/rx_text")) 
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/net.edj");	
	edje_object_file_set(o, buf, "modules/net/rx_text");
     }   
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_show(o);
   
   o = edje_object_add(nf->evas);
   nf->ttxt_obj = o;
   if (!e_theme_edje_object_set(o, "base/theme/modules/net", "modules/net/tx_text")) 
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/net.edj");	
	edje_object_file_set(o, buf, "modules/net/tx_text");
     }   
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_show(o);
   
   o = evas_object_rectangle_add(nf->evas);
   nf->event_obj = o;
   evas_object_layer_set(o, 3);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _net_face_cb_mouse_down, nf);
   evas_object_show(o);
   
   nf->gmc = e_gadman_client_new(nf->con->gadman);
   e_gadman_client_domain_set(nf->gmc, "module.net", net_count++);
   e_gadman_client_policy_set(nf->gmc,
			      E_GADMAN_POLICY_ANYWHERE |
			      E_GADMAN_POLICY_HMOVE |
			      E_GADMAN_POLICY_HSIZE |
			      E_GADMAN_POLICY_VMOVE |
			      E_GADMAN_POLICY_VSIZE);
   //e_gadman_client_min_size_set(nf->gmc, 40, 40);
   //e_gadman_client_max_size_set(nf->gmc, 128, 128);
   e_gadman_client_auto_size_set(nf->gmc, 40, 40);
   e_gadman_client_align_set(nf->gmc, 1.0, 1.0);
   e_gadman_client_resize(nf->gmc, 40, 40);
   e_gadman_client_change_func_set(nf->gmc, _net_face_cb_gmc_change, nf);
   e_gadman_client_load(nf->gmc);
   evas_event_thaw(nf->evas);
   
   return nf;
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
   e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");   
   e_menu_item_callback_set(mi, _net_face_cb_menu_configure, nf);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/gadgets");   
   e_menu_item_callback_set(mi, _net_face_cb_menu_edit, nf);
}

static void
_net_face_enable(Net_Face *nf) 
{
   nf->conf->enabled = 1;
   e_config_save_queue();
   evas_object_show(nf->net_obj);
   evas_object_show(nf->event_obj);
   evas_object_show(nf->chart_obj);
   evas_object_show(nf->rtxt_obj);
   evas_object_show(nf->ttxt_obj);   
}

static void
_net_face_disable(Net_Face *nf) 
{
   nf->conf->enabled = 0;
   e_config_save_queue();
   evas_object_hide(nf->event_obj);
   evas_object_hide(nf->net_obj);
   evas_object_hide(nf->chart_obj);
   evas_object_hide(nf->rtxt_obj);
   evas_object_hide(nf->ttxt_obj);
}

static void 
_net_face_free(Net_Face *nf) 
{
   e_object_unref(E_OBJECT(nf->con));
   e_object_del(E_OBJECT(nf->menu));
   
   if (nf->monitor)
     ecore_timer_del(nf->monitor);
   if (nf->menu)
     e_object_del(E_OBJECT(nf->menu));
   if (nf->event_obj)
     evas_object_del(nf->event_obj);
   if (nf->net_obj)
     evas_object_del(nf->net_obj);
   if (nf->chart_obj)
     evas_object_del(nf->chart_obj);
   if (nf->rtxt_obj)
     evas_object_del(nf->rtxt_obj);
   if (nf->ttxt_obj)
     evas_object_del(nf->ttxt_obj);
   if (nf->old_rx)
     _net_face_graph_clear(nf);
   
   if (nf->gmc) 
     {
	e_gadman_client_save(nf->gmc);
	e_object_del(E_OBJECT(nf->gmc));
     }
   
   nf->net->faces = evas_list_remove(nf->net->faces, nf);
   
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
	evas_object_move(nf->event_obj, x, y);
	evas_object_move(nf->chart_obj, x, y);
	evas_object_move(nf->rtxt_obj, x, y);
	evas_object_move(nf->ttxt_obj, x, y);	
	evas_object_resize(nf->net_obj, w, h);
	evas_object_resize(nf->event_obj, w, h);
	evas_object_resize(nf->chart_obj, w, h);
	evas_object_resize(nf->rtxt_obj, w, h);
	evas_object_resize(nf->ttxt_obj, w, h);	
	_net_face_graph_clear(nf);
	break;
      case E_GADMAN_CHANGE_RAISE:
	evas_object_raise(nf->net_obj);
	evas_object_raise(nf->event_obj);
	evas_object_raise(nf->chart_obj);
	evas_object_raise(nf->rtxt_obj);
	evas_object_raise(nf->ttxt_obj);	
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
   if (!nf)
     return;
   
   _configure_net_module(nf);
}

static int 
_net_face_update_values(void *data) 
{
   Net_Face *nf;
   FILE *stat;
   char dev[64];
   char buf[256];
   static unsigned long old_in = 0;
   static unsigned long old_out = 0;
   unsigned long in = 0;
   unsigned long out = 0;
   unsigned long dummy = 0;
   int found;
   long max_in = 171008;
   long max_out = 28672;
   long bytes_in;
   long bytes_out;
   int in_use = 0;
   int out_use = 0;
   
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
	if (!strcmp(dev, strdup(nf->conf->device))) 
	  {
	     found = 1;
	     break;
	  }
     }
   fclose(stat);
   
   if (!found) 
     return 1;
   
   if (old_in && old_out) 
     {
	bytes_in = in - old_in;
	bytes_out = out - old_out;
	
	in_use = (int)((bytes_in * 100L) / max_in);
	out_use = (int)((bytes_out * 100L) / max_out);
     }
   else 
     {
	in_use = 0;
	out_use = 0;	
     }
   
   old_in = in;
   old_out = out;
   
   /* Update the modules text */
   char in_str[100];
   char out_str[100];
   
   if (!nf->conf->show_text) 
     {
	edje_object_part_text_set(nf->ttxt_obj, "tx-text", "");
	edje_object_part_text_set(nf->rtxt_obj, "rx-text", "");		
     }
    else 
     {
	if (bytes_in > 1048576) 
	  {
	     bytes_in = bytes_in / 1048576;
	     snprintf(in_str, sizeof(in_str), "Rx: %d Mb", bytes_in);
	  }
	else if (bytes_in > 1024 && bytes_in < 1048576) 
	  {
	     bytes_in = bytes_in / 1024; 
	     snprintf(in_str, sizeof(in_str), "Rx: %d Kb", bytes_in);
	  }
	else
	  snprintf(in_str, sizeof(in_str), "Rx: %d B", bytes_in);	     
	
	if (bytes_out > 1048576) 
	  {
	     bytes_out = bytes_out / 1048576;
	     snprintf(out_str, sizeof(out_str), "Tx: %d Mb", bytes_out);
	  }
	else if (bytes_out > 1024 && bytes_out < 1048576) 
	  {
	     bytes_out = bytes_out / 1024; 
	     snprintf(out_str, sizeof(out_str), "Tx: %d Kb", bytes_out);
	  }
	else
	  snprintf(out_str, sizeof(out_str), "Tx: %d B", bytes_out);	     

	edje_object_part_text_set(nf->ttxt_obj, "tx-text", out_str);
	edje_object_part_text_set(nf->rtxt_obj, "rx-text", in_str);
     }
   
   if (nf->conf->show_graph) 
     _net_face_graph_values(nf, out_use, in_use);
   else 
     _net_face_graph_clear(nf);
   
   return 1;
}

static void 
_net_face_graph_values(Net_Face *nf, int tx_val, int rx_val) 
{
   int x, y, w, h;
   Evas_Object *o;
   Evas_Object *last = NULL;
   Evas_List *l;
   int i, j = 0;

   evas_object_geometry_get(nf->chart_obj, &x, &y, &w, &h);

   if (rx_val > 100)
     rx_val = 100;
   
   rx_val = (int)(((double)rx_val) * (((double)h) / ((double)100)));      
   
   o = evas_object_line_add(nf->evas);
   edje_object_part_swallow(nf->chart_obj, "lines", o);
   evas_object_layer_set(o, 1);
   if (rx_val == 0)
     evas_object_hide(o);
   else 
     {
	evas_object_line_xy_set(o, (x + w), (y + h), (x + w), ((y + h) - rx_val));
	evas_object_color_set(o, 255, 0, 0, 100);
	evas_object_pass_events_set(o, 1);
	evas_object_show(o);
     }
   
   nf->old_rx = evas_list_prepend(nf->old_rx, o);
   l = nf->old_rx;
   for (i = (x + w); l && (j -2) < w; l = l->next, j++) 
     {
	Evas_Coord oy;
	Evas_Object *lo;
	
	lo = (Evas_Object *)evas_list_data(l);
	evas_object_geometry_get(lo, NULL, &oy, NULL, NULL);
	evas_object_move(lo, i--, oy);
	last = lo;
     }
   
   if ((j - 2) >= w) 
     {
	nf->old_rx = evas_list_remove(nf->old_rx, last);
	evas_object_del(last);
     }      
}

static void 
_net_face_graph_clear(Net_Face *nf) 
{
   Evas_List *l;
   
   for (l = nf->old_rx; l; l = l->next) 
     {
	Evas_Object *o;
	o = evas_list_data(l);
	evas_object_del(o);
     }
   evas_list_free(nf->old_rx);
   nf->old_rx = NULL;

   for (l = nf->old_tx; l; l = l->next) 
     {
	Evas_Object *o;
	o = evas_list_data(l);
	evas_object_del(o);
     }
   evas_list_free(nf->old_tx);
   nf->old_tx = NULL;
}

