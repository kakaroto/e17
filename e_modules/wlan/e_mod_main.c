#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

static Wlan      *_wlan_init(E_Module *m);
static void      _wlan_shutdown(Wlan *n);
static void      _wlan_config_menu_new(Wlan *n);

static Wlan_Face *_wlan_face_init(Wlan *n, E_Container *con);
static void      _wlan_face_menu_new(Wlan_Face *nf);
static void      _wlan_face_enable(Wlan_Face *nf);
static void      _wlan_face_disable(Wlan_Face *nf);
static void      _wlan_face_free(Wlan_Face *nf);
static void      _wlan_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static void      _wlan_face_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void      _wlan_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi);
static void      _wlan_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);
static int       _wlan_face_update_values(void *data);
static void      _wlan_face_graph_values(Wlan_Face *wf, int val);
static void      _wlan_face_graph_clear(Wlan_Face *wf);

static int wlan_count;
static E_Config_DD *conf_edd;
static E_Config_DD *conf_face_edd;

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
   e_config_domain_save("module.wlan", conf_edd, n->conf);
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
   e_module_dialog_show(_("Enlightenment Wlan Monitor Module"),
			_("This module is used to monitor a wlan device."));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m)
{
   Wlan *n;
   Evas_List *l;
   E_Container *con;

   n = m->data;
   if (!n)
     return 0;
   if (!n->faces)
     return 0;

   for (l = n->faces; l; l = l->next)
     {
	Wlan_Face *nf;

	nf = l->data;
	if (!nf)
	  continue;

	con = e_container_current_get(e_manager_current_get());
	if (nf->con == con)
	  {
	     _configure_wlan_module(nf);
	     break;
	  }
     }
   return 1;
}

static Wlan *
_wlan_init(E_Module *m)
{
   Wlan *n;
   E_Menu_Item *mi;
   Evas_List *mans, *l, *l2, *fl;

   n = E_NEW(Wlan, 1);
   if (!n)
     return NULL;

   conf_face_edd = E_CONFIG_DD_NEW("Wlan_Config_Face", Config_Face);
#undef T
#undef D
#define T Config_Face
#define D conf_face_edd
   E_CONFIG_VAL(D, T, enabled, UCHAR);
   E_CONFIG_VAL(D, T, device, STR);
   E_CONFIG_VAL(D, T, check_interval, INT);
   E_CONFIG_VAL(D, T, show_text, INT);
   E_CONFIG_VAL(D, T, show_graph, INT);

   conf_edd = E_CONFIG_DD_NEW("Wlan_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, faces, conf_face_edd);

   n->conf = e_config_domain_load("module.wlan", conf_edd);
   if (!n->conf)
     n->conf = E_NEW(Config, 1);

   _wlan_config_menu_new(n);

   mans = e_manager_list();
   fl = n->conf->faces;
   for (l = mans; l; l = l->next)
     {
	E_Manager *man;

	man = l->data;
	for (l2 = man->containers; l2; l2 = l2->next)
	  {
	     E_Container *con;
	     Wlan_Face *nf;

	     con = l2->data;
	     nf = _wlan_face_init(n, con);
	     if (nf)
	       {
		  if (!fl)
		    {
		       nf->conf = E_NEW(Config_Face, 1);
		       nf->conf->enabled = 1;
		       nf->conf->device = (char *)evas_stringshare_add("wlan0");
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

		  nf->monitor = ecore_timer_add((double)nf->conf->check_interval, _wlan_face_update_values, nf);

		  _wlan_face_menu_new(nf);

		  mi = e_menu_item_new(n->config_menu);
		  e_menu_item_label_set(mi, _("Configuration"));
		  e_menu_item_callback_set(mi, _wlan_face_cb_menu_configure, nf);

		  mi = e_menu_item_new(n->config_menu);
		  e_menu_item_label_set(mi, con->name);
		  e_menu_item_submenu_set(mi, nf->menu);

		  if (!nf->conf->enabled)
		    _wlan_face_disable(nf);
	       }
	  }
     }
   return n;
}

static void
_wlan_shutdown(Wlan *n)
{
   E_CONFIG_DD_FREE(conf_edd);
   E_CONFIG_DD_FREE(conf_face_edd);

   while (n->faces)
     _wlan_face_free(n->faces->data);

   e_object_del(E_OBJECT(n->config_menu));
   evas_list_free(n->conf->faces);

   E_FREE(n->conf);
   E_FREE(n);
}

static void
_wlan_config_menu_new(Wlan *n)
{
   E_Menu *mn;

   mn = e_menu_new();
   n->config_menu = mn;
}

static Wlan_Face *
_wlan_face_init(Wlan *n, E_Container *con)
{
   Wlan_Face *nf;
   Evas_Object *o;
   char buf[4096];

   nf = E_NEW(Wlan_Face, 1);
   if (!nf)
     return NULL;
   nf->wlan = n;
   n->faces = evas_list_append(n->faces, nf);

   nf->con = con;
   e_object_ref(E_OBJECT(con));
   nf->evas = con->bg_evas;

   evas_event_freeze(nf->evas);

   o = edje_object_add(nf->evas);
   nf->wlan_obj = o;

   if (!e_theme_edje_object_set(o, "base/theme/modules/wlan", "modules/wlan/main"))
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/wlan.edj");
	edje_object_file_set(o, buf, "modules/wlan/main");
     }
   evas_object_layer_set(o, 1);
   evas_object_show(o);

   o = edje_object_add(nf->evas);
   nf->chart_obj = o;
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 1);
   evas_object_pass_events_set(o, 1);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_show(o);

   o = edje_object_add(nf->evas);
   nf->txt_obj = o;
   if (!e_theme_edje_object_set(o, "base/theme/modules/wlan", "modules/wlan/text"))
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/wlan.edj");
	edje_object_file_set(o, buf, "modules/wlan/text");
     }   
   evas_object_layer_set(o, 3);
   evas_object_repeat_events_set(o, 1);
   evas_object_pass_events_set(o, 1);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_show(o);
   
   o = evas_object_rectangle_add(nf->evas);
   nf->event_obj = o;
   evas_object_layer_set(o, 4);
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
   e_gadman_client_auto_size_set(nf->gmc, 40, 40);
   e_gadman_client_align_set(nf->gmc, 1.0, 1.0);
   e_gadman_client_resize(nf->gmc, 40, 40);
   e_gadman_client_change_func_set(nf->gmc, _wlan_face_cb_gmc_change, nf);
   e_gadman_client_load(nf->gmc);
   evas_event_thaw(nf->evas);

   return nf;
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
   e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
   e_menu_item_callback_set(mi, _wlan_face_cb_menu_configure, nf);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/gadgets");
   e_menu_item_callback_set(mi, _wlan_face_cb_menu_edit, nf);
}

static void
_wlan_face_enable(Wlan_Face *nf)
{
   nf->conf->enabled = 1;
   e_config_save_queue();
   evas_object_show(nf->wlan_obj);
   evas_object_show(nf->chart_obj);
   evas_object_show(nf->event_obj);
   evas_object_show(nf->txt_obj);   
}

static void
_wlan_face_disable(Wlan_Face *nf)
{
   nf->conf->enabled = 0;
   e_config_save_queue();
   evas_object_hide(nf->event_obj);
   evas_object_hide(nf->chart_obj);
   evas_object_hide(nf->wlan_obj);
   evas_object_hide(nf->txt_obj);   
}

static void
_wlan_face_free(Wlan_Face *nf)
{
   e_object_unref(E_OBJECT(nf->con));

   if (nf->monitor)
     ecore_timer_del(nf->monitor);
   if (nf->menu)
     e_object_del(E_OBJECT(nf->menu));
   if (nf->event_obj)
     evas_object_del(nf->event_obj);
   if (nf->wlan_obj)
     evas_object_del(nf->wlan_obj);
   if (nf->chart_obj)
     evas_object_del(nf->chart_obj);
   if (nf->txt_obj)
     evas_object_del(nf->txt_obj);
   if (nf->old_values)
     _wlan_face_graph_clear(nf);
   
   if (nf->gmc)
     {
	e_gadman_client_save(nf->gmc);
	e_object_del(E_OBJECT(nf->gmc));
     }

   nf->wlan->faces = evas_list_remove(nf->wlan->faces, nf);

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
	evas_object_move(nf->event_obj, x, y);
	evas_object_move(nf->chart_obj, x, y);
	evas_object_move(nf->txt_obj, x, y);	
	evas_object_resize(nf->wlan_obj, w, h);
	evas_object_resize(nf->event_obj, w, h);
	evas_object_resize(nf->chart_obj, w, h);
	evas_object_resize(nf->txt_obj, w, h);
	_wlan_face_graph_clear(nf);
	break;
      case E_GADMAN_CHANGE_RAISE:
	evas_object_raise(nf->wlan_obj);
	evas_object_raise(nf->event_obj);
	evas_object_raise(nf->chart_obj);
	evas_object_raise(nf->txt_obj);	
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
   if (!nf)
     return;

   _configure_wlan_module(nf);
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
   char in_str[100];

   nf = data;

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
        if (!strcmp(iface, strdup(nf->conf->device)))
          {
             found_dev = 1;
             break;
          }
     }
   fclose(stat);

   if (!found_dev)
     return 1;

   /* Update the modules text */
   if (nf->conf->show_text)
     {
	snprintf(in_str, sizeof(in_str), "LNK: %d%%", (100 - (wlan_level - wlan_noise)));
	edje_object_part_text_set(nf->wlan_obj, "link-text", in_str);
     }   
   else
     edje_object_part_text_set(nf->wlan_obj, "link-text", "");
   
   if (nf->conf->show_graph) 
     _wlan_face_graph_values(nf, (100 - (wlan_level - wlan_noise)));
   else
     _wlan_face_graph_clear(nf);
     
   return 1;
}

static void
_wlan_face_graph_values(Wlan_Face *wf, int val)
{
   int x, y, w, h;
   Evas_Object *o;
   Evas_Object *last = NULL;
   Evas_List *l;
   int i, j = 0;
   int v;
   
   evas_object_geometry_get(wf->chart_obj, &x, &y, &w, &h);

   v = (int)((double)val * ((double)h / (double)100));      
   o = evas_object_line_add(wf->evas);
   edje_object_part_swallow(wf->chart_obj, "lines", o);
   evas_object_layer_set(o, 1);
   if (val == 0)
     evas_object_hide(o);
   else 
     {
	evas_object_line_xy_set(o, (x + w), (y + h), (x + w), ((y + h) - v));
	evas_object_color_set(o, 255, 0, 0, 100);
	evas_object_pass_events_set(o, 1);
	evas_object_show(o);
     }
   
   wf->old_values = evas_list_prepend(wf->old_values, o);
   l = wf->old_values;
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
	wf->old_values = evas_list_remove(wf->old_values, last);
	evas_object_del(last);
     }
}

static void
_wlan_face_graph_clear(Wlan_Face *wf)
{
   Evas_List *l;

   for (l = wf->old_values; l; l = l->next) 
     {
	Evas_Object *o;
	o = evas_list_data(l);
	evas_object_del(o);
     }
   evas_list_free(wf->old_values);
   wf->old_values = NULL;
}

