#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <e.h>
#include <Ecore.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

static int bar_count;
static Ecore_Event_Handler *_mount_exe_exit_handler = NULL;

/* Private Module Routines Prototypes */
static void _parse_fstab(Mount *mnt);
static int _parse_file(char *file, Mount_Point *mntpoint);
static void _setup_mountpoints(Mount *mnt);
static int _is_mounted(Mount_Point *mntpoint);

static Mount *_mount_new(E_Module *m);
static int _mount_box_init(Mount_Box *mntbox);
static Mount_Icon *_mount_icon_new(Mount_Box *mntbox, Mount_Point *point);
static void _mount_mountpoint(Mount_Icon *mnticon);
static void _unmount_mountpoint(Mount_Icon *mnticon);
static void _eject_mountpoint(Mount_Icon *mnticon);
static void _mount_free(Mount *mnt);
static void _mount_box_free(Mount_Box *mntbox);
static void _mount_box_icon_free(Mount_Icon *mnticon);
static void _mount_config_menu_new(Mount *mnt);
static void _mount_box_menu_new(Mount_Box *mntbox);
static void _mount_box_disable(Mount_Box *mntbox);
static void _mount_box_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi);
static void _mount_box_cb_menu_configure(void *data, E_Menu *mn,
                                         E_Menu_Item *mi);

static void _mount_box_cb_gmc_change(void *data, E_Gadman_Client *gmc,
                                     E_Gadman_Change change);
static void _mount_box_cb_intercept_move(void *data, Evas_Object *o,
                                         Evas_Coord x, Evas_Coord y);
static void _mount_box_cb_intercept_resize(void *data, Evas_Object *o,
                                           Evas_Coord w, Evas_Coord h);
static void _mount_icon_cb_intercept_move(void *data, Evas_Object *o,
                                          Evas_Coord x, Evas_Coord y);
static void _mount_icon_cb_intercept_resize(void *data, Evas_Object *o,
                                            Evas_Coord w, Evas_Coord h);
static void _mount_box_iconsize_change(Mount_Box *mntbox);
static void _mount_box_cb_mouse_down(void *data, Evas *e,
                                     Evas_Object *obj, void *event_info);
static void _change_orient_horiz(Mount *mnt);
static void _change_orient_vert(Mount *mnt);
static void _set_state(Mount_Icon *ic);
static void _mount_icon_cb_edje_mouse_down(void *data, Evas_Object *o,
                                           const char *emission,
                                           const char *source);
static int _mount_exe_cb_exit(void *data, int type, void *event);

/* Begin Required Module Routines */
EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "Mount"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   Mount *mnt;

   mnt = _mount_new(m);
   m->config_menu = mnt->config_menu;
   return mnt;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   Mount *mnt;

   mnt = m->data;
   if (mnt)
     {
        if (m->config_menu)
          {
             e_menu_deactivate(m->config_menu);
             e_object_del(E_OBJECT(m->config_menu));
             m->config_menu = NULL;
          }
        if (mnt->config_dialog)
          {
             e_object_del(E_OBJECT(mnt->config_dialog));
             mnt->config_dialog = NULL;
          }
        _mount_free(mnt);
     }
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   Mount *mnt;

   mnt = m->data;
   if (mnt)
      e_config_domain_save("module.mount", mnt->conf_edd, mnt->conf);

   return 1;
}

EAPI int
e_modapi_info(E_Module *m)
{
   m->icon_file = strdup(PACKAGE_DATA_DIR "/module_icon.png");
   return 1;
}

EAPI int
e_modapi_about(E_Module *m)
{
   e_module_dialog_show(_("Enlightenment Mount Module"),
                        _("This is a mount module to mount/unmount devices."));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m)
{
   Mount *mnt;

   mnt = m->data;
   if (mnt)
      _config_mount_module(mnt->con, mnt);
   return 1;
}
/* End Required Module Routines */

/* Begin Private Module Routines */
static void
_setup_mountpoints(Mount *mnt)
{
   /* Parse fstab for user mountable devices */
   _parse_fstab(mnt);
}

static void
_parse_fstab(Mount *mnt)
{
   FILE *f;
   char s[1024];
   char *token = NULL;
   char *info[4];
   int i, user_mnt;
   char *p;
   Mount_Point *point;

   if ((f = fopen(FSTAB, "r")) == NULL)
     {
        e_module_dialog_show(_("Mount Module"), _("Cannot Parse fstab"));
        return;
     }

   *s = 0;
   for (; fgets(s, sizeof(s), f);)
     {
        /* skip comments and blank lines */
        if (!(*s) || (*s == '\n') || (*s == '#'))
           continue;
        for (i = 0; i < 4; i++)
          {
             info[i] = NULL;
          }

        /* parse tokens we need */
        i = 0;
        token = strtok(s, " \t");
        if (token)
           info[i++] = strdup(token);
        while ((token = strtok(NULL, " \t")) && (i < 4))
           info[i++] = strdup(token);

        /* see if device is mountable by user */
        user_mnt = 0;
        p = info[3];
        while ((p = strstr(p, "user")))
          {
             if (p != strstr(info[3], "user_xattr"))
               {
                  user_mnt = 1;
                  break;
               }
             p++;
          }

        if (user_mnt)
          {
             point = E_NEW(Mount_Point, 1);

             if (!point)
                return;
             point->device = strdup(info[0]);
             point->path = strdup(info[1]);
             if ((strstr(point->path, "cdrw")) ||
                 (strstr(point->path, "cdrecorder")))
               {
                  point->part = strdup("cdrw");
                  point->eject_mode = AUTO_EJECT;
               }
             else if (strstr(point->path, "hd"))
               {
                  point->part = strdup("harddisk");
                  point->eject_mode = MANUAL_EJECT;
               }
             else if (strstr(point->path, "cdrom"))
               {
                  point->part = strdup("cdrom");
                  point->eject_mode = AUTO_EJECT;
               }
             else if (strstr(point->path, "zip"))
               {
                  point->part = strdup("zip");
                  point->eject_mode = MANUAL_EJECT;
               }
             else if (strstr(point->path, "dvd"))
               {
                  point->part = strdup("dvd");
                  point->eject_mode = AUTO_EJECT;
               }
             else if (strstr(point->path, "floppy"))
               {
                  point->part = strdup("floppy");
                  point->eject_mode = MANUAL_EJECT;
               }
             else if (strstr(point->path, "usb"))
               {
                  point->part = strdup("usb");
                  point->eject_mode = MANUAL_EJECT;
               }
             else if (strstr(point->path, "stick"))
               {
                  point->part = strdup("usb");
                  point->eject_mode = MANUAL_EJECT;
               }
             else if (strstr(point->path, "psp"))
               {
                  point->part = strdup("psp");
                  point->eject_mode = MANUAL_EJECT;
               }
             else if (strstr(point->path, "ipod"))
               {
                  point->part = strdup("ipod");
                  point->eject_mode = MANUAL_EJECT;
               }
             else if (strstr(point->path, "camera"))
               {
                  point->part = strdup("camera");
                  point->eject_mode = MANUAL_EJECT;
               }
             else if (strstr(point->path, "smb"))
               {
                  point->part = strdup("network");
                  point->eject_mode = MANUAL_EJECT;
               }
             else
               {
                  point->part = strdup("harddisk");
                  point->eject_mode = MANUAL_EJECT;
               }
             point->mounted = _is_mounted(point);
             mnt->mntpoints = evas_list_append(mnt->mntpoints, point);
          }
     }

   for (i = 0; i < 4; i++)
     {
        if (info[i])
           free(info[i]);
     }

   fclose(f);
   return;
}

static int
_parse_file(char *fl, Mount_Point *mntpoint)
{
   FILE *f;
   char s[1024];
   char *token = NULL;
   char *device = NULL;
   char *path = NULL;
   int mounted;

   mounted = 0;
   if ((f = fopen(fl, "r")) == NULL)
      return -1;
   *s = 0;
   for (; fgets(s, sizeof(s), f);)
     {
        /* skip comments */
        if (!(*s) || (*s == '\n') || (*s == '#'))
           continue;
        token = strtok(s, " \t");
        if (token)
           device = strdup(token);
        token = NULL;
        token = strtok(NULL, " \t");
        if (token)
           path = strdup(token);
        if (path)
          {
             if (!strcmp(mntpoint->path, path))
                mounted = 1;
          }
     }
   fclose(f);
   free(device);
   free(path);
   return mounted;
}

static int
_is_mounted(Mount_Point *mntpoint)
{
   int mounted;

   /* Check proc mounts */
   mounted = _parse_file(PROCMOUNTS, mntpoint);
   if (mounted <= 0)
     {
        /* Check mtab */
        mounted = _parse_file(MTAB, mntpoint);
        if (mounted <= 0)
           mounted = 0;
     }
   return mounted;
}

static void
_mount_free(Mount *mnt)
{
   _mount_box_free(mnt->box);

   if (mnt->config_menu_options)
      e_object_del(E_OBJECT(mnt->config_menu_options));
   if (mnt->config_menu_size)
      e_object_del(E_OBJECT(mnt->config_menu_size));
   if (mnt->config_menu)
      e_object_del(E_OBJECT(mnt->config_menu));

   while (mnt->mntpoints)
     {
        Mount_Point *point;

        point = mnt->mntpoints->data;
        if (point->device)
           free(point->device);
        if (point->path)
           free(point->path);
        if (point->part)
           free(point->part);

        mnt->mntpoints = evas_list_remove_list(mnt->mntpoints, mnt->mntpoints);
     }

   free(mnt->conf);
   E_CONFIG_DD_FREE(mnt->conf_edd);

   free(mnt);
}

static void
_mount_box_free(Mount_Box *mntbox)
{
   if (mntbox->con)
      e_object_unref(E_OBJECT(mntbox->con));
   if (mntbox->menu)
      e_object_del(E_OBJECT(mntbox->menu));

   while (mntbox->icons)
      _mount_box_icon_free(mntbox->icons->data);

   if (mntbox->bar_object)
      evas_object_del(mntbox->bar_object);
   if (mntbox->box_object)
      evas_object_del(mntbox->box_object);
   if (mntbox->event_object)
      evas_object_del(mntbox->event_object);

   if (mntbox->gmc)
      e_gadman_client_save(mntbox->gmc);
   if (mntbox->gmc)
      e_object_del(E_OBJECT(mntbox->gmc));

   free(mntbox->conf);
   E_CONFIG_DD_FREE(mntbox->conf_box_edd);

   free(mntbox);
   bar_count--;
}

static void
_mount_box_icon_free(Mount_Icon *mnticon)
{
   mnticon->mntbox->icons = evas_list_remove(mnticon->mntbox->icons, mnticon);

   edje_object_signal_callback_del(mnticon->icon_object, "mouse,down,1", "icon",
                                   _mount_icon_cb_edje_mouse_down);
   if (mnticon->bg_object)
      evas_object_del(mnticon->bg_object);
   if (mnticon->icon_object)
      evas_object_del(mnticon->icon_object);
   if (mnticon->overlay_object)
      evas_object_del(mnticon->overlay_object);
   if (mnticon->txt_object)
      evas_object_del(mnticon->txt_object);
   free(mnticon);
}

static Mount *
_mount_new(E_Module *m)
{
   Mount *mnt;
   Evas_List *managers, *l, *l2;

   bar_count = 0;

   /* Create New Mount Module Structure */
   mnt = E_NEW(Mount, 1);

   if (!mnt)
      return NULL;

   /* Setup Initial Module Config */
   mnt->conf_edd = E_CONFIG_DD_NEW("Mount_Config", Config);
#undef T
#undef D
#define T Config
#define D mnt->conf_edd
   E_CONFIG_VAL(D, T, icon_size, INT);
   E_CONFIG_VAL(D, T, orientation, INT);
   E_CONFIG_VAL(D, T, show_labels, UCHAR);
   //E_CONFIG_LIST(D, T, bars, conf_box_edd);

   /* Try To Load An Existing Config */
   mnt->conf = e_config_domain_load("module.mount", mnt->conf_edd);
   if (!mnt->conf)
     {
        /* Create New Config */
        mnt->conf = E_NEW(Config, 1);

        mnt->conf->icon_size = 24;
        mnt->conf->orientation = MOUNT_ORIENT_HORIZ;
        mnt->conf->show_labels = 1;
     }

   /* Set Config Limits */
   E_CONFIG_LIMIT(mnt->conf->icon_size, 8, 128);
   E_CONFIG_LIMIT(mnt->conf->orientation, MOUNT_ORIENT_VERT,
                  MOUNT_ORIENT_HORIZ);
   //E_CONFIG_LIMIT(mnt->conf->show_labels, 0, 1);

   /* Create New Config Menu */
   _mount_config_menu_new(mnt);

   /* Setup Mount Points */
   _setup_mountpoints(mnt);

   managers = e_manager_list();

   for (l = managers; l; l = l->next)
     {
        E_Manager *man;

        man = l->data;
        for (l2 = man->containers; l2; l2 = l2->next)
          {
             E_Container *con;
             Mount_Box *mntbox;
             E_Menu_Item *mi;

             con = l2->data;
             mntbox = E_NEW(Mount_Box, 1);

             if (mntbox)
               {
                  mntbox->conf_box_edd =
                     E_CONFIG_DD_NEW("Mount_Config_Box", Config_Box);
#undef T
#undef D
#define T Config_Box
#define D mntbox->conf_box_edd
                  E_CONFIG_VAL(D, T, enabled, UCHAR);

                  mntbox->con = con;
                  mntbox->evas = con->bg_evas;
                  mntbox->conf = E_NEW(Config_Box, 1);
                  mntbox->conf->enabled = 1;
                  mnt->box = mntbox;
                  mntbox->mnt = mnt;

                  if (!_mount_box_init(mntbox))
                     return NULL;
                  _mount_box_menu_new(mntbox);

                  mi = e_menu_item_new(mnt->config_menu);
                  e_menu_item_label_set(mi, _("Configuration"));
                  e_menu_item_callback_set(mi, _mount_box_cb_menu_configure,
                                           mntbox);

                  mi = e_menu_item_new(mnt->config_menu);
                  e_menu_item_label_set(mi, con->name);
                  e_menu_item_submenu_set(mi, mntbox->menu);

                  if (!mntbox->conf->enabled)
                     _mount_box_disable(mntbox);
               }
          }
     }
   return mnt;
}

static void
_mount_config_menu_new(Mount *mnt)
{
   E_Menu *mn;

   mn = e_menu_new();
   mnt->config_menu = mn;
}

static int
_mount_box_init(Mount_Box *mntbox)
{
   Evas_Object *o;
   Evas_List *l;
   E_Gadman_Policy policy;
   Evas_Coord x, y, w, h;
   char buffer[1024];

   if (!mntbox)
      return 0;

   snprintf(buffer, sizeof(buffer), PACKAGE_DATA_DIR "/mount.edj");

   e_object_ref(E_OBJECT(mntbox->con));

   mntbox->x = mntbox->y = mntbox->w = mntbox->h = -1;

   evas_event_freeze(mntbox->evas);

   o = edje_object_add(mntbox->evas);
   mntbox->bar_object = o;
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/mount", "modules/mount/main"))
      edje_object_file_set(o, buffer, "modules/mount/main");
   evas_object_show(o);

   o = evas_object_rectangle_add(mntbox->evas);
   mntbox->event_object = o;
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
                                  _mount_box_cb_mouse_down, mntbox);
   evas_object_show(o);

   o = e_box_add(mntbox->evas);
   mntbox->box_object = o;
   evas_object_intercept_move_callback_add(o, _mount_box_cb_intercept_move,
                                           mntbox);
   evas_object_intercept_resize_callback_add(o, _mount_box_cb_intercept_resize,
                                             mntbox);
   e_box_freeze(o);
   edje_object_part_swallow(mntbox->bar_object, "items", o);
   evas_object_show(o);

   if (mntbox->mnt->mntpoints)
     {
        for (l = mntbox->mnt->mntpoints; l; l = l->next)
          {
             /* Create New Icon */
             Mount_Icon *mnticon;
             Mount_Point *point;

             point = l->data;
             mnticon = _mount_icon_new(mntbox, point);
             _set_state(mnticon);
          }
     }

   mntbox->align_req = 0.5;
   mntbox->align = 0.5;
   e_box_align_set(mntbox->box_object, 0.5, 0.5);

   e_box_thaw(mntbox->box_object);

   evas_object_resize(mntbox->bar_object, 1000, 1000);
   edje_object_calc_force(mntbox->bar_object);
   edje_object_part_geometry_get(mntbox->bar_object, "items", &x, &y, &w, &h);

   o = edje_object_add(mntbox->evas);
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/mount", "modules/mount/icon"))
      edje_object_file_set(o, buffer, "modules/mount/icon");
   evas_object_resize(o, mntbox->mnt->conf->icon_size,
                      mntbox->mnt->conf->icon_size);
   edje_object_calc_force(o);
   edje_object_part_geometry_get(o, "item", &x, &y, &w, &h);
   mntbox->icon_inset.l = x;
   mntbox->icon_inset.r = mntbox->mnt->conf->icon_size - (x + w);
   mntbox->icon_inset.t = y;
   mntbox->icon_inset.b = mntbox->mnt->conf->icon_size - (y + h);
   evas_object_del(o);

   policy =
      E_GADMAN_POLICY_ANYWHERE | E_GADMAN_POLICY_HMOVE | E_GADMAN_POLICY_VMOVE;
   mntbox->gmc = e_gadman_client_new(mntbox->con->gadman);
   e_gadman_client_domain_set(mntbox->gmc, "module.mount", bar_count++);
   e_gadman_client_policy_set(mntbox->gmc, policy);
   e_gadman_client_min_size_set(mntbox->gmc, 8, 8);
   e_gadman_client_max_size_set(mntbox->gmc, 3200, 3200);
   e_gadman_client_align_set(mntbox->gmc, 0.5, 0.5);
   e_gadman_client_resize(mntbox->gmc, 400, 40);
   e_gadman_client_change_func_set(mntbox->gmc, _mount_box_cb_gmc_change,
                                   mntbox);
   e_gadman_client_load(mntbox->gmc);

   evas_event_thaw(mntbox->evas);

   _mount_box_iconsize_change(mntbox);
   if (mntbox->mnt->conf->orientation == MOUNT_ORIENT_HORIZ)
     {
        _change_orient_horiz(mntbox->mnt);
     }
   else
     {
        _change_orient_vert(mntbox->mnt);
     }

   return 1;
}

static void
_mount_box_menu_new(Mount_Box *mntbox)
{
   E_Menu *mn;
   E_Menu_Item *mi;

   mn = e_menu_new();
   mntbox->menu = mn;

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_menu_item_callback_set(mi, _mount_box_cb_menu_configure, mntbox);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_menu_item_callback_set(mi, _mount_box_cb_menu_edit, mntbox);
}

static void
_mount_box_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Mount_Box *mntbox;
   Mount *mnt;

   mntbox = data;
   if (!mntbox)
      return;
   mnt = mntbox->mnt;
   _config_mount_module(mnt->con, mnt);
}

static void
_mount_box_disable(Mount_Box *mntbox)
{
   mntbox->conf->enabled = 0;
   if (mntbox->bar_object)
      evas_object_hide(mntbox->bar_object);
   if (mntbox->box_object)
      evas_object_hide(mntbox->box_object);
   if (mntbox->event_object)
      evas_object_hide(mntbox->event_object);
   e_config_save_queue();
}

static void
_mount_box_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi)
{
   Mount_Box *mntbox;

   mntbox = data;
   e_gadman_mode_set(mntbox->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void
_mount_box_cb_gmc_change(void *data, E_Gadman_Client *gmc,
                         E_Gadman_Change change)
{
   Mount_Box *mntbox;

   mntbox = data;
   switch (change)
     {
     case E_GADMAN_CHANGE_MOVE_RESIZE:
        e_gadman_client_geometry_get(mntbox->gmc, &mntbox->x, &mntbox->y,
                                     &mntbox->w, &mntbox->h);
        edje_extern_object_min_size_set(mntbox->box_object, 0, 0);
        edje_object_part_swallow(mntbox->bar_object, "items",
                                 mntbox->box_object);

        evas_object_move(mntbox->bar_object, mntbox->x, mntbox->y);
        evas_object_move(mntbox->event_object, mntbox->x, mntbox->y);
        evas_object_move(mntbox->box_object, mntbox->x, mntbox->y);

        evas_object_resize(mntbox->bar_object, mntbox->w, mntbox->h);
        evas_object_resize(mntbox->event_object, mntbox->w, mntbox->h);
        evas_object_resize(mntbox->box_object, mntbox->w, mntbox->h);
        break;
     case E_GADMAN_CHANGE_EDGE:
        break;
     case E_GADMAN_CHANGE_ZONE:
        break;
     case E_GADMAN_CHANGE_RAISE:
        break;
     }
}

static Mount_Icon *
_mount_icon_new(Mount_Box *mntbox, Mount_Point *point)
{
   Mount_Icon *mnticon;
   Evas_Object *o;
   Evas_Coord w, h, x, y;
   char buffer[1024];

   mnticon = E_NEW(Mount_Icon, 1);

   if (!mnticon)
      return NULL;

   snprintf(buffer, sizeof(buffer), PACKAGE_DATA_DIR "/mount.edj");
   mnticon->mntbox = mntbox;
   mnticon->mntpoint = point;
   mntbox->icons = evas_list_append(mntbox->icons, mnticon);

   o = edje_object_add(mntbox->evas);
   mnticon->bg_object = o;
   evas_object_intercept_move_callback_add(o, _mount_icon_cb_intercept_move,
                                           mnticon);
   evas_object_intercept_resize_callback_add(o, _mount_icon_cb_intercept_resize,
                                             mnticon);
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/mount", "modules/mount/icon"))
      edje_object_file_set(o, buffer, "modules/mount/icon");
   evas_object_show(o);

   o = edje_object_add(mntbox->evas);
   mnticon->icon_object = o;
   edje_extern_object_min_size_set(o, mntbox->mnt->conf->icon_size,
                                   mntbox->mnt->conf->icon_size);
   evas_object_resize(o, mntbox->mnt->conf->icon_size,
                      mntbox->mnt->conf->icon_size);
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/mount", "modules/mount/icon"))
      edje_object_file_set(o, buffer, "modules/mount/icon");
   edje_object_signal_callback_add(o, "mouse,down,1", "icon",
                                   _mount_icon_cb_edje_mouse_down, mnticon);
   edje_object_part_swallow(mnticon->bg_object, "item", o);
   evas_object_show(o);

   edje_object_signal_emit(o, "set_icon", point->part);
   edje_object_message_signal_process(o);

   o = edje_object_add(mntbox->evas);
   mnticon->overlay_object = o;
   evas_object_show(o);

   o = edje_object_add(mntbox->evas);
   mnticon->txt_object = o;
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/mount", "modules/mount/icon"))
      edje_object_file_set(o, buffer, "modules/mount/icon");
   evas_object_show(o);

   char *strip = strrchr(point->path, '/');

   if (!strip)
     {
        strip = point->path;
     }
   else
     {
        strip++;
     }
   edje_object_part_text_set(o, "label", strip);
   if (mntbox->mnt->conf->show_labels)
     {
        edje_object_signal_emit(o, "show_label", "");
     }
   else
     {
        edje_object_signal_emit(o, "hide_label", "");
     }
   edje_object_message_signal_process(o);

   o = edje_object_add(mntbox->evas);
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/mount", "modules/mount/icon"))
      edje_object_file_set(o, buffer, "modules/mount/icon");
   evas_object_resize(o, mntbox->mnt->conf->icon_size,
                      mntbox->mnt->conf->icon_size);
   edje_object_calc_force(o);
   edje_object_part_geometry_get(o, "item", &x, &y, &w, &h);
   mntbox->icon_inset.l = x;
   mntbox->icon_inset.r = mntbox->mnt->conf->icon_size - (x + w);
   mntbox->icon_inset.t = y;
   mntbox->icon_inset.b = mntbox->mnt->conf->icon_size - (y + h);
   evas_object_del(o);

   w = mntbox->mnt->conf->icon_size + mntbox->icon_inset.l +
      mntbox->icon_inset.r;
   h = mntbox->mnt->conf->icon_size + mntbox->icon_inset.t +
      mntbox->icon_inset.b;

   e_box_pack_end(mntbox->box_object, mnticon->bg_object);
   e_box_pack_options_set(mnticon->bg_object, 1, 1, 0, 0, 0.5, 0.5, w, h, w, h);

   return mnticon;
}

static void
_mount_box_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x,
                             Evas_Coord y)
{
   Mount_Box *mntbox;

   mntbox = data;
   evas_object_move(o, x, y);
   if (mntbox->bar_object)
      evas_object_move(mntbox->bar_object, x, y);
   if (mntbox->event_object)
      evas_object_move(mntbox->event_object, x, y);
   if (mntbox->box_object)
      evas_object_move(mntbox->box_object, x, y);
}

static void
_mount_box_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w,
                               Evas_Coord h)
{
   Mount_Box *mntbox;

   mntbox = data;
   evas_object_resize(o, w, h);
   if (mntbox->bar_object)
      evas_object_resize(mntbox->bar_object, w, h);
   if (mntbox->event_object)
      evas_object_resize(mntbox->event_object, w, h);
   if (mntbox->box_object)
      evas_object_resize(mntbox->box_object, w, h);
}

static void
_mount_icon_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x,
                              Evas_Coord y)
{
   Mount_Icon *mnticon;

   mnticon = data;
   evas_object_move(o, x, y);
   if (mnticon->bg_object)
      evas_object_move(mnticon->bg_object, x, y);
   if (mnticon->icon_object)
      evas_object_move(mnticon->icon_object, x, y);
   if (mnticon->overlay_object)
      evas_object_move(mnticon->overlay_object, x, y);
   if (mnticon->txt_object)
      evas_object_move(mnticon->txt_object, x, y);
}

static void
_mount_icon_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w,
                                Evas_Coord h)
{
   Mount_Icon *mnticon;

   mnticon = data;
   evas_object_resize(o, w, h);
   if (mnticon->bg_object)
      evas_object_resize(mnticon->bg_object, w, h);
   if (mnticon->overlay_object)
      evas_object_resize(mnticon->overlay_object, w, h);
   if (mnticon->icon_object)
      evas_object_resize(mnticon->icon_object, w, h);
   if (mnticon->txt_object)
      evas_object_resize(mnticon->txt_object, w, h);
}

static void
_mount_box_iconsize_change(Mount_Box *mntbox)
{
   Evas_List *l;
   Evas_Coord w, h, x, y;
   char buffer[1024];

   if ((mntbox->x == -1) || (mntbox->y == -1) ||
       (mntbox->w == -1) || (mntbox->h == -1))
      return;

   snprintf(buffer, sizeof(buffer), PACKAGE_DATA_DIR "/mount.edj");

   evas_event_freeze(mntbox->evas);
   e_box_freeze(mntbox->box_object);
   for (l = mntbox->icons; l; l = l->next)
     {
        Mount_Icon *ic;
        Evas_Object *o;

        ic = l->data;

        o = ic->icon_object;
        evas_object_resize(o, mntbox->mnt->conf->icon_size,
                           mntbox->mnt->conf->icon_size);
        edje_object_part_swallow(ic->bg_object, "item", o);

        o = ic->overlay_object;
        evas_object_resize(o, mntbox->mnt->conf->icon_size,
                           mntbox->mnt->conf->icon_size);

        o = edje_object_add(mntbox->evas);
        if (!e_theme_edje_object_set
            (o, "base/theme/modules/mount", "modules/mount/icon"))
           edje_object_file_set(o, buffer, "modules/mount/icon");
        evas_object_resize(o, mntbox->mnt->conf->icon_size,
                           mntbox->mnt->conf->icon_size);
        edje_object_calc_force(o);
        edje_object_part_geometry_get(o, "item", &x, &y, &w, &h);
        mntbox->icon_inset.l = x;
        mntbox->icon_inset.r = mntbox->mnt->conf->icon_size - (x + w);
        mntbox->icon_inset.t = y;
        mntbox->icon_inset.b = mntbox->mnt->conf->icon_size - (y + h);
        evas_object_del(o);

        w = mntbox->mnt->conf->icon_size + mntbox->icon_inset.l +
           mntbox->icon_inset.r;
        h = mntbox->mnt->conf->icon_size + mntbox->icon_inset.t +
           mntbox->icon_inset.b;

        e_box_pack_options_set(ic->bg_object, 1, 1,     /* fill */
                               0, 0,    /* expand */
                               0.0, 0.5,        /* align */
                               w, h,    /* min */
                               w, h     /* max */
           );
     }
   e_box_thaw(mntbox->box_object);
   evas_event_thaw(mntbox->evas);

   if (mntbox->mnt->conf->orientation == MOUNT_ORIENT_VERT)
     {
        e_gadman_client_resize(mntbox->gmc, w,
                               (h * (evas_list_count(mntbox->icons))));
     }
   else
     {
        e_gadman_client_resize(mntbox->gmc,
                               (w * (evas_list_count(mntbox->icons))), h);
     }
}

static void
_change_orient_horiz(Mount *mnt)
{
   Evas_List *l2;
   Evas_Object *o;
   Evas_Coord w, h;
   Mount_Box *mntbox;

   mntbox = mnt->box;
   evas_event_freeze(mntbox->evas);

   o = mntbox->bar_object;
   edje_object_signal_emit(o, "set_orientation", "horiz");
   edje_object_message_signal_process(o);

   e_box_freeze(mntbox->box_object);

   w = mntbox->mnt->conf->icon_size + mntbox->icon_inset.l +
      mntbox->icon_inset.r;
   h = mntbox->mnt->conf->icon_size + mntbox->icon_inset.t +
      mntbox->icon_inset.b;

   for (l2 = mntbox->icons; l2; l2 = l2->next)
     {
        Mount_Icon *ic;

        ic = l2->data;
        o = ic->bg_object;
        edje_object_signal_emit(o, "set_orientation", "horiz");
        edje_object_message_signal_process(o);

        o = ic->icon_object;
        edje_object_signal_emit(o, "set_orientation", "horiz");
        edje_object_message_signal_process(o);

        e_box_pack_options_set(ic->bg_object, 1, 1,     /* fill */
                               0, 0,    /* expand */
                               0.5, 0.5,        /* align */
                               w, h,    /* min */
                               w, h     /* max */
           );
     }
   e_box_align_set(mntbox->box_object, 0.5, 0.5);
   e_box_orientation_set(mntbox->box_object, 1);
   e_box_thaw(mntbox->box_object);
   evas_event_thaw(mntbox->evas);

   e_gadman_client_resize(mntbox->gmc, (w * (evas_list_count(mntbox->icons))),
                          h);

   _mount_box_iconsize_change(mntbox);
}

static void
_change_orient_vert(Mount *mnt)
{
   Evas_List *l2;
   Evas_Object *o;
   Evas_Coord w, h;
   Mount_Box *mntbox;

   mntbox = mnt->box;
   evas_event_freeze(mntbox->evas);

   o = mntbox->bar_object;
   edje_object_signal_emit(o, "set_orientation", "vert");
   edje_object_message_signal_process(o);

   e_box_freeze(mntbox->box_object);

   w = mntbox->mnt->conf->icon_size + mntbox->icon_inset.l +
      mntbox->icon_inset.r;
   h = mntbox->mnt->conf->icon_size + mntbox->icon_inset.t +
      mntbox->icon_inset.b;

   for (l2 = mntbox->icons; l2; l2 = l2->next)
     {
        Mount_Icon *ic;

        ic = l2->data;
        o = ic->bg_object;
        edje_object_signal_emit(o, "set_orientation", "vert");
        edje_object_message_signal_process(o);

        o = ic->icon_object;
        edje_object_signal_emit(o, "set_orientation", "vert");
        edje_object_message_signal_process(o);

        e_box_pack_options_set(ic->bg_object, 1, 1,     /* fill */
                               0, 0,    /* expand */
                               0.5, 0.5,        /* align */
                               w, h,    /* min */
                               w, h     /* max */
           );
     }
   e_box_align_set(mntbox->box_object, 0.5, 0.5);
   e_box_orientation_set(mntbox->box_object, 0);
   e_box_thaw(mntbox->box_object);
   evas_event_thaw(mntbox->evas);

   e_gadman_client_resize(mntbox->gmc, w,
                          (h * (evas_list_count(mntbox->icons))));

   _mount_box_iconsize_change(mntbox);
}

static void
_mount_mountpoint(Mount_Icon *mnticon)
{
   char buf[1024];
   Ecore_Exe *x;

   fprintf(stderr, "Trying To Mount %s\n", mnticon->mntpoint->path);
   snprintf(buf, sizeof(buf), MOUNT " %s", mnticon->mntpoint->path);

   x = ecore_exe_run(buf, mnticon);
   if (x)
      ecore_exe_tag_set(x, "Mount");
}

static void
_unmount_mountpoint(Mount_Icon *mnticon)
{
   char buf[1024];
   Ecore_Exe *x;

   fprintf(stderr, "Trying To Unount %s\n", mnticon->mntpoint->path);
   snprintf(buf, sizeof(buf), UMOUNT " %s", mnticon->mntpoint->path);

   x = ecore_exe_run(buf, mnticon);
   if (x)
      ecore_exe_tag_set(x, "Unmount");
}

static void
_eject_mountpoint(Mount_Icon *mnticon)
{
   char buf[1024];
   Ecore_Exe *x;

   fprintf(stderr, "Trying To Eject %s\n", mnticon->mntpoint->path);
   snprintf(buf, sizeof(buf), EJECT " %s", mnticon->mntpoint->device);

   x = ecore_exe_run(buf, mnticon);
   if (x)
      ecore_exe_tag_set(x, "Eject");
}

static void
_mount_box_cb_mouse_down(void *data, Evas *e, Evas_Object *obj,
                         void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   Mount_Box *mntbox;
   E_Manager *man;
   E_Container *con;

   ev = event_info;
   mntbox = data;
   if (ev->button == 3)
     {
        man = e_manager_current_get();
        if (!man)
           return;
        con = e_container_current_get(man);
        if (!con)
           return;
        if (con == NULL)
           return;

        e_menu_activate_mouse(mntbox->menu, e_zone_current_get(con),
                              ev->output.x, ev->output.y, 1, 1,
                              E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
        e_util_container_fake_mouse_up_later(con, 3);
     }
}

static void
_set_state(Mount_Icon *ic)
{
   if (ic->mntpoint->mounted)
     {
        edje_object_signal_emit(ic->icon_object, "set_mounted", "icon");
     }
   else
     {
        edje_object_signal_emit(ic->icon_object, "set_unmounted", "icon");
     }
}

static void
_mount_icon_cb_edje_mouse_down(void *data, Evas_Object *o,
                               const char *emission, const char *source)
{
   Mount_Icon *ic;

   ic = data;
   if (!strcmp(emission, "mouse,down,1"))
     {
        _mount_exe_exit_handler =
           ecore_event_handler_add(ECORE_EXE_EVENT_DEL, _mount_exe_cb_exit,
                                   NULL);
        if (ic->mntpoint->mounted == 0)
          {
             /* Mount */
             _mount_mountpoint(ic);
          }
        else
          {
#ifdef HAVE_EJECT
             if (ic->mntpoint->eject_mode == AUTO_EJECT)
               {
                  _eject_mountpoint(ic);
               }
             else
               {
                  _unmount_mountpoint(ic);
               }
#else
             /* Unmount */
             _unmount_mountpoint(ic);
#endif
          }
     }
}

static int
_mount_exe_cb_exit(void *data, int type, void *event)
{
   Ecore_Exe_Event_Del *ev;
   Ecore_Exe *x;
   Mount_Icon *ic;
   Mount_Point *point;
   char *tag;
   int mounted;

   ev = event;
   if (!ev->exe)
      return 1;
   x = ev->exe;
   if (!x)
      return 1;

   ic = ecore_exe_data_get(x);
   tag = ecore_exe_tag_get(x);
   point = ic->mntpoint;
   mounted = _is_mounted(point);
   x = NULL;

   ecore_event_handler_del(_mount_exe_exit_handler);

   point->mounted = mounted;
   _set_state(ic);

   if (!strcmp(tag, "Unmount"))
     {
        if (mounted)
          {
             e_module_dialog_show(_("Mount Module"),
                                  _
                                  ("Unmount Failed !!<br>Perhaps this device is in use or is not mounted."));
          }
     }
   else if (!strcmp(tag, "Mount"))
     {
        if (!mounted)
          {
             e_module_dialog_show(_("Mount Module"),
                                  _
                                  ("Mount Failed !!<br>Please check that media is inserted"));
          }
     }
   else if (!strcmp(tag, "Eject"))
     {
        if (mounted)
          {
             e_module_dialog_show(_("Mount Module"), _("Eject Failed !!"));
          }
     }
   return 0;
}

void
_mount_cb_config_updated(void *data)
{
   Mount *m;
   Mount_Box *mntbox;
   Evas_List *l;

   m = data;
   if (!m)
      return;

   mntbox = m->box;

   //_mount_box_iconsize_change(mntbox);

   for (l = mntbox->icons; l; l = l->next)
     {
        Mount_Icon *ic;

        ic = l->data;
        if (m->conf->show_labels)
          {
             edje_object_signal_emit(ic->txt_object, "show_label", "");
          }
        else
          {
             edje_object_signal_emit(ic->txt_object, "hide_label", "");
          }
     }

   if (m->conf->orientation == MOUNT_ORIENT_HORIZ)
     {
        _change_orient_horiz(m);
     }
   else
     {
        _change_orient_vert(m);
     }
}
