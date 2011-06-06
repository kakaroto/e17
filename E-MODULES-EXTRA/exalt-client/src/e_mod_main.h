#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include <e.h>
#include <Exalt_DBus.h>
#include <Exalt.h>
#include <E_Notify.h>

#ifdef ENABLE_NLS
# include <libintl.h>
# define D_(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain,codeset)
# define D_(string) (string)
#endif

/* Macros used for config file versioning */
#define MOD_CONFIG_FILE_EPOCH 0x0001
#define MOD_CONFIG_FILE_GENERATION 0x008d
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

#define __UNUSED__ __attribute__((unused))

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;
typedef struct _Instance Instance;
typedef enum _Popup_Enum Popup_Enum;
typedef enum _Iface_Type Iface_Type;
typedef struct _Popup_Elt Popup_Elt;
typedef struct _Wired_Dialog Wired_Dialog;
typedef struct _Wired_Dialog_Basic Wired_Dialog_Basic;
typedef struct _Wireless_Dialog Wireless_Dialog;
typedef struct _Network_Dialog_New Network_Dialog_New;
typedef struct _Network_Dialog Network_Dialog;
typedef struct _Network_Dialog_Basic Network_Dialog_Basic;
typedef struct _DNS_Dialog DNS_Dialog;
typedef struct _Network_Conf_Dialog Network_Conf_Dialog;

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
   int mode;
   int notification;
   int save_network;
};

/* This struct used to hold config for individual items from above list */
struct _Config_Item
{
   /* unique id */
   const char *id;

   /* actual config properties; Define your own per-item (pos, clr) */
   int mode;
   int notification;
   int save_network;
};

struct _Wired_Dialog
{
    E_Dialog *dialog;
    Popup_Elt* iface;

    int dhcp;

    Evas_Object *btn_activate;
    Evas_Object* nothinhg;
    Evas_Object *btn_deactivate;

    Evas_Object *radio_dhcp;
    Evas_Object *icon;

    Evas_Object *radio_static;

    Evas_Object* entry_ip;
    char* ip;
    Evas_Object* entry_netmask;
    char* netmask;
    Evas_Object* entry_gateway;
    char* gateway;

    Evas_Object* entry_cmd;

    char* cmd;
};

struct _Wired_Dialog_Basic
{
    E_Dialog *dialog;
    Popup_Elt* iface;

    Evas_Object *btn;
    Evas_Object *icon;
};


struct _Wireless_Dialog
{
    E_Dialog *dialog;
    Popup_Elt* iface;

    Evas_Object *btn_activate;
    Evas_Object *btn_deactivate;
    Evas_Object *icon;

    Evas_Object *btn_new;
};

struct _Network_Dialog_New
{
    E_Dialog *dialog;
    Popup_Elt *iface;

    Evas_Object *flist_enc;
    Evas_Object *flist_iface;

    int enc;
    Evas_Object *radio_noenc;
    Evas_Object *radio_wep_ascii;
    Evas_Object *radio_wep_hexa;
    Evas_Object *radio_wpa_personnal;


    Evas_Object *entry_essid;
    char *essid;
    Evas_Object *lbl_pwd;
    Evas_Object *entry_pwd;
    char *pwd;


    Evas_Object *entry_ip;
    char *ip;
    Evas_Object *entry_netmask;
    char *netmask;
    Evas_Object *entry_gateway;
    char *gateway;
    Evas_Object *lbl_cmd;
    Evas_Object *entry_cmd;
    char *cmd;
};

struct _Network_Dialog
{
    E_Dialog *dialog;
    Popup_Elt* network;

    E_Dialog *fsel_win;
    Evas_Object *fsel;

    Evas_Object* table;
    Evas_Object* f_iface;

    Evas_Object* lbl_essid;
    Evas_Object* lbl_address;
    Evas_Object* lbl_quality;

    int dhcp;
    Evas_Object *btn_activate;
    Evas_Object* nothinhg;
    Evas_Object *btn_deactivate;

    Evas_Object *radio_dhcp;
    Evas_Object *icon;

    Evas_Object *radio_static;

    Evas_Object* entry_ip;
    char* ip;
    Evas_Object* entry_netmask;
    char* netmask;
    Evas_Object* entry_gateway;
    char* gateway;

    Evas_Object* lbl_pwd;
    Evas_Object* entry_pwd;
    char* pwd;

    Evas_Object* lbl_ca_cert;
    Evas_Object* entry_ca_cert;
    char *ca_cert;
    char *fs_ca_cert;

    Evas_Object* lbl_client_cert;
    Evas_Object* entry_client_cert;
    char* client_cert;
    char *fs_client_cert;

    Evas_Object* lbl_private_key;
    Evas_Object* entry_private_key;
    char* private_key;
    char *fs_private_key;

    Evas_Object *check_favoris;
    int favoris;

    //wep
    Evas_Object *radio_wep_hexa;
    Evas_Object *radio_wep_ascii;
    int wep_key_hexa;

    //wpa
    int ie_choice;
    Evas_Object *radio_wpa[32];

    Evas_Object* entry_cmd;

    char* cmd;
};

struct _Network_Dialog_Basic
{
    E_Dialog *dialog;
    Popup_Elt* network;

    //main list of the window
    Evas_Object *list;
    //the first frame list, with the name of the network
    Evas_Object *flist;
    Evas_Object *btn;
    Evas_Object *icon;

    E_Dialog *fsel_win;
    Evas_Object *fsel;

    char *current_essid;

    //the second flist, whith the entry for the password
    Evas_Object *frame;
    Evas_Object* lbl_pwd;
    Evas_Object* entry_pwd;
    char* pwd;

    Evas_Object* lbl_ca_cert;
    Evas_Object* entry_ca_cert;
    char *ca_cert;
    char *fs_ca_cert;

    Evas_Object* lbl_client_cert;
    Evas_Object* entry_client_cert;
    char* client_cert;
    char *fs_client_cert;

    Evas_Object* lbl_private_key;
    Evas_Object* entry_private_key;
    char* private_key;
    char *fs_private_key;

    //wep
    int wep_key_hexa;
};

struct _DNS_Dialog
{
    E_Dialog *dialog;

    Evas_Object *list;
    Evas_Object *entry_ip;
    char *ip;

    Evas_Object *bt_add;
    Evas_Object *bt_replace;
    Evas_Object *bt_delete;
};

struct _Network_Conf_Dialog
{
    E_Dialog *dialog;

    Evas_Object *list;
    Evas_Object *list_favoris;

    Evas_Object *bt_delete;
    Evas_Object *bt_add;
    Evas_Object *bt_remove;
};

struct _Instance
{
    /* pointer to this gadget's container */
    E_Gadcon_Client *gcc;

    /* evas_object used to display */
    Evas_Object *o_exalt;

    Evas_Object *gc_icon;

    Eina_List* l;

    E_Gadcon_Popup  *popup;
    Evas_Object     *popup_ilist_obj;

    Wired_Dialog wired;
    Wired_Dialog_Basic wired_basic;
    Wireless_Dialog wireless;
    Network_Dialog_New network_new;
    Network_Dialog network;
    Network_Dialog_Basic network_basic;
    DNS_Dialog dns;
    Network_Conf_Dialog network_conf;

    Exalt_DBus_Conn *conn;

    Ecore_Timer *timer_test_service;

    /* popup anyone ? */
    E_Menu *menu;
    E_Menu *sub_menu;

    /* Config_Item structure. Every gadget should have one :) */
    Config_Item *conf_item;
};

enum Encryption_Enum
{
    ENCRYPTION_NONE,
    ENCRYPTION_WEP_ASCII,
    ENCRYPTION_WEP_HEXA
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
    int nb_use;

    Popup_Enum type;
    char* iface;
    Iface_Type iface_type;

    int is_link;
    int is_up;
    //use with a wireless interface
    int is_connected;

    Evas_Object* icon;

    char* essid;
    //when a wireless network is new we set is_find=2
    //each scan result we decrement is_find
    //when we detect a wireless network we set is_find=2
    //if is_find=0 we remove the network
    //With this way, a network is removed if we didn't detected it 2 times
    //It avoid to see the network flashing in the list because the scan does'nt detect the network correctly
    int is_find;
    Exalt_Wireless_Network* n;
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
E_Config_Dialog *e_int_config_exalt_module(E_Container *con, const char *params __UNUSED__);

extern Config *exalt_conf;

void response_cb(Exalt_DBus_Response* response, void* data );
void notify_cb(char* eth, Exalt_Enum_Action action, void* user_data);
void notify_scan_cb(char* iface, Eina_List* networks, void* user_data );
E_Notification* notification_new();

void popup_init(Instance* inst);
void popup_show(Instance* inst);
void popup_hide(Instance *inst);
void popup_create(Instance* inst);
void popup_update(Instance* inst, Exalt_DBus_Response* response);
void popup_ip_update(Instance* inst, const char* iface, const char* ip);
void popup_iface_add(Instance* inst, const char* iface, Iface_Type iface_type);
void popup_iface_remove(Instance *inst, const char*  iface);
void popup_cb_dns(void *data, void *data2);
void popup_cb_network(void *data, void *data2);
void popup_cb_ifnet_sel(void *data);
void popup_up_update(Instance* inst, const char* iface, int is_up);
void popup_link_update(Instance* inst, const char* iface, int is_link);
void popup_icon_update(Instance* inst, const char* iface);
void popup_iface_label_create(Popup_Elt *elt, char *buf, int buf_size, const char* ip);
void popup_notify_scan(char* iface, Eina_List* networks, void* user_data );
void popup_network_interval_get(Instance* inst, const char* iface, int *id_first, int* id_last, Eina_List** first, Eina_List** last);
void popup_iface_essid_create(Popup_Elt *elt, char *buf, int buf_size, int quality);
Eina_Bool popup_scan_timer_cb(void *data);
void popup_elt_free(Popup_Elt* elt);

#ifdef ELIVE
void popup_cb_elive_modem(void *data, void *data2);
void popup_cb_elive_mobile_phone(void *data, void *data2);
#endif

void if_wired_dialog_init(Instance* inst);
void if_wired_dialog_show(Instance* inst);
void if_wired_dialog_set(Instance *inst, Popup_Elt* iface);
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


void if_wired_dialog_basic_init(Instance* inst);
void if_wired_dialog_basic_create(Instance* inst);
void if_wired_dialog_basic_show(Instance* inst);
void if_wired_dialog_basic_set(Instance *inst, Popup_Elt* iface);
void if_wired_dialog_basic_hide(Instance *inst);
void if_wired_dialog_basic_update(Instance* inst,Exalt_DBus_Response *response);
void if_wired_dialog_basic_icon_update(Instance *inst);
void if_wired_dialog_basic_cb_del(E_Win *win);
void if_wired_dialog_basic_cb_close(void *data, E_Dialog *dialog);
void if_wired_dialog_basic_cb_activate(void *data, void *data2);
void if_wired_dialog_basic_cb_deactivate(void *data, void *data2);


void if_wireless_dialog_init(Instance* inst);
void if_wireless_dialog_create(Instance* inst);
void if_wireless_dialog_show(Instance* inst);
void if_wireless_dialog_set(Instance *inst, Popup_Elt* iface);
void if_wireless_dialog_hide(Instance *inst);
void if_wireless_dialog_update(Instance* inst,Exalt_DBus_Response *response);
void if_wireless_dialog_icon_update(Instance *inst);
void if_wireless_disabled_update(Instance *inst);
void if_wireless_dialog_cb_ok(void *data, E_Dialog *dialog);
void if_wireless_dialog_cb_del(E_Win *win);
void if_wireless_dialog_cb_activate(void *data, void*data2);
void if_wireless_dialog_cb_deactivate(void *data, void*data2);
void if_wireless_dialog_cb_new(void *data, void*data2);


void if_network_dialog_new_init(Instance* inst);
void if_network_dialog_new_create(Instance* inst);
void if_network_dialog_new_show(Instance* inst);
void if_network_dialog_new_set(Instance *inst, Popup_Elt* iface);
void if_network_dialog_new_hide(Instance *inst);
void if_network_dialog_new_cb_ok(void *data, E_Dialog *dialog);
void if_network_dialog_new_cb_apply(void *data, E_Dialog *dialog);
void if_network_dialog_new_cb_cancel(void *data, E_Dialog *dialog);
void if_network_dialog_new_cb_del(E_Win *win);
void if_network_dialog_new_disabled_update(void *data, Evas_Object *obj, void *event_info);
void if_network_dialog_new_cb_entry(void *data, void* data2);
void if_network_dialog_new_update(Instance* inst,Exalt_DBus_Response *response);

void if_network_dialog_init(Instance* inst);
void if_network_dialog_create(Instance* inst);
void if_network_dialog_show(Instance* inst);
void if_network_dialog_set(Instance *inst, Popup_Elt* network);
void if_network_dialog_hide(Instance *inst);
void if_network_dialog_cb_del(E_Win *win);
Evas_Object* if_network_dialog_wep_new(Instance* inst,Exalt_Wireless_Network* n);
Evas_Object* if_network_dialog_wpa_new(Instance* inst,Exalt_Wireless_Network*n);

void if_network_dialog_update(Instance* inst,Exalt_DBus_Response *response);
void if_network_dialog_icon_update(Instance *inst);
void if_network_dialog_cb_activate(void *data, void*data2);
void if_network_dialog_cb_deactivate(void *data, void*data2);
void if_network_disabled_update(Instance *inst);
void if_network_dialog_cb_dhcp(void *data, Evas_Object *obj, void *event_info);
void if_network_dialog_cb_entry(void *data, void* data2);
void if_network_dialog_cb_cancel(void *data, E_Dialog *dialog);
void if_network_dialog_cb_ok(void *data, E_Dialog *dialog);
void if_network_dialog_cb_apply(void *data, E_Dialog *dialog);
void if_network_dialog_cb_ca_cert(void *data, void *data2);
void if_network_dialog_cb_fs_ca_cert(void *data, Evas_Object *obj);
void if_network_dialog_cb_fs_ca_cert_ok(void *data, E_Dialog *dialog);
void if_network_dialog_cb_client_cert(void *data, void *data2);
void if_network_dialog_cb_fs_client_cert(void *data, Evas_Object *obj);
void if_network_dialog_cb_fs_client_cert_ok(void *data, E_Dialog *dialog);
void if_network_dialog_cb_private_key(void *data, void *data2);
void if_network_dialog_cb_fs_private_key(void *data, Evas_Object *obj);
void if_network_dialog_cb_fs_private_key_ok(void *data, E_Dialog *dialog);
void if_network_dialog_cb_fs_cancel(void *data, E_Dialog *dialog );

void if_network_dialog_basic_init(Instance* inst);
void if_network_dialog_basic_create(Instance* inst);
void if_network_dialog_basic_show(Instance* inst);
void if_network_dialog_basic_set(Instance *inst, Popup_Elt* network);
void if_network_dialog_basic_hide(Instance *inst);
void if_network_dialog_basic_update(Instance* inst,Exalt_DBus_Response *response);
void if_network_dialog_basic_icon_update(Instance *inst);
void if_network_dialog_basic_cb_del(E_Win *win);
void if_network_dialog_basic_cb_close(void *data, E_Dialog *dialog);
void if_network_dialog_basic_cb_connect(void *data, void*data2);
void if_network_dialog_basic_cb_disconnect(void *data, void*data2);
Evas_Object* if_network_dialog_basic_wep_new(Instance* inst,Exalt_Wireless_Network* n);
Evas_Object* if_network_dialog_basic_wpa_new(Instance* inst,Exalt_Wireless_Network* n);
void if_network_basic_dialog_cb_ca_cert(void *data, void *data2);
void if_network_basic_dialog_cb_fs_ca_cert(void *data, Evas_Object *obj);
void if_network_basic_dialog_cb_fs_ca_cert_ok(void *data, E_Dialog *dialog);
void if_network_basic_dialog_cb_client_cert(void *data, void *data2);
void if_network_basic_dialog_cb_fs_client_cert(void *data, Evas_Object *obj);
void if_network_basic_dialog_cb_fs_client_cert_ok(void *data, E_Dialog *dialog);
void if_network_basic_dialog_cb_private_key(void *data, void *data2);
void if_network_basic_dialog_cb_fs_private_key(void *data, Evas_Object *obj);
void if_network_basic_dialog_cb_fs_private_key_ok(void *data, E_Dialog *dialog);
void if_network_basic_dialog_cb_fs_cancel(void *data, E_Dialog *dialog );


void dns_dialog_init(Instance* inst);
void dns_dialog_create(Instance* inst);
void dns_dialog_show(Instance* inst);
void dns_dialog_hide(Instance *inst);
void dns_dialog_update(Instance* inst,Exalt_DBus_Response *response);
void dns_disabled_update(Instance *inst);
void dns_dialog_cb_close(void *data, E_Dialog *dialog);
void dns_dialog_cb_del(E_Win *win);
void dns_dialog_cb_list(void *data);
void dns_dialog_cb_entry(void *data, void* data2);
void dns_dialog_cb_add(void *data, void *data2);
void dns_dialog_cb_replace(void *data, void *data2);
void dns_dialog_cb_delete(void *data, void *data2);


void network_conf_dialog_init(Instance* inst);
void network_conf_dialog_create(Instance* inst);
void network_conf_dialog_show(Instance* inst);
void network_conf_dialog_hide(Instance *inst);
void network_conf_dialog_cb_close(void *data, E_Dialog *dialog);
void network_conf_dialog_cb_del(E_Win *win);
void network_conf_dialog_cb_delete(void *data, void *data2);
void network_conf_dialog_update(Instance* inst,Exalt_DBus_Response *response);
void network_conf_dialog_cb_list(void *data);
void network_conf_dialog_cb_list_favoris(void *data);
void network_conf_dialog_cb_add(void *data, void *data2);
void network_conf_dialog_cb_remove(void *data, void *data2);


#endif
