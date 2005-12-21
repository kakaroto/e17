#include "e.h"
#include "config.h"
#include "e_mod_main.h"

/* Protos */
static Rss      *_rss_init(E_Module *m);
static void     _rss_free(Rss *r);
static E_Menu   *_rss_config_menu_new(Rss *r);
static void     _rss_menu_cb_configure(void *data, E_Menu *mn, E_Menu_Item *mi);
static int      _rss_face_init(Rss_Face *rf);
static void     _rss_face_free(Rss_Face *rf);
static void     _rss_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);

static int rss_count;

E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
     "Rss"
};

void *
e_modapi_init(E_Module *m) 
{
   Rss *r;
   
   r = _rss_init(m);
   m->config_menu = _rss_config_menu_new(r);
   return r;
}

int
e_modapi_shutdown(E_Module *m) 
{
   Rss *r;
   
   r = m->data;
   if (r) 
     {
	if (m->config_menu) 
	  {   
	     e_menu_deactivate(m->config_menu);
	     e_object_del(E_OBJECT(m->config_menu));
	     m->config_menu = NULL;
	  }
	_rss_free(r);
     }
   return 1;
}

int
e_modapi_save(E_Module *m) 
{
   Rss *r;
   
   r = m->data;
   if (r) e_config_domain_save("module.rss", r->conf_edd, r->conf);
   return 1;
}

int
e_modapi_info(E_Module *m) 
{
   m->icon_file = strdup(PACKAGE_DATA_DIR"/module_icon.png");
   return 1;
}

int
e_modapi_about(E_Module *m) 
{
   e_module_dialog_show(_("Enlightenment Rss Module"), 
			_("A module to display RSS feeds"));
   return 1;
}

int
e_modapi_config(E_Module *m) 
{
   return 1;
}

static Rss
*_rss_init(E_Module *m) 
{
   Rss *r;
   Evas_List *managers, *cons, *l;
   
   rss_count = 0;
   
   r = E_NEW(Rss, 1);
   if (!r) return NULL;
   
   /* Handle Config */
   r->conf_edd = E_CONFIG_DD_NEW("Rss_Config", Config);
#undef T
#undef D
#define T Config
#define D r->conf_edd
   E_CONFIG_VAL(D, T, update_rate, INT);
   
   r->conf = e_config_domain_load("module.rss", r->conf_edd);
   if (!r->conf) 
     {
	r->conf = E_NEW(Config, 1);
	r->conf->update_rate = 15;
	r->conf->browser = (char *)evas_stringshare_add("");
	r->conf->proxy = (char *)evas_stringshare_add("");
	r->conf->proxy_port = 3128;
     }
   E_CONFIG_LIMIT(r->conf->update_rate, 1, 60);
   
   managers = e_manager_list();
   for (l = managers; l; l = l->next) 
     {
	E_Manager *man;
	man = l->data;
	for (cons = man->containers; cons; cons = cons->next) 
	  {
	     E_Container *con;
	     Rss_Face *rf;
	     
	     con = cons->data;
	     rf = E_NEW(Rss_Face, 1);
	     if (rf) 
	       {
		  rf->rss = r;
		  rf->con = con;
		  rf->evas = con->bg_evas;
		  r->face = rf;
		  if (!_rss_face_init(rf)) return NULL;
	       }	     
	  }
     }
   return r;   
}

static void
_rss_free(Rss *r) 
{
   if (r->face) _rss_face_free(r->face);
   if (r->config_menu) e_object_del(E_OBJECT(r->config_menu));
   if (r->conf->browser) evas_stringshare_del(r->conf->browser);   
   if (r->conf->proxy) evas_stringshare_del(r->conf->proxy);
   free(r->conf);
   E_CONFIG_DD_FREE(r->conf_edd);
   free(r);
}

static E_Menu
*_rss_config_menu_new(Rss *r) 
{
   E_Menu *mn;
   E_Menu_Item *mi;
   
   mn = e_menu_new();
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_menu_item_callback_set(mi, _rss_menu_cb_configure, r);
   r->config_menu = mn;
   return mn;
}

static void
_rss_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   Rss *r;
   
   r = data;
   if (!r) return;
   printf("Call Rss Configure\n");
   return;
}

static int
_rss_face_init(Rss_Face *rf) 
{
   Evas_Object *o;
   E_Gadman_Policy policy;
   char buffer[1024];
   
   if (!rf) return 0;
   snprintf(buffer, sizeof(buffer), PACKAGE_DATA_DIR"/rss.edj");
   
   e_object_ref(E_OBJECT(rf->con));
   
   evas_event_freeze(rf->evas);
   
   o = edje_object_add(rf->evas);
   rf->box_obj = o;
   edje_object_file_set(o, strdup(buffer), "modules/rss/main");
   evas_object_show(o);

   o = e_box_add(rf->evas);
   rf->item_obj = o;
   e_box_freeze(o);
   edje_object_part_swallow(rf->box_obj, "items", o);
   evas_object_show(o);
   e_box_thaw(o);
   
   policy = E_GADMAN_POLICY_ANYWHERE | E_GADMAN_POLICY_HMOVE | E_GADMAN_POLICY_VMOVE | E_GADMAN_POLICY_HSIZE | E_GADMAN_POLICY_VSIZE;
   rf->gmc = e_gadman_client_new(rf->con->gadman);
   e_gadman_client_domain_set(rf->gmc, "module.rss", rss_count++);
   e_gadman_client_policy_set(rf->gmc, policy);
   e_gadman_client_min_size_set(rf->gmc, 8, 8);
   e_gadman_client_max_size_set(rf->gmc, 3200, 3200);
   e_gadman_client_align_set(rf->gmc, 0.5, 0.5);
   e_gadman_client_resize(rf->gmc, 200, 100);
   e_gadman_client_change_func_set(rf->gmc, _rss_face_cb_gmc_change, rf);
   e_gadman_client_load(rf->gmc);
   
   evas_event_thaw(rf->evas);
   return 1;
}

static void
_rss_face_free(Rss_Face *rf) 
{
   if (rf->con) e_object_unref(E_OBJECT(rf->con));
   if (rf->box_obj) evas_object_del(rf->box_obj);
   if (rf->item_obj) evas_object_del(rf->item_obj);
   if (rf->gmc) 
     { 
	e_gadman_client_save(rf->gmc);
	e_object_del(E_OBJECT(rf->gmc));
     }
   free(rf);
   rss_count--;
}

static void
_rss_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change) 
{
   Rss_Face *rf;
   Evas_Coord x, y, w, h;
   
   rf = data;
   switch (change) 
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	e_gadman_client_geometry_get(rf->gmc, &x, &y, &w, &h);
	edje_extern_object_min_size_set(rf->box_obj, 0, 0);
	
	evas_object_move(rf->box_obj, x, y);
	evas_object_move(rf->item_obj, x, y);

	evas_object_resize(rf->box_obj, w, h);
	evas_object_resize(rf->item_obj, w, h);

	break;
     }
}

