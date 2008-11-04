#include "e_mod_main.h"

/* Gadcon function protos */
static E_Gadcon_Client *_gc_init      (E_Gadcon *gc, 
                                       const char *name, 
                                       const char *id, 
                                       const char *style);
static void             _gc_shutdown  (E_Gadcon_Client *gcc);
static char            *_gc_label     (E_Gadcon_Client_Class *client_class);
static Evas_Object     *_gc_icon      (E_Gadcon_Client_Class *client_class, Evas *evas);
static const char      *_gc_id_new    (E_Gadcon_Client_Class *client_class);
static void             _gc_id_del    (E_Gadcon_Client_Class *client_class, const char *id);

/* Callback function protos */
static int  _notification_cb_notify              (E_Notification_Daemon *daemon, 
                                                  E_Notification *n);
static void _notification_cb_close_notification  (E_Notification_Daemon *daemon, 
                                                  unsigned int id);

/* Config function protos */
static Config *_notification_cfg_new   (void);
static void    _notification_cfg_free  (Config *cfg);

/* Global variables */
E_Module *notification_mod = NULL;
Config   *notification_cfg = NULL;
int       uuid             = 0;

static E_Config_DD  *conf_edd      = NULL;
static E_Config_DD  *conf_item_edd = NULL;

/* Gadcon Api Functions */
const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "notification", 
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, _gc_id_del},
   E_GADCON_CLIENT_STYLE_PLAIN
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Notification_Box *b;
   E_Gadcon_Client *gcc;
   Config_Item *ci;
   Instance *inst;

   inst = E_NEW(Instance, 1);
   ci   = notification_box_config_item_get(id);
   b    = notification_box_get(ci->id, gc->evas);

   inst->ci    = ci;
   b->inst     = inst;
   inst->n_box = b;

   gcc = e_gadcon_client_new(gc, name, id, style, b->o_box);
   gcc->data = inst;
   inst->gcc = gcc;
   
   evas_object_event_callback_add(b->o_box, EVAS_CALLBACK_MOVE,
                                  notification_box_cb_obj_moveresize, inst);
   evas_object_event_callback_add(b->o_box, EVAS_CALLBACK_RESIZE,
                                  notification_box_cb_obj_moveresize, inst);
   notification_cfg->instances = eina_list_append(notification_cfg->instances, inst);
   _gc_orient(gcc, gc->orient);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
   notification_box_hide(inst->n_box);
   notification_cfg->instances = eina_list_remove(notification_cfg->instances, inst);
   free(inst);
}

void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{
   Instance *inst;

   inst = gcc->data;
   switch (orient)
     {
      case E_GADCON_ORIENT_FLOAT:
      case E_GADCON_ORIENT_HORIZ:
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	notification_box_orient_set(inst->n_box, 1);
	e_gadcon_client_aspect_set(gcc, MAX(eina_list_count(inst->n_box->icons), 1) * 16, 16);
	break;
      case E_GADCON_ORIENT_VERT:
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_RIGHT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_LB:
      case E_GADCON_ORIENT_CORNER_RB:
	notification_box_orient_set(inst->n_box, 0);
	e_gadcon_client_aspect_set(gcc, 16, MAX(eina_list_count(inst->n_box->icons), 1) * 16);
	break;
      default:
	break;
     }
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(E_Gadcon_Client_Class *client_class)
{
   return D_("Notification Box");
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-notification.edj",
	    e_module_dir_get(notification_mod));
   if (!e_theme_edje_object_set(o, "base/theme/modules/notification",
                                "icon"))
     edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class)
{
   Config_Item *ci;

   ci = notification_box_config_item_get(NULL);
   return ci->id;
}

static void
_gc_id_del(E_Gadcon_Client_Class *client_class, const char *id)
{
   Config_Item *ci;

   notification_box_del(id);
   ci = notification_box_config_item_get(id);
   if (ci)
     {
	if (ci->id) evas_stringshare_del(ci->id);
	notification_cfg->items = eina_list_remove(notification_cfg->items, ci);
        free(ci);
     }
}

/* Module Api Functions */
EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "Notification"};

EAPI void *
e_modapi_init(E_Module *m) 
{
   E_Notification_Daemon *d;
   char buf[PATH_MAX];

   snprintf(buf, sizeof(buf), "%s/e-module-notification.edj", m->dir);
   /* register config panel entry */
   e_configure_registry_category_add("extensions", 90, D_("Extensions"), NULL, 
                                     "enlightenment/extensions");
   e_configure_registry_item_add("extensions/notification", 30, D_("Notification"), NULL, 
                                 buf, e_int_config_notification_module);

   conf_item_edd = E_CONFIG_DD_NEW("Notification_Config_Item", Config_Item);
   #undef T
   #undef D
   #define T Config_Item
   #define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, show_label, INT);
   E_CONFIG_VAL(D, T, show_popup, INT);
   E_CONFIG_VAL(D, T, focus_window, INT);
   E_CONFIG_VAL(D, T, store_low, INT);
   E_CONFIG_VAL(D, T, store_normal, INT);
   E_CONFIG_VAL(D, T, store_critical, INT);

   conf_edd = E_CONFIG_DD_NEW("Notification_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D conf_edd
   E_CONFIG_VAL(D, T, version, INT);
   E_CONFIG_VAL(D, T, show_low, INT);
   E_CONFIG_VAL(D, T, show_normal, INT);
   E_CONFIG_VAL(D, T, show_critical, INT);
   E_CONFIG_VAL(D, T, direction, INT);
   E_CONFIG_VAL(D, T, gap, INT);
   E_CONFIG_VAL(D, T, placement.x, INT);
   E_CONFIG_VAL(D, T, placement.y, INT);
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   notification_cfg = e_config_domain_load("module.notification", conf_edd);
   if (notification_cfg)
     {
        if (notification_cfg->version == 0)
          {
             _notification_cfg_free(notification_cfg);
             notification_cfg = NULL;
          }
        if ((notification_cfg->version >> 16) < MOD_CFG_FILE_EPOCH) 
          {
             _notification_cfg_free(notification_cfg);
             notification_cfg = NULL;
	     e_util_dialog_show(D_("Notification Configuration Updated"),
                                D_("Notification Module Configuration data needed "
                                   "upgrading. Your old configuration<br> has been"
                                   " wiped and a new set of defaults initialized. "
                                   "This<br>will happen regularly during "
                                   "development, so don't report a<br>bug. "
                                   "This simply means the Notification module needs "
                                   "new configuration<br>data by default for "
                                   "usable functionality that your old<br>"
                                   "configuration simply lacks. This new set of "
                                   "defaults will fix<br>that by adding it in. "
                                   "You can re-configure things now to your<br>"
                                   "liking. Sorry for the inconvenience.<br>"));
          }
        else if (notification_cfg->version > MOD_CFG_FILE_VERSION) 
          {
             _notification_cfg_free(notification_cfg);
             notification_cfg = NULL;
	     e_util_dialog_show(D_("Notification Configuration Updated"),
                                D_("Your Notification Module Configuration is NEWER "
                                   "than the Notification Module version. This is "
                                   "very<br>strange. This should not happen unless"
                                   " you downgraded<br>the Notification Module or "
                                   "copied the configuration from a place where"
                                   "<br>a newer version of the Notification Module "
                                   "was running. This is bad and<br>as a "
                                   "precaution your configuration has been now "
                                   "restored to<br>defaults. Sorry for the "
                                   "inconvenience.<br>"));
          }
     }

   if (!notification_cfg) 
     {
       notification_cfg = _notification_cfg_new();
     }
   else if (eina_list_count(notification_cfg->items) > 0)
     {
       Config_Item *ci;
       const char *p;

       /* Init uuid */
	ci = eina_list_last(notification_cfg->items)->data;
	p = strrchr(ci->id, '.');
	if (p) uuid = atoi(p + 1);
     }

   /* set up the notification daemon */
   d = e_notification_daemon_add("e_notification_module", "Enlightenment");
   if (!d)
     {
	_notification_cfg_free(notification_cfg);
	notification_cfg = NULL;
	e_util_dialog_show(D_("Error During DBus Init!"),
			   D_("Error during DBus init! Please check if "
			      "dbus is correctly installed and running."));
	return NULL;
     }
   notification_cfg->daemon = d;
   notification_cfg->default_timeout = 5.0;
   e_notification_daemon_data_set(d, notification_cfg);
   e_notification_daemon_callback_notify_set(d, _notification_cb_notify);
   e_notification_daemon_callback_close_notification_set(d, _notification_cb_close_notification);

   /* set up the borders events callbacks */
   notification_cfg->handlers = eina_list_append
     (notification_cfg->handlers, ecore_event_handler_add
      (E_EVENT_BORDER_REMOVE, notification_box_cb_border_remove, NULL));

   notification_mod = m;
   e_gadcon_provider_register(&_gc_class);
   return m;
}

EAPI int 
e_modapi_shutdown(E_Module *m __UNUSED__) 
{
   e_gadcon_provider_unregister(&_gc_class);

   while (notification_cfg->handlers)
     {
       ecore_event_handler_del(notification_cfg->handlers->data);
       notification_cfg->handlers = eina_list_remove_list(notification_cfg->handlers,
                                                          notification_cfg->handlers);
     }

   if (notification_cfg->cfd) e_object_del(E_OBJECT(notification_cfg->cfd));
   e_configure_registry_item_del("extensions/notification");
   e_configure_registry_category_del("extensions");

   if (notification_cfg->menu)
     {
       e_menu_post_deactivate_callback_set(notification_cfg->menu, NULL, NULL);
       e_object_del(E_OBJECT(notification_cfg->menu));
       notification_cfg->menu = NULL;
     }

   while (notification_cfg->items)
     {
       Config_Item *ci;

       ci = notification_cfg->items->data;
       notification_cfg->items = eina_list_remove_list(notification_cfg->items,
                                                       notification_cfg->items);
       if (ci->id) evas_stringshare_del(ci->id);
       free(ci);
     }

   notification_box_shutdown();
   notification_popup_shutdown();

   e_notification_daemon_free(notification_cfg->daemon);
   _notification_cfg_free(notification_cfg);
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   notification_mod = NULL;

   return 1;
}

EAPI int 
e_modapi_save(E_Module *m __UNUSED__) 
{
   int ret;
   ret = e_config_domain_save("module.notification", conf_edd, notification_cfg);
   return ret;
}

/* Callbacks */
static int
_notification_cb_notify(E_Notification_Daemon *daemon __UNUSED__, E_Notification *n)
{
   unsigned int replaces_id;
   unsigned int new_id;
   int stacked, popuped;

   replaces_id = e_notification_replaces_id_get(n);
   new_id = notification_cfg->next_id++;
   e_notification_id_set(n, new_id);

   popuped = notification_popup_notify(n, replaces_id, new_id);
   stacked = notification_box_notify(n, replaces_id, new_id);
   
   if (!popuped && !stacked)
     {
       e_notification_hint_urgency_set(n, 4);
       notification_popup_notify(n, replaces_id, new_id);
     }
   
   return new_id;
}

static void
_notification_cb_close_notification(E_Notification_Daemon *daemon __UNUSED__, 
                                    unsigned int id)
{
   notification_popup_close(id);
}

static Config *
_notification_cfg_new(void)
{
  Config *cfg;

  cfg = E_NEW(Config, 1);
  cfg->cfd           = NULL;
  cfg->version       = MOD_CFG_FILE_VERSION;
  cfg->show_low      = 0;
  cfg->show_normal   = 1;
  cfg->show_critical = 1;
  cfg->direction     = DIRECTION_DOWN;
  cfg->gap           = 10;
  cfg->placement.x   = 10;
  cfg->placement.y   = 10;
  e_modapi_save(notification_mod);

  return cfg;
}

static void
_notification_cfg_free(Config *cfg)
{
  E_FREE(cfg);
}
