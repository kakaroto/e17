#include <E_Notification_Daemon.h>
#include <e.h>
#include "e_mod_main.h"

typedef struct _Popup_Data Popup_Data;
struct _Popup_Data
{
  E_Notification *notif;
  E_Win *win;
  Evas *e;
  Evas_Object *theme;
  const char *app_name;
  Evas_Object *app_icon;
  Ecore_Timer *timer;
};

typedef struct _Daemon_Data Daemon_Data;
struct _Daemon_Data
{
  E_Notification_Daemon *daemon;
  Evas_List *popups;
  
  float default_timeout;
  int next_id;
};

/* local function protos */
static int  _notification_cb_notify(E_Notification_Daemon *daemon, E_Notification *n);
static void _notification_cb_close_notification(E_Notification_Daemon *daemon, 
                                                unsigned int id);
static int  _notification_timer_cb(void *data);
static void _notification_theme_cb_deleted(void *data, Evas_Object *obj, 
                                           const char *emission, const char *source);
static void _notification_theme_cb_close(void *data, Evas_Object *obj, 
                                         const char *emission, const char *source);
static void _notification_theme_cb_find(void *data, Evas_Object *obj, 
                                        const char *emission, const char *source);

static Popup_Data *_notification_popup_new(E_Notification *n);
static void        _notification_popup_place(Popup_Data *popup, int num);
static void        _notification_popup_refresh(Popup_Data *popup);
static Popup_Data *_notification_popup_find(unsigned int id);
static void        _notification_popup_del(unsigned int id, 
                                           E_Notification_Closed_Reason reason);
static void        _notification_popdown(Popup_Data *popup, 
                                         E_Notification_Closed_Reason reason);

static char *_notification_format_message(E_Notification *n);

static Config *_notification_cfg_new(void);
static void    _notification_cfg_free(Config *cfg);
/* Global variables */
static Daemon_Data *dd;
static E_Config_DD *conf_edd = NULL;
E_Module *notification_mod = NULL;
Config   *notification_cfg = NULL;

/* Module Api Functions */
EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "Notification"};

EAPI void *
e_modapi_init(E_Module *m) 
{
   E_Notification_Daemon *d;

   dd = calloc(1, sizeof(Daemon_Data));

   /* register config panel entry */
   e_configure_registry_category_add("extensions", 90, D_("Extensions"), NULL, 
                                     "enlightenment/extensions");
   e_configure_registry_item_add("extensions/notification", 30, D_("Notification"), NULL, 
                                 "enlightenment/e", e_int_config_notification_module);

   conf_edd = E_CONFIG_DD_NEW("Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D conf_edd
   E_CONFIG_VAL(D, T, version, INT);
   E_CONFIG_VAL(D, T, direction, INT);
   E_CONFIG_VAL(D, T, gap, INT);
   E_CONFIG_VAL(D, T, placement.x, INT);
   E_CONFIG_VAL(D, T, placement.y, INT);

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

   if (!notification_cfg) notification_cfg = _notification_cfg_new();

   /* set up the daemon */
   d = e_notification_daemon_add("e_notification_module", "Enlightenment");
   e_notification_daemon_data_set(d, dd);
   dd->daemon = d;
   dd->default_timeout = 5.0;
   e_notification_daemon_callback_notify_set(d, _notification_cb_notify);
   e_notification_daemon_callback_close_notification_set(d, _notification_cb_close_notification);

   notification_mod = m;
   return m;
}

EAPI int 
e_modapi_shutdown(E_Module *m __UNUSED__) 
{
   Popup_Data *popup;
   Evas_List *l, *next;

   if (notification_cfg->cfd) e_object_del(E_OBJECT(notification_cfg->cfd));
   e_configure_registry_item_del("extensions/notification");
   e_configure_registry_category_del("extensions");

   for (l = dd->popups; l && (popup = l->data); l = next)
     {
       next = l->next;
       _notification_popdown(popup, E_NOTIFICATION_CLOSED_REQUESTED);
       dd->popups = evas_list_remove_list(dd->popups, l);
     }
   e_notification_daemon_free(dd->daemon);
   free(dd);
   notification_mod = NULL;
   _notification_cfg_free(notification_cfg);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int 
e_modapi_save(E_Module *m __UNUSED__) 
{
   return e_config_domain_save("module.notification", conf_edd, notification_cfg);
}

/* Callbacks */
static int
_notification_cb_notify(E_Notification_Daemon *daemon __UNUSED__, E_Notification *n)
{
   unsigned int replaces_id;
   unsigned int new_id;
   int timeout;
   Popup_Data *popup = NULL;
   
   replaces_id = e_notification_replaces_id_get(n);
   if (replaces_id && (popup = _notification_popup_find(replaces_id))) 
     {
       if (popup->notif) e_notification_unref(popup->notif);
       e_notification_ref(n);
       popup->notif = n;
       edje_object_signal_emit(popup->theme, "notification,del", "notification");
     }

   if (!popup)
     {
       popup = _notification_popup_new(n);
       dd->popups = evas_list_append(dd->popups, popup);
       edje_object_signal_emit(popup->theme, "notification,new", "notification");
     }
   
   new_id = dd->next_id++;
   e_notification_id_set(n, new_id);
   
   if (popup->timer) ecore_timer_del(popup->timer);
   timeout = e_notification_timeout_get(popup->notif);
   if (timeout == 0)
     popup->timer = NULL;
   else
     popup->timer = ecore_timer_add(timeout == -1 ? dd->default_timeout : (float)timeout / 1000, 
                                    _notification_timer_cb, 
                                    popup);
   
   return new_id;
}

static void
_notification_cb_close_notification(E_Notification_Daemon *daemon __UNUSED__, 
                                    unsigned int id)
{
   _notification_popup_del(id, 
                           E_NOTIFICATION_CLOSED_REQUESTED);
}

static int
_notification_timer_cb(void *data)
{
   Popup_Data *popup = data;
   _notification_popup_del(e_notification_id_get(popup->notif), 
                           E_NOTIFICATION_CLOSED_EXPIRED);
   return 0;
}

static void
_notification_theme_cb_deleted(void *data, 
                               Evas_Object *obj __UNUSED__, 
                               const char *emission __UNUSED__, 
                               const char *source __UNUSED__)
{
  Popup_Data *popup = data;
  _notification_popup_refresh(popup);
  edje_object_signal_emit(popup->theme, "notification,new", "notification");
}

static void
_notification_theme_cb_close(void *data, 
                             Evas_Object *obj __UNUSED__, 
                             const char *emission __UNUSED__, 
                             const char *source __UNUSED__)
{
  Popup_Data *popup = data;
  _notification_popup_del(e_notification_id_get(popup->notif), 
                          E_NOTIFICATION_CLOSED_DISMISSED);
}

static void
_notification_theme_cb_find(void *data, 
                            Evas_Object *obj __UNUSED__, 
                            const char *emission __UNUSED__, 
                            const char *source __UNUSED__)
{
  Popup_Data *popup = data;
  Evas_List *l;

  if (!popup->app_name) return;

  for (l = e_border_client_list(); l; l = l->next)
    {
      int compare_len;
      E_Border *bd = l->data;

      compare_len = strlen(popup->app_name);
      if (strlen(bd->client.icccm.name) < compare_len)
        compare_len = strlen(bd->client.icccm.name);

      /* We can't be sure that the app_name really match the application name.
       * Some plugin put their name instead. But this search gives some good
       * results.
       */
      if (!strncasecmp(bd->client.icccm.name, popup->app_name, compare_len))
        {
          e_desk_show(bd->desk);
          e_border_show(bd);
          e_border_raise(bd);
          e_border_focus_set_with_pointer(bd);
          break;
        }
    }
}

/* Local functions */
static Popup_Data *
_notification_popup_new(E_Notification *n)
{
   E_Container *con;
   Popup_Data *popup;
   char buf[PATH_MAX];
   Ecore_X_Window_State state[5] = { 
     ECORE_X_WINDOW_STATE_STICKY,
     ECORE_X_WINDOW_STATE_SKIP_TASKBAR,
     ECORE_X_WINDOW_STATE_SKIP_PAGER,
     ECORE_X_WINDOW_STATE_HIDDEN,
     ECORE_X_WINDOW_STATE_ABOVE
   };

   popup = calloc(1, sizeof(Popup_Data));
   if (!popup) return NULL;
   e_notification_ref(n);
   popup->notif = n;

   con = e_container_current_get(e_manager_current_get());

   /* Create the popup window */
   popup->win = e_win_new(con);
   e_win_name_class_set(popup->win, "E", "_notification_dialog");
   e_win_title_set(popup->win, "Event Notification");
   e_win_borderless_set(popup->win, 1);
   e_win_placed_set(popup->win, 1);
   e_win_sticky_set(popup->win, 1);
   ecore_x_icccm_transient_for_set(popup->win->evas_win, con->win);
   ecore_x_icccm_protocol_set(popup->win->evas_win, ECORE_X_WM_PROTOCOL_TAKE_FOCUS, 0);

   ecore_x_netwm_window_type_set(popup->win->evas_win, ECORE_X_WINDOW_TYPE_DOCK);
   ecore_x_netwm_window_state_set(popup->win->evas_win, state, 6);

   popup->e = e_win_evas_get(popup->win);

   /* Setup the theme */
   snprintf(buf, sizeof(buf), "%s/e-module-notification.edj", notification_mod->dir);
   popup->theme = edje_object_add(popup->e);
   if (!e_theme_edje_object_set(popup->theme, "base/theme/modules/notification",
                                "modules/notification/main"))
     edje_object_file_set(popup->theme, buf, "modules/notification/main");
   evas_object_show(popup->theme);
   edje_object_signal_callback_add(popup->theme, "notification,deleted", "theme", 
                                   _notification_theme_cb_deleted, popup);
   edje_object_signal_callback_add(popup->theme, "notification,close", "theme", 
                                   _notification_theme_cb_close, popup);
   edje_object_signal_callback_add(popup->theme, "notification,find", "theme", 
                                   _notification_theme_cb_find, popup);

   /* Uncomment to use shaped popups */
   //e_win_shaped_set(popup->win, 1);
   //e_win_avoid_damage_set(popup->win, 1);

   _notification_popup_refresh(popup);
   _notification_popup_place(popup, evas_list_count(dd->popups));
   e_win_show(popup->win);

   return popup;
}

static void
_notification_popup_place(Popup_Data *popup, int num)
{
   int x, y, w, h, dir = 0;

   evas_object_geometry_get(popup->theme, NULL, NULL, &w, &h);
   if (e_notification_hint_xy_get(popup->notif, &x, &y))
     {
       if (!popup->win->container) return;

       if (x + w > popup->win->container->w)
         x -= w;
       if (y + h > popup->win->container->h)
         y -= h;
       e_win_move(popup->win, x, y);
     }
   else
     {
       switch (notification_cfg->direction)
         {
         case DIRECTION_DOWN:
         case DIRECTION_RIGHT:
           dir = 1;
           break;
         case DIRECTION_UP:
         case DIRECTION_LEFT:
           dir = -1;
           break;
         }

       if (notification_cfg->direction == DIRECTION_DOWN ||
           notification_cfg->direction == DIRECTION_UP)
         e_win_move(popup->win, 
                    notification_cfg->placement.x, 
                    notification_cfg->placement.y 
                    + dir * num * (h + notification_cfg->gap));
       else
         e_win_move(popup->win, 
                    notification_cfg->placement.x 
                    + dir * num * (w + notification_cfg->gap), 
                    notification_cfg->placement.y);
     }
}

static void
_notification_popup_refresh(Popup_Data *popup)
{
   const char *icon_path;
   char *msg;
   void *img;
   int w, h;

   if (!popup) return;

   popup->app_name = e_notification_app_name_get(popup->notif);

   if (popup->app_icon) 
     {
       edje_object_part_unswallow(popup->theme, popup->app_icon);
       evas_object_del(popup->app_icon);
     }

   /* Check if the app specify an icon either by a path or by a hint */
   if ((icon_path = e_notification_app_icon_get(popup->notif)) && *icon_path)
     {
       popup->app_icon = evas_object_image_add(popup->e);
       evas_object_image_load_scale_down_set(popup->app_icon, 1);
       evas_object_image_load_size_set(popup->app_icon, 80, 80);
       evas_object_image_file_set(popup->app_icon, icon_path, NULL);
       evas_object_image_fill_set(popup->app_icon, 0, 0, 80, 80);
     }
   else if ((img = e_notification_hint_icon_data_get(popup->notif)))
     {
       popup->app_icon = e_notification_image_evas_object_add(popup->e, img);
     }
   evas_object_image_size_get(popup->app_icon, &w, &h);
   edje_extern_object_min_size_set(popup->app_icon, w, h);
   edje_extern_object_max_size_set(popup->app_icon, w, h);
   edje_object_part_swallow(popup->theme, "notification.swallow.app_icon", popup->app_icon);

   /* Fill up the event message */
   msg = _notification_format_message(popup->notif);
   edje_object_part_text_set(popup->theme, "notification.textblock.message", msg);
   free(msg);

   /* Compute the new size of the popup */
   edje_object_size_min_calc(popup->theme, &w, &h);
   e_win_size_min_set(popup->win, w, h);
   e_win_size_max_set(popup->win, w, h);
   e_win_resize(popup->win, w, h);
   evas_object_resize(popup->theme, w, h);
   edje_object_calc_force(popup->theme);
}

static Popup_Data *
_notification_popup_find(unsigned int id)
{
   Evas_List *l;
   Popup_Data *popup;

   for (l = dd->popups; l && (popup = l->data); l = l->next)
     if (e_notification_id_get(popup->notif) == id) return popup;

   return NULL;
}

static void
_notification_popup_del(unsigned int id, E_Notification_Closed_Reason reason)
{
   Popup_Data *popup;
   Evas_List *l, *next;
   int i;

   for (l = dd->popups, i = 0; l && (popup = l->data); l = next)
     {
       next = l->next;
       if (e_notification_id_get(popup->notif) == id)
         {
           _notification_popdown(popup, reason);
           dd->popups = evas_list_remove_list(dd->popups, l);
         }
       else
         {
           _notification_popup_place(popup, i);
           i++;
         }
     }
}

static void
_notification_popdown(Popup_Data *popup, E_Notification_Closed_Reason reason)
{
   ecore_timer_del(popup->timer);
   e_win_hide(popup->win);
   evas_object_del(popup->app_icon);
   evas_object_del(popup->theme);
   e_object_del(E_OBJECT(popup->win));
   e_notification_closed_set(popup->notif, 1);
   e_notification_daemon_signal_notification_closed(dd->daemon, 
                                                    e_notification_id_get(popup->notif), 
                                                    reason);
   e_notification_unref(popup->notif);
   free(popup);
}

static char *
_notification_format_message(E_Notification *n)
{
   char *msg;
   const char *orig;
   char *dest;
   int len;
   int size = 512;

   msg = calloc(1, 512);
   snprintf(msg, 511, "<subject>%s</subject><br><body>",
            e_notification_summary_get(n));
   len = strlen(msg);

   for (orig = e_notification_body_get(n), dest = msg + strlen(msg); orig && *orig; orig++)
     {
       if (len >= size - 4)
         {
           size = len + 512;
           msg = realloc(msg, size);
           msg = memset(msg + len, 0, size - len);
           dest = msg + len;
         }

       if (*orig == '\n')
         {
           dest[0] = '<'; 
           dest[1] = 'b'; 
           dest[2] = 'r'; 
           dest[3] = '>';
           len += 4;
           dest += 4;
         }
       else
         {
           *dest = *orig;
           len++;
           dest++;
         }
     }

   return msg;
}

static Config *
_notification_cfg_new(void)
{
  Config *cfg;

  cfg = E_NEW(Config, 1);
  cfg->cfd         = NULL;
  cfg->version     = MOD_CFG_FILE_VERSION;
  cfg->direction   = DIRECTION_DOWN;
  cfg->gap         = 10;
  cfg->placement.x = 10;
  cfg->placement.y = 10;
  e_modapi_save(notification_mod);

  return cfg;
}

static void
_notification_cfg_free(Config *cfg)
{
  E_FREE(cfg);
}
