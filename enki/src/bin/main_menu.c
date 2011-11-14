#include "main.h"
#include "slideshow.h"
#include "evas_object/photo_object.h"

static Evas_Object *inwin = NULL;
static Evas_Object *list = NULL;

static Elm_Genlist_Item_Class itc_menu;

static Evas_Object *libraries_list;
static Elm_Genlist_Item *bt_import;
static Elm_Genlist_Item *bt_slideshow;
static Elm_Genlist_Item *bt_del_bg;
static Elm_Genlist_Item *bt_album_new;
static Elm_Genlist_Item *bt_sync;

static void
_libraries_cb(void *data, Evas_Object *obj, void *event_info);

static char *
_gl_label_get(void *data, Evas_Object *obj, const char *part);
static void
_gl_sel(void *data, Evas_Object *obj, void *event_info);
static Evas_Object *
_gl_icon_get(void *data, Evas_Object *obj, const char *part);

static Elm_Gengrid_Item_Class itc_grid;

static char *
_library_get(void *data, Evas_Object *obj, const char *part);
static void
_library_del(void *data, Evas_Object *obj);
static Evas_Object *
_library_icon_get(void *data, Evas_Object *obj, const char *part);

static void
_library_select(void *data, Evas_Object *obj, void *event_info);
static void
_new_library();
static void
_new_library_done_cb(void *data, Evas_Object *obj, void *event_info);

static void
_slideshow_cb();
static void
_album_new_cb();
static void
_import_cb();
static void
_del_bg_cb();
static void
_preferences_cb();
static void
_quit_cb();
static void
_sync_cb();

typedef struct
{
   Eina_Bool is_new_library;
   const char *path;
   Enlil_Photo *photo1;
   Enlil_Photo *photo2;
} Library;

void
main_menu_new(Evas_Object *edje)
{
   Elm_Genlist_Item *item;
   Evas_Object *bt;

   //Libraries
   //libraries_list = edje_object_part_external_object_get(edje, "object.main_menu.list_libraries");
   libraries_list = elm_gengrid_add(edje);
   elm_gengrid_item_size_set(libraries_list, 256, 256);
   if (!edje_object_part_swallow(edje, "object.main_menu.list_libraries",
                                 libraries_list)) edje_object_part_swallow(
                                                                           global_object,
                                                                           "object.main_menu.list_libraries",
                                                                           libraries_list);

   itc_grid.func.label_get = _library_get;
   itc_grid.func.del = _library_del;
   itc_grid.func.content_get = _library_icon_get;
   itc_grid.item_style = "enki";
   //

   itc_menu.item_style = "default";
   itc_menu.func.label_get = _gl_label_get;
   itc_menu.func.content_get = _gl_icon_get;
   itc_menu.func.state_get = NULL;
   itc_menu.func.del = NULL;

   bt_slideshow = enki_elm_genlist_item_menu_append(edje, &itc_menu,
                                                    (void*) MENU_SLIDESHOW,
                                                    NULL, _gl_sel,
                                                    (void*) MENU_SLIDESHOW);

   bt_album_new = enki_elm_genlist_item_menu_append(edje, &itc_menu,
                                                    (void*) MENU_ALBUM_NEW,
                                                    NULL, _gl_sel,
                                                    (void*) MENU_ALBUM_NEW);

   bt_import = enki_elm_genlist_item_menu_append(edje, &itc_menu,
                                                 (void*) MENU_IMPORT_PHOTOS,
                                                 NULL, _gl_sel,
                                                 (void*) MENU_IMPORT_PHOTOS);

   bt_del_bg = enki_elm_genlist_item_menu_append(edje, &itc_menu,
                                                 (void*) MENU_DEL_BG, NULL,
                                                 _gl_sel, (void*) MENU_DEL_BG);

   item = enki_elm_genlist_item_menu_append(edje, &itc_menu,
                                            (void*) MENU_PREFERENCES, NULL,
                                            _gl_sel, (void*) MENU_PREFERENCES);

   bt_sync = enki_elm_genlist_item_menu_append(edje, &itc_menu,
                                               (void*) MENU_WEBSYNC, NULL,
                                               _gl_sel, (void*) MENU_WEBSYNC);

   bt
            = edje_object_part_external_object_get(edje,
                                                   "object.menu.bt.preferences");
   if (bt) evas_object_smart_callback_add(bt, "clicked",
                                          (Evas_Smart_Cb) _preferences_cb, NULL);

   bt = edje_object_part_external_object_get(edje, "object.menu.bt.libraries");
   if (bt) evas_object_smart_callback_add(bt, "clicked", _libraries_cb, edje);

   //
   //   bt = enki_elm_genlist_item_menu_append(edje, &itc_menu,
   //                                          (void*)MENU_CLOSEENKI, NULL,
   //                                          _gl_sel, (void*)MENU_CLOSEENKI);

   //library list
   main_menu_update_libraries_list();
}

static char *
_gl_label_get(void *data, Evas_Object *obj, const char *part)
{
   if ((int) data == MENU_SLIDESHOW) return strdup("Slideshow");
   if ((int) data == MENU_ALBUM_NEW) return strdup("New Album");
   if ((int) data == MENU_IMPORT_PHOTOS) return strdup("Import Photos");
   if ((int) data == MENU_DEL_BG) return strdup("Remove the wallpaper");
   if ((int) data == MENU_PREFERENCES) return strdup("Preferences");
   if ((int) data == MENU_WEBSYNC) return strdup("Launch WEB Synchronization");
   if ((int) data == MENU_CLOSEENKI) return strdup("Close Enki");
}

static void
_gl_sel(void *data, Evas_Object *obj, void *event_info)
{
   if ((int) data == MENU_SLIDESHOW) _slideshow_cb();
   if ((int) data == MENU_ALBUM_NEW) _album_new_cb();
   if ((int) data == MENU_IMPORT_PHOTOS) _import_cb();
   if ((int) data == MENU_DEL_BG) _del_bg_cb();
   if ((int) data == MENU_PREFERENCES) _preferences_cb();
   if ((int) data == MENU_WEBSYNC) _sync_cb();
   if ((int) data == MENU_CLOSEENKI) _quit_cb();

   Elm_Genlist_Item *item = elm_genlist_selected_item_get(obj);
   elm_genlist_item_selected_set(item, EINA_FALSE);
}

static Evas_Object *
_gl_icon_get(void *data, Evas_Object *obj, const char *part)
{
   Evas_Object *icon = elm_icon_add(obj);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   if (strcmp(part, "elm.swallow.icon")) return NULL;

   if ((int) data == MENU_SLIDESHOW)
      elm_icon_file_set(icon, Theme, "icons/slideshow");
   else if ((int) data == MENU_ALBUM_NEW)
      elm_icon_file_set(icon, Theme, "icons/add");
   else if ((int) data == MENU_IMPORT_PHOTOS)
      elm_icon_file_set(icon, Theme, "icons/add");
   else if ((int) data == MENU_DEL_BG)
      elm_icon_file_set(icon, Theme, "icons/delete");
   else if ((int) data == MENU_WEBSYNC)
      elm_icon_file_set(icon, Theme, "icons/websync");
   else if ((int) data == MENU_CLOSEENKI)
      elm_icon_file_set(icon, Theme, "icons/close");
   else
      elm_icon_file_set(icon, Theme, "icons/menu");

   return icon;
}

void
main_menu_loading_disable_set(Eina_Bool disabled)
{
   elm_object_disabled_set(libraries_list, disabled);
   elm_genlist_item_disabled_set(bt_import, disabled);
   elm_genlist_item_disabled_set(bt_album_new, disabled);
   elm_genlist_item_disabled_set(bt_slideshow, disabled);
   elm_genlist_item_disabled_set(bt_sync, disabled);
}

void
main_menu_sync_disable_set(Eina_Bool disabled)
{
   elm_object_disabled_set(libraries_list, disabled);
}

void
main_menu_nolibrary_disabled_set(Eina_Bool disabled)
{
   main_menu_loading_disable_set(disabled);
   elm_object_disabled_set(libraries_list, EINA_FALSE);
   elm_genlist_item_disabled_set(bt_del_bg, disabled);
   elm_genlist_item_disabled_set(bt_sync, disabled);
}

void
main_menu_update_libraries_list()
{
   Enlil_String *string;
   Eina_List *list = enlil_library_eet_path_load();

   elm_gengrid_clear(libraries_list);

   //
   Library *lib = calloc(1, sizeof(Library));
   lib->is_new_library = EINA_TRUE;
   lib->path = eina_stringshare_add("Add a new Library");
   elm_gengrid_item_append(libraries_list, &itc_grid, lib, _library_select,
                           NULL);
   //

EINA_LIST_FREE(list, string)
{
   Library *lib = calloc(1, sizeof(Library));
   lib->is_new_library = EINA_FALSE;
   lib->path = eina_stringshare_add(string->string);

   lib->photo1 = enlil_photo_new();
   Enlil_Photo_Data *photo_data1 = calloc(1, sizeof(Enlil_Photo_Data));
   enlil_photo_user_data_set(lib->photo1, photo_data1, enlil_photo_data_free);
   enlil_photo_mustNotBeSaved_set(lib->photo1, EINA_TRUE);

   lib->photo2 = enlil_photo_new();
   Enlil_Photo_Data *photo_data2 = calloc(1, sizeof(Enlil_Photo_Data));
   enlil_photo_user_data_set(lib->photo2, photo_data2, enlil_photo_data_free);
   enlil_photo_mustNotBeSaved_set(lib->photo2, EINA_TRUE);

   photo_data1->library_item = elm_gengrid_item_append(libraries_list, &itc_grid, lib, _library_select, NULL);
   photo_data2->library_item = photo_data1->library_item;

   EINA_STRINGSHARE_DEL(string->string);
   FREE(string);
}
}

static char *
_library_get(void *data, Evas_Object *obj, const char *part)
{
   Library *lib = data;
   char *name = strdup(lib->path);
   char *name_old = name;

   if (strlen(name) > 40)
   {
      name = name + strlen(name) - 40;

      int i = 0;
      name--;
      while (i < 3 && name >= name_old)
      {
         *name = '.';
         name--;
         i++;
      }
      name++;
   }

   name = strdup(name);
   FREE(name_old);

   return name;
}

static void
_library_del(void *data, Evas_Object *obj)
{
   Library *lib = data;
   EINA_STRINGSHARE_DEL(lib->path);
   if (lib->photo1) enlil_photo_free(&(lib->photo1));
   if (lib->photo2) enlil_photo_free(&(lib->photo2));
   FREE(lib);
}

static Evas_Object *
_library_icon_get(void *data, Evas_Object *obj, const char *part)
{
   const char *s1 = NULL, *s2 = NULL, *g1 = NULL, *g2 = NULL;
   Library *lib = data;

   if (strcmp(part, "elm.swallow.icon")) return NULL;

   if (lib->is_new_library)
   {
      Evas_Object *o = elm_layout_add(obj);
      elm_layout_file_set(o, Theme, "photo_library");

      Evas_Object *icon = elm_icon_add(obj);
      elm_icon_file_set(icon, Theme, "icons/plus");
      elm_icon_fill_outside_set(icon, EINA_TRUE);
      elm_layout_content_set(o, "object.photo.front.swallow", icon);

      icon = elm_icon_add(obj);
      elm_icon_file_set(icon, Theme, "icons/folder");
      elm_icon_fill_outside_set(icon, EINA_TRUE);
      elm_layout_content_set(o, "object.photo.back.swallow", icon);

      return o;
   }

   Evas_Object *o = elm_layout_add(obj);
   elm_layout_file_set(o, Theme, "photo_library");

   //
   Enlil_Photo *photo1 = enlil_library_photo_get(lib->path, 1);
   if (photo1)
   {
      if (enlil_photo_path_get(lib->photo1) != enlil_photo_path_get(photo1)
               || enlil_photo_file_name_get(lib->photo1)
                        != enlil_photo_file_name_get(photo1))
      {
         enlil_photo_path_set(lib->photo1, enlil_photo_path_get(photo1));
         enlil_photo_file_name_set(lib->photo1,
                                   enlil_photo_file_name_get(photo1));
      }
      Enlil_Photo_Data *photo_data1 = enlil_photo_user_data_get(lib->photo1);

      if (photo1 && !photo_data1->cant_create_thumb) s1
               = enlil_thumb_photo_get(lib->photo1, Enlil_THUMB_FDO_LARGE,
                                       thumb_done_cb, thumb_error_cb, NULL);

      enlil_photo_free(&(photo1));
      //

      //
      Enlil_Photo *photo2 = enlil_library_photo_get(lib->path, 2);
      if (enlil_photo_path_get(lib->photo2) != enlil_photo_path_get(photo2)
               || enlil_photo_file_name_get(lib->photo2)
                        != enlil_photo_file_name_get(photo2))
      {
         enlil_photo_path_set(lib->photo2, enlil_photo_path_get(photo2));
         enlil_photo_file_name_set(lib->photo2,
                                   enlil_photo_file_name_get(photo2));
      }
      Enlil_Photo_Data *photo_data2 = enlil_photo_user_data_get(lib->photo2);

      if (photo2 && !photo_data2->cant_create_thumb) s2
               = enlil_thumb_photo_get(lib->photo2, Enlil_THUMB_FDO_LARGE,
                                       thumb_done_cb, thumb_error_cb, NULL);

      enlil_photo_free(&(photo2));
      //

      if (!s1)
      {
         s1 = Theme;
         g1 = "libraries/icon/nophoto";
      }
      if (!s2)
      {
         s2 = Theme;
         g2 = "libraries/icon/nophoto";
      }
   }
   else
   {
      s1 = Theme;
      s2 = Theme;
      g1 = "libraries/icon/nophoto";
      g2 = "libraries/icon/nophoto";
   }

   Evas_Object *icon = elm_icon_add(obj);
   elm_icon_file_set(icon, s1, g1);
   elm_icon_fill_outside_set(icon, EINA_TRUE);
   elm_layout_content_set(o, "object.photo.front.swallow", icon);

   icon = elm_icon_add(obj);
   elm_icon_file_set(icon, s2, g2);
   elm_icon_fill_outside_set(icon, EINA_TRUE);
   elm_layout_content_set(o, "object.photo.back.swallow", icon);

   evas_object_show(o);
   return o;
}

static void
_library_select(void *data, Evas_Object *obj, void *event_info)
{
   Library *lib = elm_gengrid_item_data_get(elm_gengrid_selected_item_get(obj));

   if (lib->is_new_library)
   {
      _new_library();
   }
   else
   {
      library_set(lib->path);
      select_list_photo();
   }

   //unselect items
   elm_gengrid_item_selected_set(elm_gengrid_selected_item_get(obj), EINA_FALSE);
   //
}

static void
_new_library()
{
   Evas_Object *fs, *vbox;

   //create inwin & file selector
   inwin = elm_win_inwin_add(enlil_data->win->win);
   evas_object_show(inwin);

   vbox = elm_box_add(inwin);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(vbox);
   elm_win_inwin_content_set(inwin, vbox);

   fs = elm_fileselector_add(inwin);
   elm_fileselector_folder_only_set(fs, 1);
   elm_fileselector_expandable_set(fs, EINA_FALSE);
   elm_fileselector_path_set(fs, getenv("HOME"));
   evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(vbox, fs);
   evas_object_show(fs);

   evas_object_smart_callback_add(fs, "done", _new_library_done_cb, inwin);
}

static void
_new_library_done_cb(void *data, Evas_Object *obj, void *event_info)
{
   const char *selected = event_info;
   if (selected)
   {
      LOG_INFO("Create new library: %s\n", elm_fileselector_selected_get(obj));
      if (!ecore_file_exists(selected)) ecore_file_mkdir(selected);
      if (ecore_file_is_dir(selected))
      {
         library_set(selected);
         select_list_photo();
         main_menu_update_libraries_list();
      }
   }

   evas_object_del(data);
}

static void
_slideshow_cb()
{
   Enlil_Photo *photo = NULL;
   PL_Child_Item *item = NULL;

   Eina_List *items =
            photos_list_object_selected_get(enlil_data->list_photo->o_list);

   if (items)
   {
      item = eina_list_data_get(items);
      photo = photos_list_object_item_data_get(item);
   }

   slideshow_clear();
   slideshow_library_add(enlil_data->library, photo);
   slideshow_show();
}

static void
_album_new_cb()
{
   inwin_album_new_new(NULL, NULL);
}

static void
_import_cb()
{
   import_new(enlil_data->win->win);
}

static void
_del_bg_cb()
{
   char buf[PATH_MAX];
   Enlil_String s;
   Eet_Data_Descriptor *edd;

   enlil_win_bg_set(enlil_data->win, NULL);

   s.string = NULL;
   edd = enlil_string_edd_new();

   snprintf(buf, PATH_MAX, "%s %s", APP_NAME " background",
            enlil_library_path_get(enlil_data->library));
   enlil_eet_app_data_save(edd, buf, &s);
   eet_data_descriptor_free(edd);
}

static void
_preferences_cb()
{
   inwin_preferences_new();
}

static void
_sync_cb()
{
   enlil_netsync_job_sync_albums_append(enlil_data->library,
                                        netsync_album_new_cb,
                                        netsync_album_notinnetsync_cb,
                                        netsync_album_notuptodate_cb,
                                        netsync_album_netsyncnotuptodate_cb,
                                        netsync_album_uptodate_cb, enlil_data);
}

static void
_quit_cb()
{
   close_cb(NULL, NULL, NULL);
}

static void
_libraries_cb(void *data, Evas_Object *obj, void *event_info)
{
   edje_object_signal_emit(data, "main_panel,menu,show", "");
   edje_object_signal_emit(global_object, "main_panel,menu,show", "");
   enlil_data->list_left->is_map = EINA_FALSE;
}
