#include "main.h"

Enlil_Photo *current_photo = NULL;

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



Evas_Object *menu_photo_new(Evas_Object *parent)
{
   Evas_Object *menu;
   Elm_Menu_Item *mi;
   ASSERT_RETURN(parent != NULL);

   menu = elm_menu_add(parent);
   evas_object_smart_callback_add(menu, "clicked", NULL, menu);

   elm_menu_item_add(menu, NULL, "icons/undo", D_("Undo"), _undo_cb, NULL);
   elm_menu_item_add(menu, NULL, "icons/redo", D_("Redo"), _redo_cb, NULL);

   elm_menu_item_separator_add(menu, NULL);


   elm_menu_item_add(menu, NULL, "icons/1_1", D_("1:1"), _1_1_cb, NULL);
   elm_menu_item_add(menu, NULL, "icons/fit", D_("Fit"), _fit_cb, NULL);
   elm_menu_item_add(menu, NULL, "icons/fill", D_("Fill"), _fill_cb, NULL);

   elm_menu_item_separator_add(menu, NULL);
   mi = elm_menu_item_add(menu, NULL, NULL, D_("Transform"), NULL, NULL);

   elm_menu_item_add(menu, mi, "icons/flip/vertical", D_("Flip vertical"), _flip_vertical_cb, NULL);
   elm_menu_item_add(menu, mi, "icons/flip/horizontal", D_("Flip Horizontal"), _flip_horizontal_cb, NULL);

   elm_menu_item_separator_add(menu, mi);

   elm_menu_item_add(menu, mi, "icons/rotate/90", D_("Rotate +90°"), _rotation_90_cb, NULL);
   elm_menu_item_add(menu, mi, "icons/rotate/90/reverse", D_("Rotate -90°"), _rotation_R90_cb, NULL);
   elm_menu_item_add(menu, mi, "icons/rotate/180", D_("Rotate 180°"), _rotation_180_cb, NULL);

   mi = elm_menu_item_add(menu, NULL, NULL, D_("Effects"), NULL, NULL);

   elm_menu_item_add(menu, mi, "icons/blur", D_("Blur"), _blur_cb, NULL);
   elm_menu_item_add(menu, mi, "icons/sharpen", D_("Sharpen"), _sharpen_cb, NULL);
   elm_menu_item_add(menu, mi, "icons/sepia", D_("Sepia"), _sepia_cb, NULL);
   elm_menu_item_add(menu, mi, "icons/grayscale", D_("Grayscale"), _grayscale_cb, NULL);

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

static void _undo_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_undo(current_photo);
}

static void _redo_cb(void *data, Evas_Object *obj, void *event_info)
{
   panel_image_redo(current_photo);
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

