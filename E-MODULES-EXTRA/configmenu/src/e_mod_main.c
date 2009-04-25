#include <e.h>
#include "e_mod_main.h"

/* Local Function Prototypes */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);

static void _config_conf_new(void);
static void _config_conf_free(void);
static int _config_conf_timer(void *data);
static Config_Item *_config_conf_item_get(const char *id);
static void _config_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _config_cb_menu_post(void *data, E_Menu *menu);
static void _config_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);

/* Local Structures */
typedef struct _Instance Instance;
struct _Instance 
{
   /* pointer to this gadget's container */
   E_Gadcon_Client *gcc;

   /* evas_object used to display */
   Evas_Object *o_config;

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
Config *config_conf = NULL;

E_Int_Menu_Augmentation *maug = NULL;

/* menu item callback(s) */
static void 
_e_mod_run_cb(void *data, E_Menu *m, E_Menu_Item *mi)
{
  Eina_List *l;
  char buf[1024];
  
  for (l = e_configure_registry; l; l = l->next)
    {
      Eina_List *ll;
      E_Configure_Cat *ecat;
	
      ecat = l->data;
      if ((ecat->pri >= 0) && (ecat->items))
	{
	  for (ll = ecat->items; ll; ll = ll->next)
	    {
	      E_Configure_It *eci;
	      char buf[1024];
		  
	      eci = ll->data;
	      if (eci->pri >= 0 && eci == data)
		{
		  snprintf(buf, sizeof(buf), "%s/%s", ecat->cat, eci->item);
		  e_configure_registry_call(buf, m->zone->container, NULL);
		}
	    }
	}
    }
}

static void _config_pre_activate_cb(void *data, E_Menu *m)
{
  E_Configure_Cat *ecat = data;
  Eina_List *l;
  E_Menu_Item *mi;
  /*XXX is this the right way to not initiate the menu a second time ?*/
  if (e_menu_item_nth(m, 0)) return;
  
  for (l = ecat->items; l; l = l->next)
    {
      E_Configure_It *eci;
      char buf[1024];
		  
      eci = l->data;
      if (eci->pri >= 0)
	{
	  mi = e_menu_item_new(m);
	  e_menu_item_label_set(mi, eci->label);
	  if(eci->icon)
	  e_util_menu_item_edje_icon_set(mi, eci->icon);
	  
	  e_menu_item_callback_set(mi, _e_mod_run_cb, eci);
	}
    }
}


/* menu item add hook */
void
_e_mod_config_menu_add(void *data, E_Menu *m)
{
  E_Menu_Item *mi;
  E_Menu *sub;
  
  Eina_List *l;
  
  for (l = e_configure_registry; l; l = l->next)
    {
      Eina_List *ll;
      E_Configure_Cat *ecat;
	
      ecat = l->data;
      if ((ecat->pri >= 0) && (ecat->items))
	{
	  mi = e_menu_item_new(m);
	  e_menu_item_label_set(mi, ecat->label);
	  if(ecat->icon)
	    e_util_menu_item_edje_icon_set(mi, ecat->icon);

	  sub = e_menu_new();
	  e_menu_item_submenu_set(mi, sub);
	  e_menu_pre_activate_callback_set(sub, _config_pre_activate_cb, ecat);
	}
    }
}

static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "config", 
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, 
          _gc_id_new, NULL},
   E_GADCON_CLIENT_STYLE_PLAIN
};

EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "Config"};

/*
 * Module Functions
 */
EAPI void *
e_modapi_init(E_Module *m) 
{
   char buf[4096];

   /* Location of message catalogs for localization */
   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   /* Location of theme to load for this module */
   snprintf(buf, sizeof(buf), "%s/e-module-config.edj", m->dir);

   /* Display this Modules config info in the main Config Panel */

   /* starts with a category */
   e_configure_registry_category_add("advanced", 80, "Advanced", 
                                     NULL, "enlightenment/advanced");
   /* add right-side item */
   e_configure_registry_item_add("advanced/config", 110, "Config", 
                                 NULL, buf, e_int_config_config_module);

   /* Define EET Data Storage */
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
   config_conf = e_config_domain_load("module.config", conf_edd);
   if (config_conf) 
     {
        /* Check config version */
        if ((config_conf->version >> 16) < MOD_CONFIG_FILE_EPOCH) 
          {
             /* config too old */
             _config_conf_free();
	     ecore_timer_add(1.0, _config_conf_timer,
			     D_("Config Menu Module Configuration data needed "
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
        else if (config_conf->version > MOD_CONFIG_FILE_VERSION) 
          {
             /* config too new...wtf ? */
             _config_conf_free();
	     ecore_timer_add(1.0, _config_conf_timer, 
			     D_("Your Config Menu Module configuration is NEWER "
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
   if (!config_conf) _config_conf_new();

   /* create a link from the modules config to the module
    * this is not written */
   config_conf->module = m;

   /* Tell any gadget containers (shelves, etc) that we provide a module
    * for the user to enjoy */
   e_gadcon_provider_register(&_gc_class);

   if(config_conf->switch1)
     maug = e_int_menus_menu_augmentation_add("config/0", _e_mod_config_menu_add, NULL, NULL, NULL);

   
   /* Give E the module */
   return m;
}

/*
 * Function to unload the module
 */
EAPI int 
e_modapi_shutdown(E_Module *m) 
{

  /* remove module-supplied menu additions */
  if (maug)
    {
      e_int_menus_menu_augmentation_del("config/0", maug);
      maug = NULL;
    }
  
   /* Unregister the config dialog from the main panel */
   e_configure_registry_item_del("advanced/config");

   /* Remove the config panel category if we can. E will tell us.
    category stays if other items using it */
   e_configure_registry_category_del("advanced");

   /* Kill the config dialog */
   if (config_conf->cfd) e_object_del(E_OBJECT(config_conf->cfd));
   config_conf->cfd = NULL;

   /* Tell E the module is now unloaded. Gets removed from shelves, etc. */
   config_conf->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);

   /* Cleanup our item list */
   while (config_conf->conf_items) 
     {
        Config_Item *ci = NULL;

        /* Grab an item from the list */
        ci = config_conf->conf_items->data;

        /* remove it */
        config_conf->conf_items = 
          eina_list_remove_list(config_conf->conf_items, 
                                config_conf->conf_items);

        /* cleanup stringshares */
        if (ci->id) eina_stringshare_del(ci->id);

        /* keep the planet green */
        E_FREE(ci);
     }

   /* Cleanup the main config structure */
   E_FREE(config_conf);

   /* Clean EET */
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

/*
 * Function to Save the modules config
 */ 
EAPI int 
e_modapi_save(E_Module *m) 
{
   e_config_domain_save("module.config", conf_edd, config_conf);
   return 1;
}

/* Local Functions */

/* Called when Gadget_Container says go */
static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   Instance *inst = NULL;
   char buf[4096];

   /* theme file */
   snprintf(buf, sizeof(buf), "%s/e-module-config.edj", 
            config_conf->module->dir);

   /* New visual instance, any config ? */
   inst = E_NEW(Instance, 1);
   inst->conf_item = _config_conf_item_get(id);

   /* create on-screen object */
   inst->o_config = edje_object_add(gc->evas);
   /* we have a theme ? */
   if (!e_theme_edje_object_set(inst->o_config, "base/theme/modules/config", 
                                "modules/config/main"))
     edje_object_file_set(inst->o_config, buf, "modules/config/main");

   /* Start loading our module on screen via container */
   inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->o_config);
   inst->gcc->data = inst;

   /* hook a mouse down. we want/have a popup menu, right ? */
   evas_object_event_callback_add(inst->o_config, EVAS_CALLBACK_MOUSE_DOWN, 
                                  _config_cb_mouse_down, inst);

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
   if (inst->o_config) 
     {
        /* remove mouse down callback hook */
        evas_object_event_callback_del(inst->o_config, EVAS_CALLBACK_MOUSE_DOWN, 
                                       _config_cb_mouse_down);
        evas_object_del(inst->o_config);
     }
   E_FREE(inst);
}

/* For for when container says we are changing position */
static void 
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient) 
{
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

/* Gadget/Module label */
static char *
_gc_label(E_Gadcon_Client_Class *client_class) 
{
   return D_("Config Menu");
}

/* so E can keep a unique instance per-container */
static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class) 
{
   Config_Item *ci = NULL;

   ci = _config_conf_item_get(NULL);
   return ci->id;
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas) 
{
   Evas_Object *o = NULL;
   char buf[4096];

   /* theme */
   snprintf(buf, sizeof(buf), "%s/e-module-config.edj", config_conf->module->dir);

   /* create icon object */
   o = edje_object_add(evas);

   /* load icon from theme */
   edje_object_file_set(o, buf, "icon");

   return o;
}

/* new module needs a new config :), or config too old and we need one anyway */
static void 
_config_conf_new(void) 
{
   Config_Item *ci = NULL;
   char buf[128];

   config_conf = E_NEW(Config, 1);
   config_conf->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) if ((config_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

   /* setup defaults */
   IFMODCFG(0x008d);
   config_conf->switch1 = 1;
   _config_conf_item_get(NULL);
   IFMODCFGEND;

   /* update the version */
   config_conf->version = MOD_CONFIG_FILE_VERSION;

   /* setup limits on the config properties here (if needed) */

   /* save the config to disk */
   e_config_save_queue();
}

static void 
_config_conf_free(void) 
{
   /* cleanup any stringshares here */
   while (config_conf->conf_items) 
     {
        Config_Item *ci = NULL;

        ci = config_conf->conf_items->data;
        config_conf->conf_items = 
          eina_list_remove_list(config_conf->conf_items, 
                                config_conf->conf_items);
        /* EPA */
        if (ci->id) eina_stringshare_del(ci->id);
        E_FREE(ci);
     }

   E_FREE(config_conf);
}

/* timer for the config oops dialog */
static int 
_config_conf_timer(void *data) 
{
   e_util_dialog_show( D_("Config Menu Configuration Updated"), data);
   return 0;
}

/* function to search for any Config_Item struct for this Item
 * create if needed */
static Config_Item *
_config_conf_item_get(const char *id) 
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
        for (l = config_conf->conf_items; l; l = l->next) 
          {
             if (!(ci = l->data)) continue;
             if ((ci->id) && (!strcmp(ci->id, id))) return ci;
          }
     }
   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->switch2 = 0;
   config_conf->conf_items = eina_list_append(config_conf->conf_items, ci);
   return ci;
}

/* Pants On */
static void 
_config_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event) 
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
       Evas_Coord x, y, w, h;
       int cx, cy;

       evas_object_geometry_get(inst->o_config, &x, &y, &w, &h); 
       e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon,
					 &cx, &cy, NULL, NULL);
       x += cx;
       y += cy;
       if (!inst->menu)
	 { 
	   inst->menu = e_menu_new();
	   _e_mod_config_menu_add(NULL, inst->menu);
	 }
	 
       if (inst->menu)
	 {
	   int dir;
	     
	   e_menu_post_deactivate_callback_set(inst->menu,
					       _config_cb_menu_post,
					       inst);
	   dir = E_MENU_POP_DIRECTION_AUTO;
	   switch (inst->gcc->gadcon->orient)
	     {
	     case E_GADCON_ORIENT_TOP:
	       dir = E_MENU_POP_DIRECTION_DOWN;
	       break;
	     case E_GADCON_ORIENT_BOTTOM:
	       dir = E_MENU_POP_DIRECTION_UP;
	       break;
	     case E_GADCON_ORIENT_LEFT:
	       dir = E_MENU_POP_DIRECTION_RIGHT;
	       break;
	     case E_GADCON_ORIENT_RIGHT:
	       dir = E_MENU_POP_DIRECTION_LEFT;
	       break;
	     case E_GADCON_ORIENT_CORNER_TL:
	       dir = E_MENU_POP_DIRECTION_DOWN;
	       break;
	     case E_GADCON_ORIENT_CORNER_TR:
	       dir = E_MENU_POP_DIRECTION_DOWN;
	       break;
	     case E_GADCON_ORIENT_CORNER_BL:
	       dir = E_MENU_POP_DIRECTION_UP;
	       break;
	     case E_GADCON_ORIENT_CORNER_BR:
	       dir = E_MENU_POP_DIRECTION_UP;
	       break;
	     case E_GADCON_ORIENT_CORNER_LT:
	       dir = E_MENU_POP_DIRECTION_RIGHT;
	       break;
	     case E_GADCON_ORIENT_CORNER_RT:
	       dir = E_MENU_POP_DIRECTION_LEFT;
	       break;
	     case E_GADCON_ORIENT_CORNER_LB:
	       dir = E_MENU_POP_DIRECTION_RIGHT;
	       break;
	     case E_GADCON_ORIENT_CORNER_RB:
	       dir = E_MENU_POP_DIRECTION_LEFT;
	       break;
	     case E_GADCON_ORIENT_FLOAT:
	     case E_GADCON_ORIENT_HORIZ:
	     case E_GADCON_ORIENT_VERT:
	     default:
	       dir = E_MENU_POP_DIRECTION_AUTO;
	       break;
	     }
	   e_menu_activate_mouse(inst->menu,
				 e_util_zone_current_get(e_manager_current_get()),
				 x, y, w, h,
				 dir, ev->timestamp);
	   edje_object_signal_emit(inst->o_config, "e,state,focused", "e");
	 }
     }
   
   else if ((ev->button == 3) && (!inst->menu)) 
     {
        /* grab current zone */
        zone = e_util_zone_current_get(e_manager_current_get());

        /* create popup menu */
        inst->menu = e_menu_new();
        e_menu_post_deactivate_callback_set(inst->menu, _config_cb_menu_post, 
                                            inst);

        mi = e_menu_item_new(inst->menu);
        e_menu_item_label_set(mi, D_("Configuration"));
        e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
        e_menu_item_callback_set(mi, _config_cb_menu_configure, NULL);

        mi = e_menu_item_new(inst->menu);
        e_menu_item_separator_set(mi, 1);

        /* Each Gadget Client has a utility menu from the Container */
        e_gadcon_client_util_menu_items_append(inst->gcc, inst->menu, 0);
        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, 
                                          NULL, NULL);

        /* show the menu relative to gadgets position */
        e_menu_activate_mouse(inst->menu, zone, (x + ev->output.x), 
                              (y + ev->output.y), 1, 1, 
                              E_MENU_POP_DIRECTION_AUTO, ev->timestamp);
        evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button, 
                                 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

/* popup menu closing, cleanup */
static void 
_config_cb_menu_post(void *data, E_Menu *menu) 
{
   Instance *inst = NULL;

   if (!(inst = data)) return;
   if (!inst->menu) return;
   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

/* call configure from popup */
static void 
_config_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   if (!config_conf) return;
   if (config_conf->cfd) return;
   e_int_config_config_module(mn->zone->container);
}
