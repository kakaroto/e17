#include "e_mod_main.h"

/* Notification box protos */
static Notification_Box  *_notification_box_new           (const char *id, Evas *evas);
static void               _notification_box_free          (Notification_Box *b);
static void               _notification_box_evas_set      (Notification_Box *b, 
                                                           Evas *evas);
static void               _notification_box_empty         (Notification_Box *b);
static void               _notification_box_resize_handle (Notification_Box *b);
static void               _notification_box_empty_handle  (Notification_Box *b);
static Evas_List         *_notification_box_find          (E_Notification_Urgency urgency);

/* Notification box icons protos */
static Notification_Box_Icon *_notification_box_icon_new         (Notification_Box *b, 
                                                                  E_Notification *n, 
                                                                  E_Border *bd,
                                                                  unsigned int id);
static void                   _notification_box_icon_free        (Notification_Box_Icon *ic);
static void                   _notification_box_icon_fill        (Notification_Box_Icon *ic, 
                                                                  E_Notification *n);
static void                   _notification_box_icon_fill_label  (Notification_Box_Icon *ic);
static void                   _notification_box_icon_empty       (Notification_Box_Icon *ic);
static Notification_Box_Icon *_notification_box_icon_find        (Notification_Box *b, 
                                                                  E_Border *bd,
                                                                  unsigned int n_id);
static void                   _notification_box_icon_signal_emit (Notification_Box_Icon *ic, 
                                                                  char *sig, 
                                                                  char *src);

/* Utils */
static E_Border  *_notification_find_source_border    (E_Notification *n);

/* Notification box callbacks */
static void  _notification_box_cb_menu_post         (void *data, E_Menu *m);
static void  _notification_box_cb_empty_mouse_down  (void *data, 
                                                     Evas *e, 
                                                     Evas_Object *obj, 
                                                     void *event_info);
static void  _notification_box_cb_icon_move         (void *data, 
                                                     Evas *e, 
                                                     Evas_Object *obj, 
                                                     void *event_info);
static void  _notification_box_cb_icon_resize       (void *data, 
                                                     Evas *e, 
                                                     Evas_Object *obj, 
                                                     void *event_info);
static void  _notification_box_cb_icon_mouse_in     (void *data, 
                                                     Evas *e, 
                                                     Evas_Object *obj, 
                                                     void *event_info);
static void  _notification_box_cb_icon_mouse_out    (void *data, 
                                                     Evas *e, 
                                                     Evas_Object *obj, 
                                                     void *event_info);
static void  _notification_box_cb_icon_mouse_up     (void *data, 
                                                     Evas *e, 
                                                     Evas_Object *obj, 
                                                     void *event_info);
static void  _notification_box_cb_icon_mouse_down   (void *data, 
                                                     Evas *e, 
                                                     Evas_Object *obj, 
                                                     void *event_info);

static int   _notification_box_cb_icon_mouse_still_in  (void *data);
static void  _notification_box_cb_menu_configuration   (void *data, 
                                                        E_Menu *m, 
                                                        E_Menu_Item *mi);

int 
notification_box_notify(E_Notification *n, 
                        unsigned int replaces_id, 
                        unsigned int id)
{
  Evas_List *l, *n_box;
  E_Border *bd;

  bd = _notification_find_source_border(n);

  n_box = _notification_box_find(e_notification_hint_urgency_get(n));
  for (l = n_box; l; l = l->next)
    {
      Notification_Box *b;
      Notification_Box_Icon *ic = NULL;

      b = l->data;
      if (bd || replaces_id)
        ic = _notification_box_icon_find(b, bd, replaces_id);
      if (ic) 
        {
          e_notification_unref(ic->notif);
          e_notification_ref(n);
          ic->notif = n;
          ic->n_id = id;
          _notification_box_icon_empty(ic);
          _notification_box_icon_fill(ic, n);
        }
      else
        {
          ic = _notification_box_icon_new(b, n, bd, id);
          if (!ic) continue;
          b->icons = evas_list_append(b->icons, ic);
          e_box_pack_end(b->o_box, ic->o_holder);
        }
      _notification_box_empty_handle(b);
      _notification_box_resize_handle(b);
      _gc_orient(b->inst->gcc);
    }
  evas_list_free(n_box);

  return 1;
}

void
notification_box_shutdown(void)
{
  Notification_Box *b;

  while (notification_cfg->n_box)
    {
      b = notification_cfg->n_box->data;
      if (b) _notification_box_free(b);
      notification_cfg->n_box = evas_list_remove_list(notification_cfg->n_box,
                                                      notification_cfg->n_box);
    }
}

void
notification_box_del(const char *id)
{
  Evas_List *l;
  Notification_Box *b;

  /* Find old config */
  for (l = notification_cfg->n_box; l; l = l->next)
    {
      b = l->data;

      if (b->id && !strcmp(b->id, id))
        {
          _notification_box_free(b);
          notification_cfg->n_box = evas_list_remove(notification_cfg->n_box, b);
          return;
        }
    }
}

void
notification_box_show(Notification_Box *b)
{
  Evas_List *l;

  evas_object_show(b->o_box);
  if (b->o_empty) evas_object_show(b->o_empty);
  for (l = b->icons; l; l = l->next)
    {
      Notification_Box_Icon *ic;

      if (!(ic = l->data)) continue;
      evas_object_show(ic->o_holder);
      evas_object_show(ic->o_holder2);
      evas_object_show(ic->o_icon);
      evas_object_show(ic->o_icon2);
    }
}

void
notification_box_hide(Notification_Box *b)
{
  Evas_List *l;

  evas_object_hide(b->o_box);
  if (b->o_empty) evas_object_hide(b->o_empty);
  for (l = b->icons; l; l = l->next)
    {
      Notification_Box_Icon *ic;

      if (!(ic = l->data)) continue;
      evas_object_hide(ic->o_holder);
      evas_object_hide(ic->o_holder2);
      evas_object_hide(ic->o_icon);
      evas_object_hide(ic->o_icon2);
    }
}

Notification_Box *
notification_box_get(const char *id, Evas *evas)
{
  Evas_List *l;
  Notification_Box *b;

  /* Find old config */
  for (l = notification_cfg->n_box; l; l = l->next)
    {
      b = l->data;

      if (b->id && !strcmp(b->id, id))
        {
          _notification_box_evas_set(b, evas);
          notification_box_show(b);
          return b;
        }
    }

  b = _notification_box_new(id, evas);
  notification_cfg->n_box = evas_list_append(notification_cfg->n_box, b);
  return b;
}

Config_Item *
notification_box_config_item_get(const char *id)
{
  Evas_List *l;
  Config_Item *ci;
  char buf[128];

  if (!id)
    {
      snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, ++uuid);
      id = buf;
    }
  else
    {
      /* Find old config */
      for (l = notification_cfg->items; l; l = l->next)
        {
          ci = l->data;
          if ((ci->id) && (!strcmp(ci->id, id)))
            return ci;
        }
    }

  ci = E_NEW(Config_Item, 1);
  ci->id = evas_stringshare_add(id);
  ci->show_label     = 1;
  ci->show_popup     = 1;
  ci->focus_window   = 1;
  ci->store_low      = 1;
  ci->store_normal   = 1;
  ci->store_critical = 0;
  notification_cfg->items = evas_list_append(notification_cfg->items, ci);

  return ci;
}

void
notification_box_orient_set(Notification_Box *b, int horizontal)
{
  e_box_orientation_set(b->o_box, horizontal);
  e_box_align_set(b->o_box, 0.5, 0.5);
}

void
notification_box_cb_obj_moveresize(void *data, 
                                   Evas *e __UNUSED__, 
                                   Evas_Object *obj __UNUSED__, 
                                   void *event_info __UNUSED__)
{
  Instance *inst;

  inst = data;
  _notification_box_resize_handle(inst->n_box);
}

int notification_box_cb_border_remove(void *data __UNUSED__, 
                                      int type __UNUSED__, 
                                      void *event)
{
  E_Event_Border_Remove *ev;
  Notification_Box_Icon *ic;
  Evas_List *l;

  ev = event;
  for (l = notification_cfg->instances; l; l = l->next)
    {
      Instance *inst;
      Notification_Box *b;

      if (!(inst = l->data)) continue;
      b = inst->n_box;

      ic = _notification_box_icon_find(b, ev->border, 0);
      if (!ic) continue;
      b->icons = evas_list_remove(b->icons, ic);
      _notification_box_icon_free(ic);
      _notification_box_empty_handle(b);
      _notification_box_resize_handle(b);
      _gc_orient(inst->gcc);
    }
  return 1;
}

static Notification_Box *
_notification_box_new(const char *id, Evas *evas)
{
  Notification_Box *b;

  b = E_NEW(Notification_Box, 1);
  b->id = evas_stringshare_add(id);
  b->o_box = e_box_add(evas);
  e_box_homogenous_set(b->o_box, 1);
  e_box_orientation_set(b->o_box, 1);
  e_box_align_set(b->o_box, 0.5, 0.5);
  _notification_box_empty(b);
  return b;
}

static void
_notification_box_free(Notification_Box *b)
{
  _notification_box_empty(b);
  evas_stringshare_del(b->id);
  evas_object_del(b->o_box);
  if (b->o_empty) evas_object_del(b->o_empty);
  b->o_empty = NULL;
  free(b);
}

static void
_notification_box_evas_set(Notification_Box *b, Evas *evas)
{
  Evas_List *l, *new_icons=NULL;

  evas_object_del(b->o_box);
  if (b->o_empty) evas_object_del(b->o_empty);
  b->o_empty = NULL;
  b->o_box = e_box_add(evas);

  e_box_homogenous_set(b->o_box, 1);
  e_box_orientation_set(b->o_box, 1);
  e_box_align_set(b->o_box, 0.5, 0.5);

  for (l = b->icons; l; l = l->next)
    {
      Notification_Box_Icon *ic, *new_ic;

      ic = l->data;
      if (!ic) continue;

      new_ic = _notification_box_icon_new(b, ic->notif, ic->border, ic->n_id);
      _notification_box_icon_free(ic);
      new_icons = evas_list_append(new_icons, new_ic);

      e_box_pack_end(b->o_box, new_ic->o_holder);
    }
  evas_list_free(b->icons);
  b->icons = new_icons;
  _notification_box_empty_handle(b);
  _notification_box_resize_handle(b);
}

static void
_notification_box_empty(Notification_Box *b)
{
  while (b->icons)
    {
      _notification_box_icon_free(b->icons->data);
      b->icons = evas_list_remove_list(b->icons, b->icons);
    }
  _notification_box_empty_handle(b);
}

static void
_notification_box_resize_handle(Notification_Box *b)
{
  Evas_List *l;
  Notification_Box_Icon *ic;
  Evas_Coord w, h;

  evas_object_geometry_get(b->o_box, NULL, NULL, &w, &h);
  if (e_box_orientation_get(b->o_box))
    w = h;
  else
    h = w;
  e_box_freeze(b->o_box);
  for (l = b->icons; l; l = l->next)
    {
      ic = l->data;
      e_box_pack_options_set(ic->o_holder,
                             1, 1, /* fill */
                             0, 0, /* expand */
                             0.5, 0.5, /* align */
                             w, h, /* min */
                             w, h /* max */
                            );
    }
  e_box_thaw(b->o_box);
}

static void
_notification_box_empty_handle(Notification_Box *b)
{
  if (!b->icons)
    {
      if (!b->o_empty)
        {
          Evas_Coord w, h;

          b->o_empty = evas_object_rectangle_add(evas_object_evas_get(b->o_box));
          evas_object_event_callback_add(b->o_empty, EVAS_CALLBACK_MOUSE_DOWN, 
                                         _notification_box_cb_empty_mouse_down, b);
          evas_object_color_set(b->o_empty, 0, 0, 0, 0);
          evas_object_show(b->o_empty);
          e_box_pack_end(b->o_box, b->o_empty);
          evas_object_geometry_get(b->o_box, NULL, NULL, &w, &h);
          if (e_box_orientation_get(b->o_box))
            w = h;
          else
            h = w;
          e_box_pack_options_set(b->o_empty,
                                 1, 1, /* fill */
                                 1, 1, /* expand */
                                 0.5, 0.5, /* align */
                                 w, h, /* min */
                                 9999, 9999 /* max */
                                );
        }
    }
  else if (b->o_empty)
    {
      evas_object_del(b->o_empty);
      b->o_empty = NULL;
    }
}

static Evas_List *
_notification_box_find(E_Notification_Urgency urgency)
{
  Evas_List *n_box = NULL;
  Evas_List *l;

  for (l = notification_cfg->instances; l; l = l->next)
    {
      Instance *inst;

      inst = l->data;
      if (urgency == E_NOTIFICATION_URGENCY_LOW && inst->ci->store_low)
        n_box = evas_list_append(n_box, inst->n_box);
      else if (urgency == E_NOTIFICATION_URGENCY_NORMAL && inst->ci->store_normal)
        n_box = evas_list_append(n_box, inst->n_box);
      else if (urgency == E_NOTIFICATION_URGENCY_CRITICAL && inst->ci->store_critical)
        n_box = evas_list_append(n_box, inst->n_box);
    }
  return n_box;
}

static Notification_Box_Icon *
_notification_box_icon_new(Notification_Box *b, E_Notification *n, E_Border *bd, unsigned int id)
{
  Notification_Box_Icon *ic;

  ic = E_NEW(Notification_Box_Icon, 1);
  if (bd) e_object_ref(E_OBJECT(bd));
  e_notification_ref(n);
  ic->label = e_notification_app_name_get(n);
  ic->n_box = b;
  ic->n_id = id;
  ic->border = bd;
  ic->notif = n;
  ic->o_holder = edje_object_add(evas_object_evas_get(b->o_box));
  e_theme_edje_object_set(ic->o_holder, "base/theme/modules/ibox",
                          "e/modules/ibox/icon");
  evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_IN,  
                                 _notification_box_cb_icon_mouse_in,  ic);
  evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_OUT, 
                                 _notification_box_cb_icon_mouse_out, ic);
  evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_DOWN, 
                                 _notification_box_cb_icon_mouse_down, ic);
  evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_UP, 
                                 _notification_box_cb_icon_mouse_up, ic);
  evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOVE, 
                                 _notification_box_cb_icon_move, ic);
  evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_RESIZE, 
                                 _notification_box_cb_icon_resize, ic);
  evas_object_show(ic->o_holder);

  ic->o_holder2 = edje_object_add(evas_object_evas_get(b->o_box));
  e_theme_edje_object_set(ic->o_holder2, "base/theme/modules/ibox",
                          "e/modules/ibox/icon_overlay");
  evas_object_layer_set(ic->o_holder2, 9999);
  evas_object_pass_events_set(ic->o_holder2, 1);
  evas_object_show(ic->o_holder2);

  _notification_box_icon_fill(ic, n);
  return ic;
}

static void
_notification_box_icon_free(Notification_Box_Icon *ic)
{
  if (notification_cfg->menu)
    {
      e_menu_post_deactivate_callback_set(notification_cfg->menu, NULL, NULL);
      e_object_del(E_OBJECT(notification_cfg->menu));
      notification_cfg->menu = NULL;
    }
  _notification_box_icon_empty(ic);
  evas_object_del(ic->o_holder);
  evas_object_del(ic->o_holder2);
  if (ic->border) e_object_unref(E_OBJECT(ic->border));
  if (ic->notif) e_notification_unref(ic->notif);
  free(ic);
}

static void
_notification_box_icon_fill(Notification_Box_Icon *ic, E_Notification *n)
{
  void *img;
  const char *icon_path;
  Evas_Object *app_icon;
  Evas_Object *dummy = NULL;
  int w, h = 0;

  if ((icon_path = e_notification_app_icon_get(n)) && *icon_path)
    {
      if (!strncmp(icon_path, "file://", 7)) icon_path += 7;
      app_icon = evas_object_image_add(evas_object_evas_get(ic->n_box->o_box));
      evas_object_image_load_scale_down_set(app_icon, 1);
      evas_object_image_load_size_set(app_icon, 80, 80);
      evas_object_image_file_set(app_icon, icon_path, NULL);
      evas_object_image_fill_set(app_icon, 0, 0, 80, 80);
    }
  else if ((img = e_notification_hint_icon_data_get(n)))
    {
      app_icon = e_notification_image_evas_object_add(evas_object_evas_get(ic->n_box->o_box), img);
    }
  else
    {
      char buf[PATH_MAX];

      snprintf(buf, sizeof(buf), "%s/e-module-notification.edj", notification_mod->dir);
      dummy = edje_object_add(evas_object_evas_get(ic->n_box->o_box));
      if (!e_theme_edje_object_set(dummy, "base/theme/modules/notification",
                                   "modules/notification/logo"))
        edje_object_file_set(dummy, buf, "modules/notification/logo");
      evas_object_resize(dummy, 80, 80);
      app_icon = edje_object_part_object_get(dummy, "image");
    }
  evas_object_image_size_get(app_icon, &w, &h);

  ic->o_icon = e_icon_add(evas_object_evas_get(ic->n_box->o_box));
  e_icon_alpha_set(ic->o_icon, 1);
  e_icon_data_set(ic->o_icon, evas_object_image_data_get(app_icon, 0), w, h);
  edje_object_part_swallow(ic->o_holder, "e.swallow.content", ic->o_icon);
  evas_object_pass_events_set(ic->o_icon, 1);
  evas_object_show(ic->o_icon);

  ic->o_icon2 = e_icon_add(evas_object_evas_get(ic->n_box->o_box));
  e_icon_alpha_set(ic->o_icon2, 1);
  e_icon_data_set(ic->o_icon2, evas_object_image_data_get(app_icon, 0), w, h);
  edje_object_part_swallow(ic->o_holder2, "e.swallow.content", ic->o_icon2);
  evas_object_pass_events_set(ic->o_icon2, 1);
  evas_object_show(ic->o_icon2);

  if (dummy) evas_object_del(dummy);
  evas_object_del(app_icon);
  _notification_box_icon_fill_label(ic);
}

static void
_notification_box_icon_fill_label(Notification_Box_Icon *ic)
{
  const char *label = NULL;

  if (ic->border) 
    label = ic->border->client.netwm.name;

  if (!label) label = e_notification_app_name_get(ic->notif);
  edje_object_part_text_set(ic->o_holder, "e.text.label", label);
  edje_object_part_text_set(ic->o_holder2, "e.text.label", label);
}

static void
_notification_box_icon_empty(Notification_Box_Icon *ic)
{
  if (ic->o_icon) evas_object_del(ic->o_icon);
  if (ic->o_icon2) evas_object_del(ic->o_icon2);
  ic->o_icon = NULL;
  ic->o_icon2 = NULL;
}

static Notification_Box_Icon *
_notification_box_icon_find(Notification_Box *b, E_Border *bd, unsigned int n_id)
{
  Evas_List *l;

  for (l = b->icons; l; l = l->next)
    {
      Notification_Box_Icon *ic;

      if (!(ic = l->data)) continue;
      if (ic->border == bd || ic->n_id == n_id)
        return ic;
    }

  return NULL;
}

static void
_notification_box_icon_signal_emit(Notification_Box_Icon *ic, char *sig, char *src)
{
  if (ic->o_holder)
    edje_object_signal_emit(ic->o_holder, sig, src);
  if (ic->o_icon)
    edje_object_signal_emit(ic->o_icon, sig, src);
  if (ic->o_holder2)
    edje_object_signal_emit(ic->o_holder2, sig, src);
  if (ic->o_icon2)
    edje_object_signal_emit(ic->o_icon2, sig, src);
}

static E_Border *
_notification_find_source_border(E_Notification *n)
{
  const char *app_name;
  Evas_List *l;

  if (!(app_name = e_notification_app_name_get(n))) return NULL;

  for (l = e_border_client_list(); l; l = l->next)
    {
      int compare_len;
      E_Border *bd = l->data;

      if (!bd) continue;
      /* We can't be sure that the app_name really match the application name.
       * Some plugin put their name instead. But this search gives some good
       * results.
       */
      if (bd->client.icccm.name)
        {
          compare_len = MIN_LEN(bd->client.icccm.name, app_name);
          if (!strncasecmp(bd->client.icccm.name, app_name, compare_len))
            return bd;
        }
      if (bd->client.icccm.class)
        {
          compare_len = MIN_LEN(bd->client.icccm.class, app_name);
          if (!strncasecmp(bd->client.icccm.class, app_name, compare_len))
            return bd;
        }
    }
  return NULL;
}

static void
_notification_box_cb_menu_post(void *data __UNUSED__, E_Menu *m __UNUSED__)
{
  if (!notification_cfg->menu) return;
  e_object_del(E_OBJECT(notification_cfg->menu));
  notification_cfg->menu = NULL;
}

static void
_notification_box_cb_empty_mouse_down(void *data, 
                                      Evas *e __UNUSED__, 
                                      Evas_Object *obj __UNUSED__, 
                                      void *event_info __UNUSED__)
{
  Evas_Event_Mouse_Down *ev;
  Notification_Box *b;

  ev = event_info;
  b = data;
  if (!notification_cfg->menu)
    {
      E_Menu *mn;
      E_Menu_Item *mi;
      int cx, cy, cw, ch;

      mn = e_menu_new();
      e_menu_post_deactivate_callback_set(mn, _notification_box_cb_menu_post, NULL);
      notification_cfg->menu = mn;

      mi = e_menu_item_new(mn);
      e_menu_item_label_set(mi, D_("Configuration"));
      e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
      e_menu_item_callback_set(mi, _notification_box_cb_menu_configuration, b);

      mi = e_menu_item_new(mn);
      e_menu_item_separator_set(mi, 1);

      e_gadcon_client_util_menu_items_append(b->inst->gcc, mn, 0);

      e_gadcon_canvas_zone_geometry_get(b->inst->gcc->gadcon,
                                        &cx, &cy, &cw, &ch);
      e_menu_activate_mouse(mn,
                            e_util_zone_current_get(e_manager_current_get()),
                            cx + ev->output.x, cy + ev->output.y, 1, 1,
                            E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
    }
}

static void
_notification_box_cb_icon_move(void *data, 
                               Evas *e __UNUSED__, 
                               Evas_Object *obj __UNUSED__, 
                               void *event_info __UNUSED__)
{
  Notification_Box_Icon *ic;
  Evas_Coord x, y;

  ic = data;
  evas_object_geometry_get(ic->o_holder, &x, &y, NULL, NULL);
  evas_object_move(ic->o_holder2, x, y);
  evas_object_raise(ic->o_holder2);
}

static void
_notification_box_cb_icon_resize(void *data, 
                                 Evas *e __UNUSED__, 
                                 Evas_Object *obj __UNUSED__, 
                                 void *event_info __UNUSED__)
{
  Notification_Box_Icon *ic;
  Evas_Coord w, h;

  ic = data;
  evas_object_geometry_get(ic->o_holder, NULL, NULL, &w, &h);
  evas_object_resize(ic->o_holder2, w, h);
  evas_object_raise(ic->o_holder2);
}

static void
_notification_box_cb_icon_mouse_in(void *data, 
                                   Evas *e __UNUSED__, 
                                   Evas_Object *obj __UNUSED__, 
                                   void *event_info __UNUSED__)
{
  Notification_Box_Icon *ic;
  Config_Item *ci;

  if (!(ic = data) || !ic->n_box || !ic->n_box->inst) return;
  if (!(ci = ic->n_box->inst->ci)) return;

  _notification_box_icon_signal_emit(ic, "e,state,focused", "e");
  if (ci->show_label)
    {
      _notification_box_icon_fill_label(ic);
      _notification_box_icon_signal_emit(ic, "e,action,show,label", "e");
    }
  if (ci->show_popup && !ic->popup && !ic->mouse_in_timer)
    ic->mouse_in_timer = ecore_timer_add(0.5, _notification_box_cb_icon_mouse_still_in, ic);
}

static void
_notification_box_cb_icon_mouse_out(void *data, 
                                    Evas *e __UNUSED__, 
                                    Evas_Object *obj __UNUSED__, 
                                    void *event_info __UNUSED__)
{
  Notification_Box_Icon *ic;

  ic = data;
  _notification_box_icon_signal_emit(ic, "e,state,unfocused", "e");
  if (ic->n_box->inst->ci->show_label)
    _notification_box_icon_signal_emit(ic, "e,action,hide,label", "e");

  if (ic->mouse_in_timer)
    {
      ecore_timer_del(ic->mouse_in_timer);
      ic->mouse_in_timer = NULL;
    }
  if (ic->popup)
    {
      notification_popup_close(e_notification_id_get(ic->notif));
      ic->popup = 0;
    }
}

static void
_notification_box_cb_icon_mouse_up(void *data, 
                                   Evas *e __UNUSED__, 
                                   Evas_Object *obj __UNUSED__, 
                                   void *event_info)
{
  Evas_Event_Mouse_Up *ev;
  Notification_Box *b;
  Notification_Box_Icon *ic;

  ev = event_info;
  ic = data;
  b = ic->n_box;
  if ((ev->button == 1))
    {
      if (b->inst->ci->focus_window && ic->border)
        {
          e_border_uniconify(ic->border);
          e_desk_show(ic->border->desk);
          e_border_show(ic->border);
          e_border_raise(ic->border);
          e_border_focus_set(ic->border, 1, 1);
        }
      b->icons = evas_list_remove(b->icons, ic);
      _notification_box_icon_free(ic);
      _notification_box_empty_handle(b);
      _notification_box_resize_handle(b);
      _gc_orient(b->inst->gcc);
    }
}

static void
_notification_box_cb_icon_mouse_down(void *data, 
                                     Evas *e __UNUSED__, 
                                     Evas_Object *obj __UNUSED__, 
                                     void *event_info)
{
  Evas_Event_Mouse_Down *ev;
  Notification_Box_Icon *ic;

  ev = event_info;
  ic = data;
  if ((ev->button == 3) && (!notification_cfg->menu))
    {
      E_Menu *mn;
      E_Menu_Item *mi;
      int cx, cy, cw, ch;

      mn = e_menu_new();
      e_menu_post_deactivate_callback_set(mn, _notification_box_cb_menu_post, NULL);
      notification_cfg->menu = mn;

      mi = e_menu_item_new(mn);
      e_menu_item_label_set(mi, D_("Configuration"));
      e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
      e_menu_item_callback_set(mi, _notification_box_cb_menu_configuration, ic->n_box);

      mi = e_menu_item_new(mn);
      e_menu_item_separator_set(mi, 1);

      e_gadcon_client_util_menu_items_append(ic->n_box->inst->gcc, mn, 0);

      e_gadcon_canvas_zone_geometry_get(ic->n_box->inst->gcc->gadcon,
                                        &cx, &cy, &cw, &ch);
      e_menu_activate_mouse(mn,
                            e_util_zone_current_get(e_manager_current_get()),
                            cx + ev->output.x, cy + ev->output.y, 1, 1,
                            E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
    }
}

static int
_notification_box_cb_icon_mouse_still_in(void *data)
{
  Notification_Box_Icon *ic;

  ic = data;
  e_notification_timeout_set(ic->notif, 0);
  e_notification_hint_urgency_set(ic->notif, 4);
  ic->popup = notification_popup_notify(ic->notif, 
                                        e_notification_id_get(ic->notif),
                                        e_notification_id_get(ic->notif));
  ecore_timer_del(ic->mouse_in_timer);
  ic->mouse_in_timer = NULL;
  return 0;
}

static void
_notification_box_cb_menu_configuration(void *data, 
                                        E_Menu *m __UNUSED__, 
                                        E_Menu_Item *mi __UNUSED__)
{
  Notification_Box *b;
  int ok = 1;
  Evas_List *l;

  b = data;
  for (l = notification_cfg->config_dialog; l; l = l->next)
    {
      E_Config_Dialog *cfd;

      cfd = l->data;
      if (cfd->data == b->inst->ci)
        {
          ok = 0;
          break;
        }
    }
  if (ok) config_notification_box_module(b->inst->ci);
}
