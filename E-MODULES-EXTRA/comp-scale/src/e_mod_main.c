#include <e.h>
#include "e_mod_main.h"

//TODO
//~ handle add, delete border
//~ how to handle/disable possible desk show events
//  while scale is active?
//

static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);

static void _scale_conf_new(void);
static void _scale_conf_free(void);
static Config_Item *_scale_conf_item_get(const char *id);
static void _scale_gc_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _scale_gc_cb_menu_post(void *data, E_Menu *menu);
static void _scale_gc_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);

static E_Action *act = NULL;

static void
_e_mod_action(const char *params)
{
   if (!params) return;

   if (!strncmp(params, "go_pager", 8))
     {
	pager_run(params);
     }
   else if (!strncmp(params, "go_scale", 8))
     {
	scale_run(params);
     }
}

static void
_e_mod_action_cb_edge(E_Object *obj, const char *params, E_Event_Zone_Edge *ev)
{
   _e_mod_action(params);
}

static void
_e_mod_action_cb(E_Object *obj, const char *params)
{
   _e_mod_action(params);
}

/* Module and Gadcon stuff */

typedef struct _Instance Instance;

struct _Instance
{
  E_Gadcon_Client *gcc;
  Evas_Object *o_scale;
  E_Menu *menu;
  Config_Item *conf_item;
};

static int uuid = 0;
static Eina_List *instances = NULL;
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;
Config *scale_conf = NULL;

static const E_Gadcon_Client_Class _gc_class =
  {
    GADCON_CLIENT_CLASS_VERSION, "scale",
    {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon,
     _gc_id_new, NULL, NULL},
    E_GADCON_CLIENT_STYLE_PLAIN
  };

EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "Scale"};

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   snprintf(buf, sizeof(buf), "%s/e-module-scale.edj", m->dir);

   e_configure_registry_category_add("appearance", 80, D_("Look"),
                                     NULL, "preferences-look");
   e_configure_registry_item_add("appearance/comp-scale", 110, D_("Composite Scale Windows"),
                                 NULL, buf, e_int_config_scale_module);

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
   E_CONFIG_VAL(D, T, fade_popups, UCHAR);
   E_CONFIG_VAL(D, T, fade_desktop, UCHAR);

   E_CONFIG_VAL(D, T, layout_mode, INT);
   E_CONFIG_VAL(D, T, grow, UCHAR);
   E_CONFIG_VAL(D, T, tight, UCHAR);
   E_CONFIG_VAL(D, T, scale_duration, DOUBLE);
   E_CONFIG_VAL(D, T, spacing, DOUBLE);

   E_CONFIG_VAL(D, T, desks_layout_mode, INT);
   E_CONFIG_VAL(D, T, desks_grow, UCHAR);
   E_CONFIG_VAL(D, T, desks_tight, UCHAR);
   E_CONFIG_VAL(D, T, desks_duration, DOUBLE);
   E_CONFIG_VAL(D, T, desks_spacing, DOUBLE);
   E_CONFIG_VAL(D, T, fade_windows, UCHAR);

   E_CONFIG_VAL(D, T, pager_duration, DOUBLE);
   E_CONFIG_VAL(D, T, pager_fade_windows, UCHAR);
   E_CONFIG_VAL(D, T, pager_fade_popups, UCHAR);
   E_CONFIG_VAL(D, T, pager_fade_desktop, UCHAR);

   E_CONFIG_LIST(D, T, conf_items, conf_item_edd);

   scale_conf = e_config_domain_load("module.scale", conf_edd);
   if (scale_conf)
     {
	if (!e_util_module_config_check(D_("Scale Windows"),
					scale_conf->version,
					MOD_CONFIG_FILE_EPOCH,
					MOD_CONFIG_FILE_VERSION))
	  _scale_conf_free();
     }

   if (!scale_conf) _scale_conf_new();

   scale_conf->module = m;

   snprintf(buf, sizeof(buf), "%s/e-module-scale.edj",
            scale_conf->module->dir);
   scale_conf->theme_path = eina_stringshare_add(buf);

   e_gadcon_provider_register(&_gc_class);

   act = e_action_add("scale-windows");
   if (act)
     {
	act->func.go = _e_mod_action_cb;
	act->func.go_edge = _e_mod_action_cb_edge;
	e_action_predef_name_set(D_("Desktop"), D_("Scale Windows"),
				 "scale-windows", "go_scale", NULL, 0);
	e_action_predef_name_set(D_("Desktop"), D_("Scale Windows (All Desktops)"),
				 "scale-windows", "go_scale_all", NULL, 0);
	e_action_predef_name_set(D_("Desktop"), D_("Scale Pager"),
				 "scale-windows", "go_pager", NULL, 0);
     }

   return m;
}


EAPI int
e_modapi_shutdown(E_Module *m)
{
   e_configure_registry_item_del("appearance/comp-scale");
   e_configure_registry_category_del("appearance");

   if (scale_conf->cfd) e_object_del(E_OBJECT(scale_conf->cfd));
   scale_conf->cfd = NULL;

   scale_conf->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);

   while (scale_conf->conf_items)
     {
        Config_Item *ci = NULL;
        ci = scale_conf->conf_items->data;

        scale_conf->conf_items =
          eina_list_remove_list(scale_conf->conf_items,
                                scale_conf->conf_items);

        if (ci->id) eina_stringshare_del(ci->id);
        E_FREE(ci);
     }

   eina_stringshare_del(scale_conf->theme_path);
   E_FREE(scale_conf);

   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);

   if (act)
     {
	e_action_predef_name_del(D_("Desktop"), D_("Scale Windows"));
	e_action_predef_name_del(D_("Desktop"), D_("Scale Windows (All Desktops)"));
	e_action_predef_name_del(D_("Desktop"), D_("Scale Pager"));
	e_action_del("scale-windows");
     }

   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save("module.scale", conf_edd, scale_conf);
   return 1;
}

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Instance *inst = NULL;

   inst = E_NEW(Instance, 1);
   inst->conf_item = _scale_conf_item_get(id);

   inst->o_scale = edje_object_add(gc->evas);

   if (!e_theme_edje_object_set(inst->o_scale, "base/theme/modules/scale",
                                "modules/scale/main"))
     edje_object_file_set(inst->o_scale, scale_conf->theme_path, "modules/scale/main");

   inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->o_scale);
   inst->gcc->data = inst;

   evas_object_event_callback_add(inst->o_scale, EVAS_CALLBACK_MOUSE_DOWN,
                                  _scale_gc_cb_mouse_down, inst);

   instances = eina_list_append(instances, inst);

   return inst->gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst = NULL;

   if (!(inst = gcc->data)) return;
   instances = eina_list_remove(instances, inst);

   if (inst->menu)
     {
        e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
        e_object_del(E_OBJECT(inst->menu));
        inst->menu = NULL;
     }
   if (inst->o_scale)
     {
        evas_object_event_callback_del(inst->o_scale, EVAS_CALLBACK_MOUSE_DOWN,
                                       _scale_gc_cb_mouse_down);
        evas_object_del(inst->o_scale);
     }
   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(E_Gadcon_Client_Class *client_class)
{
   return D_("Scale");
}

static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class)
{
   Config_Item *ci = NULL;

   ci = _scale_conf_item_get(NULL);
   return ci->id;
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas)
{
   Evas_Object *o = NULL;

   o = edje_object_add(evas);

   edje_object_file_set(o, scale_conf->theme_path, "icon");

   return o;
}

static void
_scale_conf_new(void)
{
   scale_conf = E_NEW(Config, 1);
   scale_conf->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) if ((scale_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

   IFMODCFG(0x0001);
   scale_conf->grow = 1;
   scale_conf->tight = 1;
   scale_conf->scale_duration = 0.4;
   scale_conf->spacing = 10;
   scale_conf->fade_windows = 1;
   scale_conf->fade_popups = 0;
   scale_conf->fade_desktop = 1;
   scale_conf->desks_duration = 0.6;
   scale_conf->desks_spacing = 44;
   scale_conf->layout_mode = 0;
   scale_conf->desks_layout_mode = 1;
   scale_conf->pager_duration = 0.4;
   scale_conf->pager_fade_popups = 1;
   scale_conf->pager_fade_desktop = 0;
   scale_conf->pager_fade_windows = 1;
   _scale_conf_item_get(NULL);
   IFMODCFGEND;

   scale_conf->version = MOD_CONFIG_FILE_VERSION;

   e_config_save_queue();
}

static void
_scale_conf_free(void)
{
   while (scale_conf->conf_items)
     {
        Config_Item *ci = NULL;

        ci = scale_conf->conf_items->data;
        scale_conf->conf_items =
          eina_list_remove_list(scale_conf->conf_items,
                                scale_conf->conf_items);
        if (ci->id) eina_stringshare_del(ci->id);
        E_FREE(ci);
     }

   E_FREE(scale_conf);
}

static Config_Item *
_scale_conf_item_get(const char *id)
{
   Eina_List *l = NULL;
   Config_Item *ci = NULL;
   char buf[128];

   if (!id)
     {
        snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, ++uuid);
        id = buf;
     }
   else
     {
        uuid++;
        for (l = scale_conf->conf_items; l; l = l->next)
          {
             if (!(ci = l->data)) continue;
             if ((ci->id) && (!strcmp(ci->id, id))) return ci;
          }
     }
   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->switch2 = 0;
   scale_conf->conf_items = eina_list_append(scale_conf->conf_items, ci);
   return ci;
}

static void
_scale_gc_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event)
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
	_e_mod_action_cb(NULL, NULL);
     }
   else if (ev->button == 2)
     {
	_e_mod_action_cb(NULL, "show_all_desks");
     }
   else if ((ev->button == 3) && (!inst->menu))
     {
        E_Menu *ma, *mg;

        zone = e_util_zone_current_get(e_manager_current_get());

        ma = e_menu_new();
        e_menu_post_deactivate_callback_set(ma, _scale_gc_cb_menu_post, inst);
        inst->menu = ma;

        mg = e_menu_new();

        mi = e_menu_item_new(mg);
        e_menu_item_label_set(mi, D_("Settings"));
        e_util_menu_item_theme_icon_set(mi, "preferences-system");
        e_menu_item_callback_set(mi, _scale_gc_cb_menu_configure, NULL);

        e_gadcon_client_util_menu_items_append(inst->gcc, ma, mg, 0);
        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y,
                                          NULL, NULL);

        e_menu_activate_mouse(ma, zone, (x + ev->output.x),
                              (y + ev->output.y), 1, 1,
                              E_MENU_POP_DIRECTION_AUTO, ev->timestamp);
        evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
                                 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

static void
_scale_gc_cb_menu_post(void *data, E_Menu *menu)
{
   Instance *inst = NULL;

   if (!(inst = data)) return;
   if (!inst->menu) return;
   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

static void
_scale_gc_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi)
{
   if (!scale_conf) return;
   if (scale_conf->cfd) return;
   e_int_config_scale_module(mn->zone->container, NULL);
}

