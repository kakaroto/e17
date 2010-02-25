// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "main.h"

Enlil_Photo *current_photo = NULL;

static Evas_Object *inwin;

static Evas_Object *menu_file;
static Elm_Menu_Item *inew_root;
static Elm_Menu_Item *iroots;
static Elm_Menu_Item *iimport;
static Elm_Menu_Item *islideshow;
static Elm_Menu_Item *idel_bg;
static Elm_Menu_Item *isave;
static Elm_Menu_Item *isaveas;
static Elm_Menu_Item *ialbum_new;
static Elm_Menu_Item *i1_1;
static Elm_Menu_Item *ifit;
static Elm_Menu_Item *ifill;
static Elm_Menu_Item *i90;
static Elm_Menu_Item *ir90;
static Elm_Menu_Item *i180;
static Elm_Menu_Item *ivertical;
static Elm_Menu_Item *ihorizontal;
static Elm_Menu_Item *iblur;
static Elm_Menu_Item *isharpen;
static Elm_Menu_Item *isepia;
static Elm_Menu_Item *igrayscale;
static Elm_Menu_Item *iredo;
static Elm_Menu_Item *iundo;
static Elm_Menu_Item *iphoto_delete;
static Elm_Menu_Item *iphoto_thumb_reload;
static Elm_Menu_Item *iphoto_exif_reload;
static Elm_Menu_Item *iphoto_iptc_reload;
static Elm_Menu_Item *iphoto_bg;

static void _quit_cb(void *data, Evas_Object *obj, void *event_info);
static void _import_cb(void *data, Evas_Object *obj, void *event_info);
static void _save_cb(void *data, Evas_Object *obj, void *event_info);
static void _save_as_cb(void *data, Evas_Object *obj, void *event_info);
static void _slideshow_cb(void *data, Evas_Object *obj, void *event_info);
static void _1_1_cb(void *data, Evas_Object *obj, void *event_info);
static void _fit_cb(void *data, Evas_Object *obj, void *event_info);
static void _fill_cb(void *data, Evas_Object *obj, void *event_info);
static void _rotation_90_cb(void *data, Evas_Object *obj, void *event_info);
static void _rotation_R90_cb(void *data, Evas_Object *obj, void *event_info);
static void _rotation_180_cb(void *data, Evas_Object *obj, void *event_info);
static void _flip_vertical_cb(void *data, Evas_Object *obj, void *event_info);
static void _flip_horizontal_cb(void *data, Evas_Object *obj, void *event_info);
static void _blur_cb(void *data, Evas_Object *obj, void *event_info);
static void _sharpen_cb(void *data, Evas_Object *obj, void *event_info);
static void _sepia_cb(void *data, Evas_Object *obj, void *event_info);
static void _grayscale_cb(void *data, Evas_Object *obj, void *event_info);
static void _undo_cb(void *data, Evas_Object *obj, void *event_info);
static void _redo_cb(void *data, Evas_Object *obj, void *event_info);
static void _photo_delete_cb(void *data, Evas_Object *obj, void *event_info);
static void _photo_thumb_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void _photo_iptc_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void _photo_exif_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void _photo_wall_set_cb(void *data, Evas_Object *obj, void *event_info);
static void _del_bg_cb(void *data, Evas_Object *obj, void *event_info);

static void _geocaching_import_cb(void *data, Evas_Object *obj, void *event_info);
static void _geocaching_import_done_cb(void *data, Evas_Object *obj, void *event_info);

static void _preferences_cb(void *data, Evas_Object *obj, void *event_info);


static void _new_root_cb(void *data, Evas_Object *obj, void *event_info);
static void _new_root_done_cb(void *data, Evas_Object *obj, void *event_info);

static void _root_open_cb(void *data, Evas_Object *obj, void *event_info);

static void _album_new_cb(void *data, Evas_Object *obj, void *event_info);


Evas_Object *menu_new(Evas_Object *parent)
{
   Evas_Object *menu, *tm, *icon;
   Elm_Toolbar_Item *item;
   Elm_Menu_Item *mi;

   menu = elm_toolbar_add(parent);
   elm_toolbar_homogenous_set(menu, 0);
   elm_toolbar_align_set(menu, 0.0);
   evas_object_size_hint_weight_set(menu, 1.0, 0.0);
   evas_object_size_hint_align_set(menu, EVAS_HINT_FILL, 0.0);

   //
   item = elm_toolbar_item_add(menu, NULL, "File", NULL, NULL);
   elm_toolbar_item_menu_set(item, 1);
   tm = elm_toolbar_item_menu_get(item);
   menu_file = tm;
   elm_toolbar_menu_parent_set(menu, parent);

   inew_root = elm_menu_item_add(tm, NULL, NULL, D_("New Library folder"), _new_root_cb, NULL);
   iroots = elm_menu_item_add(tm, NULL, NULL, D_("Open Library"), NULL, NULL);
   elm_menu_item_separator_add(tm, NULL);
   isave = elm_menu_item_add(tm, NULL, NULL, D_("Save"), _save_cb, NULL);
   isaveas = elm_menu_item_add(tm, NULL, NULL, D_("Save as ..."), _save_as_cb, NULL);

   elm_menu_item_separator_add(tm, NULL);
   iimport = elm_menu_item_add(tm, NULL, NULL, D_("Import"), NULL, NULL);
   mi = elm_menu_item_add(tm, iimport, NULL, D_("Import photos"), _import_cb, NULL);

   elm_menu_item_separator_add(tm, iimport);

   mi = elm_menu_item_add(tm, NULL, NULL, D_("Quit"), _quit_cb, NULL);
   //

   //
   item = elm_toolbar_item_add(menu, NULL, D_("Tools"), NULL, NULL);
   elm_toolbar_item_menu_set(item, 1);
   tm = elm_toolbar_item_menu_get(item);
   elm_toolbar_menu_parent_set(menu, parent);

   elm_menu_item_add(tm, NULL, NULL, D_("Preferences"), _preferences_cb, NULL);
   //

   //
   item = elm_toolbar_item_add(menu, NULL, D_("View"), NULL, NULL);
   elm_toolbar_item_menu_set(item, 1);
   tm = elm_toolbar_item_menu_get(item);
   elm_toolbar_menu_parent_set(menu, parent);

   islideshow = elm_menu_item_add(tm, NULL, NULL, D_("Slideshow"), _slideshow_cb, NULL);
   idel_bg = elm_menu_item_add(tm, NULL, NULL, D_("Remove the wallpaper"), _del_bg_cb, NULL);
   //

   //
   item = elm_toolbar_item_add(menu, NULL, "Album", NULL, NULL);
   elm_toolbar_item_menu_set(item, 1);
   tm = elm_toolbar_item_menu_get(item);
   elm_toolbar_menu_parent_set(menu, parent);

   ialbum_new = elm_menu_item_add(tm, NULL, NULL, D_("New album"), _album_new_cb, NULL);
   //

   //
   item = elm_toolbar_item_add(menu, NULL, "Photo", NULL, NULL);
   elm_toolbar_item_menu_set(item, 1);
   tm = elm_toolbar_item_menu_get(item);
   elm_toolbar_menu_parent_set(menu, parent);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/undo");
   iundo = elm_menu_item_add(tm, NULL, icon, D_("Undo"), _undo_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/redo");
   iredo = elm_menu_item_add(tm, NULL, icon, D_("Redo"), _redo_cb, NULL);

   elm_menu_item_separator_add(tm, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/1_1");
   i1_1 = elm_menu_item_add(tm, NULL, icon, D_("1:1"), _1_1_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/fit");
   ifit = elm_menu_item_add(tm, NULL, icon, D_("Fit"), _fit_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/fill");
   ifill = elm_menu_item_add(tm, NULL, icon, D_("Fill"), _fill_cb, NULL);

   elm_menu_item_separator_add(tm, NULL);
   mi = elm_menu_item_add(tm, NULL, NULL, D_("Transform"), NULL, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/flip/vertical");
   ivertical = elm_menu_item_add(tm, mi, icon, D_("Flip vertical"), _flip_vertical_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/flip/horizontal");
   ihorizontal = elm_menu_item_add(tm, mi, icon, D_("Flip Horizontal"), _flip_horizontal_cb, NULL);

   elm_menu_item_separator_add(tm, mi);
   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/rotate/90");
   i90 = elm_menu_item_add(tm, mi, icon, D_("Rotate +90°"), _rotation_90_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/rotate/90/reverse");
   ir90 = elm_menu_item_add(tm, mi, icon, D_("Rotate -90°"), _rotation_R90_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/rotate/180");
   i180 = elm_menu_item_add(tm, mi, icon, D_("Rotate 180°"), _rotation_180_cb, NULL);

   mi = elm_menu_item_add(tm, NULL, NULL, D_("Effects"), NULL, NULL);


   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/blur");
   iblur = elm_menu_item_add(tm, mi, icon, D_("Blur"), _blur_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/sharpen");
   isharpen = elm_menu_item_add(tm, mi, icon, D_("Sharpen"), _sharpen_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/sepia");
   isepia = elm_menu_item_add(tm, mi, icon, D_("Sepia"), _sepia_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/grayscale");
   igrayscale = elm_menu_item_add(tm, mi, icon, D_("Grayscale"), _grayscale_cb, NULL);

   elm_menu_item_separator_add(tm, NULL);

   mi = elm_menu_item_add(tm, NULL, NULL, D_("Set as wallpaper"), NULL, NULL);
   iphoto_bg = elm_menu_item_add(tm, mi, NULL, D_("To the application"), _photo_wall_set_cb, NULL);

   iphoto_thumb_reload = elm_menu_item_add(tm, NULL, NULL, D_("Recreate the thumbnail"), _photo_thumb_reload_cb, NULL);
   iphoto_exif_reload = elm_menu_item_add(tm, NULL, NULL, D_("Reload the exifs data"), _photo_exif_reload_cb, NULL);
   iphoto_iptc_reload = elm_menu_item_add(tm, NULL, NULL, D_("Reload the IPTC data"), _photo_iptc_reload_cb, NULL);
   elm_menu_item_separator_add(tm, NULL);

   iphoto_delete = elm_menu_item_add(tm, NULL, NULL, D_("Delete"), _photo_delete_cb, NULL);
   //


   //
   item = elm_toolbar_item_add(menu, NULL, "Geocaching", NULL, NULL);
   elm_toolbar_item_menu_set(item, 1);
   tm = elm_toolbar_item_menu_get(item);
   elm_toolbar_menu_parent_set(menu, parent);

   elm_menu_item_add(tm, NULL, NULL, D_("Import a GPX file"), _geocaching_import_cb, NULL);
   //


   evas_object_show(menu);
   return menu;
}

Evas_Object *menu_photo_new(Evas_Object *parent)
{
   Evas_Object *menu, *icon;
   Elm_Menu_Item *mi;
   ASSERT_RETURN(parent != NULL);

   menu = elm_menu_add(parent);
   evas_object_smart_callback_add(menu, "clicked", NULL, menu);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/undo");
   elm_menu_item_add(menu, NULL, icon, D_("Undo"), _undo_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/redo");
   elm_menu_item_add(menu, NULL, icon, D_("Redo"), _redo_cb, NULL);

   elm_menu_item_separator_add(menu, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/1_1");
   elm_menu_item_add(menu, NULL, icon, D_("1:1"), _1_1_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/fit");
   elm_menu_item_add(menu, NULL, icon, D_("Fit"), _fit_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/fill");
   elm_menu_item_add(menu, NULL, icon, D_("Fill"), _fill_cb, NULL);

   elm_menu_item_separator_add(menu, NULL);
   mi = elm_menu_item_add(menu, NULL, NULL, D_("Transform"), NULL, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/flip/vertical");
   elm_menu_item_add(menu, mi, icon, D_("Flip vertical"), _flip_vertical_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/flip/horizontal");
   elm_menu_item_add(menu, mi, icon, D_("Flip Horizontal"), _flip_horizontal_cb, NULL);

   elm_menu_item_separator_add(menu, mi);
   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/rotate/90");
   elm_menu_item_add(menu, mi, icon, D_("Rotate +90°"), _rotation_90_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/rotate/90/reverse");
   elm_menu_item_add(menu, mi, icon, D_("Rotate -90°"), _rotation_R90_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/rotate/180");
   elm_menu_item_add(menu, mi, icon, D_("Rotate 180°"), _rotation_180_cb, NULL);

   mi = elm_menu_item_add(menu, NULL, NULL, D_("Effects"), NULL, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/blur");
   elm_menu_item_add(menu, mi, icon, D_("Blur"), _blur_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/sharpen");
   elm_menu_item_add(menu, mi, icon, D_("Sharpen"), _sharpen_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/sepia");
   elm_menu_item_add(menu, mi, icon, D_("Sepia"), _sepia_cb, NULL);

   icon = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_icon_file_set(icon, PACKAGE_DATA_DIR"/theme.edj", "icons/grayscale");
   elm_menu_item_add(menu, mi, icon, D_("Grayscale"), _grayscale_cb, NULL);

   elm_menu_item_separator_add(menu, NULL);

   mi = elm_menu_item_add(menu, NULL, NULL, D_("Set as wallpaper"), NULL, NULL);
   elm_menu_item_add(menu, mi, NULL, D_("To the application"), _photo_wall_set_cb, NULL);

   elm_menu_item_add(menu, NULL, NULL, D_("Recreate the thumbnail"), _photo_thumb_reload_cb, NULL);
   elm_menu_item_add(menu, NULL, NULL, D_("Reload the exifs data"), _photo_exif_reload_cb, NULL);
   elm_menu_item_add(menu, NULL, NULL, D_("Reload the IPTC data"), _photo_iptc_reload_cb, NULL);

   elm_menu_item_separator_add(menu, NULL);

   mi = elm_menu_item_add(menu, NULL, NULL, D_("Delete the photo"), _photo_delete_cb, NULL);

   return menu;
}

static void _preferences_cb(void *data, Evas_Object *obj, void *event_info)
{
	inwin_preferences_new();
}

static void _new_root_cb(void *data, Evas_Object *obj, void *event_info)
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
   elm_fileselector_is_save_set(fs, 1);
   elm_fileselector_expandable_set(fs, EINA_FALSE);
   elm_fileselector_path_set(fs,  getenv("HOME"));
   evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(vbox, fs);
   evas_object_show(fs);

   evas_object_smart_callback_add(fs, "done", _new_root_done_cb, NULL);
}

static void _new_root_done_cb(void *data, Evas_Object *obj, void *event_info)
{
   const char *selected = event_info;

   if (selected)
     {
	if(!ecore_file_exists(selected))
	  ecore_file_mkdir(selected);
	if(ecore_file_is_dir(selected))
	  root_set(selected);
     }
   evas_object_del(inwin);
}

void menu_update_root_list(Eina_List *list)
{
   const Eina_List *l, *l_next;
   Enlil_String *string;
   Elm_Menu_Item *mi;

   EINA_LIST_FOREACH_SAFE(elm_menu_item_subitems_get(iroots), l, l_next, mi)
      elm_menu_item_del(mi);

   EINA_LIST_FOREACH(list, l, string)
      elm_menu_item_add(menu_file, iroots, NULL, string->string, _root_open_cb,
	    NULL);
}

void menu_loading_disable_set(Eina_Bool disabled)
{
   elm_menu_item_disabled_set(inew_root, disabled);
   elm_menu_item_disabled_set(iroots, disabled);
   elm_menu_item_disabled_set(iimport, disabled);
   elm_menu_item_disabled_set(ialbum_new, disabled);
   elm_menu_item_disabled_set(islideshow, disabled);
}

void menu_sync_disable_set(Eina_Bool disabled)
{
   elm_menu_item_disabled_set(inew_root, disabled);
   elm_menu_item_disabled_set(iroots, disabled);
}

void menu_redo_disabled_set(Eina_Bool disabled)
{
   elm_menu_item_disabled_set(iredo, disabled);
}

void menu_undo_disabled_set(Eina_Bool disabled)
{
   elm_menu_item_disabled_set(iundo, disabled);
}

void menu_photo_disabled_set(Eina_Bool disabled)
{
   elm_menu_item_disabled_set(isave, disabled);
   elm_menu_item_disabled_set(isaveas, disabled);
   elm_menu_item_disabled_set(i1_1, disabled);
   elm_menu_item_disabled_set(ifit, disabled);
   elm_menu_item_disabled_set(ifill, disabled);
   elm_menu_item_disabled_set(i90, disabled);
   elm_menu_item_disabled_set(ir90, disabled);
   elm_menu_item_disabled_set(i180, disabled);
   elm_menu_item_disabled_set(ivertical, disabled);
   elm_menu_item_disabled_set(ihorizontal, disabled);
   elm_menu_item_disabled_set(iblur, disabled);
   elm_menu_item_disabled_set(isharpen, disabled);
   elm_menu_item_disabled_set(isepia, disabled);
   elm_menu_item_disabled_set(igrayscale, disabled);
   elm_menu_item_disabled_set(iphoto_delete, disabled);
   elm_menu_item_disabled_set(iphoto_thumb_reload, disabled);
   elm_menu_item_disabled_set(iphoto_exif_reload, disabled);
   elm_menu_item_disabled_set(iphoto_iptc_reload, disabled);
   elm_menu_item_disabled_set(iphoto_bg, disabled);
}

void menu_noroot_disabled_set(Eina_Bool disabled)
{
   menu_photo_disabled_set(disabled);
   menu_loading_disable_set(disabled);
   elm_menu_item_disabled_set(inew_root, EINA_FALSE);
   elm_menu_item_disabled_set(iroots, EINA_FALSE);
   elm_menu_item_disabled_set(idel_bg, disabled);
}


static void _quit_cb(void *data, Evas_Object *obj, void *event_info)
{
   close_cb(NULL, NULL, NULL);
}

static void _import_cb(void *data, Evas_Object *obj, void *event_info)
{
   import_new(enlil_data->win->win);
}

static void _root_open_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elm_Menu_Item *mi = event_info;

   root_set(elm_menu_item_label_get(mi));
}

static void _undo_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_undo(current_photo);
}

static void _redo_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_redo(current_photo);
}

static void _save_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_save(current_photo);
}

static void _save_as_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_save_as(current_photo);
}

static void _slideshow_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Photo *photo = NULL;
   PL_Child_Item *item = NULL;

   Eina_List *items = photos_list_object_selected_get(enlil_data->list_photo->o_list);

   if(items)
     {
	item = eina_list_data_get(items);
	photo = photos_list_object_item_data_get(item);
     }

   slideshow_clear();
   slideshow_root_add(enlil_data->root, photo);
   slideshow_show();
}

static void _album_new_cb(void *data, Evas_Object *obj, void *event_info)
{
   inwin_album_new_new(NULL, NULL);
}

static void _1_1_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_1_1(current_photo);
}

static void _fit_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_fit(current_photo);
}


static void _fill_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_fill(current_photo);
}

static void _rotation_90_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_rotation_90(current_photo);
}

static void _rotation_R90_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_rotation_R90(current_photo);
}

static void _rotation_180_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_rotation_180(current_photo);
}

static void _flip_vertical_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_flip_vertical(current_photo);
}

static void _flip_horizontal_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_flip_horizontal(current_photo);
}

static void _blur_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_blur(current_photo);
}

static void _sharpen_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_sharpen(current_photo);
}

static void _sepia_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_sepia(current_photo);
}

static void _grayscale_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_grayscale(current_photo);
}

static void _photo_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
   inwin_photo_delete_new(enlil_data->win->win, NULL, NULL, eina_list_append(NULL, current_photo));
}

static void _photo_thumb_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(current_photo);
   enlil_photo_thumb_fdo_normal_set(current_photo, NULL);
   enlil_photo_thumb_fdo_large_set(current_photo, NULL);
   photos_list_object_item_update(photo_data->list_photo_item);
}

static void _photo_exif_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   enlil_exif_job_prepend(current_photo, exif_load_done, current_photo);
}

static void _photo_iptc_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   enlil_iptc_job_prepend(current_photo, iptc_load_done, current_photo);
}

static void _del_bg_cb(void *data, Evas_Object *obj, void *event_info)
{
   char buf[PATH_MAX];
   Enlil_String s;
   Eet_Data_Descriptor *edd;

   enlil_win_bg_set(enlil_data->win, NULL);

   s.string = NULL;
   edd = enlil_string_edd_new();

   snprintf(buf, PATH_MAX, "%s %s", APP_NAME" background", enlil_root_path_get(enlil_data->root));
   enlil_eet_app_data_save(edd, buf, &s);
   eet_data_descriptor_free(edd);
}

static void _photo_wall_set_cb(void *data, Evas_Object *obj, void *event_info)
{
   char buf[PATH_MAX];
   Enlil_String s;
   Eet_Data_Descriptor *edd;

   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(current_photo),
	 enlil_photo_file_name_get(current_photo));
   enlil_win_bg_set(enlil_data->win, buf);

   s.string = eina_stringshare_add(buf);
   edd = enlil_string_edd_new();

   snprintf(buf, PATH_MAX, "%s %s", APP_NAME" background", enlil_root_path_get(enlil_data->root));
   enlil_eet_app_data_save(edd, buf, &s);
   eet_data_descriptor_free(edd);
   eina_stringshare_del(s.string);
}

static void _geocaching_import_cb(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *fs, *vbox;

   inwin = elm_win_inwin_add(enlil_data->win->win);
   evas_object_show(inwin);

   vbox = elm_box_add(inwin);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(vbox);
   elm_win_inwin_content_set(inwin, vbox);

   fs = elm_fileselector_add(inwin);
   elm_fileselector_expandable_set(fs, EINA_FALSE);
   elm_fileselector_path_set(fs,  getenv("HOME"));
   evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(vbox, fs);
   evas_object_show(fs);

   evas_object_smart_callback_add(fs, "done", _geocaching_import_done_cb, NULL);
}

static void _geocaching_import_done_cb(void *data, Evas_Object *obj, void *event_info)
{
   const char *selected = event_info;

   if (selected)
     {
	char *ext = strchr(selected, '.');
	if(ext && !strcmp(ext, ".gpx"))
	  {
	     enlil_geocaching_import(selected, geocaching_done_cb, NULL);
	  }
     }
   evas_object_del(inwin);
}

