/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"
#include <strings.h>

/* Local Structures */
typedef struct _Instance Instance;
struct _Instance 
{
   /* pointer to this gadget's container */
   E_Gadcon_Client *gcc;

   /* evas_object used to display */
   Evas_Object *o_drawer;

   /* popup anyone ? */
   E_Menu *menu;

   /* Config_Item structure. Every gadget should have one :) */
   Config_Item *conf_item;

   E_Gadcon_Popup *popup;

   Drawer_Plugin *source;
   Drawer_Plugin *view;
   
   Eina_List *handlers;

   Eina_Bool updated_content : 1;
   Eina_Bool is_floating : 1;
};

/* Local Function Prototypes */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);

static void _drawer_conf_new(void);
static void _drawer_conf_free(void);
static int _drawer_conf_timer(void *data);
static Config_Item *_drawer_conf_item_get(const char *id);

static Instance *_drawer_instance_get(Config_Item *ci);

static void _drawer_shelf_update(Instance *inst, Drawer_Source_Item *si);
static void _drawer_popup_create(Instance *inst);
static void _drawer_popup_show(Instance *inst);
static void _drawer_popup_hide(Instance *inst);
static void _drawer_popup_update(Instance *inst);
static void _drawer_container_update(Instance *inst);
static void _drawer_container_setup(Instance *inst, E_Gadcon_Orient orient);

static Drawer_Plugin *_drawer_plugin_new(Instance *inst, const char *name, const char *category, size_t size);
static void _drawer_plugin_destroy(Instance *inst, Drawer_Plugin *p);
static Drawer_Source *_drawer_source_new(Instance *inst, const char *name);
static Drawer_View *_drawer_view_new(Instance *inst, const char *name);

static int _drawer_source_update_cb(void *data, int ev_type, void *event __UNUSED__);
static int _drawer_view_activate_cb(void *data, int ev_type, void *event);

static void _drawer_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _drawer_cb_menu_post(void *data, E_Menu *menu);
static void _drawer_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);
static void _drawer_popup_resize_cb(Evas_Object *obj, int *w, int *h);

/* Local Variables */
static int uuid = 0;
static Eina_List *instances = NULL;
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *drawer_conf = NULL;

EAPI int DRAWER_EVENT_SOURCE_UPDATE = 0;
EAPI int DRAWER_EVENT_VIEW_ITEM_ACTIVATE = 0;

static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "drawer", 
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, 
          _gc_id_new, NULL},
   E_GADCON_CLIENT_STYLE_PLAIN
};

EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "Drawer"};

/*
 * Module Functions
 */
EAPI void *
e_modapi_init(E_Module *m) 
{
   char buf[4096];

   /* Location of theme to load for this module */
   snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", m->dir);

   /* Define EET Data Storage */
   conf_item_edd = E_CONFIG_DD_NEW("Config_Item", Config_Item);
   #undef T
   #undef D
   #define T Config_Item
   #define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, source, STR);
   E_CONFIG_VAL(D, T, view, STR);

   conf_edd = E_CONFIG_DD_NEW("Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D conf_edd
   E_CONFIG_VAL(D, T, version, INT);
   E_CONFIG_LIST(D, T, conf_items, conf_item_edd); /* the list */

   /* Tell E to find any existing module data. First run ? */
   drawer_conf = e_config_domain_load("module.drawer", conf_edd);
   if (drawer_conf) 
     {
        /* Check config version */
        if ((drawer_conf->version >> 16) < MOD_CONFIG_FILE_EPOCH) 
          {
             /* config too old */
             _drawer_conf_free();
	     ecore_timer_add(1.0, _drawer_conf_timer,
			     D_("Drawer Module Configuration data needed "
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
        else if (drawer_conf->version > MOD_CONFIG_FILE_VERSION) 
          {
             /* config too new...wtf ? */
             _drawer_conf_free();
	     ecore_timer_add(1.0, _drawer_conf_timer, 
			     D_("Your Drawer Module configuration is NEWER "
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
   if (!drawer_conf) _drawer_conf_new();

   /* create a link from the modules config to the module
    * this is not written */
   drawer_conf->module = m;

   /* Tell any gadget containers (shelves, etc) that we provide a module
    * for the user to enjoy */
   e_gadcon_provider_register(&_gc_class);

   if (!DRAWER_EVENT_SOURCE_UPDATE)
     DRAWER_EVENT_SOURCE_UPDATE = ecore_event_type_new();
   if (!DRAWER_EVENT_VIEW_ITEM_ACTIVATE)
     DRAWER_EVENT_VIEW_ITEM_ACTIVATE = ecore_event_type_new();

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
   e_configure_registry_item_del("extensions/drawer");

   /* Remove the config panel category if we can. E will tell us.
    category stays if other items using it */
   e_configure_registry_category_del("extensions");

   /* Kill the config dialog */
   if (drawer_conf->cfd) e_object_del(E_OBJECT(drawer_conf->cfd));
   drawer_conf->cfd = NULL;

   /* Tell E the module is now unloaded. Gets removed from shelves, etc. */
   drawer_conf->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);

   /* Cleanup our item list */
   while (drawer_conf->conf_items) 
     {
        Config_Item *ci = NULL;

        /* Grab an item from the list */
        ci = drawer_conf->conf_items->data;

        /* remove it */
        drawer_conf->conf_items = 
          eina_list_remove_list(drawer_conf->conf_items, 
                                drawer_conf->conf_items);

        /* cleanup stringshares */
        if (ci->id) eina_stringshare_del(ci->id);

        /* keep the planet green */
        E_FREE(ci);
     }

   /* Cleanup the main config structure */
   E_FREE(drawer_conf);

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
   Eina_List *l = NULL;
   Instance *inst = NULL;

   EINA_LIST_FOREACH(instances, l, inst)
     {
	if (inst->view && inst->view->func.config_save)
	  inst->view->func.config_save(inst->view);
	if (inst->source && inst->source->func.config_save)
	  inst->source->func.config_save(inst->source);
     }

   e_config_domain_save("module.drawer", conf_edd, drawer_conf);
   return 1;
}

EAPI Eina_List *
drawer_plugins_list(Drawer_Plugin_Category cat)
{
   char buf[4096];
   const char *moddir;
   Ecore_List *files = NULL;
   Eina_List *ret = NULL;
   char *mod = NULL;

   if (cat == DRAWER_SOURCES)
     snprintf(buf, sizeof(buf), "drawer/%s/plugins/sources/", MODULE_ARCH);
   else if (cat == DRAWER_VIEWS)
     snprintf(buf, sizeof(buf), "drawer/%s/plugins/views/", MODULE_ARCH);

   moddir = e_path_find(path_modules, buf);
   if (!moddir) return NULL;
   if (!(files = ecore_file_ls(moddir))) return NULL;

   ecore_list_first_goto(files);
   while ((mod = ecore_list_next(files))) 
     {
	Efreet_Desktop *desk = NULL;
	Drawer_Plugin_Type *pi = NULL;
	char buf2[4096];
	char *ri;

	ri = rindex(mod, '.');
	if (strncmp(ri, ".desktop", 8))
	  continue;

	pi = E_NEW(Drawer_Plugin_Type, 1);

	snprintf(buf2, sizeof(buf2), "%s%s", moddir, mod);
	if (!(desk = efreet_desktop_get(buf2))) continue;
	if (desk->x) 
	  pi->title = eina_stringshare_add(ecore_hash_get(desk->x, "X-Drawer-Title"));
	if (!pi->title) pi->title = eina_stringshare_add(desk->name);

	pi->name = evas_stringshare_add(desk->name);
	pi->comment = evas_stringshare_add(desk->comment);

	ret = eina_list_append(ret, pi);
	efreet_desktop_free(desk);
     }
   free(mod);
   if (files) ecore_list_destroy(files);

   return ret;
}

EAPI void
drawer_plugins_list_free(Eina_List *list)
{
   Eina_List *l = NULL;
   Drawer_Plugin_Type *pi = NULL;

   EINA_LIST_FOREACH(list, l, pi)
     {
	if (pi->title) eina_stringshare_del(pi->title);
	if (pi->name) eina_stringshare_del(pi->name);
	if (pi->comment) eina_stringshare_del(pi->comment);
     }
   eina_list_free(list);
}

EAPI Drawer_Plugin *
drawer_plugin_load(Config_Item *ci, Drawer_Plugin_Category cat, const char *name)
{
   Instance *inst = NULL;
   
   inst = _drawer_instance_get(ci);

   if (cat == DRAWER_SOURCES)
    return DRAWER_PLUGIN(_drawer_source_new(inst, name));
   else if (cat == DRAWER_VIEWS)
     return DRAWER_PLUGIN(_drawer_view_new(inst, name));

   if (inst->source && inst->view && inst->is_floating)
     _drawer_container_update(inst);
}

EAPI Evas_Object *
drawer_plugin_config_button_get(Config_Item *ci, Evas *evas, Drawer_Plugin_Category cat)
{
   Instance *inst = NULL;
   Drawer_Plugin *p = NULL;

   inst =  _drawer_instance_get(ci);
   if (cat == DRAWER_SOURCES)
     p = inst->source;
   else if (cat == DRAWER_VIEWS)
     p = inst->view;

   if (p->func.config_get)
     return p->func.config_get(p, evas);
   else
     return e_widget_label_add(evas, D_("The plugin is not configurable"));
}

EAPI Evas_Object *
drawer_util_icon_create(Drawer_Source_Item *si, Evas *evas, int w, int h)
{
   if (si->desktop)
     return e_util_desktop_icon_add(si->desktop, MAX(w, h), evas);
}

/* Local Functions */

static void
_drawer_shelf_update(Instance *inst, Drawer_Source_Item *si)
{
   Evas_Object *o = NULL;

   o = edje_object_part_swallow_get(inst->o_drawer, "e.swallow.content");
   if (o)
     {
	evas_object_del(o);
	o = NULL;
     }

   if (!si && inst->source && inst->source->enabled)
     {
	Eina_List *l = NULL;

	/* XXX: better to have a function that retuns the first list instead */
	l = DRAWER_SOURCE(inst->source)->func.list(DRAWER_SOURCE(inst->source));
	if (l)
	  si = l->data;
     }

   if (si)
     o = drawer_util_icon_create(si, evas_object_evas_get(inst->o_drawer), 120, 120);
   else
     {
	char buf[4096];

	snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", 
	      drawer_conf->module->dir);
	o = edje_object_add(evas_object_evas_get(inst->o_drawer));
	if (!e_theme_edje_object_set(o, "base/theme/modules/drawer", 
				     "modules/drawer/main/content"))
	  edje_object_file_set(o, buf, "modules/drawer/main/content");
     }

   if (o)
     {
	edje_object_part_swallow(inst->o_drawer, "e.swallow.content", o);
	evas_object_show(o);
     }
}

static void
_drawer_popup_create(Instance *inst)
{
   inst->popup = e_gadcon_popup_new(inst->gcc, _drawer_popup_resize_cb);
   if (!(e_theme_edje_object_set(inst->popup->o_bg,
	       "base/theme/modules/drawer", "modules/drawer/container")))
     {
	char buf[4096];

	snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", drawer_conf->module->dir);
	if (edje_file_group_exists(buf, "modules/drawer/container"))
	  edje_object_file_set(inst->popup->o_bg, buf, "modules/drawer/container");
	else
	  e_theme_edje_object_set(inst->popup->o_bg, "base/theme/gadman", "e/gadman/popup");
     }
   e_popup_edje_bg_object_set(inst->popup->win, inst->popup->o_bg);

   _drawer_popup_update(inst);
}

static void
_drawer_popup_show(Instance *inst)
{
   edje_object_signal_emit(inst->o_drawer, "e,action,popup_show", "drawer");
   e_gadcon_popup_show(inst->popup);
   e_shelf_locked_set(inst->gcc->gadcon->shelf, 1);
}

static void
_drawer_popup_hide(Instance *inst)
{
   edje_object_signal_emit(inst->o_drawer, "e,action,popup_hide", "drawer");
   e_gadcon_popup_hide(inst->popup);
   e_shelf_locked_set(inst->gcc->gadcon->shelf, 0);
}

static void
_drawer_popup_update(Instance *inst)
{
   Evas_Object *o = NULL;
   Eina_List *l = NULL;

   l = DRAWER_SOURCE(inst->source)->func.list(DRAWER_SOURCE(inst->source));
   _drawer_shelf_update(inst, (Drawer_Source_Item *) l->data);

   o = DRAWER_VIEW(inst->view)->func.render(DRAWER_VIEW(inst->view),
	 inst->popup->win->evas, l);
   evas_object_data_set(o, "drawer_popup_data", inst);
   e_gadcon_popup_content_set(inst->popup, o);
}

static void
_drawer_container_update(Instance *inst)
{
   Evas_Object *o = NULL;
   Eina_List *l = NULL;

   o = edje_object_part_swallow_get(inst->o_drawer, "e.swallow.content");
   if (o) {
	evas_object_del(o);
	o = NULL;
   }
   l = DRAWER_SOURCE(inst->source)->func.list(DRAWER_SOURCE(inst->source));
   o = DRAWER_VIEW(inst->view)->func.render(DRAWER_VIEW(inst->view),
	 evas_object_evas_get(inst->o_drawer), l);
   edje_object_part_swallow(inst->o_drawer, "e.swallow.content", o);
   evas_object_show(o);
}

static void
_drawer_container_setup(Instance *inst, E_Gadcon_Orient orient)
{
   Evas_Object *o = NULL;
   char buf[4096];

   inst->is_floating = (orient == E_GADCON_ORIENT_FLOAT);

   /* theme file */
   snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", 
            drawer_conf->module->dir);

   o = edje_object_part_swallow_get(inst->o_drawer, "e.swallow.content");
   if (o)
     edje_object_part_unswallow(inst->o_drawer, o);
   if (inst->is_floating)
     {
	if (!e_theme_edje_object_set(inst->o_drawer, "base/theme/modules/drawer", 
		 "modules/drawer/main_float"))
	  edje_object_file_set(inst->o_drawer, buf, "modules/drawer/main_float");
     }
   else
     {
	if (!e_theme_edje_object_set(inst->o_drawer, "base/theme/modules/drawer", 
		 "modules/drawer/main"))
	  edje_object_file_set(inst->o_drawer, buf, "modules/drawer/main");
     }
   if (o)
     edje_object_part_swallow(inst->o_drawer, "e.swallow.content", o);

}

static Drawer_Plugin *
_drawer_plugin_new(Instance *inst, const char *name, const char *category, size_t size)
{
   Drawer_Plugin *p;
   char buf[4096];
   const char *modpath;

   p = calloc(1, size);
   snprintf(buf, sizeof(buf), "drawer/%s/plugins/%s/%s.so", MODULE_ARCH, category, name);
   modpath = e_path_find(path_modules, buf);
   if (!modpath)
     {
	e_util_dialog_show( D_("Drawer Plugins"),
	      D_("The plugin '%s' was not found."), name);
	p->error = EINA_TRUE;
	goto init_done;
     }
   p->handle = dlopen(modpath, RTLD_NOW | RTLD_GLOBAL);
   if (!p->handle)
     {
	e_util_dialog_show( D_("Drawer Plugins"),
	      D_("The plugin '%s' could not be opened."), name);
	p->error = EINA_TRUE;
	goto init_done;
     }

   p->func.init = dlsym(p->handle, "drawer_plugin_init");
   p->func.shutdown = dlsym(p->handle, "drawer_plugin_shutdown");
   p->api = dlsym(p->handle, "drawer_plugin_api");
   if ((!p->func.init) ||
       (!p->func.shutdown) ||
       (!p->api)
      )
     {
	e_util_dialog_show( D_("Drawer Plugins"),
	      D_("The plugin '%s' does not contain all required functions."), name);
	p->api = NULL;
	p->func.init = NULL;
	p->func.shutdown = NULL;
	dlclose(p->handle);
	p->error = EINA_TRUE;
	goto init_done;
     }

   if (p->api->version < DRAWER_PLUGIN_API_VERSION)
     {
	e_util_dialog_show( D_("Drawer Plugins"),
	      D_("The plugin '%s' does not have the required API version."), name);
	p->api = NULL;
	dlclose(p->handle);
	p->handle = NULL;
	p->error = EINA_TRUE;
	goto init_done;
     }

   p->func.config_get = dlsym(p->handle, "drawer_plugin_config_get");
   p->func.config_save = dlsym(p->handle, "drawer_plugin_config_save");

init_done:

   p->name = eina_stringshare_add(name);
   if (modpath)
     {
	char *d;

	d = ecore_file_dir_get(modpath);
	p->dir = eina_stringshare_add(d);
	free(d);
	eina_stringshare_del(modpath);
     }

   return p;
}

static void
_drawer_plugin_destroy(Instance *inst, Drawer_Plugin *p)
{
   if (!p->enabled || p->error) return;
   if (p->func.config_save)
     p->func.config_save(p);
   p->func.shutdown(p);
   if (p->name) eina_stringshare_del(p->name);
   if (p->dir) eina_stringshare_del(p->dir);
   if (p->handle) dlclose(p->handle);

   if (inst->source == p)
     inst->source = NULL;
   else if (inst->view == p)
     inst->view = NULL;

   free(p);
}

static Drawer_Source *
_drawer_source_new(Instance *inst, const char *name)
{
   Drawer_Plugin *p = NULL;
   Drawer_Source *s = NULL;

   if (!name) return NULL;
   if (inst->source)
     _drawer_plugin_destroy(inst, inst->source);

   p = _drawer_plugin_new(inst, name, "sources", sizeof(Drawer_Source));
   s = DRAWER_SOURCE(p);
   if (p->error)
     goto init_done;

   s->func.list = dlsym(p->handle, "drawer_source_list");

   if (!s->func.list)
     {
	e_util_dialog_show( D_("Drawer Plugins"),
	      D_("The plugin '%s' does not contain all required functions."), name);
	s->func.list = NULL;
	dlclose(p->handle);
	p->error = EINA_TRUE;
	goto init_done;
     }

   s->func.activate = dlsym(p->handle, "drawer_source_activate");
   /* XXX: Needs an optional description method */

init_done:

   inst->conf_item->source = eina_stringshare_add(name);
   inst->source = p;

   if (!p->error && (p->data = p->func.init(p, inst->conf_item->id)))
     p->enabled = EINA_TRUE;

   if (!inst->is_floating)
     _drawer_shelf_update(inst, NULL);
   return s;
}

static Drawer_View *
_drawer_view_new(Instance *inst, const char *name)
{
   Drawer_Plugin *p = NULL;
   Drawer_View *v = NULL;

   if (!name) return NULL;
   if (inst->view)
     _drawer_plugin_destroy(inst, inst->view);

   p = _drawer_plugin_new(inst, name, "views", sizeof(Drawer_View));
   v = DRAWER_VIEW(p);
   if (p->error)
     goto init_done;

   v->func.render = dlsym(p->handle, "drawer_view_render");

   if (!v->func.render)
     {
	e_util_dialog_show( D_("Drawer Plugins"),
	      D_("The plugin '%s' does not contain all required functions."), name);
	v->func.render = NULL;
	dlclose(p->handle);
	p->error = EINA_TRUE;
	goto init_done;
     }

   v->func.content_size_get = dlsym(p->handle, "drawer_view_content_size_get");
   v->func.orient_set = dlsym(p->handle, "drawer_view_orient_set");

init_done:

   inst->conf_item->view = eina_stringshare_add(name);
   inst->view = p;

   if (!p->error && (p->data = p->func.init(p, inst->conf_item->id)))
     p->enabled = EINA_TRUE;

   if (v->func.orient_set)
     v->func.orient_set(v, inst->gcc->gadcon->orient);
   return v;
}

/* Called when Gadget_Container says go */
static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   Instance *inst = NULL;

   /* New visual instance, any config ? */
   inst = E_NEW(Instance, 1);
   inst->conf_item = _drawer_conf_item_get(id);

   /* create on-screen object */
   inst->o_drawer = edje_object_add(gc->evas);

   _drawer_container_setup(inst, gc->orient);
   /* Start loading our module on screen via container */
   inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->o_drawer);
   inst->gcc->data = inst;

   /* hook a mouse down. we want/have a popup menu, right ? */
   evas_object_event_callback_add(inst->o_drawer, EVAS_CALLBACK_MOUSE_DOWN, 
                                  _drawer_cb_mouse_down, inst);

   /* add to list of running instances so we can cleanup later */
   instances = eina_list_append(instances, inst);

   inst->handlers = eina_list_append(inst->handlers,
	 ecore_event_handler_add(DRAWER_EVENT_SOURCE_UPDATE,
	    _drawer_source_update_cb, inst));
   inst->handlers = eina_list_append(inst->handlers,
	 ecore_event_handler_add(DRAWER_EVENT_VIEW_ITEM_ACTIVATE,
	    _drawer_view_activate_cb, inst));

   if (inst->conf_item->source)
     _drawer_source_new(inst, inst->conf_item->source);
   else if (!inst->is_floating)
     _drawer_shelf_update(inst, NULL);

   if (inst->conf_item->view)
     _drawer_view_new(inst, inst->conf_item->view);

   if (inst->source && inst->view && inst->is_floating)
     _drawer_container_update(inst);

   /* return the Gadget_Container Client */
   return inst->gcc;
}

/* Called when Gadget_Container says stop */
static void 
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst = NULL;
   Evas_Object *o;

   if (!(inst = gcc->data)) return;
   instances = eina_list_remove(instances, inst);

   o = edje_object_part_swallow_get(inst->o_drawer, "e.swallow.content");
   if (o) evas_object_del(o);
   /* kill popup menu */
   if (inst->menu) 
     {
        e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
        e_object_del(E_OBJECT(inst->menu));
        inst->menu = NULL;
     }
   /* delete the visual */
   if (inst->o_drawer) 
     {
        /* remove mouse down callback hook */
        evas_object_event_callback_del(inst->o_drawer, EVAS_CALLBACK_MOUSE_DOWN, 
                                       _drawer_cb_mouse_down);
        evas_object_del(inst->o_drawer);
     }
   if (inst->popup)
     _drawer_popup_hide(inst);

   E_FREE_LIST(inst->handlers, ecore_event_handler_del);
   E_FREE(inst);
}

/* For for when container says we are changing position */
static void 
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient) 
{
   Instance *inst = NULL;
   
   inst = gcc->data;
   _drawer_container_setup(inst, orient);
   if (inst->view && DRAWER_VIEW(inst->view)->func.orient_set)
     DRAWER_VIEW(inst->view)->func.orient_set(DRAWER_VIEW(inst->view), orient);

   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

/* Gadget/Module label */
static char *
_gc_label(E_Gadcon_Client_Class *client_class) 
{
   return D_("Drawer");
}

/* so E can keep a unique instance per-container */
static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class) 
{
   Config_Item *ci = NULL;

   ci = _drawer_conf_item_get(NULL);
   return ci->id;
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas) 
{
   Evas_Object *o = NULL;
   char buf[4096];

   /* theme */
   snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", drawer_conf->module->dir);

   /* create icon object */
   o = edje_object_add(evas);

   /* load icon from theme */
   edje_object_file_set(o, buf, "icon");

   return o;
}

/* new module needs a new config :), or config too old and we need one anyway */
static void 
_drawer_conf_new(void) 
{
   drawer_conf = E_NEW(Config, 1);
   drawer_conf->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) if ((drawer_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

   /* setup defaults */
   IFMODCFG(0x008d);
   IFMODCFGEND;

   /* update the version */
   drawer_conf->version = MOD_CONFIG_FILE_VERSION;

   /* setup limits on the config properties here (if needed) */

   /* save the config to disk */
   e_config_save_queue();
}

static void 
_drawer_conf_free(void) 
{
   /* cleanup any stringshares here */
   while (drawer_conf->conf_items) 
     {
        Config_Item *ci = NULL;

        ci = drawer_conf->conf_items->data;
        drawer_conf->conf_items = 
          eina_list_remove_list(drawer_conf->conf_items, 
                                drawer_conf->conf_items);
        /* EPA */
        if (ci->id) eina_stringshare_del(ci->id);
        if (ci->source) eina_stringshare_del(ci->source);
        if (ci->view) eina_stringshare_del(ci->view);
        E_FREE(ci);
     }

   E_FREE(drawer_conf);
}

/* timer for the config oops dialog */
static int 
_drawer_conf_timer(void *data) 
{
   e_util_dialog_internal(D_("Drawer Configuration Updated"), (char *) data); \
   return 0;
}

/* function to search for any Config_Item struct for this Item
 * create if needed */
static Config_Item *
_drawer_conf_item_get(const char *id) 
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
        for (l = drawer_conf->conf_items; l; l = l->next) 
          {
             if (!(ci = l->data)) continue;
             if ((ci->id) && (!strcmp(ci->id, id))) return ci;
          }
     }
   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);

   if (!ci->source) ci->source = eina_stringshare_add("launcher");
   if (!ci->view) ci->view = eina_stringshare_add("list");

   drawer_conf->conf_items = eina_list_append(drawer_conf->conf_items, ci);
   return ci;
}

static Instance *
_drawer_instance_get(Config_Item *ci)
{
   Eina_List *l = NULL;
   Instance *inst = NULL;

   EINA_LIST_FOREACH(instances, l, inst)
     {
	if (inst->conf_item == ci)
	  return inst;
     }
}

static int
_drawer_source_update_cb(void *data, int ev_type, void *event __UNUSED__)
{
   Instance *inst = NULL;
   int visible = 0;

   inst = data;
   if (ev_type != DRAWER_EVENT_SOURCE_UPDATE) return 1;
   inst->updated_content = EINA_TRUE;

   if (inst->view && inst->is_floating)
     _drawer_container_update(inst);

   return 1;
}

static int
_drawer_view_activate_cb(void *data, int ev_type, void *event)
{
   Drawer_View *v = NULL;
   Drawer_Event_View_Activate *ev = NULL;
   Eina_List *l = NULL;
   Instance *inst = NULL;

   if (ev_type != DRAWER_EVENT_VIEW_ITEM_ACTIVATE) return 1;
   inst = data;
   ev = event;
   v = ev->view;

   DRAWER_SOURCE(inst->source)->func.activate(DRAWER_SOURCE(inst->source), ev->data, inst->gcc->gadcon->zone);

   if (!inst->is_floating)
     _drawer_popup_hide(inst);

   return 0;
}

/* Pants On */
static void 
_drawer_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Instance *inst = NULL;
   Evas_Event_Mouse_Down *ev = NULL;

   if (!(inst = data)) return;
   ev = event;
   if (ev->button == 1 && !inst->is_floating && inst->source && inst->view
	 && inst->source->enabled && inst->view->enabled)
     {
	if (!inst->popup) _drawer_popup_create(inst);
	if (inst->popup->win->visible)
	  _drawer_popup_hide(inst);
	else
	  {
	     if (inst->updated_content)
	       _drawer_popup_update(inst);
	     _drawer_popup_show(inst);
	  }
	return;
     }
   else if ((ev->button == 3) && (!inst->menu)) 
     {
	E_Zone *zone = NULL;
	E_Menu_Item *mi = NULL;
	int x, y;

        /* grab current zone */
        zone = e_util_zone_current_get(e_manager_current_get());

        /* create popup menu */
        inst->menu = e_menu_new();
        e_menu_post_deactivate_callback_set(inst->menu, _drawer_cb_menu_post, 
                                            inst);

        mi = e_menu_item_new(inst->menu);
        e_menu_item_label_set(mi, D_("Settings"));
        e_util_menu_item_edje_icon_set(mi, "widget/config");
        e_menu_item_callback_set(mi, _drawer_cb_menu_configure, inst);

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
_drawer_cb_menu_post(void *data, E_Menu *menu) 
{
   Instance *inst = NULL;

   if (!(inst = data)) return;
   if (!inst->menu) return;
   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

/* call configure from popup */
static void 
_drawer_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Instance *inst = NULL;
   
   inst = data;
   if (!drawer_conf) return;
   if (drawer_conf->cfd) return;
   e_int_config_drawer_module(mn->zone->container, inst->conf_item);
}

static void
_drawer_popup_resize_cb(Evas_Object *obj, int *w, int *h)
{
   Instance *inst = NULL;
   Drawer_View *v = NULL;
   int cw, ch;

   inst = evas_object_data_get(obj, "drawer_popup_data");
   v = DRAWER_VIEW(inst->view);
   if (!v->func.content_size_get) return;
   v->func.content_size_get(v, inst->popup->gcc, &cw, &ch);

   if (cw) *w = cw;
   if (ch) *h = ch;
}
