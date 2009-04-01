/*  Copyright (C) 2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of places.
 *  places is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  places is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with places.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <e.h>
#include <E_DBus.h>
#include <E_Hal.h>
#include <sys/vfs.h>
#include <errno.h>
#include "config.h"
#include "e_mod_main.h"
#include "e_mod_places.h"

#define FREE_STR(str) \
   if (str) free(str); \
   str = NULL;

/* Local Function Prototypes */
static int _places_poller(void *data);
static void _places_print_volume(Volume *v);
static void _places_error_show(const char *title, const char *text1, const char *text2, const char *text3);
static void _places_run_fm(void *data, E_Menu *m, E_Menu_Item *mi);
static void _places_volume_add(const char *udi);
static void _places_volume_del(Volume *v);
static void _places_mount_volume(Volume *vol);
static const char *_places_human_size_get(unsigned long long size);
static unsigned long long _places_free_space_get(const char *mount);
static void _places_update_size(Evas_Object *obj, Volume *vol);

// Edje callbacks
void _places_icon_activated_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void _places_custom_icon_activated_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void _places_eject_activated_cb(void *data, Evas_Object *o, const char *emission, const char *source);

// Hal callbacks
void _places_mount_cb(void *user_data, void *method_return, DBusError *error);
void _places_unmount_cb(void *user_data, void *method_return, DBusError *error);
void _places_eject_cb(void *user_data, void *method_return, DBusError *error);
void _places_device_add_cb(void *data, DBusMessage *msg);
void _places_device_rem_cb(void *data, DBusMessage *msg);
void _places_volume_prop_modified_cb(void *data, DBusMessage *msg);
void _places_volume_all_cb(void *user_data, void *reply_data, DBusError *error);
void _places_volume_cb(void *user_data, void *reply_data, DBusError *error);
void _places_volume_properties_cb(void *data, void *reply_data, DBusError *error);
void _places_storage_properties_cb(void *data, void *reply_data, DBusError *error);

/* Local Variables */
static E_DBus_Connection *conn;
static E_DBus_Signal_Handler *sh_added, *sh_removed;
static Ecore_Timer *poller;
static char theme_file[PATH_MAX];
Eina_List *volumes;

/* Implementation */
void
places_init(void)
{
   volumes = NULL;
   
   /* theme file (maybe check if found in the current theme) */
   snprintf(theme_file, PATH_MAX, "%s/e-module-places.edj", places_conf->module->dir);
   
   conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
   if (!conn)
   {
      printf("Error connecting to system bus. Is it running?\n");
      return;
   }

   sh_added = e_dbus_signal_handler_add(conn, "org.freedesktop.Hal",
                                        "/org/freedesktop/Hal/Manager",
                                        "org.freedesktop.Hal.Manager",
                                        "DeviceAdded",
                                        _places_device_add_cb, NULL);
   sh_removed = e_dbus_signal_handler_add(conn, "org.freedesktop.Hal",
                                          "/org/freedesktop/Hal/Manager",
                                          "org.freedesktop.Hal.Manager",
                                          "DeviceRemoved",
                                          _places_device_rem_cb, NULL);


   e_hal_manager_find_device_by_capability(conn, "volume",
                                           _places_volume_all_cb, NULL);

   poller = ecore_timer_add(3, _places_poller, NULL);
}

void
places_shutdown(void)
{
   ecore_timer_del(poller);

   e_dbus_signal_handler_del(conn, sh_added);
   e_dbus_signal_handler_del(conn, sh_removed);

   while (volumes)
      _places_volume_del((Volume*)volumes->data);

   e_dbus_connection_close(conn);
}

void
places_update_all_gadgets(void)
{
   Eina_List *l;
   
   for (l = instances; l; l = l->next)
   {
      Instance *inst;
      
      inst = l->data;
      places_fill_box(inst->o_box);
   }
}

int
_places_volume_sort_cb(const void *d1, const void *d2)
{
   const Volume *v1 = d1;
   const Volume *v2 = d2;

   if (v1->removable && !v2->removable) return(1);
   if (v2->removable && !v1->removable) return(-1);
   if(!v1 || !v1->label) return(1);
   if(!v2 || !v2->label) return(-1);

   return strcmp(v1->label, v2->label);
}


void
_places_custom_volume(Evas_Object *box, const char *label, const char *icon, const char *uri)
{
   int min_w, min_h, max_w, max_h;
   Evas_Object *o, *sep, *i;

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
   //if (!strcmp(vol->mount_point, "/"))
   //   e_box_pack_start(box, o);
   //else
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
   int min_w, min_h, max_w, max_h;
   Evas_Object *o, *sep, *icon;

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

   volumes = eina_list_sort(volumes, 0, _places_volume_sort_cb);
   for (l = volumes; l; l = l->next)
   {
      Volume *vol = l->data;

      if (!vol->valid) continue;

      /* volume object */
      o = edje_object_add(evas_object_evas_get(box));
      edje_object_file_set(o, theme_file, "modules/places/main");
      vol->obj = o;

      //set volume label
      if (vol->label && strlen(vol->label) > 0)
         edje_object_part_text_set(o, "volume_label", vol->label);
      else if (vol->mount_point && !strcmp(vol->mount_point, "/"))
         edje_object_part_text_set(o, "volume_label", D_("Filesystem"));
      else if (vol->mount_point && strlen(vol->mount_point) > 0)
         edje_object_part_text_set(o, "volume_label",
                                   ecore_file_file_get(vol->mount_point));
      else
         edje_object_part_text_set(o, "volume_label", D_("No Name"));

      //set free label e gauge
      _places_update_size(o, vol);

      //choose icon
      vol->icon = "e/icons/drive-harddisk";
      if (!strcmp(vol->drive_type, "cdrom"))
      {
         vol->icon = "e/icons/drive-optical";
         if (!strcmp(vol->fstype, "udf"))
            edje_object_signal_emit(o, "icon,tag,dvd", "places");
      }
      else if (!strcmp(vol->model, "\"PSP\" MS"))
         vol->icon = "modules/places/icon/psp";
      else if (!strcmp(vol->drive_type, "sd_mmc"))
         vol->icon = "modules/places/icon/sdmmc";
      else if (!strcmp(vol->drive_type, "memory_stick"))
         vol->icon = "modules/places/icon/ms";
      else if (!strcmp(vol->model, "iPod"))
         vol->icon = "modules/places/icon/ipod";
      else if (!strcmp(vol->fstype, "ext3"))
         edje_object_signal_emit(o, "icon,tag,ext3", "places");
      else if (!strcmp(vol->fstype, "vfat") || !strcmp(vol->fstype, "ntfs"))
         edje_object_signal_emit(o, "icon,tag,fat", "places");
      else if (!strcmp(vol->fstype, "hfs") || !strcmp(vol->fstype, "hfsplus"))
         edje_object_signal_emit(o, "icon,tag,hfs", "places");

      //set icon
      icon = edje_object_add(evas_object_evas_get(box));
      if (strncmp(vol->icon, "e/", 2))
         edje_object_file_set(icon, theme_file, vol->icon);
      else
         edje_object_file_set(icon,
                              e_theme_edje_file_get("base/theme/fileman",
                                                    vol->icon), vol->icon);
      edje_object_part_swallow(o, "icon", icon);

      //set mount/eject icon
      if (vol->requires_eject || (vol->mounted && strcmp(vol->mount_point, "/")) ||
          !strcmp(vol->bus, "usb")) //Some usb key don't have requires_eject set (probably an hal error)
        edje_object_signal_emit(o, "icon,eject,show", "places");
      else
        edje_object_signal_emit(o, "icon,eject,hide", "places");

      /* orient the separator*/
      if (!e_box_orientation_get(box))
         edje_object_signal_emit(o, "separator,set,horiz", "places");
      else
         edje_object_signal_emit(o, "separator,set,vert", "places");
      
      /* connect signals from edje */
      edje_object_signal_callback_add(o, "icon,activated", "places",
                                      _places_icon_activated_cb, vol);
      edje_object_signal_callback_add(o, "eject,activated", "places",
                                      _places_eject_activated_cb, vol);

      /* pack the volume in the box */
      evas_object_show(o);
      edje_object_size_min_get(o, &min_w, &min_h);
      edje_object_size_max_get(o, &max_w, &max_h);
      if (!strcmp(vol->mount_point, "/"))
         e_box_pack_start(box, o);
      else
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
               e_menu_item_callback_set(mi, _places_run_fm, strdup(uri->path)); //TODO free somewhere
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

   /* Home */
   if (places_conf->show_home)
   {
      mi = e_menu_item_new(em);
      e_menu_item_label_set(mi, D_("Home"));
      e_util_menu_item_theme_icon_set(mi, "user-home");
      e_menu_item_callback_set(mi, _places_run_fm, (char*)e_user_homedir_get());
   }

   /* Desktop */
   if (places_conf->show_desk)
   {
      mi = e_menu_item_new(em);
      e_menu_item_label_set(mi, D_("Desktop"));
      e_util_menu_item_theme_icon_set(mi, "user-desktop");
      snprintf(buf, sizeof(buf), "%s/Desktop", (char*)e_user_homedir_get());
      e_menu_item_callback_set(mi, _places_run_fm, strdup(buf)); //TODO free somewhere
   }

   /* Trash */
   if (places_conf->show_trash)
   {
      mi = e_menu_item_new(em);
      e_menu_item_label_set(mi, D_("Trash"));
      e_util_menu_item_theme_icon_set(mi, "folder");
      e_menu_item_callback_set(mi, _places_run_fm, "trash:///");
   }

   /* File System */
   if (places_conf->show_root)
   {
      mi = e_menu_item_new(em);
      e_menu_item_label_set(mi, D_("Filesystem"));
      e_util_menu_item_theme_icon_set(mi, "drive-harddisk");
      e_menu_item_callback_set(mi, _places_run_fm, "/");
   }

   /* Temp */
   if (places_conf->show_temp)
   {
      mi = e_menu_item_new(em);
      e_menu_item_label_set(mi, D_("Temp"));
      e_util_menu_item_theme_icon_set(mi, "user-temp");
      e_menu_item_callback_set(mi, _places_run_fm, "/tmp");
   }

   //separator
   if (places_conf->show_home || places_conf->show_desk ||
       places_conf->show_trash || places_conf->show_root ||
       places_conf->show_temp)
   {
      mi = e_menu_item_new(em);
      e_menu_item_separator_set(mi, 1);
   }

   /* Volumes */
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
      {
         if (strncmp(vol->icon, "e/", 2))
            e_menu_item_icon_edje_set(mi, theme_file, vol->icon);
         else
            e_menu_item_icon_edje_set(mi,
                                      e_theme_edje_file_get("base/theme/fileman",
                                                            vol->icon), vol->icon);
      }
      e_menu_item_callback_set(mi, places_menu_click_cb, (void*)vol);
      volumes_visible = 1;
   }

   /* Favorites */
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
static int
_places_poller(void *data)
{
   Eina_List *l;

   for (l = volumes; l; l = l->next)
   {
      Volume *vol = l->data;

      if (!vol->valid || !vol->mounted) continue;
      _places_update_size(vol->obj, vol);
   }

   return 1;
}

static void
_places_print_volume(Volume *v)
{
   printf("Got volume %s\n", v->udi);
   printf("  label: %s\n",v->label);
   printf("  mounted: %d\n", v->mounted);
   printf("  m_point: %s\n", v->mount_point);
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
_places_volume_add(const char *udi)
{
   Volume *v;
   if (!udi) return;
   
   v = E_NEW(Volume, 1);
   if (!v) return;

   v->udi = eina_stringshare_add(udi);
   v->valid = 0;
   v->obj = NULL;
   v->icon = NULL;
   v->to_mount = 0;
   v->force_open = 0;
   
   if (places_conf->auto_mount)
      v->to_mount = 1;
   
   if (places_conf->auto_open)
      v->force_open = 1;
   
   volumes = eina_list_append(volumes, v);
   e_hal_device_get_all_properties(conn, v->udi, _places_volume_properties_cb, v);
   
   v->sh_prop = e_dbus_signal_handler_add(conn, "org.freedesktop.Hal", v->udi,
                                          "org.freedesktop.Hal.Device",
                                          "PropertyModified",
                                          _places_volume_prop_modified_cb, v);
}

static void
_places_volume_del(Volume *v)
{
   e_dbus_signal_handler_del(conn, v->sh_prop);
   volumes = eina_list_remove(volumes, v);
   eina_stringshare_del(v->udi);
   eina_stringshare_del(v->label);
   eina_stringshare_del(v->mount_point);
   eina_stringshare_del(v->fstype);
   eina_stringshare_del(v->bus);
   eina_stringshare_del(v->drive_type);
   eina_stringshare_del(v->model);
   eina_stringshare_del(v->vendor);
   eina_stringshare_del(v->serial);
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
   struct statfs s;
//printf("GET SPACE of %s\n", mount);
   if (!mount) return 0;
   if (statfs(mount, &s) != 0)
      return 0;
//printf("   SPACE %d\n", (s.f_bfree * s.f_bsize));
   return (unsigned long long)(s.f_bfree * s.f_bsize);
}

static void
_places_error_show(const char *title, const char *text1, const char *text2, const char *text3)
{
   char str[4096];
   E_Dialog *dia;

   snprintf(str, sizeof(str),"%s<br><br>%s<br>%s", text1, text2, text3);
   
   dia = e_dialog_new(NULL, "E", "_places_error");
   e_dialog_title_set(dia, title);
   e_dialog_icon_set(dia, "drive-harddisk", 64);
   e_dialog_button_add(dia, "OK", NULL, NULL, NULL);
   e_dialog_text_set(dia, str);
   e_dialog_show(dia);
}

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
   unsigned long long free;
   
   //Free label
   tot_h = _places_human_size_get(vol->size);
   if (vol->mounted)
   {
      free = _places_free_space_get(vol->mount_point);
      free_h = _places_human_size_get(free);
      snprintf(buf, sizeof(buf), "%s %s %s",free_h, D_("free of"),tot_h);
      edje_object_part_text_set(obj, "size_label", buf);
      eina_stringshare_del(free_h);
   }
   else
   {
      snprintf(buf, sizeof(buf), D_("%s Not Mounted"), tot_h);
      edje_object_part_text_set(obj, "size_label", buf);
   }
   eina_stringshare_del(tot_h);

   //Gauge
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
   Ecore_List *opt = NULL;
   char buf[256];

   if ((!strcmp(vol->fstype, "vfat")) || (!strcmp(vol->fstype, "ntfs")))
   {
      opt = ecore_list_new();
      snprintf(buf, sizeof(buf), "uid=%i", (int)getuid());
      ecore_list_append(opt, buf);
   }
   e_hal_device_volume_mount(conn, vol->udi, vol->mount_point, vol->fstype, opt, _places_mount_cb, vol);
   vol->to_mount = 0;
   if (opt) ecore_list_destroy(opt);
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
      e_hal_device_volume_unmount(conn, vol->udi, NULL, _places_unmount_cb, vol);
   else
      e_hal_device_volume_eject(conn, vol->udi, NULL, _places_eject_cb, vol);
   
}

/***********************/
/* HAL /DBUS Callbacks */
/***********************/
static int
_places_open_when_mounted(void *data)
{
   Volume *vol = data;

   if (vol->mount_point)
   {
      _places_run_fm((void*)vol->mount_point, NULL, NULL);
      return 0;
   }

   return 1;
}

void
_places_mount_cb(void *user_data, void *method_return, DBusError *error)
{
   Volume *vol = user_data;

   if (dbus_error_is_set(error))
   {
      _places_error_show("Mount Error", "Can't mount device.",error->name, error->message);
      dbus_error_free(error);
      return;
   }

   if (vol->force_open)
   {
      ecore_timer_add(0.1, _places_open_when_mounted, vol);
      vol->force_open = 0;
   }
}

void
_places_unmount_cb(void *user_data, void *method_return, DBusError *error)
{
   Volume *vol = user_data;

   if (dbus_error_is_set(error))
   {
      _places_error_show("Unmount Error", "Can't unmount device.",error->name, error->message);
      dbus_error_free(error);
   }
}

void
_places_eject_cb(void *user_data, void *method_return, DBusError *error)
{
   Volume *vol = user_data;

   if (dbus_error_is_set(error))
   {
      _places_error_show("Eject Error", "Can't eject device.",error->name, error->message);
      dbus_error_free(error);
   }
}

/* Dbus CB - Generic device added */
void
_places_device_add_cb(void *data, DBusMessage *msg)
{
   DBusError err;
   char *udi;

   dbus_error_init(&err);
   dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);

   //printf("PLACES DBUS CB UDI:%s\n", udi);
   e_hal_device_query_capability(conn, udi, "volume",
                                 _places_volume_cb,
                                 (void*)eina_stringshare_add(udi));
}

/* Dbus CB - Generic device removed */
void
_places_device_rem_cb(void *data, DBusMessage *msg)
{
   DBusError err;
   char *udi;
   Eina_List *l;

   dbus_error_init(&err);
   dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);

   for (l = volumes; l; l = l->next)
   {
      Volume *v;
      
      v = l->data;
      if (!strcmp(v->udi, udi))
      {
         printf("PLACES Removed %s\n", v->udi);
         _places_volume_del(v);
         places_update_all_gadgets();
         return;
      }
   }
}

/* Dbus CB - Called when a device change some properties */
void
_places_volume_prop_modified_cb(void *data, DBusMessage *msg)
{
   Volume *v = data;
   printf("properties\n");
   e_hal_device_get_all_properties(conn, v->udi, _places_volume_properties_cb, v);
}

/* Dbus CB - Reply of all device of type "volume" */
void
_places_volume_all_cb(void *user_data, void *reply_data, DBusError *error)
{
   E_Hal_Manager_Find_Device_By_Capability_Return *ret = reply_data;
   Eina_List *l;
   char *udi;
  
   if (!ret || !ret->strings) return;

   if (dbus_error_is_set(error)) 
   {
      // XXX handle...
      dbus_error_free(error);
      return;
   }

   EINA_LIST_FOREACH(ret->strings, l, udi)
      _places_volume_add(udi);
   
   //TODO free ret??
}

/* Dbus CB - Reply of capability of type "volume" */
void
_places_volume_cb(void *user_data, void *reply_data, DBusError *error)
{
   Volume *v;
   char *udi = user_data;
   E_Hal_Device_Query_Capability_Return *ret = reply_data;

   if (ret && ret->boolean)
   {
      //~ printf("PLACES DBUS CB UDI:%s\n", udi);
      _places_volume_add(udi);
   }

   eina_stringshare_del(udi);
}

/* Dbus CB - Volume get properties */
void
_places_volume_properties_cb(void *data, void *reply_data, DBusError *error)
{
   Volume *v = data;
   E_Hal_Device_Get_All_Properties_Return *reply = reply_data;
   int err = 0;
   char *str = NULL;

   /* skip volumes with volume.ignore set */
   if (e_hal_property_bool_get(reply, "volume.ignore", &err) || err)
      return;

   /* skip volumes that aren't filesystems */
   str = e_hal_property_string_get(reply, "volume.fsusage", &err);
   if (err || !str || strcmp(str, "filesystem"))
   {
      FREE_STR(str);
      return;
   }
   //~ v->uuid = e_hal_property_string_get(ret, "volume.uuid", &err);
  //~ if (err) goto error;

   str = e_hal_property_string_get(reply, "volume.label", &err);
   if (!err) v->label = eina_stringshare_add(str);
   FREE_STR(str);

   v->mounted = e_hal_property_bool_get(reply, "volume.is_mounted", &err);

   str = e_hal_property_string_get(reply, "volume.mount_point", &err);
   if (!err) v->mount_point = eina_stringshare_add(str);
   FREE_STR(str);

   str = e_hal_property_string_get(reply, "volume.fstype", &err);
   if (!err) v->fstype = eina_stringshare_add(str);
   FREE_STR(str);

   v->size = e_hal_property_uint64_get(reply, "volume.size", &err);
   
  //~ v->partition = e_hal_property_bool_get(ret, "volume.is_partition", &err);
  //~ if (err) goto error;

  //~ if (v->partition)
  //~ {
    //~ v->partition_label = e_hal_property_string_get(ret, "volume.partition.label", &err);
    //~ if (err) goto error;
  //~ }

   str = e_hal_property_string_get(reply, "info.parent", &err);
   if (!err && str)
   {
      e_hal_device_get_all_properties(conn, str, _places_storage_properties_cb, v);
      FREE_STR(str);
   }

   return;
}

/* Dbus CB - Storage get properties */
void
_places_storage_properties_cb(void *data, void *reply_data, DBusError *error)
{
   Volume *v = data;
   E_Hal_Properties *ret = reply_data;
   int err = 0;
   char *str;

   if (!v) return;
   if (dbus_error_is_set(error)) 
   {
      dbus_error_free(error);
      return;
   }
   
   str = e_hal_property_string_get(ret, "storage.bus", &err);
   if (!err) v->bus = eina_stringshare_add(str);
   FREE_STR(str);
   
   
   str = e_hal_property_string_get(ret, "storage.drive_type", &err);
   if (!err) v->drive_type = eina_stringshare_add(str);
   FREE_STR(str);
   
   str = e_hal_property_string_get(ret, "storage.model", &err);
   if (!err) v->model = eina_stringshare_add(str);
   FREE_STR(str);
   
   str = e_hal_property_string_get(ret, "storage.vendor", &err);
   if (!err) v->vendor = eina_stringshare_add(str);
   FREE_STR(str);
   
   str = e_hal_property_string_get(ret, "storage.serial", &err);
   if (!err) v->serial = eina_stringshare_add(str);
   FREE_STR(str);
   
   v->removable = e_hal_property_bool_get(ret, "storage.removable", &err);
   v->requires_eject = e_hal_property_bool_get(ret, "storage.requires_eject", &err);

  //~ // if (s->removable)
   //~ {
      //~ s->media_available = e_hal_property_bool_get(ret, "storage.removable.media_available", &err);
      //~ s->media_size = e_hal_property_uint64_get(ret, "storage.removable.media_size", &err);
   //~ }

   
   //~ s->hotpluggable = e_hal_property_bool_get(ret, "storage.hotpluggable", &err);
   //~ s->media_check_enabled = e_hal_property_bool_get(ret, "storage.media_check_enabled", &err);

   //~ s->icon.drive = e_hal_property_string_get(ret, "storage.icon.drive", &err);
   //~ s->icon.volume = e_hal_property_string_get(ret, "storage.icon.volume", &err);
   
   //_places_print_volume(v);  //Use this for debug
   v->valid = 1;
   
   if (v->to_mount && !v->mounted)
     {
	Eina_Bool enabled;

	enabled = e_hal_property_bool_get(ret, "storage.automount_enabled_hint", &err);
	if (err)
	  enabled = 1; /* assume no property it is enabled */

	if (enabled)
	  _places_mount_volume(v);
     }
   v->to_mount = 0;
   
   places_update_all_gadgets(); //TODO Update only this volume, not all
   return;
}


