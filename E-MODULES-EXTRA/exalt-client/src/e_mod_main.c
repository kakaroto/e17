#include "e_mod_main.h"

/* Local Function Prototypes */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);

static void _exalt_conf_new(void);
static void _exalt_conf_free(void);
static int _exalt_conf_timer(void *data);
static Config_Item *_exalt_conf_item_get(const char *id);
static void _exalt_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _exalt_cb_menu_post(void *data, E_Menu *menu);
static void _exalt_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);

/* Local Variables */
static int uuid = 0;
static Eina_List *instances = NULL;
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;
Config *exalt_conf = NULL;

static const E_Gadcon_Client_Class _gc_class =
{
    GADCON_CLIENT_CLASS_VERSION, "exalt",
    {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon,
        _gc_id_new, NULL},
    E_GADCON_CLIENT_STYLE_PLAIN
};

EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "exalt"};

/*
 * Module Functions
 */
    EAPI void *
e_modapi_init(E_Module *m)
{
    char buf[4096];
    char domain[4096];

    snprintf (domain, sizeof (domain), "%s/locale", m->dir);
    bindtextdomain (PACKAGE, domain);
    bind_textdomain_codeset (PACKAGE, "UTF-8");

    /* Location of theme to load for this module */
    snprintf(buf, sizeof(buf), "%s/e-module-exalt.edj", m->dir);

    /* Display this Modules config info in the main Config Panel */

    /* starts with a category */
    e_configure_registry_category_add("advanced", 80, "Advanced",
            NULL, "preferences-advanced");
    /* add right-side item */
    e_configure_registry_item_add("advanced/exalt", 110, "exalt",
            NULL, buf, e_int_config_exalt_module);

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
    exalt_conf = e_config_domain_load("module.exalt", conf_edd);
    if (exalt_conf)
    {
        /* Check config version */
        if ((exalt_conf->version >> 16) < MOD_CONFIG_FILE_EPOCH)
        {
            /* config too old */
            _exalt_conf_free();
            ecore_timer_add(1.0, _exalt_conf_timer,
                    "exalt Module Configuration data needed "
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
        else if (exalt_conf->version > MOD_CONFIG_FILE_VERSION)
        {
            /* config too new...wtf ? */
            _exalt_conf_free();
            ecore_timer_add(1.0, _exalt_conf_timer,
                    "Your exalt Module configuration is NEWER "
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
    if (!exalt_conf) _exalt_conf_new();

    /* create a link from the modules config to the module
     * this is not written */
    exalt_conf->module = m;

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
    e_configure_registry_item_del("advanced/exalt");

    /* Remove the config panel category if we can. E will tell us.
       category stays if other items using it */
    e_configure_registry_category_del("advanced");

    /* Kill the config dialog */
    if (exalt_conf->cfd) e_object_del(E_OBJECT(exalt_conf->cfd));
    exalt_conf->cfd = NULL;

    /* Tell E the module is now unloaded. Gets removed from shelves, etc. */
    exalt_conf->module = NULL;
    e_gadcon_provider_unregister(&_gc_class);

    /* Cleanup our item list */
    while (exalt_conf->conf_items)
    {
        Config_Item *ci = NULL;

        /* Grab an item from the list */
        ci = exalt_conf->conf_items->data;
        /* remove it */
        exalt_conf->conf_items =
            eina_list_remove_list(exalt_conf->conf_items,
                    exalt_conf->conf_items);
        /* cleanup stringshares */
        if (ci->id) eina_stringshare_del(ci->id);

        /* keep the planet green */
        E_FREE(ci);
    }

    /* Cleanup the main config structure */
    E_FREE(exalt_conf);

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
    e_config_domain_save("module.exalt", conf_edd, exalt_conf);
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
    snprintf(buf, sizeof(buf), "%s/e-module-exalt.edj",
            exalt_conf->module->dir);

    /* New visual instance, any config ? */
    inst = E_NEW(Instance, 1);
    inst->conf_item = _exalt_conf_item_get(id);

    /* create on-screen object */
    inst->o_exalt = edje_object_add(gc->evas);
    /* we have a theme ? */
    if (!e_theme_edje_object_set(inst->o_exalt, "base/theme/modules/exalt",
                "modules/exalt/main"))
        edje_object_file_set(inst->o_exalt, buf, "modules/exalt/main");

    /* Start loading our module on screen via container */
    inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->o_exalt);
    inst->gcc->data = inst;

    /* hook a mouse down. we want/have a popup menu, right ? */
    evas_object_event_callback_add(inst->o_exalt, EVAS_CALLBACK_MOUSE_DOWN,
            _exalt_cb_mouse_down, inst);

    /* add to list of running instances so we can cleanup later */
    instances = eina_list_append(instances, inst);

    inst-> l = NULL;
    exalt_dbus_init();
    inst->conn = exalt_dbus_connect();
    if(inst->conn)
    {
        exalt_dbus_response_notify_set(inst->conn,response_cb,inst);
        exalt_dbus_notify_set(inst->conn,notify_cb,inst);
        exalt_dbus_scan_notify_set(inst->conn,notify_scan_cb,inst);
    }

    if_wired_dialog_init(inst);
    if_network_dialog_init(inst);
    popup_init(inst);

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
    if (inst->sub_menu)
    {
        e_menu_post_deactivate_callback_set(inst->sub_menu, NULL, NULL);
        e_object_del(E_OBJECT(inst->sub_menu));
        inst->sub_menu = NULL;
    }
    /* delete the visual */
    if (inst->o_exalt)
    {
        /* remove mouse down callback hook */
        evas_object_event_callback_del(inst->o_exalt, EVAS_CALLBACK_MOUSE_DOWN,
                _exalt_cb_mouse_down);
        evas_object_del(inst->o_exalt);
    }

    exalt_dbus_free(&(inst->conn));
    exalt_dbus_shutdown();

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
    return D_("exalt");
}

/* so E can keep a unique instance per-container */
    static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class)
{
    Config_Item *ci = NULL;

    ci = _exalt_conf_item_get(NULL);
    return ci->id;
}

    static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas)
{
    Evas_Object *o = NULL;
    char buf[4096];

    /* theme */
    snprintf(buf, sizeof(buf), "%s/e-module-exalt.edj", exalt_conf->module->dir);

    /* create icon object */
    o = edje_object_add(evas);

    /* load icon from theme */
    edje_object_file_set(o, buf, "icon");

    return o;
}

/* new module needs a new config :), or config too old and we need one anyway */
    static void
_exalt_conf_new(void)
{
    Config_Item *ci = NULL;
    char buf[128];

    exalt_conf = E_NEW(Config, 1);
    exalt_conf->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) if ((exalt_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

    /* setup defaults */
    IFMODCFG(0x008d);
    exalt_conf->switch1 = 1;
    _exalt_conf_item_get(NULL);
    IFMODCFGEND;

    /* update the version */
    exalt_conf->version = MOD_CONFIG_FILE_VERSION;

    /* setup limits on the config properties here (if needed) */

    /* save the config to disk */
    e_config_save_queue();
}

    static void
_exalt_conf_free(void)
{
    /* cleanup any stringshares here */
    while (exalt_conf->conf_items)
    {
        Config_Item *ci = NULL;

        ci = exalt_conf->conf_items->data;
        exalt_conf->conf_items =
            eina_list_remove_list(exalt_conf->conf_items,
                    exalt_conf->conf_items);
        /* EPA */
        if (ci->id) eina_stringshare_del(ci->id);
        E_FREE(ci);
    }

    E_FREE(exalt_conf);
}

/* timer for the config oops dialog */
    static int
_exalt_conf_timer(void *data)
{
    e_util_dialog_show( D_("exalt Configuration Updated"), data);
    return 0;
}

/* function to search for any Config_Item struct for this Item
 * create if needed */
    static Config_Item *
_exalt_conf_item_get(const char *id)
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
        for (l = exalt_conf->conf_items; l; l = l->next)
        {
            if (!(ci = l->data)) continue;
            if ((ci->id) && (!strcmp(ci->id, id))) return ci;
        }
    }
    ci = E_NEW(Config_Item, 1);
    ci->id = eina_stringshare_add(id);
    ci->switch2 = 0;
    exalt_conf->conf_items = eina_list_append(exalt_conf->conf_items, ci);
    return ci;
}

/* Pants On */
    static void
_exalt_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event)
{
    Instance *inst = NULL;
    Evas_Event_Mouse_Down *ev;
    E_Zone *zone = NULL;
    E_Menu_Item *mi = NULL;
    int x, y;

    if (!(inst = data)) return;
    ev = event;

    if ((ev->button == 1) )
    {
        if(!inst->popup)
            popup_show(inst);
        else
            popup_hide(inst);
    }

    if ((ev->button == 3) && (!inst->menu))
    {
        /* grab current zone */
        zone = e_util_zone_current_get(e_manager_current_get());

        /* create popup menu */
        inst->menu = e_menu_new();
        e_menu_post_deactivate_callback_set(inst->menu, _exalt_cb_menu_post,
                inst);

        mi = e_menu_item_new(inst->menu);
        e_menu_item_label_set(mi, D_("Configuration"));
        e_util_menu_item_theme_icon_set(mi, "preferences-system");
        e_menu_item_callback_set(mi, _exalt_cb_menu_configure, NULL);

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
_exalt_cb_menu_post(void *data, E_Menu *menu)
{
    Instance *inst = NULL;

    if (!(inst = data)) return;
    if (!inst->menu) return;
    e_object_del(E_OBJECT(inst->menu));
    inst->menu = NULL;
}

/* call configure from popup */
    static void
_exalt_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi)
{
    if (!exalt_conf) return;
    if (exalt_conf->cfd) return;
    e_int_config_exalt_module(mn->zone->container, NULL);
}


void response_cb(Exalt_DBus_Response* response, void* data )
{
    Instance* inst = data;
    //printf("Question id: %d\n",exalt_dbus_response_msg_id_get(response));
    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_DNS_LIST_GET:
        /*    printf("DNS list:\n");

            {
                Ecore_List* l = exalt_dbus_response_list_get(response);
                char* dns;
                ecore_list_first_goto(l);
                while( (dns=ecore_list_next(l)) )
                    printf("%s\n",dns);
            }
            */
            break;
        case EXALT_DBUS_RESPONSE_DNS_ADD:
            printf("DNS added\n");
            break;
        case EXALT_DBUS_RESPONSE_DNS_DEL:
            printf("DNS deleted\n");

            break;
        case EXALT_DBUS_RESPONSE_DNS_REPLACE:
            printf("DNS replaced\n");

            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRED_LIST:
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST:
            popup_update(inst,response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            popup_update(inst,response);
            if_wired_dialog_update(inst,response);
            if_network_dialog_update(inst,response);

            break;
        case EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET:
            if_wired_dialog_update(inst,response);
            if_network_dialog_update(inst,response);

            break;
        case EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET:
            if_wired_dialog_update(inst,response);
            if_network_dialog_update(inst,response);

            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_IS:
            popup_update(inst,response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
            if_wired_dialog_update(inst,response);
            if_network_dialog_update(inst,response);

            popup_update(inst,response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_DHCP_IS:
            if_wired_dialog_update(inst,response);
            if_network_dialog_update(inst,response);

            break;
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            if_wired_dialog_update(inst,response);
            if_network_dialog_update(inst,response);

            popup_update(inst,response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_CMD_GET:
            if_wired_dialog_update(inst,response);
            if_network_dialog_update(inst,response);

            break;
        case EXALT_DBUS_RESPONSE_IFACE_CMD_SET:
            printf("%s command:\n",exalt_dbus_response_iface_get(response));

            printf("The new command is supposed to be set\n");
            break;
        case EXALT_DBUS_RESPONSE_IFACE_UP:
            break;
        case EXALT_DBUS_RESPONSE_IFACE_DOWN:
            break;
        case EXALT_DBUS_RESPONSE_WIRELESS_ESSID_GET:
            printf("%s essid:\n",exalt_dbus_response_iface_get(response));

            printf("%s\n",exalt_dbus_response_string_get(response));
            break;
        case EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_GET:
            printf("%s wpa_supplicant driver:\n",exalt_dbus_response_iface_get(response));

            printf("%s\n",exalt_dbus_response_string_get(response));
            break;
        case EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_SET:

            printf("The new driver is supposed to be set to the interface %s\n",exalt_dbus_response_iface_get(response));
            break;
        default: ;
    }
}


void notify_cb(char* eth, Exalt_Enum_Action action, void* user_data)
{
    Instance* inst = user_data;
    switch(action)
    {
        case EXALT_ETH_CB_ACTION_NEW:
        case EXALT_ETH_CB_ACTION_ADD:
            popup_iface_add(inst,eth,IFACE_WIRED);
            exalt_dbus_eth_wireless_is(inst->conn,eth);
            break;
        case EXALT_ETH_CB_ACTION_REMOVE:
            popup_iface_remove(inst,eth);
            break;
        case EXALT_ETH_CB_ACTION_UP:
        case EXALT_ETH_CB_ACTION_DOWN:
            exalt_dbus_eth_up_is(inst->conn,eth);
            break;
        case EXALT_ETH_CB_ACTION_LINK:
        case EXALT_ETH_CB_ACTION_UNLINK:
            exalt_dbus_eth_link_is(inst->conn,eth);
            break;
        case EXALT_WIRELESS_CB_ACTION_ESSIDCHANGE:
            break;
        case EXALT_ETH_CB_ACTION_ADDRESS_NEW:
            exalt_dbus_eth_ip_get(inst->conn,eth);
            break;
        case EXALT_ETH_CB_ACTION_NETMASK_NEW:
            exalt_dbus_eth_netmask_get(inst->conn,eth);
            break;
        case EXALT_ETH_CB_ACTION_GATEWAY_NEW:
            exalt_dbus_eth_gateway_get(inst->conn,eth);
            break;
        case EXALT_ETH_CB_ACTION_CONN_APPLY_START:
            break;
        case EXALT_ETH_CB_ACTION_CONN_APPLY_DONE:
            break;
        default: ;
    }
}

void notify_scan_cb(char* iface, Eina_List* networks, void* user_data )
{
    popup_notify_scan(iface,networks,user_data );
}

