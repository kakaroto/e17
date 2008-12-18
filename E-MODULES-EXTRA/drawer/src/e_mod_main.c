/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2,t0,(0
 */
#include "e_mod_main.h"
#include <strings.h>
#include <Epsilon_Request.h>

/* Local Structures */
typedef struct _Instance Instance;
typedef struct _Drawer_Epsilon_Data Drawer_Epsilon_Data;

struct _Instance 
{
   /* pointer to this gadget's container */
   E_Gadcon_Client *gcc;

   Evas_Object *o_drawer, *o_content;

   /* popup anyone ? */
   E_Menu *menu;

   /* Config_Item structure. Every gadget should have one :) */
   Config_Item *conf_item;

   E_Gadcon_Popup *popup;

   Drawer_Plugin *source;
   Drawer_Plugin *view;
   
   Eina_List *handlers;

   Eina_Bool is_floating : 1;
   Eina_Bool pop_hiding : 1;
   Eina_Bool pop_update : 1;
};

struct _Drawer_Epsilon_Data
{
   Evas_Object *o_icon;

   int w, h;
};

/* Local Function Prototypes */
static E_Gadcon_Client *_drawer_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _drawer_gc_shutdown(E_Gadcon_Client *gcc);
static void _drawer_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_drawer_gc_label(E_Gadcon_Client_Class *client_class);
static const char *_drawer_gc_id_new(E_Gadcon_Client_Class *client_class);
static Evas_Object *_drawer_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);

static void _drawer_conf_new(void);
static void _drawer_conf_free(void);
static void _drawer_conf_item_free(Config_Item *ci);
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

static int _drawer_source_update_cb(void *data __UNUSED__, int ev_type, void *event);
static int _drawer_view_activate_cb(void *data __UNUSED__, int ev_type, void *event);
static int _drawer_thumbnail_done_cb(void *data __UNUSED__, int ev_type, void *event);

static void _drawer_popup_hidden_cb(void *data, Evas_Object *obj __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__);

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

static const E_Gadcon_Client_Class _drawer_gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "drawer", 
     {_drawer_gc_init, _drawer_gc_shutdown, _drawer_gc_orient, _drawer_gc_label,
       	_drawer_gc_icon, _drawer_gc_id_new, NULL},
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
   const char *homedir;

   epsilon_request_init();

   homedir = e_user_homedir_get();
   snprintf(buf, sizeof(buf), "%s/.e/e/config/%s/module.drawer", 
	    homedir, e_config_profile_get());
   ecore_file_mkdir(buf);

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
	/* Millepedes */
	else
	  {
	     Config_Item *ci = NULL;
	     Eina_List *l, *l_next;
	     const char *dup = NULL;

	     EINA_LIST_FOREACH_SAFE(drawer_conf->conf_items, l, l_next, ci)
	       {
		  if ((!dup) || (strcmp(dup, ci->id)))
		    {
		       dup = ci->id;
		       continue;
		    }
		  else
		    {
		       _drawer_conf_item_free(ci);
		       drawer_conf->conf_items =
			  eina_list_remove_list(drawer_conf->conf_items, l);
		    }
	       }
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
   e_gadcon_provider_register(&_drawer_gc_class);

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
   e_gadcon_provider_unregister(&_drawer_gc_class);

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

	_drawer_conf_item_free(ci);
     }

   /* Cleanup the main config structure */
   E_FREE(drawer_conf);

   /* Clean EET */
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);

   epsilon_request_shutdown();

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
	  pi->title = eina_stringshare_add(eina_hash_find(desk->x, "X-Drawer-Title"));
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
	eina_stringshare_del(pi->title);
	eina_stringshare_del(pi->name);
	eina_stringshare_del(pi->comment);
     }
   eina_list_free(list);
}

EAPI Drawer_Plugin *
drawer_plugin_load(Config_Item *ci, Drawer_Plugin_Category cat, const char *name)
{
   Instance *inst = NULL;
   
   inst = _drawer_instance_get(ci);

   if (cat == DRAWER_SOURCES)
     DRAWER_PLUGIN(_drawer_source_new(inst, name));
   else if (cat == DRAWER_VIEWS)
     DRAWER_PLUGIN(_drawer_view_new(inst, name));

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
   Drawer_Epsilon_Data *ep = NULL;

   if (si->desktop)
     return e_util_desktop_icon_add(si->desktop, MAX(w, h), evas);
   else if (si->file_path)
     {
	Evas_Object *o;

	if ((e_util_glob_case_match(si->file_path, "*.desktop")) ||
	    (e_util_glob_case_match(si->file_path, "*.directory")))
	  {
	     Efreet_Desktop *desktop;

	     desktop = efreet_desktop_new(si->file_path);
	     if (!desktop) return NULL;
	     o = e_util_desktop_icon_add(desktop, MAX(w, h), evas);

	     efreet_desktop_free(desktop);
	     if (!o)
	       {
		  char buf[4096];

		  snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", 
			   drawer_conf->module->dir);
		  o = edje_object_add(evas);
		  if (!e_theme_edje_object_set(o, "base/theme/modules/drawer", 
					       "modules/drawer/main/content"))
		    edje_object_file_set(o, buf, "modules/drawer/main/content");
	       }
	     return o;
	  }
	else if (ecore_file_is_dir(si->file_path))
	  {
	     o = edje_object_add(evas);
	     e_theme_edje_object_set(o, "base/theme/fileman", "e/icons/fileman/folder");

	     return o;
	  }

#if 0
	o = e_thumb_icon_add(evas);
	if ((e_util_glob_case_match(si->file_path, "*.edj")))
	  {
	     /* FIXME: There is probably a quicker way of doing this. */
	     if (edje_file_group_exists(si->file_path, "icon"))
	       e_thumb_icon_file_set(o, si->file_path, "icon");
	     else if (edje_file_group_exists(si->file_path, "e/desktop/background"))
	       e_thumb_icon_file_set(o, si->file_path, "e/desktop/background");
	     else if (edje_file_group_exists(si->file_path, "e/init/splash"))
	       e_thumb_icon_file_set(o, si->file_path, "e/init/splash");
	     e_thumb_icon_size_set(o, w, w/4*3);
	  }
	else
	  {
	     e_thumb_icon_file_set(o, si->file_path, NULL);
	     e_thumb_icon_size_set(o, w, h);
	  }

	e_thumb_icon_begin(o);
	return o;
#endif
	o = edje_object_add(evas);

	ep = calloc(1, sizeof(Drawer_Epsilon_Data));
	ep->o_icon = o;
	ep->w = w;
	ep->h = h;

	epsilon_request_add(si->file_path, EPSILON_THUMB_NORMAL, ep);
	return o;
     }
}

/* Local Functions */

static void
_drawer_shelf_update(Instance *inst, Drawer_Source_Item *si)
{
   if (inst->o_content)
     {
	edje_object_part_unswallow(inst->o_drawer, inst->o_content);
	evas_object_del(inst->o_content);
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
     inst->o_content = drawer_util_icon_create(si, evas_object_evas_get(inst->o_drawer), 120, 120);
   else
     {
	char buf[4096];

	snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", 
	      drawer_conf->module->dir);
	inst->o_content = edje_object_add(evas_object_evas_get(inst->o_drawer));
	if (!e_theme_edje_object_set(inst->o_content, "base/theme/modules/drawer", 
				     "modules/drawer/main/content"))
	  edje_object_file_set(inst->o_content, buf, "modules/drawer/main/content");
     }

   if (inst->o_content)
     {
	edje_object_part_swallow(inst->o_drawer, "e.swallow.content", inst->o_content);
	evas_object_show(inst->o_content);
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
   edje_object_signal_callback_add(inst->popup->o_bg, "e,action,popup,hidden", "drawer", 
				   _drawer_popup_hidden_cb, inst);

   _drawer_popup_update(inst);
}

static void
_drawer_popup_show(Instance *inst)
{
   switch(inst->gcc->gadcon->orient)
     {
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_RB:
      case E_GADCON_ORIENT_RIGHT:
	 edje_object_signal_emit(inst->o_drawer, "e,action,popup,show,right", "drawer");
	 edje_object_signal_emit(inst->popup->o_bg, "e,action,popup,show,right", "drawer");
	break;
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_LB:
	 edje_object_signal_emit(inst->o_drawer, "e,action,popup,show,left", "drawer");
	 edje_object_signal_emit(inst->popup->o_bg, "e,action,popup,show,left", "drawer");
	 break;
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
	 edje_object_signal_emit(inst->o_drawer, "e,action,popup,show,top", "drawer");
	 edje_object_signal_emit(inst->popup->o_bg, "e,action,popup,show,top", "drawer");
	break;
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	 edje_object_signal_emit(inst->o_drawer, "e,action,popup,show,bottom", "drawer");
	 edje_object_signal_emit(inst->popup->o_bg, "e,action,popup,show,bottom", "drawer");
	break;
     }
   e_gadcon_popup_show(inst->popup);
   e_shelf_locked_set(inst->gcc->gadcon->shelf, 1);
}

static void
_drawer_popup_hide(Instance *inst)
{
   switch(inst->gcc->gadcon->orient)
     {
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_RB:
      case E_GADCON_ORIENT_RIGHT:
	 edje_object_signal_emit(inst->o_drawer, "e,action,popup,hide,right", "drawer");
	 edje_object_signal_emit(inst->popup->o_bg, "e,action,popup,hide,right", "drawer");
	break;
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_LB:
	 edje_object_signal_emit(inst->o_drawer, "e,action,popup,hide,left", "drawer");
	 edje_object_signal_emit(inst->popup->o_bg, "e,action,popup,hide,left", "drawer");
	 break;
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
	 edje_object_signal_emit(inst->o_drawer, "e,action,popup,hide,top", "drawer");
	 edje_object_signal_emit(inst->popup->o_bg, "e,action,popup,hide,top", "drawer");
	break;
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	 edje_object_signal_emit(inst->o_drawer, "e,action,popup,hide,bottom", "drawer");
	 edje_object_signal_emit(inst->popup->o_bg, "e,action,popup,hide,bottom", "drawer");
	break;
     }
   inst->pop_hiding = EINA_TRUE;
}

static void
_drawer_popup_update(Instance *inst)
{
   Evas_Object *o = NULL;
   Eina_List *l = NULL;
   Drawer_Source *s;

   if (inst->pop_hiding)
     {
	inst->pop_update = EINA_TRUE;
	return;
     }

   s = DRAWER_SOURCE(inst->source);
   l = s->func.list(s);
   _drawer_shelf_update(inst, (Drawer_Source_Item *) l->data);

   o = DRAWER_VIEW(inst->view)->func.render(DRAWER_VIEW(inst->view),
	 inst->popup->win->evas, l);
   if (s->func.description_get)
     edje_object_part_text_set(inst->popup->o_bg, "e.text.description",
	   s->func.description_get(s));
   evas_object_data_set(o, "drawer_popup_data", inst);
   e_gadcon_popup_content_set(inst->popup, o);

   inst->pop_update = EINA_FALSE;
}

static void
_drawer_container_update(Instance *inst)
{
   Eina_List *l = NULL;
   Drawer_Source *s;

   if (inst->o_content)
     {
	edje_object_part_unswallow(inst->o_drawer, inst->o_content);
	evas_object_del(inst->o_content);
     }
   s = DRAWER_SOURCE(inst->source);
   l = s->func.list(s);
   inst->o_content = DRAWER_VIEW(inst->view)->func.render
      (DRAWER_VIEW(inst->view),
       evas_object_evas_get(inst->o_drawer), l);

   if (s->func.description_get)
     edje_object_part_text_set(inst->o_drawer, "e.text.description",
	   s->func.description_get(s));
   edje_object_part_swallow(inst->o_drawer, "e.swallow.content", inst->o_content);
   evas_object_show(inst->o_content);
}

static void
_drawer_container_setup(Instance *inst, E_Gadcon_Orient orient)
{
   char buf[4096];

   inst->is_floating = (orient == E_GADCON_ORIENT_FLOAT);

   /* theme file */
   snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", 
            drawer_conf->module->dir);

   if (inst->o_content)
     edje_object_part_unswallow(inst->o_drawer, inst->o_content);
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
	switch(orient)
	  {
	   case E_GADCON_ORIENT_CORNER_RT:
	   case E_GADCON_ORIENT_CORNER_RB:
	   case E_GADCON_ORIENT_RIGHT:
	      edje_object_signal_emit(inst->o_drawer, "e,state,orient,right", "drawer");
	      break;
	   case E_GADCON_ORIENT_LEFT:
	   case E_GADCON_ORIENT_CORNER_LT:
	   case E_GADCON_ORIENT_CORNER_LB:
	      edje_object_signal_emit(inst->o_drawer, "e,state,orient,left", "drawer");
	      break;
	   case E_GADCON_ORIENT_TOP:
	   case E_GADCON_ORIENT_CORNER_TL:
	   case E_GADCON_ORIENT_CORNER_TR:
	      edje_object_signal_emit(inst->o_drawer, "e,state,orient,top", "drawer");
	      break;
	   case E_GADCON_ORIENT_BOTTOM:
	   case E_GADCON_ORIENT_CORNER_BL:
	   case E_GADCON_ORIENT_CORNER_BR:
	      edje_object_signal_emit(inst->o_drawer, "e,state,orient,bottom", "drawer");
	      break;
	  }
     }
   if (inst->o_content)
     edje_object_part_swallow(inst->o_drawer, "e.swallow.content", inst->o_content);

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
   eina_stringshare_del(p->name);
   eina_stringshare_del(p->dir);
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
   s->func.description_get = dlsym(p->handle, "drawer_source_description_get");

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
_drawer_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
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
				 _drawer_source_update_cb, NULL));
   inst->handlers = eina_list_append(inst->handlers,
	 ecore_event_handler_add(DRAWER_EVENT_VIEW_ITEM_ACTIVATE,
				 _drawer_view_activate_cb, NULL));
   inst->handlers = eina_list_append(inst->handlers,
	 ecore_event_handler_add(EPSILON_EVENT_DONE,
				 _drawer_thumbnail_done_cb, NULL));

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
_drawer_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst = NULL;

   if (!(inst = gcc->data)) return;
   instances = eina_list_remove(instances, inst);

   if (inst->source)
     _drawer_plugin_destroy(inst, inst->source);
   if (inst->view)
     _drawer_plugin_destroy(inst, inst->view);

   if (inst->o_content)
     {
	edje_object_part_unswallow(inst->o_drawer, inst->o_content);
	evas_object_del(inst->o_content);
     }
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
     {
	_drawer_popup_hide(inst);
	_drawer_popup_hidden_cb(inst, NULL, NULL, NULL);
	edje_object_signal_callback_del(inst->popup->o_bg, "e,action,popup,hidden",
					"drawer", _drawer_popup_hidden_cb);
	e_object_del(E_OBJECT(inst->popup));
     }

   E_FREE_LIST(inst->handlers, ecore_event_handler_del);
   E_FREE(inst);
}

/* For for when container says we are changing position */
static void 
_drawer_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient) 
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
_drawer_gc_label(E_Gadcon_Client_Class *client_class) 
{
   return D_("Drawer");
}

/* so E can keep a unique instance per-container */
static const char *
_drawer_gc_id_new(E_Gadcon_Client_Class *client_class) 
{
   Config_Item *ci = NULL;

   ci = _drawer_conf_item_get(NULL);
   return ci->id;
}

static Evas_Object *
_drawer_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas) 
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
	_drawer_conf_item_free(ci);
     }

   E_FREE(drawer_conf);
}

static void
_drawer_conf_item_free(Config_Item *ci)
{
   eina_stringshare_del(ci->id);
   eina_stringshare_del(ci->source);
   eina_stringshare_del(ci->view);
   E_FREE(ci);
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
	Instance *inst;

        /* Nothing passed, create a new id */
        snprintf(buf, sizeof(buf), "%s.%d", _drawer_gc_class.name, ++uuid);
	/* Check whether the newly generated id is taken */
	EINA_LIST_FOREACH(instances, l, inst)
	  {
	     if ((inst) && (inst->conf_item->id) && (!strcmp(inst->conf_item->id, buf)))
	       return _drawer_conf_item_get(NULL);
	  }
        id = buf;
     }
   else 
     {
	EINA_LIST_FOREACH(drawer_conf->conf_items, l, ci)
	   if ((ci->id) && (!strcmp(ci->id, id))) return ci;
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
_drawer_source_update_cb(void *data __UNUSED__, int ev_type, void *event)
{
   Instance *inst = NULL;
   Drawer_Event_Source_Update *ev;
   int visible = 0;

   ev = event;
   if (ev_type != DRAWER_EVENT_SOURCE_UPDATE) return 1;
   if (!(inst = _drawer_instance_get(_drawer_conf_item_get(ev->id)))) return 1;

   if (inst->is_floating)
     {
	if (inst->view)
	  _drawer_container_update(inst);
     }
   else if (inst->popup)
     _drawer_popup_update(inst);

   return 1;
}

static int
_drawer_view_activate_cb(void *data __UNUSED__, int ev_type, void *event)
{
   Drawer_View *v = NULL;
   Drawer_Event_View_Activate *ev = NULL;
   Eina_List *l = NULL;
   Instance *inst = NULL;

   ev = event;
   if (ev_type != DRAWER_EVENT_VIEW_ITEM_ACTIVATE) return 1;
   if (!(inst = _drawer_instance_get(_drawer_conf_item_get(ev->id)))) return 1;
   v = ev->view;

   DRAWER_SOURCE(inst->source)->func.activate(DRAWER_SOURCE(inst->source), ev->data, inst->gcc->gadcon->zone);

   if (inst->popup)
     _drawer_popup_hide(inst);

   return 0;
}

static int
_drawer_thumbnail_done_cb(void *data __UNUSED__, int ev_type, void *event)
{
   Epsilon_Request *ev;
   Drawer_Epsilon_Data *ep;
   Evas_Object *o = NULL;
   Evas *evas;

   if (!(ev = event) || !(ep = ev->data)) return 1;

   evas = evas_object_evas_get(ep->o_icon);
   if (ev->dest)
     {
	o = evas_object_image_add(evas);
	evas_object_image_size_set(o, ep->w, ep->h);
	evas_object_image_load_size_set(o, ep->w, ep->h);
	evas_object_image_fill_set(o, 0, 0, ep->w, ep->h);
	evas_object_image_file_set(o, ev->dest, NULL);
     }
   else if (ev->path)
     {
	const char *mime = NULL;

	mime = efreet_mime_globs_type_get(ev->path);
	if (mime)
	  {
	     const char *icon;

	     icon = e_fm_mime_icon_get(mime);
	     if (!icon)
	       {
		  o = edje_object_add(evas);
		  e_theme_edje_object_set(o, "base/theme/fileman", "e/icons/fileman/file");
	       }
	     else if (!strncmp(icon, "e/icons/fileman/mime/", 21))
	       {
		  o = edje_object_add(evas);
		  if (e_theme_edje_object_set(o, "base/theme/fileman", icon))
		    {
		       e_theme_edje_object_set(o, "base/theme/fileman", "e/icons/fileman/file");
		    }
	       }
	     else
	       {
		  char *p;

		  p = strrchr(icon, '.');
		  if ((p) && (!strcmp(p, ".edj")))
		    {
		       o = edje_object_add(evas);
		       if (!edje_object_file_set(o, icon, "icon"))
			 e_theme_edje_object_set(o, "base/theme/fileman", "e/icons/fileman/file");
		    }
		  else
		    {
		       o = e_icon_add(evas);
		       e_icon_file_set(o, icon);
		       e_icon_fill_inside_set(o, 1);
		    }
	       }
	  }
     }

   if (o)
     {
	if (!e_theme_edje_object_set(ep->o_icon, "base/theme/modules/drawer", 
				     "modules/drawer/icon/thumbnail"))
	  {
	     char buf[4096];

	     snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", 
		      drawer_conf->module->dir);
	     edje_object_file_set(ep->o_icon, buf, "modules/drawer/icon/thumbnail");
	  }
	edje_object_part_swallow(ep->o_icon, "e.swallow.content", o);
     }

   free(ep);
   ev->data = NULL;

   return 1;
}

static void
_drawer_popup_hidden_cb(void *data, Evas_Object *obj __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Instance *inst = NULL;

   inst = data;
   e_gadcon_popup_hide(inst->popup);
   e_shelf_locked_set(inst->gcc->gadcon->shelf, 0);

   inst->pop_hiding = EINA_FALSE;
   if (inst->pop_update)
     _drawer_popup_update(inst);
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
	if (inst->pop_hiding) return;
	if (!inst->popup) _drawer_popup_create(inst);
	if (inst->popup->win->visible)
	  _drawer_popup_hide(inst);
	else
	  _drawer_popup_show(inst);
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
   Drawer_Content_Margin margin = {0, 0, 0, 0};
   int cw, ch, px, py, pw, ph, ew, eh;

   inst = evas_object_data_get(obj, "drawer_popup_data");
   v = DRAWER_VIEW(inst->view);
   if (!v->func.content_size_get) return;

   edje_object_part_geometry_get(inst->popup->o_bg, "e.swallow.content", &px, &py, &pw, &ph);
   evas_object_geometry_get(inst->popup->o_bg, NULL, NULL, &ew, &eh);
   margin.top = py + 10; margin.right = ew - px - pw + 10;
   margin.bottom = eh - py - ph + 10; margin.left = px + 10;
   v->func.content_size_get(v, inst->popup->gcc, &margin, &cw, &ch);

   if (cw) *w = cw;
   if (ch) *h = ch;
}
