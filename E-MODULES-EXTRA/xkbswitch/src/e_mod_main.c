#include <e.h>
#include "e_mod_main.h"
#include "e_mod_parse.h"

/* Local Function Prototypes */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class __UNUSED__);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);

static void _e_xkb_cfg_new(void);
static void _e_xkb_cfg_free(void);
static Eina_Bool _e_xkb_cfg_timer(void *data);
static void _xkbswitch_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _xkbswitch_cb_menu_post(void *data, E_Menu *menu);
static void _xkbswitch_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);

/* Local Structures */
typedef struct _Instance 
{
    /* An instance of our item (module) with its elements */

    /* pointer to this gadget's container */
    E_Gadcon_Client *gcc;

    /* evas_object used to display */
    Evas_Object *o_xkbswitch;

    /* popup anyone ? */
    E_Menu *menu;
} Instance;

/* Local Variables */
static int uuid = 0;
static Eina_List *instances = NULL;
static E_Config_DD *e_xkb_cfg_edd = NULL;
static E_Config_DD *e_xkb_cfg_layout_edd = NULL;
e_xkb_cfg *e_xkb_cfg_inst = NULL;

static const E_Gadcon_Client_Class _gc_class = 
{
    GADCON_CLIENT_CLASS_VERSION, "xkbswitch", 
    {
        _gc_init,
        _gc_shutdown,
        _gc_orient,
        _gc_label,
        _gc_icon,
        _gc_id_new,
        NULL, NULL
    },
    E_GADCON_CLIENT_STYLE_PLAIN
};

/* We set the version and the name, check e_mod_main.h for more details */
EAPI E_Module_Api e_modapi = { E_MODULE_API_VERSION, "XKB Switcher" };

EAPI void *e_modapi_init(E_Module *m) 
{
    char buf[4096];

    /* Location of message catalogs for localization */
    snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
    bindtextdomain(PACKAGE, buf);
    bind_textdomain_codeset(PACKAGE, "UTF-8");

    /* Location of theme to load for this module */
    snprintf(buf, sizeof(buf), "%s/e-module-xkbswitch.edj", m->dir);

    /* Display this Modules config info in the main Config Panel */

    e_configure_registry_category_add(
        "keyboard_and_mouse", 80, D_("Input"), 
        NULL, "preferences-behavior"
    );
    e_configure_registry_item_add(
        "keyboard_and_mouse/xkbswitch",
        110, D_("XKB Switcher"), 
        NULL, buf, e_xkb_cfg_dialog
    );

    /* Define EET Data Storage for the config file */
    e_xkb_cfg_layout_edd = E_CONFIG_DD_NEW("e_xkb_cfg_layout", e_xkb_cfg_layout);
    #undef T
    #undef D
    #define T e_xkb_cfg_layout
    #define D e_xkb_cfg_layout_edd
    E_CONFIG_VAL(D, T, name,    STR);
    E_CONFIG_VAL(D, T, model,   STR);
    E_CONFIG_VAL(D, T, variant, STR);

    e_xkb_cfg_edd = E_CONFIG_DD_NEW("e_xkb_cfg", e_xkb_cfg);
    #undef T
    #undef D
    #define T e_xkb_cfg
    #define D e_xkb_cfg_edd
    E_CONFIG_VAL(D, T, version, INT);
    E_CONFIG_LIST(D, T, used_layouts, e_xkb_cfg_layout_edd);

    /* Tell E to find any existing module data. First run ? */
    e_xkb_cfg_inst = e_config_domain_load("module.xkbswitch", e_xkb_cfg_edd);
    if (e_xkb_cfg_inst) 
    {
        /* Check config version */
        if ((e_xkb_cfg_inst->version >> 16) < MOD_CONFIG_FILE_EPOCH) 
        {
            /* config too old */
            _e_xkb_cfg_free();
            ecore_timer_add(1.0, _e_xkb_cfg_timer,
                 D_("XKB Switcher Module Configuration data needed "
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
                 "liking. Sorry for the inconvenience.<br>")
            );
        }

        /* Ardvarks */
        else if (e_xkb_cfg_inst->version > MOD_CONFIG_FILE_VERSION) 
        {
            /* config too new...wtf ? */
            _e_xkb_cfg_free();
            ecore_timer_add(1.0, _e_xkb_cfg_timer, 
                D_("Your XKB Switcher Module configuration is NEWER "
                "than the module version. This is "
                "very<br>strange. This should not happen unless"
                " you downgraded<br>the module or "
                "copied the configuration from a place where"
                "<br>a newer version of the module "
                "was running. This is bad and<br>as a "
                "precaution your configuration has been now "
                "restored to<br>defaults. Sorry for the "
                "inconvenience.<br>")
            );
        }
    }

    /* if we don't have a config yet, or it got erased above, 
     * then create a default one */
    if (!e_xkb_cfg_inst) _e_xkb_cfg_new();

    /* create a link from the modules config to the module
     * this is not written */
    e_xkb_cfg_inst->module = m;

    /* Tell any gadget containers (shelves, etc) that we provide a module
     * for the user to enjoy */
    e_gadcon_provider_register(&_gc_class);

    /* Fill the lists */
    parse_rules("/usr/local/share/X11/xkb/rules/xorg.lst");

    /* Give E the module */
    return m;
}

EAPI int e_modapi_shutdown(E_Module *m) 
{
    /* Unregister the config dialog from the main panel */
    e_configure_registry_item_del("keyboard_and_mouse/xkbswitch");

    /* Remove the config panel category if we can. E will tell us.
     category stays if other items using it */
    e_configure_registry_category_del("keyboard_and_mouse");

    /* Kill the config dialog */
    if (e_xkb_cfg_inst->cfd) e_object_del(E_OBJECT(e_xkb_cfg_inst->cfd));
    e_xkb_cfg_inst->cfd = NULL;

    /* Tell E the module is now unloaded. Gets removed from shelves, etc. */
    e_xkb_cfg_inst->module = NULL;
    e_gadcon_provider_unregister(&_gc_class);

    while (e_xkb_cfg_inst->used_layouts)
    {
        e_xkb_cfg_layout *cl = NULL;

        cl = e_xkb_cfg_inst->used_layouts->data;

        e_xkb_cfg_inst->used_layouts =
            eina_list_remove_list(e_xkb_cfg_inst->used_layouts,
                                  e_xkb_cfg_inst->used_layouts);

        if (cl->name)    eina_stringshare_del(cl->name);
        if (cl->model)   eina_stringshare_del(cl->model);
        if (cl->variant) eina_stringshare_del(cl->variant);

        E_FREE(cl);
    }

    /* Cleanup the main config structure */
    E_FREE(e_xkb_cfg_inst);

    /* Clean EET */
    E_CONFIG_DD_FREE(e_xkb_cfg_layout_edd);
    E_CONFIG_DD_FREE(e_xkb_cfg_edd);

    /* Clean lists */
    clear_rules();

    return 1;
}

EAPI int e_modapi_save(E_Module *m) 
{
   e_config_domain_save("module.xkbswitch", e_xkb_cfg_edd, e_xkb_cfg_inst);
    return 1;
}

/* Locals */

/* Called when Gadget Controller (gadcon) says to appear in scene */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
    Instance *inst = NULL;
    char buf[4096];

    /* theme file */
    snprintf(
        buf, sizeof(buf), "%s/e-module-xkbswitch.edj", 
        e_xkb_cfg_inst->module->dir
    );

    /* New visual instance, any config ? */
    inst = E_NEW(Instance, 1);

    /* create on-screen object */
    inst->o_xkbswitch = edje_object_add(gc->evas);
    /* we have a theme ? */
    if (!e_theme_edje_object_set(inst->o_xkbswitch, "base/theme/modules/xkbswitch", 
                                "modules/xkbswitch/main"))
        edje_object_file_set(inst->o_xkbswitch, buf, "modules/xkbswitch/main");

    /* Start loading our module on screen via container */
    inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->o_xkbswitch);
    inst->gcc->data = inst;

    /* hook a mouse down. we want/have a popup menu, right ? */
    evas_object_event_callback_add(
        inst->o_xkbswitch,
        EVAS_CALLBACK_MOUSE_DOWN, 
        _xkbswitch_cb_mouse_down, inst
    );

    /* add to list of running instances so we can cleanup later */
    instances = eina_list_append(instances, inst);

    /* return the Gadget_Container Client */
    return inst->gcc;
}

/* Called when Gadget_Container says stop */
static void _gc_shutdown(E_Gadcon_Client *gcc) 
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
    if (inst->o_xkbswitch) 
    {
        /* remove mouse down callback hook */
        evas_object_event_callback_del(
            inst->o_xkbswitch,
            EVAS_CALLBACK_MOUSE_DOWN, 
            _xkbswitch_cb_mouse_down
        );
        evas_object_del(inst->o_xkbswitch);
    }
    E_FREE(inst);
}

/* For when container says we are changing position */
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient) 
{
    e_gadcon_client_aspect_set(gcc, 16, 16);
    e_gadcon_client_min_size_set(gcc, 16, 16);
}

/* Gadget/Module label, name for our module */
static char *_gc_label(E_Gadcon_Client_Class *client_class) 
{
    return D_("XKB Switcher");
}

/* so E can keep a unique instance per-container */
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class __UNUSED__) 
{
    return _gc_class.name;
}

static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas) 
{
    Evas_Object *o = NULL;
    char buf[4096];

    /* theme */
    snprintf(buf, sizeof(buf), "%s/e-module-xkbswitch.edj", e_xkb_cfg_inst->module->dir);

    /* create icon object */
    o = edje_object_add(evas);

    /* load icon from theme */
    edje_object_file_set(o, buf, "icon");

    return o;
}

/* new module needs a new config :), or config too old and we need one anyway */
static void _e_xkb_cfg_new(void) 
{
    char buf[128];

    e_xkb_cfg_inst = E_NEW(e_xkb_cfg, 1);
    e_xkb_cfg_inst->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) if ((e_xkb_cfg_inst->version & 0xffff) < v) {
#define IFMODCFGEND }

    /* setup defaults */
    IFMODCFG(0x008d);
    IFMODCFGEND;

    /* update the version */
    e_xkb_cfg_inst->version = MOD_CONFIG_FILE_VERSION;

    /* setup limits on the config properties here (if needed) */

    /* save the config to disk */
    e_config_save_queue();
}

/* This is called when we need to cleanup the actual configuration,
 * for example when our configuration is too old */
static void _e_xkb_cfg_free(void) 
{
    while (e_xkb_cfg_inst->used_layouts)
    {
        e_xkb_cfg_layout *cl = NULL;

        cl = e_xkb_cfg_inst->used_layouts->data;

        e_xkb_cfg_inst->used_layouts =
            eina_list_remove_list(e_xkb_cfg_inst->used_layouts,
                                  e_xkb_cfg_inst->used_layouts);

        if (cl->name)    eina_stringshare_del(cl->name);
        if (cl->model)   eina_stringshare_del(cl->model);
        if (cl->variant) eina_stringshare_del(cl->variant);

        E_FREE(cl);
    }

    E_FREE(e_xkb_cfg_inst);
}

/* timer for the config oops dialog (old configuration needs update) */
static Eina_Bool _e_xkb_cfg_timer(void *data) 
{
    e_util_dialog_internal( D_("XKB Switcher Configuration Updated"), data);
    return EINA_FALSE;
}

/* Pants On */
static void _xkbswitch_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event) 
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
        E_Menu *m;

        /* grab current zone */
        zone = e_util_zone_current_get(e_manager_current_get());

        /* create popup menu */
        m = e_menu_new();

        mi = e_menu_item_new(m);
        e_menu_item_label_set(mi, D_("Settings"));
        e_util_menu_item_theme_icon_set(mi, "preferences-system");
        e_menu_item_callback_set(mi, _xkbswitch_cb_menu_configure, NULL);

        /* Each Gadget Client has a utility menu from the Container */
        m = e_gadcon_client_util_menu_items_append(inst->gcc, m, 0);
        e_menu_post_deactivate_callback_set(m, _xkbswitch_cb_menu_post, inst);
        inst->menu = m;

        e_gadcon_canvas_zone_geometry_get(
            inst->gcc->gadcon, &x, &y, 
            NULL, NULL
        );

        /* show the menu relative to gadgets position */
        e_menu_activate_mouse(
            m, zone, (x + ev->output.x), 
            (y + ev->output.y), 1, 1, 
            E_MENU_POP_DIRECTION_AUTO, ev->timestamp
        );
        evas_event_feed_mouse_up(
            inst->gcc->gadcon->evas,
            ev->button, EVAS_BUTTON_NONE,
            ev->timestamp, NULL
        );
    }
}

/* popup menu closing, cleanup */
static void _xkbswitch_cb_menu_post(void *data, E_Menu *menu) 
{
    Instance *inst = NULL;

    if (!(inst = data)) return;
    if (!inst->menu) return;
    e_object_del(E_OBJECT(inst->menu));
    inst->menu = NULL;
}

/* call configure from popup */
static void _xkbswitch_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
    if (!e_xkb_cfg_inst) return;
    if (e_xkb_cfg_inst->cfd) return;
    e_xkb_cfg_dialog(mn->zone->container, NULL);
}
