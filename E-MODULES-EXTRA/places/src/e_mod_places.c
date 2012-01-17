#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <e.h>
#include <E_DBus.h>
#include <E_Ukit.h>
#include <sys/statvfs.h>
#include <errno.h>
#include "e_mod_main.h"
#include "e_mod_places.h"


/* Local Function Prototypes */
static void _places_udisks_test(void *data __UNUSED__, DBusMessage *msg __UNUSED__, DBusError *error);
static void _places_udisks_poll(void *data __UNUSED__, DBusMessage *msg);
static Eina_Bool _places_poller(void *data);
static void _places_print_volume(Volume *v);
// static void _places_error_show(const char *title, const char *text1, const char *text2, const char *text3);
static void _places_run_fm(void *data, E_Menu *m, E_Menu_Item *mi);
static void _places_volume_add(const char *udi, int dont_auto_mount, int dont_auto_open);
static void _places_volume_del(Volume *v);
static void _places_mount_volume(Volume *vol);
static const char *_places_human_size_get(unsigned long long size);
static unsigned long long _places_free_space_get(const char *mount);
static void _places_update_size(Evas_Object *obj, Volume *vol);

// Edje callbacks
void _places_icon_activated_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void _places_custom_icon_activated_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void _places_eject_activated_cb(void *data, Evas_Object *o, const char *emission, const char *source);

// dbus callbacks
void _places_device_add_cb(void *data, DBusMessage *msg);
void _places_device_rem_cb(void *data, DBusMessage *msg);
void _places_volume_prop_modified_cb(void *data, DBusMessage *msg);
void _places_volume_all_cb(void *user_data, void *reply_data, DBusError *error);
void _places_volume_properties_cb(void *data, void *reply_data, DBusError *error);
void _places_storage_properties_cb(void *data, void *reply_data, DBusError *error);

/* Local Variables */
static E_DBus_Connection *conn;
static E_DBus_Signal_Handler *sh_added, *sh_removed;
static E_DBus_Signal_Handler *udisks_poll = NULL;

static Ecore_Timer *poller;
static char theme_file[PATH_MAX];
Eina_List *volumes;


/* Implementation */
void
places_init(void)
{
   volumes = NULL;

   if (!e_dbus_init())
     {
        printf("Impossible to setup dbus.\n");
        return;
     }

   if (!e_ukit_init())
     {
        printf("Impossible to setup ukit.\n");
        return;
     }

   conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
   if (!conn)
   {
      printf("Error connecting to system bus. Is it running?\n");
      return;
   }
   printf("PLACES: Init\n");

   /* theme file (maybe check if found in the current theme) */
   snprintf(theme_file, PATH_MAX, "%s/e-module-places.edj", places_conf->module->dir);

//
   if (!udisks_poll)
     udisks_poll = e_dbus_signal_handler_add(conn,
                        E_DBUS_FDO_BUS, E_DBUS_FDO_PATH,E_DBUS_FDO_INTERFACE,
                        "NameOwnerChanged", _places_udisks_poll, NULL);
//
   e_dbus_get_name_owner(conn, E_UDISKS_BUS, _places_udisks_test, NULL);

//
   DBusMessage *msg;
   msg = dbus_message_new_method_call(E_UDISKS_BUS, E_UDISKS_PATH, E_UDISKS_BUS, "suuuuuup");
   e_dbus_method_call_send(conn, msg, NULL, (E_DBus_Callback_Func)_places_udisks_test, NULL, -1, NULL); /* test for not running udisks */
   dbus_message_unref(msg);
   

   poller = ecore_timer_add(3.0, _places_poller, NULL);
}

static void
_places_udisks_poll(void *data __UNUSED__, DBusMessage *msg)
{
   DBusError err;
   const char *name, *from, *to;

   dbus_error_init(&err);
   if (!dbus_message_get_args(msg, &err,
                              DBUS_TYPE_STRING, &name,
                              DBUS_TYPE_STRING, &from,
                              DBUS_TYPE_STRING, &to,
                              DBUS_TYPE_INVALID))
     dbus_error_free(&err);

   printf("PLACES: name: %s\nfrom: %s\nto: %s\n", name, from, to);
   if ((name) && !strcmp(name, E_UDISKS_BUS))
     _places_udisks_test(NULL, NULL, NULL);
}

static void
_places_udisks_test(void *data __UNUSED__, DBusMessage *msg __UNUSED__, DBusError *error)
{
   if (error && dbus_error_is_set(error))
     {
        dbus_error_free(error);
        return;
     }

   printf("PLACES: Udisk is OK!\n");
   sh_added = e_dbus_signal_handler_add(conn, E_UDISKS_BUS,
                             E_UDISKS_PATH, E_UDISKS_BUS,
                             "DeviceAdded", _places_device_add_cb, NULL);
   sh_removed = e_dbus_signal_handler_add(conn, E_UDISKS_BUS,
                             E_UDISKS_PATH, E_UDISKS_BUS,
                             "DeviceRemoved", _places_device_rem_cb, NULL);
   e_udisks_get_all_devices(conn, _places_volume_all_cb, NULL);
}

void
places_shutdown(void)
{
   if (poller) ecore_timer_del(poller);

   if (conn && sh_added) e_dbus_signal_handler_del(conn, sh_added);
   if (conn && sh_removed) e_dbus_signal_handler_del(conn, sh_removed);

   while (volumes)
     _places_volume_del((Volume*)volumes->data);

   if (conn) e_dbus_connection_close(conn);

   e_ukit_shutdown();
   e_dbus_shutdown();
}

void
places_update_all_gadgets(void)
{
   Eina_List *l;
   Instance *inst;

   EINA_LIST_FOREACH(instances, l, inst)
     places_fill_box(inst->o_box);
}

int
_places_volume_sort_cb(const void *d1, const void *d2)
{
   const Volume *v1 = d1;
   const Volume *v2 = d2;

   // removable after interal
   if (v1->removable && !v2->removable) return(1);
   if (v2->removable && !v1->removable) return(-1);
   // filesystem root on top
   if (v1->mount_point && !strcmp(v1->mount_point, "/")) return -1;
   if (v2->mount_point && !strcmp(v2->mount_point, "/")) return 1;
   // order by label
   if(!v1 || !v1->label) return(1);
   if(!v2 || !v2->label) return(-1);
   return strcmp(v1->label, v2->label);
}

void
_places_custom_volume(Evas_Object *box, const char *label, const char *icon, const char *uri)
{
   int min_w, min_h, max_w, max_h;
   Evas_Object *o, *i;

   /* volume object */
   o = edje_object_add(evas_object_evas_get(box));
   edje_object_file_set(o, theme_file, "modules/places/main");

   /* icon */
   i = edje_object_add(evas_object_evas_get(box));
   //edje_object_file_set(icon, theme_file, vol->icon);
   edje_object_file_set(i, e_theme_edje_file_get("base/theme/fileman", icon),
                        icon);
   edje_object_part_swallow(o, "icon", i);

   /* label */
   edje_object_part_text_set(o, "volume_label", label);

   /* gauge */
   edje_object_signal_emit(o, "gauge,hide", "places");
   edje_object_part_text_set(o, "size_label", "");

   /* orient the separator*/
   if (!e_box_orientation_get(box))
      edje_object_signal_emit(o, "separator,set,horiz", "places");
   else
      edje_object_signal_emit(o, "separator,set,vert", "places");

   /* connect signals from edje */
   edje_object_signal_callback_add(o, "icon,activated", "places",
                                   _places_custom_icon_activated_cb, (void*)uri);

   /* pack the volume in the box */
   evas_object_show(o);
   edje_object_size_min_get(o, &min_w, &min_h);
   edje_object_size_max_get(o, &max_w, &max_h);
   e_box_pack_end(box, o);
   e_box_pack_options_set(o,
                          1, 0, /* fill */
                          1, 0, /* expand */
                          0.5, 0.0, /* align */
                          min_w, min_h, /* min */
                          max_w, max_h /* max */
                         );
}

void
places_fill_box(Evas_Object *box)
{
   Eina_List *l;
   int min_w, min_h, max_w, max_h, found;
   Evas_Object *o, *icon;
   char *f1, *f2, *f3;
   char buf[128];

   places_empty_box(box);

   /*if (places_conf->show_home)
      _places_custom_volume(box, D_("Home"), "e/icons/fileman/home", "/home/dave");
   if (places_conf->show_desk)
      _places_custom_volume(box, D_("Desktop"), "e/icons/fileman/desktop", "/home/dave/Desktop");
   if (places_conf->show_trash)
      _places_custom_volume(box, D_("Trash"), "e/icons/fileman/trash", "trash:///");
   if (places_conf->show_root)
      _places_custom_volume(box, D_("Filesystem"), "e/icons/fileman/root", "/");
   if (places_conf->show_temp)
      _places_custom_volume(box, D_("Temp"), "e/icons/fileman/tmp", "/tmp");
   */

   // header object (visible only in vertical orientation)
   if (!e_box_orientation_get(box))
   {
      o = edje_object_add(evas_object_evas_get(box));
      edje_object_file_set(o, theme_file, "modules/places/header");
      edje_object_part_text_set(o, "label", D_("Places"));
      if (!e_box_orientation_get(box))
         edje_object_signal_emit(o, "separator,set,horiz", "places");
      else
         edje_object_signal_emit(o, "separator,set,vert", "places");
      edje_object_size_min_get(o, &min_w, &min_h);
      edje_object_size_max_get(o, &max_w, &max_h);
      evas_object_show(o);
      e_box_pack_end(box, o);
      e_box_pack_options_set(o,
                           1, 0, /* fill */
                           1, 0, /* expand */
                           0.5, 0.0, /* align */
                           min_w, min_h, /* min */
                           max_w, max_h /* max */
                           );
   }
   
   // volumes
   volumes = eina_list_sort(volumes, 0, _places_volume_sort_cb);
   for (l = volumes; l; l = l->next)
     {
        Volume *vol = l->data;

        if (!vol->valid) continue;

        //volume object
        o = edje_object_add(evas_object_evas_get(box));
        edje_object_file_set(o, theme_file, "modules/places/main");
        vol->obj = o;

        //set volume label
        if (vol->mount_point && !strcmp(vol->mount_point, "/"))
           edje_object_part_text_set(o, "volume_label", D_("Filesystem"));
        else if (vol->label && strlen(vol->label))
          edje_object_part_text_set(o, "volume_label", vol->label);
        else
          edje_object_part_text_set(o, "volume_label", D_("No Name"));

        //set free label e gauge
        _places_update_size(o, vol);

        //choose the right icon
        icon = e_icon_add(evas_object_evas_get(box));
        f1 = f2 = f3 = NULL;
        /* optical discs */
        if (!strcmp(vol->drive_type, "cdrom"))
          {
             f1 = "drive"; f2 = "optical";  // OR media-optical ??
          }
        /* flash cards */
        else if (!strcmp(vol->drive_type, "sd_mmc") ||
                 !strcmp(vol->model, "SD/MMC"))
          {
             f1 = "media"; f2 = "flash"; f3 = "sdmmc"; // NOTE sd-mmc in Oxigen :(
          }
        else if (!strcmp(vol->drive_type, "memory_stick") ||
                 !strcmp(vol->model, "MS/MS-Pro"))
          {
             f1 = "media"; f2 = "flash"; f3 = "ms"; // NOTE memory-stick in Oxigen :(
          }
        /* iPods */
        else if (!strcmp(vol->model, "iPod"))
          {
             f1 = "multimedia-player"; f2 = "apple"; f3 = "ipod";
          }
        /* generic usb drives */
        else if (!strcmp(vol->bus, "usb"))
          {
             f1 = "drive"; f2 = "removable-media"; f3 = "usb";
          }

        // search the icon, following freedesktop fallback system
        found = 0;
        if (f1 && f2 && f3)
          {
             snprintf(buf, sizeof(buf), "%s-%s-%s", f1, f2, f3);
             found = e_util_icon_theme_set(icon, buf);
          }
        if (!found && f1 && f2)
          {
             snprintf(buf, sizeof(buf), "%s-%s", f1, f2);
             found = e_util_icon_theme_set(icon, buf);
          }
        if (!found)
          {
             snprintf(buf, sizeof(buf), "drive-harddisk");
             found = e_util_icon_theme_set(icon, buf);
          }
        if (found)
          {
             edje_object_part_swallow(o, "icon", icon);
             vol->icon = eina_stringshare_add(buf);
          }
        else evas_object_del(icon);

        //set partition type tag
        if (!strcmp(vol->fstype, "ext2") || !strcmp(vol->fstype, "ext3") ||
            !strcmp(vol->fstype, "ext4") || !strcmp(vol->fstype, "reiserfs"))
          edje_object_signal_emit(o, "icon,tag,ext3", "places");
        else if (!strcmp(vol->fstype, "ufs") || !strcmp(vol->fstype, "zfs"))
          edje_object_signal_emit(o, "icon,tag,ufs", "places");
        else if (!strcmp(vol->fstype, "vfat") || !strcmp(vol->fstype, "ntfs") ||
                 !strcmp(vol->fstype, "ntfs-3g"))
          edje_object_signal_emit(o, "icon,tag,fat", "places");
        else if (!strcmp(vol->fstype, "hfs") || !strcmp(vol->fstype, "hfsplus"))
          edje_object_signal_emit(o, "icon,tag,hfs", "places");
        else if (!strcmp(vol->fstype, "udf"))
          edje_object_signal_emit(o, "icon,tag,dvd", "places");

        //set mount/eject icon
        if ((vol->requires_eject || vol->removable || vol->mounted) &&
            (vol->mount_point && strcmp(vol->mount_point, "/")))
          edje_object_signal_emit(o, "icon,eject,show", "places");
        else
          edje_object_signal_emit(o, "icon,eject,hide", "places");

        // orient the separator
        if (!e_box_orientation_get(box))
          edje_object_signal_emit(o, "separator,set,horiz", "places");
        else
          edje_object_signal_emit(o, "separator,set,vert", "places");

        // connect signals from edje
        edje_object_signal_callback_add(o, "icon,activated", "places",
                                        _places_icon_activated_cb, vol);
        edje_object_signal_callback_add(o, "eject,activated", "places",
                                        _places_eject_activated_cb, vol);

        // pack the volume in the box
        evas_object_show(o);
        edje_object_size_min_get(o, &min_w, &min_h);
        edje_object_size_max_get(o, &max_w, &max_h);
        e_box_pack_end(box, o);
        e_box_pack_options_set(o,
                               1, 0, /* fill */
                               1, 0, /* expand */
                               0.5, 0.0, /* align */
                               min_w, min_h, /* min */
                               max_w, max_h /* max */
                              );
     }
}

void
places_empty_box(Evas_Object *box)
{
   int count;

   e_box_freeze(box);
   count = e_box_pack_count_get(box);
   while (count >= 0)
     {
        Evas_Object *o;
        Evas_Object *swal;

        o = e_box_pack_object_nth(box, count);
        swal = edje_object_part_swallow_get(o, "icon");
        if (swal)
          {
             edje_object_part_unswallow(o, swal);
             evas_object_del(swal);
          }

        e_box_unpack(o);
        evas_object_del(o);

        count--;
     }
   e_box_thaw(box);
}

void
places_parse_bookmarks(E_Menu *em)
{
   char line[PATH_MAX];
   char buf[PATH_MAX];
   E_Menu_Item *mi;
   Efreet_Uri *uri;
   char *alias;
   FILE* fp;

   snprintf(buf, sizeof(buf), "%s/.gtk-bookmarks", e_user_homedir_get());
   fp = fopen(buf, "r");
   if (fp)
     {
        while(fgets(line, sizeof(line), fp))
          {
             alias = NULL;
             line[strlen(line) - 1] = '\0';
             alias = strchr(line, ' ');
             if (alias)
               {
                  line[alias-line] =  '\0';
                  alias++;
               }
             uri = efreet_uri_decode(line);
             if (uri && uri->path)
               {
                  if (ecore_file_exists(uri->path))
                    {
                       mi = e_menu_item_new(em);
                       e_menu_item_label_set(mi, alias ? alias :
                                             ecore_file_file_get(uri->path));
                       e_util_menu_item_theme_icon_set(mi, "folder");
                       e_menu_item_callback_set(mi, _places_run_fm,
                                                strdup(uri->path)); //TODO free somewhere
                    }
               }
             if (uri) efreet_uri_free(uri);
          }
        fclose(fp);
     }
}

void
places_menu_click_cb(void *data, E_Menu *m, E_Menu_Item *mi)
{
   _places_icon_activated_cb(data, NULL, NULL, NULL);
}

void
places_generate_menu(void *data, E_Menu *em)
{
   E_Menu_Item *mi;
   char buf[PATH_MAX];

   // Home
   if (places_conf->show_home)
     {
        mi = e_menu_item_new(em);
        e_menu_item_label_set(mi, D_("Home"));
        e_util_menu_item_theme_icon_set(mi, "user-home");
        e_menu_item_callback_set(mi, _places_run_fm, (char*)e_user_homedir_get());
     }

   // Desktop
   if (places_conf->show_desk)
     {
        mi = e_menu_item_new(em);
        e_menu_item_label_set(mi, D_("Desktop"));
        e_util_menu_item_theme_icon_set(mi, "user-desktop");
        snprintf(buf, sizeof(buf), "%s/Desktop", (char*)e_user_homedir_get());
        e_menu_item_callback_set(mi, _places_run_fm, strdup(buf)); //TODO free somewhere
     }

   // Trash
   if (places_conf->show_trash)
     {
        mi = e_menu_item_new(em);
        e_menu_item_label_set(mi, D_("Trash"));
        e_util_menu_item_theme_icon_set(mi, "folder");
        e_menu_item_callback_set(mi, _places_run_fm, "trash:///");
     }

   // File System
   if (places_conf->show_root)
     {
        mi = e_menu_item_new(em);
        e_menu_item_label_set(mi, D_("Filesystem"));
        e_util_menu_item_theme_icon_set(mi, "drive-harddisk");
        e_menu_item_callback_set(mi, _places_run_fm, "/");
     }

   // Temp
   if (places_conf->show_temp)
     {
        mi = e_menu_item_new(em);
        e_menu_item_label_set(mi, D_("Temp"));
        e_util_menu_item_theme_icon_set(mi, "user-temp");
        e_menu_item_callback_set(mi, _places_run_fm, "/tmp");
     }

   // Separator
   if (places_conf->show_home || places_conf->show_desk ||
       places_conf->show_trash || places_conf->show_root ||
       places_conf->show_temp)
     {
        mi = e_menu_item_new(em);
        e_menu_item_separator_set(mi, 1);
     }

   // Volumes
   Eina_Bool volumes_visible = 0;
   const Eina_List *l;
   Volume *vol;
   EINA_LIST_FOREACH(volumes, l, vol)
     {
        if (!vol->valid) continue;
        if (vol->mount_point && !strcmp(vol->mount_point, "/")) continue;

        mi = e_menu_item_new(em);
        if ((vol->label) && (vol->label[0] != '\0'))
          e_menu_item_label_set(mi, vol->label);
        else
          e_menu_item_label_set(mi, ecore_file_file_get(vol->mount_point));

        if (vol->icon)
          e_util_menu_item_theme_icon_set(mi, vol->icon);

        e_menu_item_callback_set(mi, places_menu_click_cb, (void*)vol);
        volumes_visible = 1;
     }

   // Favorites
   if (places_conf->show_bookm)
     {
        if (volumes_visible)
          {
             mi = e_menu_item_new(em);
             e_menu_item_separator_set(mi, 1);
          }
        places_parse_bookmarks(em);
     }

   e_menu_pre_activate_callback_set(em, NULL, NULL);
}

void
places_augmentation(void *data, E_Menu *em)
{
   E_Menu_Item *mi;
   E_Menu *m;

   mi = e_menu_item_new(em);
   e_menu_item_label_set(mi, D_("Places"));
   e_util_menu_item_theme_icon_set(mi, "system-file-manager");

   m = e_menu_new();
   e_menu_item_submenu_set(mi, m);

   e_menu_pre_activate_callback_set(m, places_generate_menu, NULL);
}

/* Internals */
static Eina_Bool
_places_poller(void *data)
{
   Eina_List *l;
   Volume *vol;

   EINA_LIST_FOREACH(volumes, l, vol)
     if (vol->valid && vol->mounted)
       _places_update_size(vol->obj, vol);

   return EINA_TRUE;
}

static void
_places_print_volume(Volume *v)
{
   printf("Got volume %s\n", v->udi);
   printf("  label: %s\n",v->label);
   printf("  mounted: %d\n", v->mounted);
   printf("  m_point: %s\n", v->mount_point);
   printf("  device: %s\n", v->device);
   printf("  fstype: %s\n", v->fstype);
   printf("  bus: %s\n", v->bus);
   printf("  drive_type: %s\n", v->drive_type);
   printf("  model: %s\n", v->model);
   printf("  vendor: %s\n", v->vendor);
   printf("  serial: %s\n", v->serial);
   printf("  removable: %d\n", v->removable);
   printf("  requires eject: %d\n", v->requires_eject);
}

static void
_places_volume_add(const char *udi, int dont_auto_mount, int dont_auto_open)
{
   Volume *v;
   if (!udi) return;

   v = E_NEW(Volume, 1);
   if (!v) return;

   // safe defaults
   v->udi = eina_stringshare_add(udi);
   v->valid = 0;
   v->obj = NULL;
   v->icon = NULL;
   v->device = NULL;
   v->to_mount = 0;
   v->force_open = 0;
   v->drive_type = "";
   v->model = "";
   v->bus = "";

   if (places_conf->auto_mount && !dont_auto_mount)
     v->to_mount = 1;

   if (places_conf->auto_open && !dont_auto_open)
     v->force_open = 1;

   volumes = eina_list_append(volumes, v);

   e_udisks_get_all_properties(conn, v->udi, _places_volume_properties_cb, v);

   v->sh_prop = e_dbus_signal_handler_add(conn, E_UDISKS_BUS, v->udi,
                                          E_UDISKS_INTERFACE, "Changed",
                                          _places_volume_prop_modified_cb, v);

}

static void
_places_volume_del(Volume *v)
{
   e_dbus_signal_handler_del(conn, v->sh_prop);
   volumes = eina_list_remove(volumes, v);
   if (v->udi)         eina_stringshare_del(v->udi);
   if (v->uuid)        eina_stringshare_del(v->uuid);
   if (v->label)       eina_stringshare_del(v->label);
   if (v->icon)        eina_stringshare_del(v->icon);
   if (v->mount_point) eina_stringshare_del(v->mount_point);
   if (v->device)      eina_stringshare_del(v->device);
   if (v->fstype)      eina_stringshare_del(v->fstype);
   if (v->bus)         eina_stringshare_del(v->bus);
   if (v->drive_type)  eina_stringshare_del(v->drive_type);
   if (v->model)       eina_stringshare_del(v->model);
   if (v->vendor)      eina_stringshare_del(v->vendor);
   if (v->serial)      eina_stringshare_del(v->serial);
   free(v);
}

static const char *
_places_human_size_get(unsigned long long size)
{
   double dsize;
   char hum[32], *suffix;

   dsize = (double)size;
   if (dsize < 1024)
     snprintf(hum, sizeof(hum), "%.0fb", dsize);
   else
     {
        dsize /= 1024.0;
        if (dsize < 1024)
          suffix = "KB";
        else
          {
             dsize /= 1024.0;
             if (dsize < 1024)
               suffix = "MB";
             else
               {
                  dsize /= 1024.0;
                  suffix = "GB";
               }
          }
        snprintf(hum, sizeof(hum), "%.1f%s", dsize, suffix);
     }

   return eina_stringshare_add(hum);
}

static unsigned long long
_places_free_space_get(const char *mount)
{
   struct statvfs s;
//printf("GET SPACE of %s\n", mount);
   if (!mount) return 0;
   if (statvfs(mount, &s) != 0)
     return 0;
//printf("   SPACE %d\n", (s.f_bfree * s.f_frsize));
   return (unsigned long long)s.f_bavail * (unsigned long long)s.f_frsize;
}

/*static void
_places_error_show(const char *title, const char *text1, const char *text2, const char *text3)
{
   char str[PATH_MAX];
   E_Dialog *dia;

   snprintf(str, sizeof(str),"%s<br><br>%s<br>%s", text1, text2, text3);

   dia = e_dialog_new(NULL, "E", "_places_error");
   e_dialog_title_set(dia, title);
   e_dialog_icon_set(dia, "drive-harddisk", 64);
   e_dialog_button_add(dia, "OK", NULL, NULL, NULL);
   e_dialog_text_set(dia, str);
   e_dialog_show(dia);
}*/

static void
_places_run_fm_external(const char *fm, const char *directory)
{
   char exec[PATH_MAX];

   snprintf(exec, PATH_MAX, "%s \"%s\"", (char*)fm, (char*)directory);
   e_exec(NULL, NULL, exec, NULL, NULL);
}

static void
_places_run_fm(void *data, E_Menu *m, E_Menu_Item *mi)
{
   const char *directory = data;

   if (places_conf->fm && (places_conf->fm[0] != '\0'))
     {
        _places_run_fm_external(places_conf->fm, directory);
        return;
     }

   E_Action *act = e_action_find("fileman");
   Eina_List *managers = e_manager_list();

   if (act && act->func.go && managers && managers->data)
     act->func.go(E_OBJECT(managers->data), directory);
   else
     _places_run_fm_external("thunar", directory);
}

static void
_places_update_size(Evas_Object *obj, Volume *vol)
{
   char buf[256];
   char buf2[16];
   const char *tot_h, *free_h;
   unsigned long long free = 0;

   // Free label
   tot_h = _places_human_size_get(vol->size);
   if (vol->mounted)
     {
        free = _places_free_space_get(vol->mount_point);
        free_h = _places_human_size_get(free);
        snprintf(buf, sizeof(buf), "%s %s %s", free_h, D_("free of"), tot_h);
        edje_object_part_text_set(obj, "size_label", buf);
        eina_stringshare_del(free_h);
     }
   else
     {
        snprintf(buf, sizeof(buf), D_("%s Not Mounted"), tot_h);
        edje_object_part_text_set(obj, "size_label", buf);
     }
   eina_stringshare_del(tot_h);

   // Gauge
   int percent;
   Edje_Message_Float msg_float;

   if (vol->mounted)
     {
        percent = 100 - (((long double)free / (long double)vol->size) * 100);
        snprintf(buf2, sizeof(buf2), "%d%%", percent);
        edje_object_part_text_set(obj, "percent_label", buf2);

        msg_float.val = (float)percent / 100;
        edje_object_message_send(obj, EDJE_MESSAGE_FLOAT, 1, &msg_float);
        edje_object_part_text_set(obj, "eject_label", D_("unmount"));
     }
   else
     {
        edje_object_signal_emit(obj, "gauge,hide", "places");
        edje_object_part_text_set(obj, "percent_label", "");
        edje_object_part_text_set(obj, "eject_label", D_("eject"));
     }
}

static void
_places_mount_volume(Volume *vol)
{
   Eina_List *opt = NULL;
   char buf[256];

   if ((!strcmp(vol->fstype, "vfat")) || (!strcmp(vol->fstype, "ntfs")))
     {
        snprintf(buf, sizeof(buf), "uid=%i", (int)getuid());
        opt = eina_list_append(opt, buf);
     }

   e_udisks_volume_mount(conn, vol->udi, vol->fstype, opt);

   vol->to_mount = 0;
   eina_list_free(opt);
}

/**********************/
/*   EDJE Callbacks   */
/**********************/
void
_places_icon_activated_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
   Volume *vol;

   vol = data;

   if (vol->mounted)
     _places_run_fm((void*)vol->mount_point, NULL, NULL);
   else
     {
        vol->force_open = 1;
        _places_mount_volume(vol);
     }
}

void
_places_custom_icon_activated_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
   //data is char *uri
   _places_run_fm(data, NULL, NULL);
}

void
_places_eject_activated_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
   Volume *vol = data;

   if (vol->mounted)
     e_udisks_volume_unmount(conn, vol->udi, NULL);
   else
     e_udisks_volume_eject(conn, vol->udi, NULL);
}

/***************************/
/* UDisks / DBUS Callbacks */
/***************************/

/* Dbus CB - Generic device added */
void
_places_device_add_cb(void *data, DBusMessage *msg)
{
   DBusError err;
   char *udi;

   dbus_error_init(&err);
   dbus_message_get_args(msg, &err, DBUS_TYPE_OBJECT_PATH, &udi, DBUS_TYPE_INVALID);
   _places_volume_add(udi, EINA_FALSE, EINA_FALSE);

   // TODO need to free udi and err??
}

/* Dbus CB - Generic device removed */
void
_places_device_rem_cb(void *data, DBusMessage *msg)
{
   DBusError err;
   char *udi;
   Eina_List *l;
   Volume *v;

   dbus_error_init(&err);
   dbus_message_get_args(msg, &err, DBUS_TYPE_OBJECT_PATH, &udi, DBUS_TYPE_INVALID);

   EINA_LIST_FOREACH(volumes, l, v)
     if (!strcmp(v->udi, udi))
       {
          if (v->valid)
            {
               _places_volume_del(v);
               places_update_all_gadgets();
            }
          else
            _places_volume_del(v);
          return;
       }
}

/* Dbus CB - Called when a device change some properties */
void
_places_volume_prop_modified_cb(void *data, DBusMessage *msg)
{
   Volume *v = data;
   e_udisks_get_all_properties(conn, v->udi, _places_volume_properties_cb, v);
}

/* Dbus CB - Reply of e_udisks_get_all_devices() */
void
_places_volume_all_cb(void *user_data, void *reply_data, DBusError *error)
{
   E_Ukit_String_List_Return *udisks_ret = reply_data;
   Eina_List *l;
   char *udi;

   if (dbus_error_is_set(error))
     {
        // XXX handle...
        dbus_error_free(error);
        return;
     }

   if (!udisks_ret || !udisks_ret->strings) return;
   EINA_LIST_FOREACH(udisks_ret->strings, l, udi)
      _places_volume_add(udi, EINA_TRUE, EINA_TRUE);

   //TODO free ret??
}

/* Dbus CB - Volume get properties */
void
_places_volume_properties_cb(void *data, void *reply_data, DBusError *error)
{
   Volume *v = data;
   E_Ukit_Properties *udisks_ret = reply_data;
   int err = 0;
   const char *str = NULL;

   if (!v) return;

   /* skip volumes with volume.ignore set */
   if (e_ukit_property_bool_get(udisks_ret, "DeviceIsMediaChangeDetectionInhibited", &err) || err)
      return;

   /* skip volumes that aren't filesystems */
   str = e_ukit_property_string_get(udisks_ret, "IdUsage", &err);
   if (err || !str) return;
   if (strcmp(str, "filesystem"))
     {
        if (strcmp(str, "crypto"))
          v->encrypted = e_ukit_property_bool_get(udisks_ret, "DeviceIsLuks", &err);

        if (!v->encrypted) return;
     }
   str = NULL;

   if (v->uuid) eina_stringshare_del(v->uuid);
   str = e_ukit_property_string_get(udisks_ret, "IdUuid", &err);
   if (!err && str) v->uuid = eina_stringshare_add(str);

   if (v->label) eina_stringshare_del(v->label);
   str = e_ukit_property_string_get(udisks_ret, "IdLabel", &err);
   if (!err && str && str[0]) v->label = eina_stringshare_add(str);

   if (!v->encrypted)
     {
        const Eina_List *l;   

        l = e_ukit_property_strlist_get(udisks_ret, "DeviceMountPaths", &err);
        if (!err && l)
          {
             if (v->mount_point) eina_stringshare_del(v->mount_point);
             v->mount_point = eina_stringshare_add(l->data);
             if (!v->label) v->label = eina_stringshare_add(v->mount_point);
          }

        if (v->fstype) eina_stringshare_del(v->fstype);
        str = e_ukit_property_string_get(udisks_ret, "IdType", &err);
        if (!err && str) v->fstype = eina_stringshare_add(str);

        str = e_ukit_property_string_get(udisks_ret, "DeviceFile", &err);
        if (!err && str)
          {
             if (v->device) eina_stringshare_del(v->device);
             v->device = eina_stringshare_add(str);
             if (!v->label) v->label = eina_stringshare_add(v->device);
          }

        v->size = e_ukit_property_uint64_get(udisks_ret, "DeviceSize", &err);
        v->mounted = e_ukit_property_bool_get(udisks_ret, "DeviceIsMounted", &err);
     }
   else
     v->unlocked = e_ukit_property_bool_get(udisks_ret, "DeviceIsLuksCleartext", &err);

   str = e_ukit_property_string_get(udisks_ret, "PartitionSlave", &err);
   if (!err && str)
     e_udisks_get_all_properties(conn, str, _places_storage_properties_cb, v);

}

/* Dbus CB - Storage get properties */
void
_places_storage_properties_cb(void *data, void *reply_data, DBusError *error)
{
   Volume *v = data;
   E_Ukit_Properties *udisks_ret = reply_data;
   int err = 0;
   const char *str;
   const Eina_List *l;

   if (!v) return;
   if (dbus_error_is_set(error))
     {
        dbus_error_free(error);
        return;
     }

   if (v->bus) eina_stringshare_del(v->bus);
   str = e_ukit_property_string_get(udisks_ret, "DriveConnectionInterface", &err);
   if (!err) v->bus = eina_stringshare_add(str);

   if (v->drive_type) eina_stringshare_del(v->drive_type);
   l = e_ukit_property_strlist_get(udisks_ret, "DriveMediaCompatibility", &err);
   if (!err && l) v->drive_type = eina_stringshare_add(l->data);

   if (v->model) eina_stringshare_del(v->model);
   str = e_ukit_property_string_get(udisks_ret, "DriveModel", &err);
   if (!err && str) v->model = eina_stringshare_add(str);

   if (v->vendor) eina_stringshare_del(v->vendor);
   str = e_ukit_property_string_get(udisks_ret, "DriveVendor", &err);
   if (!err && str) v->vendor = eina_stringshare_add(str);

   if (v->serial) eina_stringshare_del(v->serial);
   str = e_ukit_property_string_get(udisks_ret, "DriveSerial", &err);
   if (!err && str) v->serial = eina_stringshare_add(str);

   v->removable = e_ukit_property_bool_get(udisks_ret, "DeviceIsRemovable", &err);
   v->requires_eject = e_ukit_property_bool_get(udisks_ret, "DriveIsMediaEjectable", &err);

   v->valid = 1;

   if (v->to_mount && !v->mounted)
   {
     _places_mount_volume(v);
      v->to_mount = 0;
   }

   if (v->force_open && v->mount_point)
   {
     _places_run_fm((void*)v->mount_point, NULL, NULL);
      v->force_open = 0;
   }

   _places_print_volume(v); /* just for debug */
   places_update_all_gadgets();
}

