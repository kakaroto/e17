#include <e.h>
#include "e_mod_main.h"

/* Gadcon Protos */
static E_Gadcon_Client *_gc_init     (E_Gadcon * gc, const char *name, 
				      const char *id, const char *style);
static void             _gc_shutdown (E_Gadcon_Client * gcc);
static void             _gc_orient   (E_Gadcon_Client * gcc);
static char            *_gc_label    (void);
static Evas_Object     *_gc_icon     (Evas * evas);

/* Module Protos */
static Config_Item *_mixer_config_item_get   (const char *id);
static void         _mixer_menu_cb_post      (void *data, E_Menu *m);
static void         _mixer_cb_mouse_down     (void *data, Evas *e, 
					      Evas_Object *obj, 
					      void *event_info);
static void         _mixer_menu_cb_configure (void *data, E_Menu *m, 
					      E_Menu_Item *mi);
static void         _mixer_window_show       (void *data, int simple);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *mixer_config = NULL;

static const E_Gadcon_Client_Class _gc_class =
{
   GADCON_CLIENT_CLASS_VERSION, "mixer",
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon},
   E_GADCON_CLIENT_STYLE_PLAIN
};

typedef struct _Instance Instance;
typedef struct _Mixer Mixer;
typedef struct _Mixer_Win Mixer_Win;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Mixer           *mixer;
};

struct _Mixer
{
   Instance    *inst;
   Evas        *evas;
   Mixer_Win   *win;
   
   Evas_Object *base;
};

struct _Mixer_Win 
{
   Mixer       *mixer;
   E_Win       *window;

   Evas_Object *bg_obj;
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Instance        *inst;
   Config_Item     *ci;
   Mixer           *mixer;
   E_Gadcon_Client *gcc;
   char             buf[4096];
   
   inst = E_NEW(Instance, 1);
   if (!inst) return NULL;
   
   ci = _mixer_config_item_get(id);
   if (!ci->id) ci->id = evas_stringshare_add(id);
   
   mixer = E_NEW(Mixer, 1);
   if (!mixer) return NULL;
   mixer->inst = inst;
   mixer->evas = gc->evas;
   inst->mixer = mixer;

   snprintf(buf, sizeof(buf), "%s/mixer.edj", 
	    e_module_dir_get(mixer_config->module));

   mixer->base = edje_object_add(gc->evas);
   edje_object_file_set(mixer->base, buf, "e/modules/mixer/main");
   evas_object_show(mixer->base);
   
   gcc = e_gadcon_client_new(gc, name, id, style, mixer->base);
   gcc->data = inst;
   inst->gcc = gcc;

   evas_object_event_callback_add(mixer->base, EVAS_CALLBACK_MOUSE_DOWN, 
				  _mixer_cb_mouse_down, inst);
   
   mixer_config->instances = evas_list_append(mixer_config->instances, inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;
   Mixer    *mixer;
   
   inst = gcc->data;
   if (!inst) return;
   mixer = inst->mixer;
   if (!mixer) return;

   if (mixer->win) 
     {
	if (mixer->win->bg_obj) evas_object_del(mixer->win->bg_obj);
	e_object_del(E_OBJECT(mixer->win->window));
	E_FREE(mixer->win);
     }
   
   if (mixer->base) evas_object_del(mixer->base);
   
   mixer_config->instances = evas_list_remove(mixer_config->instances, inst);
   E_FREE(mixer);
   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc)
{
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(void)
{
   return _("Mixer");
}

static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;
   char         buf[4096];

   snprintf (buf, sizeof(buf), "%s/module.eap",
	     e_module_dir_get(mixer_config->module));

   o = edje_object_add(evas);
   edje_object_file_set(o, buf, "icon");
   return o;
}

static void
_mixer_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance              *inst;
   Evas_Event_Mouse_Down *ev;

   inst = data;
   if (!inst) return;
   
   ev = event_info;
   if ((ev->button == 3) && (!mixer_config->menu))
     {
	E_Menu      *mn;
	E_Menu_Item *mi;
	E_Zone      *zone;
	int          x, y, w, h;

	zone = e_util_zone_current_get(e_manager_current_get());
	
	mn = e_menu_new();
	e_menu_post_deactivate_callback_set(mn, _mixer_menu_cb_post, inst);
	mixer_config->menu = mn;

	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, _("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	// e_menu_item_callback_set (mi, _mixer_menu_cb_configure, inst);

	mi = e_menu_item_new(mn);
	e_menu_item_separator_set(mi, 1);

	e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
	e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, &w, &h);
	e_menu_activate_mouse(mn,zone, x + ev->output.x, y + ev->output.y, 
			      1, 1, E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
				 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
   else if (ev->button == 1) 
     {
	if (ev->flags == EVAS_BUTTON_DOUBLE_CLICK) 
	  {
	     /* Call full mixer window */
	  }
	else if (ev->flags == EVAS_BUTTON_NONE)
	  {
	     /* Call a simple window */
	     _mixer_window_show(inst, 0);
	  }
     }
}

static void
_mixer_menu_cb_post(void *data, E_Menu *m)
{
   if (!mixer_config->menu) return;
   e_object_del(E_OBJECT(mixer_config->menu));
   mixer_config->menu = NULL;
}

static Config_Item *
_mixer_config_item_get(const char *id)
{
   Evas_List   *l;
   Config_Item *ci;

   for (l = mixer_config->items; l; l = l->next)
     {
	ci = l->data;
	if (!ci->id) continue;
	if (!strcmp(ci->id, id)) return ci;
     }

   ci = E_NEW(Config_Item, 1);
   ci->id = evas_stringshare_add(id);
   ci->volume = 10;

   mixer_config->items = evas_list_append(mixer_config->items, ci);
   return ci;
}

EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION, "Mixer"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   conf_item_edd = E_CONFIG_DD_NEW("Mixer_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, volume, INT);

   conf_edd = E_CONFIG_DD_NEW("Mixer_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   mixer_config = e_config_domain_load("module.mixer", conf_edd);
   if (!mixer_config)
     {
	Config_Item *ci;

	mixer_config = E_NEW(Config, 1);

	ci = E_NEW(Config_Item, 1);
	ci->id = evas_stringshare_add("0");
	ci->volume = 10;

	mixer_config->items = evas_list_append(mixer_config->items, ci);
     }
   mixer_config->module = m;

   e_gadcon_provider_register(&_gc_class);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   mixer_config->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);

   if (mixer_config->config_dialog)
     e_object_del(E_OBJECT(mixer_config->config_dialog));

   if (mixer_config->menu)
     {
	e_menu_post_deactivate_callback_set(mixer_config->menu, NULL, NULL);
	e_object_del(E_OBJECT(mixer_config->menu));
	mixer_config->menu = NULL;
     }

   while (mixer_config->items)
     {
	Config_Item *ci;

	ci = mixer_config->items->data;
	mixer_config->items = evas_list_remove_list(mixer_config->items, 
						    mixer_config->items);
	if (ci->id) evas_stringshare_del(ci->id);
	E_FREE(ci);
     }

   E_FREE(mixer_config);
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   Evas_List *l;

   for (l = mixer_config->instances; l; l = l->next)
     {
	Instance *inst;
	Config_Item *ci;

	inst = l->data;
	ci = _mixer_config_item_get(inst->gcc->id);
	if (ci->id) evas_stringshare_del(ci->id);
	ci->id = evas_stringshare_add(inst->gcc->id);
     }
   e_config_domain_save("module.mixer", conf_edd, mixer_config);
   return 1;
}

EAPI int
e_modapi_about(E_Module *m)
{
   e_module_dialog_show(m, _("Mixer"), 
			_("Mixer module lets you change volume."));
   return 1;
}

static void 
_mixer_window_show(void *data, int simple) 
{
   /* Use simple to determine if we are showing a full mixer, 
    * or just a simple Master/PCM slider */
   
   Instance    *inst;
   Mixer       *mixer;
   
   inst = data;
   if (!inst) return;
   
   mixer = inst->mixer;
   if (!mixer) return;

}
