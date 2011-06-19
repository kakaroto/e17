#include "main.h"
#include "evas_object/photo_object.h"

Elm_Map_Marker_Class *itc;
Elm_Map_Group_Class *itc_group;

Elm_Map_Marker_Class *itc_gp;
Elm_Map_Group_Class *itc_gp_group;

static void
_geocaching_import_cb(void *data, Evas_Object *obj, void *event_info);
static void
_geocaching_import_done_cb(void *data, Evas_Object *obj, void *event_info);
static void
_slider_zoom_cb(void *data, Evas_Object *obj, void *event_info);

static void
_mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

static Evas_Object *
_marker_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data);
static Evas_Object *
_gp_marker_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data);
static Evas_Object *
_gp_marker_icon_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data);
static Evas_Object *
_gp_group_icon_get(Evas_Object *obj, void *data);

static void
_enki_open(void *data, Evas_Object *obj, void *event_info);
static void
_photo_select_cb(void *data, Evas_Object *obj, void *event_info);
static void
_menu(void *data, Evas_Object *obj, void *event_info);
static void
_tg_geocaching_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void
_tg_photos_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_geocaching_cb(void *data, Evas_Object *obj, void *event_info);

Map *
map_new(Evas_Object *edje)
{
   Evas_Object *bt;
   Map *map = calloc(1, sizeof(Map));

   //
   map->map = edje_object_part_external_object_get(edje, "object.map.map");

   itc_group = elm_map_group_class_new(map->map);

   itc = elm_map_marker_class_new(map->map);
   elm_map_marker_class_get_cb_set(itc, _marker_get);

   itc_gp_group = elm_map_group_class_new(map->map);
   elm_map_group_class_icon_cb_set(itc_gp_group, _gp_group_icon_get);
   elm_map_group_class_style_set(itc_gp_group, "empty");
   elm_map_group_class_zoom_grouped_set(itc_gp_group, 10);

   itc_gp = elm_map_marker_class_new(map->map);
   elm_map_marker_class_get_cb_set(itc_gp, _gp_marker_get);
   elm_map_marker_class_icon_cb_set(itc_gp, _gp_marker_icon_get);
   elm_map_marker_class_style_set(itc_gp, "empty");
   //

   //
   map->display_photos
            = edje_object_part_external_object_get(edje,
                                                   "object.map.display_photos");
   evas_object_smart_callback_add(map->display_photos, "changed",
                                  _tg_photos_changed_cb, map);
   //

   //
   map->display_geocaching
            = edje_object_part_external_object_get(edje,
                                                   "object.map.display_geocaching");
   evas_object_smart_callback_add(map->display_geocaching, "changed",
                                  _tg_geocaching_changed_cb, map);
   //

   //
   map->sl = edje_object_part_external_object_get(edje, "object.map.zoom");
   evas_object_smart_callback_add(map->sl, "delay,changed", _slider_zoom_cb,
                                  map);
   //

   //
   map->rect
            = (Evas_Object *) edje_object_part_object_get(edje,
                                                          "object.map.rect_zoom");
   evas_object_event_callback_add(map->rect, EVAS_CALLBACK_MOUSE_WHEEL,
                                  _mouse_wheel_cb, map);
   //

   //
   bt = edje_object_part_external_object_get(edje,
                                             "object.map.import_geocaching");
   evas_object_smart_callback_add(bt, "clicked", _geocaching_import_cb, NULL);
   //

   return map;
}

void
map_free(Map *map)
{
   evas_object_del(map->bx);
   evas_object_del(map->rect);
   map->selected = NULL;
   free(map);
}

void
map_photo_add(Map *map, Enlil_Photo *photo)
{
   char buf[PATH_MAX];
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   switch (enlil_photo_type_get(photo))
   {
   case ENLIL_PHOTO_TYPE_PHOTO:
      if (enlil_photo_longitude_get(photo) != 360
               && enlil_photo_latitude_get(photo) != 360)
      {
         photo_data->marker
                  = elm_map_marker_add(map->map,
                                       enlil_photo_longitude_get(photo),
                                       enlil_photo_latitude_get(photo), itc,
                                       itc_group, photo);
      }
      break;

   case ENLIL_PHOTO_TYPE_GPX:
      snprintf(buf, sizeof(buf), "%s/%s", enlil_photo_path_get(photo),
               enlil_photo_file_name_get(photo));
      /* photo_data->route.route = elm_map_route_from_gpx_file_add(map->map, buf); */
      photo_data->route.route = NULL;
      break;
   }
}

void
map_photo_update(Map *map, Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   if (photo_data->marker && (enlil_photo_longitude_get(photo) == 360
            || enlil_photo_latitude_get(photo) == 360))
   {
      elm_map_marker_remove(photo_data->marker);
      photo_data->marker = NULL;
      if (photo == map->selected) map->selected = NULL;
   }
   else if (photo_data->marker)
   {
      if (photo == map->selected) map->selected = NULL;
      elm_map_marker_update(photo_data->marker);
   }
   else if (enlil_photo_longitude_get(photo) != 360
            && enlil_photo_latitude_get(photo) != 360) map_photo_add(map, photo);
}

void
map_photo_remove(Map *map, Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   if (map->selected == photo) map->selected = NULL;

   switch (enlil_photo_type_get(photo))
   {
   case ENLIL_PHOTO_TYPE_PHOTO:
      elm_map_marker_remove(photo_data->marker);
      photo_data->marker = NULL;
      break;

   case ENLIL_PHOTO_TYPE_GPX:
      elm_map_route_remove(photo_data->route.route);
      photo_data->route.route = NULL;
      break;
   }
}

void
map_geocaching_add(Map *map, Enlil_Geocaching *gp)
{
   Geocaching_Data *gp_data = enlil_geocaching_user_data_get(gp);

   if (!elm_toggle_state_get(map->display_geocaching)) return;

   if (enlil_geocaching_longitude_get(gp) != 360
            && enlil_geocaching_latitude_get(gp) != 360)
   {
      gp_data->marker = elm_map_marker_add(map->map,
                                           enlil_geocaching_longitude_get(gp),
                                           enlil_geocaching_latitude_get(gp),
                                           itc_gp, itc_gp_group, gp);
   }
}

void
map_geocaching_update(Map *map, Enlil_Geocaching *gp)
{
   Geocaching_Data *gp_data = enlil_geocaching_user_data_get(gp);

   if (gp_data->marker && (enlil_geocaching_longitude_get(gp) == 360
            || enlil_geocaching_latitude_get(gp) == 360))
   {
      elm_map_marker_remove(gp_data->marker);
      gp_data->marker = NULL;
   }
   else if (gp_data->marker)
   {
      elm_map_marker_update(gp_data->marker);
   }
   else if (enlil_geocaching_longitude_get(gp) != 360
            && enlil_geocaching_latitude_get(gp) != 360) map_geocaching_add(
                                                                            map,
                                                                            gp);
}

void
map_geocaching_remove(Map *map, Enlil_Geocaching *gp)
{
   Geocaching_Data *gp_data = enlil_geocaching_user_data_get(gp);

   elm_map_marker_remove(gp_data->marker);
   gp_data->marker = NULL;
}

static void
_slider_zoom_cb(void *data, Evas_Object *obj, void *event_info)
{
   Map *map = data;

   double val = elm_slider_value_get(map->sl);
   elm_map_zoom_set(map->map, val);
}

static void
_mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Map *map = data;
   Evas_Event_Mouse_Wheel *ev = (Evas_Event_Mouse_Wheel *) event_info;
   int zoom;
   //unset the mouse wheel
   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;

   zoom = elm_map_zoom_get(map->map);

   if (ev->z > 0)
      zoom--;
   else
      zoom++;

   elm_map_zoom_mode_set(map->map, ELM_MAP_ZOOM_MODE_MANUAL);
   if (zoom >= 0 && zoom <= 18)
   {
      elm_map_zoom_set(map->map, zoom);
      elm_slider_value_set(map->sl, zoom);
   }
}

static Evas_Object *
_marker_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data)
{
   const char *s = NULL;
   Enlil_Photo *photo = (Enlil_Photo *) data;
   Enlil_Photo_Data *enlil_photo_data = enlil_photo_user_data_get(photo);

   Evas_Object *o = photo_object_add(obj);
   photo_object_theme_file_set(o, Theme, "photo");
   evas_object_size_hint_min_set(o, 192, 128);

   evas_object_smart_callback_add(o, "open", _enki_open, photo);
   evas_object_smart_callback_add(o, "select", _photo_select_cb, photo);
   evas_object_smart_callback_add(o, "clicked,menu", _menu, photo);
   evas_object_smart_callback_add(o, "clicked,right", _menu, photo);

   if (enlil_photo_data->cant_create_thumb == 1) return o;

   s = enlil_thumb_photo_get(photo, Enlil_THUMB_FDO_NORMAL, thumb_done_cb,
                             thumb_error_cb, NULL);

   evas_image_cache_flush(evas_object_evas_get(obj));

   if (s)
      photo_object_file_set(o, s, NULL);
   else
      photo_object_progressbar_set(o, EINA_TRUE);

   if (enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_VIDEO) photo_object_camera_set(
                                                                                      o,
                                                                                      EINA_TRUE);

   photo_object_text_set(o, enlil_photo_name_get(photo));

   evas_object_show(o);
   return o;
}

static Evas_Object *
_gp_marker_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data)
{
   char buf[PATH_MAX];
   Evas_Object *o, *bt, *edje, *lbl;
   Enlil_Geocaching *gp = data;
   //Geocaching_Data *gp_data = enlil_geocaching_user_data_get(gp);

   edje = edje_object_add(evas_object_evas_get(obj));
   evas_object_show(edje);
   edje_object_file_set(edje, Theme, "bubble/geocaching");
   evas_object_size_hint_min_set(edje, 300, 100);

   snprintf(buf, sizeof(buf), "<b><font_size=14>%s</font_size></b>",
            enlil_geocaching_url_name_get(gp));
   lbl = edje_object_part_external_object_get(edje,
                                              "object.geocaching.bubble.title");
   elm_label_label_set(lbl, buf);

   snprintf(buf, sizeof(buf), "<b>A cache by %s</b>",
            enlil_geocaching_gp_owner_get(gp));
   lbl = edje_object_part_external_object_get(edje,
                                              "object.geocaching.bubble.owner");
   elm_label_label_set(lbl, buf);

   lbl
            = edje_object_part_external_object_get(edje,
                                                   "object.geocaching.bubble.cache_size");
   elm_label_label_set(lbl, enlil_geocaching_gp_container_get(gp));

   lbl
            = edje_object_part_external_object_get(edje,
                                                   "object.geocaching.bubble.date_hidden");
   elm_label_label_set(lbl, enlil_geocaching_time_get(gp));

   bt = edje_object_part_external_object_get(edje,
                                             "object.geocaching.bubble.open");
   elm_button_label_set(bt, enlil_geocaching_name_get(gp));
   o = elm_icon_add(obj);
   snprintf(buf, sizeof(buf), "icons/geocaching/%s",
            enlil_geocaching_gp_type_get(gp));
   elm_icon_file_set(o, Theme, buf);
   elm_button_icon_set(bt, o);
   evas_object_smart_callback_add(bt, "clicked", _bt_geocaching_cb, gp);

   snprintf(buf, sizeof(buf), "terrain,%s", enlil_geocaching_gp_terrain_get(gp));
   edje_object_signal_emit(edje, buf, "");

   snprintf(buf, sizeof(buf), "difficulty,%s",
            enlil_geocaching_gp_difficulty_get(gp));
   edje_object_signal_emit(edje, buf, "");

   return edje;
}

static Evas_Object *
_gp_marker_icon_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data)
{
   char buf[PATH_MAX];
   Evas_Object *icon;
   Enlil_Geocaching *gp = data;
   Eina_Bool found = EINA_FALSE;
   //Geocaching_Data *gp_data = enlil_geocaching_user_data_get(gp);

   const char *s = eina_stringshare_add("Geocache Found");
   if (enlil_geocaching_sym_get(gp) == s) found = EINA_TRUE;
   EINA_STRINGSHARE_DEL(s);

   icon = elm_icon_add(obj);
   if (found)
      snprintf(buf, PATH_MAX, "icons/geocaching/found");
   else
      snprintf(buf, PATH_MAX, "icons/geocaching/%s",
               enlil_geocaching_gp_type_get(gp));

   elm_icon_file_set(icon, Theme, buf);

   evas_object_show(icon);
   return icon;
}

static Evas_Object *
_gp_group_icon_get(Evas_Object *obj, void *data)
{
   Evas_Object *icon;

   icon = elm_icon_add(obj);
   elm_icon_file_set(icon, Theme, "icons/geocaching/geocaching");

   evas_object_show(icon);
   return icon;
}

static void
_photo_select_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Photo *photo = (Enlil_Photo *) data;

   if (enlil_data->map->selected && enlil_data->map->selected != photo)
   {
      Enlil_Photo_Data *photo_data_selected =
               enlil_photo_user_data_get(enlil_data->map->selected);
      Evas_Object *o = elm_map_marker_object_get(photo_data_selected->marker);
      if (o) evas_object_smart_callback_call(o, "unselect", NULL);
   }
   enlil_data->map->selected = photo;
}

static void
_enki_open(void *data, Evas_Object *obj, void *event_info)
{
   char buf[PATH_MAX];
   Enlil_Photo *photo = (Enlil_Photo *) data;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   if (photo_data->panel_image) return;
   if (enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_VIDEO)
   {
      snprintf(buf, PATH_MAX, "%s \"%s/%s\"", media_player,
               enlil_photo_path_get(photo), enlil_photo_file_name_get(photo));
      ecore_exe_run(buf, NULL);
   }
   else
      panel_image_new(obj, photo);
}

static void
_menu(void *data, Evas_Object *obj, void *event_info)
{
   //   int x, y;
   //   Enlil_Photo *photo = data;
   //   Eina_List *photos = NULL;
   //   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   //
   //   photos = eina_list_append(photos, photo);
   //
   //   evas_pointer_output_xy_get(evas_object_evas_get(obj), &x, &y);
   //   Photo_Menu *photo_menu = photo_menu_new(photo_data->enlil_data->win->win, photo, photos);
   //   elm_menu_move(photo_menu->menu, x, y);
}

static void
_tg_geocaching_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Map *map = data;

   if (elm_toggle_state_get(map->display_geocaching))
   {
      enlil_geocaching_get(geocaching_done_cb, NULL);
   }
   else
   {
      enlil_geocaching_get(geocaching_remove_marker_cb, NULL);
   }
}

static void
_tg_photos_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Map *map = data;

   if (elm_toggle_state_get(map->display_photos))
   {
      elm_map_group_class_hide_set(map->map, itc_group, EINA_FALSE);
   }
   else
   {
      elm_map_group_class_hide_set(map->map, itc_group, EINA_TRUE);
   }
}

static void
_bt_geocaching_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Geocaching *geo = data;
   Geocaching_Data *geo_data = enlil_geocaching_user_data_get(geo);

   if (geo_data->panel_geocaching) return;

   panel_geocaching_new(obj, data);
}

static Evas_Object *inwin = NULL;
static void
_geocaching_import_cb(void *data, Evas_Object *obj, void *event_info)
{
   printf("HEHE\n");
   Evas_Object *fs, *vbox;

   inwin = elm_win_inwin_add(enlil_data->win->win);
   evas_object_show(inwin);

   vbox = elm_box_add(inwin);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(vbox);
   elm_win_inwin_content_set(inwin, vbox);

   fs = elm_fileselector_add(inwin);
   elm_fileselector_expandable_set(fs, EINA_FALSE);
   elm_fileselector_path_set(fs, getenv("HOME"));
   evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(vbox, fs);
   evas_object_show(fs);

   evas_object_smart_callback_add(fs, "done", _geocaching_import_done_cb, NULL);
}

static void
_geocaching_import_done_cb(void *data, Evas_Object *obj, void *event_info)
{
   const char *selected = event_info;

   if (selected)
   {
      char *ext = strchr(selected, '.');
      if (ext && !strcmp(ext, ".gpx"))
      {
         enlil_geocaching_import(selected, geocaching_done_cb, NULL);
      }
   }
   evas_object_del(inwin);
}
