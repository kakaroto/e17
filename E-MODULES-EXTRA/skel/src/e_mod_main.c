#include <e.h>
#include "e_mod_main.h"

/* Local Function Prototypes */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static const char *_gc_id_new(void);
static void _gc_id_del(const char *id);
static Evas_Object *_gc_icon(Evas *evas);

static void _skel_conf_new(void);
static void _skel_conf_free(void);
static int _skel_conf_timer(void *data);
static Config_Item *_skel_conf_item_get(const char *id);
static void _skel_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _skel_cb_menu_post(void *data, E_Menu *menu);
static void _skel_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);

/* Local Structures */
typedef struct _Instance Instance;
struct _Instance 
{
   /* pointer to this gadget's container */
   E_Gadcon_Client *gcc;

   /* evas_object used to display */
   Evas_Object *o_skel;

   /* popup anyone ? */
   E_Menu *menu;

   /* Config_Item structure. Every gadget should have one :) */
   Config_Item *conf_item;
};

/* Local Variables */
static int uuid = 0;
static Evas_List *instances = NULL;
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;
Config *skel_conf = NULL;

static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "skel", 
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, 
          _gc_id_new, _gc_id_del},
   E_GADCON_CLIENT_STYLE_PLAIN
};

EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "Skel"};

/*
 * Module Functions
*/
EAPI void *
e_modapi_init(E_Module *m) 
{
   char buf[4096];

   /* Location of theme to load for this module */
   snprintf(buf, sizeof(buf), "%s/e-module-skel.edj", m->dir);

   /* Display this Modules config info in the main Config Panel */

   /* starts with a category */
   e_configure_registry_category_add("advanced", 80, "Advanced", 
                                     NULL, "enlightenment/advanced");
   /* add right-side item */
   e_configure_registry_item_add("advanced/connman", 110, "Skel", 
                                 NULL, buf, e_int_config_skel_module);

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
   skel_conf = e_config_domain_load("module.skel", conf_edd);
   if (skel_conf) 
     {
        /* Check config version */
        if ((skel_conf->version >> 16) < MOD_CONFIG_FILE_EPOCH) 
          {
             /* config too old */
             _skel_conf_free();
	     ecore_timer_add(1.0, _skel_conf_timer,
			     "Skeleton Module Configuration data needed "
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
			     "liking. Sorry for the inconvenience.<br>");
          }

        /* Ardvarks */
        else if (skel_conf->version > MOD_CONFIG_FILE_VERSION) 
          {
             /* config too new...wtf ? */
             _skel_conf_free();
	     ecore_timer_add(1.0, _skel_conf_timer, 
			     "Your Skeleton Module configuration is NEWER "
			     "than the module version. This is "
			     "very<br>strange. This should not happen unless"
			     " you downgraded<br>the module or "
			     "copied the configuration from a place where"
			     "<br>a newer version of the module "
			     "was running. This is bad and<br>as a "
			     "precaution your configuration has been now "
			     "restored to<br>defaults. Sorry for the "
			     "inconvenience.<br>");
          }
     }

   /* if we don't have a config yet, or it got erased above, 
    * then create a default one */
   if (!skel_conf) _skel_conf_new();

   /* create a link from the modules config to the module
    * this is not written */
   skel_conf->module = m;

   /* Tell any gadget containers (shelves, etc) that we provide a module
    * for the user to enjoy */
   e_gadcon_provider_register(&_gc_class);

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
   e_configure_registry_item_del("advanced/connman");

   /* Remove the config panel category if we can. E will tell us.
    category stays if other items using it */
   e_configure_registry_category_del("advanced");

   /* Kill the config dialog */
   if (skel_conf->cfd) e_object_del(E_OBJECT(skel_conf->cfd));
   skel_conf->cfd = NULL;

   /* Tell E the module is now unloaded. Gets removed from shelves, etc. */
   skel_conf->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);

   /* Cleanup our item list */
   while (skel_conf->conf_items) 
     {
        Config_Item *ci = NULL;

        /* Grab an item from the list */
        ci = skel_conf->conf_items->data;
        /* remove it */
        skel_conf->conf_items = 
          evas_list_remove_list(skel_conf->conf_items, 
                                skel_conf->conf_items);
        /* cleanup stringshares !! ) */
        if (ci->id) evas_stringshare_del(ci->id);
        /* keep the planet green */
        E_FREE(ci);
     }

   /* Cleanup the main config structure */
   E_FREE(skel_conf);

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
   e_config_domain_save("module.skel", conf_edd, skel_conf);
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
   snprintf(buf, sizeof(buf), "%s/e-module-skel.edj", 
            skel_conf->module->dir);

   /* New visual instance, any config ? */
   inst = E_NEW(Instance, 1);
   inst->conf_item = _skel_conf_item_get(id);

   /* create on-screen object */
   inst->o_skel = edje_object_add(gc->evas);
   /* we have a theme ? */
   if (!e_theme_edje_object_set(inst->o_skel, "base/theme/modules/skel", 
                                "modules/skel/main"))
     edje_object_file_set(inst->o_skel, buf, "modules/skel/main");

   /* Start loading our module on screen via container */
   inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->o_skel);
   inst->gcc->data = inst;

   /* hook a mouse down. we want/have a popup menu, right ? */
   evas_object_event_callback_add(inst->o_skel, EVAS_CALLBACK_MOUSE_DOWN, 
                                  _skel_cb_mouse_down, inst);

   /* add to list of running instances so we can cleanup later */
   instances = evas_list_append(instances, inst);

   /* return the Gadget_Container Client */
   return inst->gcc;
}

/* Called when Gadget_Container says stop */
static void 
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst = NULL;

   if (!(inst = gcc->data)) return;
   instances = evas_list_remove(instances, inst);

   /* kill popup menu */
   if (inst->menu) 
     {
        e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
        e_object_del(E_OBJECT(inst->menu));
        inst->menu = NULL;
     }
   /* delete the visual */
   if (inst->o_skel) 
     {
        /* remove mouse down callback hook */
        evas_object_event_callback_del(inst->o_skel, EVAS_CALLBACK_MOUSE_DOWN, 
                                       _skel_cb_mouse_down);
        evas_object_del(inst->o_skel);
     }
   E_FREE(inst);
}

/* For for when container says we are changing position */
static void 
_gc_orient(E_Gadcon_Client *gcc) 
{
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

/* Gadget/Module label */
static char *
_gc_label(void) 
{
   return "Skeleton";
}

/* so E can keep a unique instance per-container */
static const char *
_gc_id_new(void) 
{
   Config_Item *ci = NULL;

   ci = _skel_conf_item_get(NULL);
   return ci->id;
}

/* gets called when container says remove this item */
static void 
_gc_id_del(const char *id) 
{
   Config_Item *ci = NULL;

   if (!(ci = _skel_conf_item_get(id))) return;

   /* cleanup !! */
   if (ci->id) evas_stringshare_del(ci->id);

   skel_conf->conf_items = evas_list_remove(skel_conf->conf_items, ci);
   E_FREE(ci);
}

static Evas_Object *
_gc_icon(Evas *evas) 
{
   Evas_Object *o = NULL;
   char buf[4096];

   /* theme */
   snprintf(buf, sizeof(buf), "%s/e-module-skel.edj", skel_conf->module->dir);

   /* create icon object */
   o = edje_object_add(evas);

   /* load icon from theme */
   edje_object_file_set(o, buf, "icon");

   return o;
}

/* new module needs a new config :), or config too old and we need one anyway */
static void 
_skel_conf_new(void) 
{
   Config_Item *ci = NULL;
   char buf[128];

   skel_conf = E_NEW(Config, 1);
   skel_conf->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) if ((skel_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

   /* setup defaults */
   IFMODCFG(0x008d);
   skel_conf->switch1 = 1;
   _skel_conf_item_get(NULL);
   IFMODCFGEND;

   /* update the version */
   skel_conf->version = MOD_CONFIG_FILE_VERSION;

   /* setup limits on the config properties here (if needed) */

   /* save the config to disk */
   e_config_save_queue();
}

static void 
_skel_conf_free(void) 
{
   /* cleanup any stringshares here */
   while (skel_conf->conf_items) 
     {
        Config_Item *ci = NULL;

        ci = skel_conf->conf_items->data;
        skel_conf->conf_items = 
          evas_list_remove_list(skel_conf->conf_items, 
                                skel_conf->conf_items);
        /* EPA */
        if (ci->id) evas_stringshare_del(ci->id);
        E_FREE(ci);
     }

   E_FREE(skel_conf);
}

/* timer for the config oops dialog */
static int 
_skel_conf_timer(void *data) 
{
   e_util_dialog_show("Skeleton Configuration Updated", data);
   return 0;
}

/* function to search for any Config_Item struct for this Item
 * create if needed */
static Config_Item *
_skel_conf_item_get(const char *id) 
{
   Evas_List *l = NULL;
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
        for (l = skel_conf->conf_items; l; l = l->next) 
          {
             if (!(ci = l->data)) continue;
             if ((ci->id) && (!strcmp(ci->id, id))) return ci;
          }
     }
   ci = E_NEW(Config_Item, 1);
   ci->id = evas_stringshare_add(id);
   ci->switch2 = 0;
   skel_conf->conf_items = evas_list_append(skel_conf->conf_items, ci);
   return ci;
}

/* Pants On */
static void 
_skel_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Instance *inst = NULL;
   Evas_Event_Mouse_Down *ev;
   E_Zone *zone = NULL;
   E_Menu_Item *mi = NULL;
   int x, y;

   if (!(inst = data)) return;
   ev = event;
   if ((ev->button == 3) && (!inst->menu)) 
     {
        /* grab current zone */
        zone = e_util_zone_current_get(e_manager_current_get());

        /* create popup menu */
        inst->menu = e_menu_new();
        e_menu_post_deactivate_callback_set(inst->menu, _skel_cb_menu_post, 
                                            inst);

        mi = e_menu_item_new(inst->menu);
        e_menu_item_label_set(mi, "Configuration");
        e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
        e_menu_item_callback_set(mi, _skel_cb_menu_configure, NULL);

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
_skel_cb_menu_post(void *data, E_Menu *menu) 
{
   Instance *inst = NULL;

   if (!(inst = data)) return;
   if (!inst->menu) return;
   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

/* call configure from popup */
static void 
_skel_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   if (!skel_conf) return;
   if (skel_conf->cfd) return;
   e_int_config_skel_module(mn->zone->container, NULL);
}
