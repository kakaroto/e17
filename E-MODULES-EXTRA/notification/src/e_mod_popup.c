#include "e_mod_main.h"

/* Popup function protos */
static Popup_Data *_notification_popup_new      (E_Notification *n);
static Popup_Data *_notification_popup_find     (unsigned int id);
static void        _notification_popup_place    (Popup_Data *popup, int num);
static void        _notification_popup_refresh  (Popup_Data *popup);
static void        _notification_popup_del      (unsigned int id, 
                                                 E_Notification_Closed_Reason reason);
static void        _notification_popdown        (Popup_Data *popup, 
                                                 E_Notification_Closed_Reason reason);

/* Util function protos */
static char *_notification_format_message   (E_Notification *n);

/* Callbacks */
static void _notification_theme_cb_deleted  (void *data, 
                                             Evas_Object *obj, 
                                             const char *emission, 
                                             const char *source);
static void _notification_theme_cb_close    (void *data, 
                                             Evas_Object *obj, 
                                             const char *emission, 
                                             const char *source);
static void _notification_theme_cb_find     (void *data, 
                                             Evas_Object *obj, 
                                             const char *emission, 
                                             const char *source);
static int  _notification_timer_cb          (void *data);

int
notification_popup_notify(E_Notification *n, 
                          unsigned int replaces_id, 
                          unsigned int id __UNUSED__)
{
  int timeout;
  Popup_Data *popup = NULL;
  char urgency;

  urgency = e_notification_hint_urgency_get(n);
  if (urgency == E_NOTIFICATION_URGENCY_LOW && !notification_cfg->show_low)
    return 0;
  else if (urgency == E_NOTIFICATION_URGENCY_NORMAL && !notification_cfg->show_normal)
    return 0;
  else if (urgency == E_NOTIFICATION_URGENCY_CRITICAL && !notification_cfg->show_critical)
    return 0;

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
      notification_cfg->popups = eina_list_append(notification_cfg->popups, popup);
      edje_object_signal_emit(popup->theme, "notification,new", "notification");
    }

  if (popup->timer) ecore_timer_del(popup->timer);
  timeout = e_notification_timeout_get(popup->notif);
  if (timeout == 0)
    popup->timer = NULL;
  else
    popup->timer = ecore_timer_add(timeout == -1 ? notification_cfg->default_timeout : (float)timeout / 1000, 
                                   _notification_timer_cb, 
                                   popup);
  return 1;
}

void
notification_popup_shutdown(void)
{
  Eina_List *l, *next;
  Popup_Data *popup;

  for (l = notification_cfg->popups; l && (popup = l->data); l = next)
    {
      next = l->next;
      _notification_popdown(popup, E_NOTIFICATION_CLOSED_REQUESTED);
      notification_cfg->popups = eina_list_remove_list(notification_cfg->popups, l);
    }
}

void
notification_popup_close(unsigned int id)
{
  _notification_popup_del(id, E_NOTIFICATION_CLOSED_REQUESTED);
}

static Popup_Data *
_notification_popup_new(E_Notification *n)
{
  E_Container *con;
  Popup_Data *popup;
  char buf[PATH_MAX];
  const char *shape_option;
  int shaped;

  popup = calloc(1, sizeof(Popup_Data));
  if (!popup) return NULL;
  e_notification_ref(n);
  popup->notif = n;

  con = e_container_current_get(e_manager_current_get());

  /* Create the popup window */
  popup->win = e_popup_new(e_zone_current_get(con), 0, 0, 0, 0);
  e_popup_edje_bg_object_set(popup->win, popup->theme);
  popup->e = popup->win->evas;

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

  shape_option = edje_object_data_get(popup->theme, "shaped");
  if (shape_option)
    {
      if (!strcmp(shape_option, "1"))
        shaped = 1;
      else
        shaped = 0;
      if (e_config->use_composite)
        {
          ecore_evas_alpha_set(popup->win->ecore_evas, shaped);
          e_container_window_raise(con,
                                   ecore_evas_software_x11_window_get(popup->win->ecore_evas),
                                   ecore_evas_layer_get(popup->win->ecore_evas));
        }
      else
        {
          ecore_evas_shaped_set(popup->win->ecore_evas, shaped);
          ecore_evas_avoid_damage_set(popup->win->ecore_evas, shaped);
        }
    }
  _notification_popup_refresh(popup);
  _notification_popup_place(popup, eina_list_count(notification_cfg->popups));
  e_popup_show(popup->win);

  return popup;
}

static void
_notification_popup_place(Popup_Data *popup, int num)
{
  int x, y, w, h, dir = 0;

  evas_object_geometry_get(popup->theme, NULL, NULL, &w, &h);
  if (e_notification_hint_xy_get(popup->notif, &x, &y))
    {
      E_Container *con;
      con = e_container_current_get(e_manager_current_get());

      if (x + w > con->w)
        x -= w;
      if (y + h > con->h)
        y -= h;
      e_popup_move(popup->win, x, y);
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
        e_popup_move(popup->win, 
                     notification_cfg->placement.x, 
                     notification_cfg->placement.y 
                     + dir * num * (h + notification_cfg->gap));
      else
        e_popup_move(popup->win, 
                     notification_cfg->placement.x 
                     + dir * num * (w + notification_cfg->gap), 
                     notification_cfg->placement.y);
    }
}

static void
_notification_popup_refresh(Popup_Data *popup)
{
  const char *icon_path;
  const char *app_icon_max;
  char *msg;
  void *img;
  int w, h, width = 80, height = 80;

  if (!popup) return;

  popup->app_name = e_notification_app_name_get(popup->notif);

  if (popup->app_icon) 
    {
      edje_object_part_unswallow(popup->theme, popup->app_icon);
      evas_object_del(popup->app_icon);
      popup->app_icon = NULL;
    }

  app_icon_max = edje_object_data_get(popup->theme, "app_icon_max");
  if (app_icon_max)
    {
      char *endptr;

      errno = 0;
      width = strtol(app_icon_max, &endptr, 10);
      if ((errno != 0 && width == 0) || endptr == app_icon_max) 
        {
          width = 80;
          height = 80;
        }
      else
        {
          endptr++;
          if (endptr) height = strtol(endptr, NULL, 10);
          else height = 80;
        }
    }

  /* Check if the app specify an icon either by a path or by a hint */
  if ((icon_path = e_notification_app_icon_get(popup->notif)) && *icon_path)
    {
      if (!strncmp(icon_path, "file://", 7)) icon_path += 7;
      popup->app_icon = evas_object_image_add(popup->e);
      evas_object_image_file_set(popup->app_icon, icon_path, NULL);
      evas_object_image_size_get(popup->app_icon, &w, &h);
      evas_object_image_fill_set(popup->app_icon, 0, 0, w, h);
    }
  else if ((img = e_notification_hint_icon_data_get(popup->notif)))
    {
      popup->app_icon = e_notification_image_evas_object_add(popup->e, img);
      evas_object_image_size_get(popup->app_icon, &w, &h);
    }
  else
    {
      char buf[PATH_MAX];

      snprintf(buf, sizeof(buf), "%s/e-module-notification.edj", notification_mod->dir);
      popup->app_icon = edje_object_add(popup->e);
      if (!e_theme_edje_object_set(popup->app_icon, "base/theme/modules/notification",
                                   "modules/notification/logo"))
        edje_object_file_set(popup->app_icon, buf, "modules/notification/logo");
      w = width; h = height;
    }

  if (w > width || h > height)
    {
      int v;
      v = w > h ? w : h;
      h = h * height / v;
      w = w * width / v;
      evas_object_image_fill_set(popup->app_icon, 0, 0, w, h);
      evas_object_resize(popup->app_icon, w, h);
      edje_extern_object_min_size_set(popup->app_icon, w, h);
      edje_extern_object_max_size_set(popup->app_icon, w, h);
    }
  else
    {
      evas_object_resize(popup->app_icon, w, h);
      edje_extern_object_min_size_set(popup->app_icon, w, h);
      edje_extern_object_max_size_set(popup->app_icon, w, h);
    }
  
  edje_object_calc_force(popup->theme);
  edje_object_part_swallow(popup->theme, "notification.swallow.app_icon", popup->app_icon);
  edje_object_signal_emit(popup->theme, "notification,icon", "notification");

  /* Fill up the event message */
  msg = _notification_format_message(popup->notif);
  edje_object_part_text_set(popup->theme, "notification.textblock.message", msg);
  free(msg);

  /* Compute the new size of the popup */
  edje_object_calc_force(popup->theme);
  edje_object_size_min_calc(popup->theme, &w, &h);
  e_popup_resize(popup->win, w, h);
  evas_object_resize(popup->theme, w, h);
}

static Popup_Data *
_notification_popup_find(unsigned int id)
{
  Eina_List *l;
  Popup_Data *popup;

  for (l = notification_cfg->popups; l && (popup = l->data); l = l->next)
    if (e_notification_id_get(popup->notif) == id) return popup;

  return NULL;
}

static void
_notification_popup_del(unsigned int id, E_Notification_Closed_Reason reason)
{
  Popup_Data *popup;
  Eina_List *l, *next;
  int i;

  for (l = notification_cfg->popups, i = 0; l && (popup = l->data); l = next)
    {
      next = l->next;
      if (e_notification_id_get(popup->notif) == id)
        {
          _notification_popdown(popup, reason);
          notification_cfg->popups = eina_list_remove_list(notification_cfg->popups, l);
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
  if (popup->timer) ecore_timer_del(popup->timer);
  e_popup_hide(popup->win);
  evas_object_del(popup->app_icon);
  evas_object_del(popup->theme);
  e_object_del(E_OBJECT(popup->win));
  e_notification_closed_set(popup->notif, 1);
  e_notification_daemon_signal_notification_closed(notification_cfg->daemon, 
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
  Eina_List *l;

  if (!popup->app_name) return;

  for (l = e_border_client_list(); l; l = l->next)
    {
      size_t compare_len;
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

static int
_notification_timer_cb(void *data)
{
  Popup_Data *popup = data;
  _notification_popup_del(e_notification_id_get(popup->notif), 
                          E_NOTIFICATION_CLOSED_EXPIRED);
  return 0;
}

