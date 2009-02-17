#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include <libexalt_dbus.h>
#include <e.h>

/* Macros used for config file versioning */
#define MOD_CONFIG_FILE_EPOCH 0x0001
#define MOD_CONFIG_FILE_GENERATION 0x008d
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)


/* Gettext: you need to use the D_ prefix for all your messages,
 * like  printf D_("Hello World\n");  so can be replaced by gettext */
#define D_(str) dgettext(PACKAGE, str)

#define __UNUSED__ __attribute__((unused))

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;
typedef struct _Instance Instance;
typedef enum _Popup_Enum Popup_Enum;
typedef enum _Iface_Type Iface_Type;
typedef struct _Popup_Elt Popup_Elt;
typedef struct _Wired_Dialog Wired_Dialog;


/* Base config struct. Store Item Count, etc
 *
 * *module (not written to disk) (E Interaction)
 * *cfd (not written to disk) (config dialog)
 *
 * Store list of your items that you want to keep. (sorting)
 * Can define per-module config properties here.
 *
 * Version used to know when user config too old */
struct _Config
{
   E_Module *module;
   E_Config_Dialog *cfd;

   /* The list; their location on screen ? */
   Eina_List *conf_items;

   /* config file version */
   int version;

   /* actual config properties; Define your own. (per-module) */
   unsigned char switch1;
};

/* This struct used to hold config for individual items from above list */
struct _Config_Item
{
   /* unique id */
   const char *id;

   /* actual config properties; Define your own per-item (pos, clr) */
   int switch2;
};

struct _Wired_Dialog
{
    E_Dialog *dialog;
    char* iface;
    int dhcp;

    Evas_Object *icon;

    Evas_Object *btn_activate;
    Evas_Object *btn_deactivate;

    Evas_Object *radio_dhcp;
    Evas_Object *radio_static;

    Evas_Object* entry_ip;
    char* ip;
    Evas_Object* entry_netmask;
    char* netmask;
    Evas_Object* entry_gateway;
    char* gateway;

    Evas_Object* entry_cmd;
    char* cmd;

    int is_link;
    int is_up;
};

struct _Instance
{
    /* pointer to this gadget's container */
    E_Gadcon_Client *gcc;

    /* evas_object used to display */
    Evas_Object *o_exalt;

    Eina_List* l;

    E_Gadcon_Popup  *popup;
    Evas_Object     *popup_ilist_obj;

    Wired_Dialog wired;

    exalt_dbus_conn *conn;

    /* popup anyone ? */
    E_Menu *menu;
    E_Menu *sub_menu;

    /* Config_Item structure. Every gadget should have one :) */
    Config_Item *conf_item;
};

enum _Iface_Type
{
    IFACE_WIRELESS,
    IFACE_WIRED
};

enum _Popup_Enum
{
    POPUP_IFACE,
    POPUP_NETWORK
};

struct _Popup_Elt
{
    Instance* inst;
    Popup_Enum type;
    char* iface;
    Iface_Type iface_type;

    int is_link;
    int is_up;

    Evas_Object* icon;

    char* essid;
    int is_find;
    Exalt_DBus_Wireless_Network* w;
    Ecore_Timer* scan_timer;
};


/* Setup the E Module Version, Needed to check if module can run. */
EAPI extern E_Module_Api e_modapi;

/* E API Module Interface Declarations
 *
 * Need to initialize, shutdown, save the module */
EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

/* Function for calling the modules config dialog */
EAPI E_Config_Dialog *e_int_config_exalt_module(E_Container *con, const char *params __UNUSED__);

extern Config *exalt_conf;

void response_cb(Exalt_DBus_Response* response, void* data );
void notify_cb(char* eth, Exalt_Enum_Action action, void* user_data);
void notify_scan_cb(char* iface, Eina_List* networks, void* user_data );

void popup_init(Instance* inst);
void popup_show(Instance* inst);
void popup_hide(Instance *inst);
void popup_create(Instance* inst);
void popup_update(Instance* inst, Exalt_DBus_Response* response);
void popup_ip_update(Instance* inst, char* iface, char* ip);
void popup_iface_add(Instance* inst, const char* iface, Iface_Type iface_type);
void popup_cb_setup(void *data, void *data2);
void popup_cb_ifnet_sel(void *data);
void popup_up_update(Instance* inst, char* iface, int is_up);
void popup_link_update(Instance* inst, char* iface, int is_link);
void popup_icon_update(Instance* inst, const char* iface);
void popup_iface_label_create(Popup_Elt *elt, char *buf, int buf_size, char* ip);
void popup_notify_scan(char* iface, Eina_List* networks, void* user_data );
void popup_network_interval_get(Instance* inst, char* iface, int *id_first, int* id_last, Eina_List** first, Eina_List** last);
void popup_iface_essid_create(Popup_Elt *elt, char *buf, int buf_size, int quality);
int popup_scan_timer_cb(void *data);
void popup_elt_free(Popup_Elt* elt);



void if_wired_dialog_init(Instance* inst);
void if_wired_dialog_show(Instance* inst);
void if_wired_dialog_set(Instance *inst, char* iface);
void if_wired_dialog_hide(Instance *inst);
void if_wired_dialog_create(Instance* inst);
void if_wired_dialog_cb_del(E_Win *win);
void if_wired_dialog_cb_dhcp(void *data, Evas_Object *obj, void *event_info);
void if_wired_dialog_cb_cancel(void *data, E_Dialog *dialog);
void if_wired_dialog_cb_ok(void *data, E_Dialog *dialog);
void if_wired_dialog_cb_apply(void *data, E_Dialog *dialog);
void if_wired_dialog_cb_entry(void *data, void* data2);
void if_wired_disabled_update(Instance *inst);
void if_wired_dialog_cb_activate(void *data, void*data2);
void if_wired_dialog_cb_deactivate(void *data, void*data2);
void if_wired_dialog_icon_update(Instance *inst);

#endif
