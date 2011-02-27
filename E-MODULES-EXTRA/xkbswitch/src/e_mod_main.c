#include <e.h>
#include "e_mod_main.h"
#include "e_mod_parse.h"

/* Local Function Prototypes */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);

static void _xkbswitch_conf_new(void);
static void _xkbswitch_conf_free(void);
static Eina_Bool _xkbswitch_conf_timer(void *data);
static Config_Item *_xkbswitch_conf_item_get(const char *id);
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

    /* Config_Item structure. Every gadget should have one :) */
    Config_Item *conf_item;
} Instance;

/* Local Variables */
static int uuid = 0;
static Eina_List *instances = NULL;
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;
Config *xkbswitch_conf = NULL;

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
        NULL, buf, e_int_config_xkbswitch_module
    );

    /* Define EET Data Storage for the config file */
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
    xkbswitch_conf = e_config_domain_load("module.xkbswitch", conf_edd);
    if (xkbswitch_conf) 
    {
        /* Check config version */
        if ((xkbswitch_conf->version >> 16) < MOD_CONFIG_FILE_EPOCH) 
        {
            /* config too old */
            _xkbswitch_conf_free();
            ecore_timer_add(1.0, _xkbswitch_conf_timer,
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
        else if (xkbswitch_conf->version > MOD_CONFIG_FILE_VERSION) 
        {
            /* config too new...wtf ? */
            _xkbswitch_conf_free();
            ecore_timer_add(1.0, _xkbswitch_conf_timer, 
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
    if (!xkbswitch_conf) _xkbswitch_conf_new();

    /* create a link from the modules config to the module
     * this is not written */
    xkbswitch_conf->module = m;

    /* Tell any gadget containers (shelves, etc) that we provide a module
     * for the user to enjoy */
    e_gadcon_provider_register(&_gc_class);

    /* Fill the lists */
    parse_rules("/usr/local/share/X11/xkb/rules/xorg.xml");

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
    if (xkbswitch_conf->cfd) e_object_del(E_OBJECT(xkbswitch_conf->cfd));
    xkbswitch_conf->cfd = NULL;

    /* Tell E the module is now unloaded. Gets removed from shelves, etc. */
    xkbswitch_conf->module = NULL;
    e_gadcon_provider_unregister(&_gc_class);

    /* Cleanup our item list */
    while (xkbswitch_conf->conf_items) 
    {
        Config_Item *ci = NULL;

        /* Grab an item from the list */
        ci = xkbswitch_conf->conf_items->data;

        /* remove it */
        xkbswitch_conf->conf_items = 
          eina_list_remove_list(xkbswitch_conf->conf_items, 
                                xkbswitch_conf->conf_items);

        /* cleanup stringshares */
        if (ci->id) eina_stringshare_del(ci->id);

        /* keep the planet green */
        E_FREE(ci);
    }

    /* Cleanup the main config structure */
    E_FREE(xkbswitch_conf);

    /* Clean EET */
    E_CONFIG_DD_FREE(conf_item_edd);
    E_CONFIG_DD_FREE(conf_edd);

    /* Clean lists */
    clear_rules();

    return 1;
}

EAPI int e_modapi_save(E_Module *m) 
{
   e_config_domain_save("module.xkbswitch", conf_edd, xkbswitch_conf);
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
        xkbswitch_conf->module->dir
    );

    /* New visual instance, any config ? */
    inst = E_NEW(Instance, 1);
    inst->conf_item = _xkbswitch_conf_item_get(id);

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
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class) 
{
    Config_Item *ci = NULL;

    ci = _xkbswitch_conf_item_get(NULL);
    return ci->id;
}

static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas) 
{
    Evas_Object *o = NULL;
    char buf[4096];

    /* theme */
    snprintf(buf, sizeof(buf), "%s/e-module-xkbswitch.edj", xkbswitch_conf->module->dir);

    /* create icon object */
    o = edje_object_add(evas);

    /* load icon from theme */
    edje_object_file_set(o, buf, "icon");

    return o;
}

/* new module needs a new config :), or config too old and we need one anyway */
static void _xkbswitch_conf_new(void) 
{
    Config_Item *ci = NULL;
    char buf[128];

    xkbswitch_conf = E_NEW(Config, 1);
    xkbswitch_conf->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) if ((xkbswitch_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

    /* setup defaults */
    IFMODCFG(0x008d);
    xkbswitch_conf->switch1 = 1;
    _xkbswitch_conf_item_get(NULL);
    IFMODCFGEND;

    /* update the version */
    xkbswitch_conf->version = MOD_CONFIG_FILE_VERSION;

    /* setup limits on the config properties here (if needed) */

    /* save the config to disk */
    e_config_save_queue();
}

/* This is called when we need to cleanup the actual configuration,
 * for example when our configuration is too old */
static void _xkbswitch_conf_free(void) 
{
    /* cleanup any stringshares here */
    while (xkbswitch_conf->conf_items) 
    {
        Config_Item *ci = NULL;

        ci = xkbswitch_conf->conf_items->data;
        xkbswitch_conf->conf_items = 
          eina_list_remove_list(xkbswitch_conf->conf_items, 
                                xkbswitch_conf->conf_items);
        /* EPA */
        if (ci->id) eina_stringshare_del(ci->id);
        E_FREE(ci);
    }

    E_FREE(xkbswitch_conf);
}

/* timer for the config oops dialog (old configuration needs update) */
static Eina_Bool _xkbswitch_conf_timer(void *data) 
{
    e_util_dialog_internal( D_("XKB Switcher Configuration Updated"), data);
    return EINA_FALSE;
}

/* function to search for any Config_Item struct for this Item
 * create if needed */
static Config_Item *_xkbswitch_conf_item_get(const char *id) 
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
        uuid++;
        for (l = xkbswitch_conf->conf_items; l; l = l->next) 
        {
            if (!(ci = l->data)) continue;
            if ((ci->id) && (!strcmp(ci->id, id))) return ci;
        }
    }
    ci = E_NEW(Config_Item, 1);
    ci->id = eina_stringshare_add(id);
    ci->switch2 = 0;
    xkbswitch_conf->conf_items = eina_list_append(xkbswitch_conf->conf_items, ci);
    return ci;
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
        E_Menu *ma, *mg;

        /* grab current zone */
        zone = e_util_zone_current_get(e_manager_current_get());

        /* create popup menu */
        ma = e_menu_new();
        e_menu_post_deactivate_callback_set(ma, _xkbswitch_cb_menu_post, inst);
        inst->menu = ma;

        mg = e_menu_new();

        mi = e_menu_item_new(mg);
        e_menu_item_label_set(mi, D_("Settings"));
        e_util_menu_item_theme_icon_set(mi, "preferences-system");
        e_menu_item_callback_set(mi, _xkbswitch_cb_menu_configure, NULL);

        /* Each Gadget Client has a utility menu from the Container */
        e_gadcon_client_util_menu_items_append(inst->gcc, ma, mg, 0);
        e_gadcon_canvas_zone_geometry_get(
            inst->gcc->gadcon, &x, &y, 
            NULL, NULL
        );

        /* show the menu relative to gadgets position */
        e_menu_activate_mouse(
            ma, zone, (x + ev->output.x), 
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
    if (!xkbswitch_conf) return;
    if (xkbswitch_conf->cfd) return;
    e_int_config_xkbswitch_module(mn->zone->container, NULL);
}
